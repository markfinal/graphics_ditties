#include <d3d12.h>
#include <dxgi1_4.h>
#include <dxgidebug.h>
#include <directx/d3dx12.h>
#include <cassert>
#include <tuple>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

#include <iostream>

static void error_callback(int code, const char* description)
{
    std::cerr << "glfw error code: " << code << " (" << description << ")" << std::endl;
}

static IDXGIAdapter1 *enumerateAdapters(IDXGIFactory4 *factory)
{
    HRESULT hr;
    IDXGIAdapter1 *adapter = nullptr;
    for (UINT adapterIndex = 0;
         DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(adapterIndex, &adapter);
         ++adapterIndex)
    {
        DXGI_ADAPTER_DESC1 desc;
        hr = adapter->GetDesc1(&desc);
        if (FAILED(hr))
        {
            continue;
        }

        // don't create, just check
        if (SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
        {
            break;
        }

        adapter->Release();
        adapter = nullptr;
    }

    return adapter;
}

static D3D_FEATURE_LEVEL checkFeatureSupport(ID3D12Device *device)
{
    static const D3D_FEATURE_LEVEL s_featureLevels[] =
    {
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    D3D12_FEATURE_DATA_FEATURE_LEVELS featLevels =
    {
        _countof(s_featureLevels), s_featureLevels, D3D_FEATURE_LEVEL_11_0
    };

    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    HRESULT hr = device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featLevels, sizeof(featLevels));
    if (SUCCEEDED(hr))
    {
        featureLevel = featLevels.MaxSupportedFeatureLevel;
    }

    return featureLevel;
}

static ID3D12CommandQueue *createCommandQueue(ID3D12Device* device)
{
    D3D12_COMMAND_QUEUE_DESC cqDesc = {};

    ID3D12CommandQueue *commandQueue = nullptr;
    HRESULT hr = device->CreateCommandQueue(&cqDesc, IID_PPV_ARGS(&commandQueue)); // create the command queue
    if (FAILED(hr))
    {
        return nullptr;
    }

    return commandQueue;
}

static IDXGISwapChain3 *createSwapChain(const int width, const int height, ID3D12CommandQueue *commandQueue, IDXGIFactory4 *factory, HWND hwnd)
{
    DXGI_MODE_DESC backBufferDesc = {};
    backBufferDesc.Width = width;
    backBufferDesc.Height = height;
    backBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    DXGI_SAMPLE_DESC sampleDesc = {};
    sampleDesc.Count = 1;

    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = 2;
    swapChainDesc.BufferDesc = backBufferDesc;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.OutputWindow = hwnd;
    swapChainDesc.SampleDesc = sampleDesc;
    swapChainDesc.Windowed = true;

    IDXGISwapChain *tempSwapChain;

    factory->CreateSwapChain(
        commandQueue,
        &swapChainDesc,
        &tempSwapChain
    );

    auto swapChain = static_cast<IDXGISwapChain3*>(tempSwapChain);

    auto frameIndex = swapChain->GetCurrentBackBufferIndex();

    return swapChain;
}

static std::tuple<ID3D12DescriptorHeap*, UINT, std::vector<ID3D12Resource*>> createRenderTargetViews(ID3D12Device *device, IDXGISwapChain3 *swapChain)
{
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = 2;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ID3D12DescriptorHeap *rtvDescriptorHeap = nullptr;
    HRESULT hr = device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvDescriptorHeap));
    if (FAILED(hr))
    {
        return std::make_tuple(nullptr, 0, std::vector<ID3D12Resource*>());
    }

    auto rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

    std::vector<ID3D12Resource*> renderTargets;
    renderTargets.resize(2);
    for (int i = 0; i < 2; i++)
    {
        // first we get the n'th buffer in the swap chain and store it in the n'th
        // position of our ID3D12Resource array
        hr = swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i]));
        if (FAILED(hr))
        {
            return std::make_tuple(nullptr, 0, std::vector<ID3D12Resource*>());
        }

        // the we "create" a render target view which binds the swap chain buffer (ID3D12Resource[n]) to the rtv handle
        device->CreateRenderTargetView(renderTargets[i], nullptr, rtvHandle);

        // we increment the rtv handle by the rtv descriptor size we got above
        rtvHandle.Offset(1, rtvDescriptorSize);
    }

    return std::make_tuple(rtvDescriptorHeap, rtvDescriptorSize, renderTargets);
}

static std::vector<ID3D12CommandAllocator*> createCommandAllocators(ID3D12Device *device)
{
    std::vector<ID3D12CommandAllocator*> commandAllocator;
    commandAllocator.resize(2);
    for (int i = 0; i < 2; i++)
    {
        HRESULT hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator[i]));
        if (FAILED(hr))
        {
            return std::vector<ID3D12CommandAllocator*>();
        }
    }
    return commandAllocator;
}

static ID3D12GraphicsCommandList *createCommandList(ID3D12Device *device, const std::vector<ID3D12CommandAllocator*> &commandAllocator)
{
    ID3D12GraphicsCommandList *commandList = nullptr;
    HRESULT hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator[0], NULL, IID_PPV_ARGS(&commandList));
    if (FAILED(hr))
    {
        return nullptr;
    }
    commandList->Close();
    return commandList;
}

static std::tuple<HANDLE, std::vector<UINT64>, std::vector<ID3D12Fence*>> createFences(ID3D12Device *device)
{
    std::vector<ID3D12Fence*> fence;
    std::vector<UINT64> fenceValue;
    fence.resize(2);
    fenceValue.resize(2);
    for (int i = 0; i < 2; i++)
    {
        HRESULT hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence[i]));
        if (FAILED(hr))
        {
            return std::make_tuple(nullptr, std::vector<UINT64>(), std::vector<ID3D12Fence*>());
        }
        fenceValue[i] = 0;
    }

    auto fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (fenceEvent == nullptr)
    {
        return std::make_tuple(nullptr, std::vector<UINT64>(), std::vector<ID3D12Fence*>());
    }

    return std::make_tuple(fenceEvent, fenceValue, fence);
}

static UINT waitForPreviousFrame(IDXGISwapChain3 *swapChain, const std::vector<ID3D12Fence*> &fence, std::vector<UINT64> &fenceValue, const HANDLE fenceEvent)
{
    HRESULT hr;

    auto frameIndex = swapChain->GetCurrentBackBufferIndex();

    if (fence[frameIndex]->GetCompletedValue() < fenceValue[frameIndex])
    {
        // we have the fence create an event which is signaled once the fence's current value is "fenceValue"
        hr = fence[frameIndex]->SetEventOnCompletion(fenceValue[frameIndex], fenceEvent);
        if (FAILED(hr))
        {
            // TODO?
            //Running = false;
        }

        // We will wait until the fence has triggered the event that it's current value has reached "fenceValue". once it's value
        // has reached "fenceValue", we know the command queue has finished executing
        WaitForSingleObject(fenceEvent, INFINITE);
    }

    // increment fenceValue for next frame
    fenceValue[frameIndex]++;

    return frameIndex;
}

static void flushGpu(std::vector<UINT64> &fenceValue, const std::vector<ID3D12Fence*> &fence, HANDLE fenceEvent, ID3D12CommandQueue *commandQueue)
{
    for (int i = 0; i < 2; i++)
    {
        uint64_t fenceValueForSignal = ++fenceValue[i];
        commandQueue->Signal(fence[i], fenceValueForSignal);
        if (fence[i]->GetCompletedValue() < fenceValue[i])
        {
            fence[i]->SetEventOnCompletion(fenceValueForSignal, fenceEvent);
            WaitForSingleObject(fenceEvent, INFINITE);
        }
    }
}

static UINT updatePipeline(
    IDXGISwapChain3 *swapChain,
    const std::vector<ID3D12Fence*>& fence,
    std::vector<UINT64>& fenceValue,
    const HANDLE fenceEvent,
    const std::vector<ID3D12CommandAllocator*>& commandAllocator,
    ID3D12GraphicsCommandList *commandList,
    const std::vector<ID3D12Resource*> &renderTargets,
    ID3D12DescriptorHeap *rtvDescriptorHeap,
    const UINT rtvDescriptorSize)
{
    auto frameIndex = waitForPreviousFrame(swapChain, fence, fenceValue, fenceEvent);

    HRESULT hr = commandAllocator[frameIndex]->Reset();
    if (FAILED(hr))
    {
        // TODO?
        //Running = false;
    }

    hr = commandList->Reset(commandAllocator[frameIndex], NULL);
    if (FAILED(hr))
    {
        // TODO?
        //Running = false;
    }

    commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, rtvDescriptorSize);

    commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    if (0 == frameIndex)
    {
        const float clearColor[] = { 0.2f, 0.9f, 0.1f, 1.0f };
        commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    }
    else
    {
        const float clearColor[] = { 1.0f, 0.6f, 0.9f, 1.0f };
        commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    }

    commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

    hr = commandList->Close();
    if (FAILED(hr))
    {
        // TODO?
        //Running = false;
    }

    return frameIndex;
}

static void render(ID3D12CommandList *commandList, ID3D12CommandQueue *commandQueue, UINT frameIndex, const std::vector<ID3D12Fence*> &fence, const std::vector<UINT64> &fenceValue)
{
    ID3D12CommandList* ppCommandLists[] = { commandList };

    commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    HRESULT hr = commandQueue->Signal(fence[frameIndex], fenceValue[frameIndex]);
    if (FAILED(hr))
    {
        // TODO?
        //Running = false;
    }
}

int main()
{
    HRESULT result;

    ID3D12Debug *debugInterface = nullptr;
    result = D3D12GetDebugInterface(
        IID_PPV_ARGS(&debugInterface)
    );
    assert(SUCCEEDED(result));
    debugInterface->EnableDebugLayer();

    UINT dxgiFactoryFlags = 0;
    dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;

    IDXGIFactory4 *factory = nullptr;
    result = CreateDXGIFactory2(
        dxgiFactoryFlags,
        IID_PPV_ARGS(&factory)
    );
    assert(SUCCEEDED(result));

    auto *adapter = enumerateAdapters(factory);
    assert(nullptr != adapter);

    IDXGIDebug *dxgiDebug = nullptr;
    result = DXGIGetDebugInterface1(
        0,
        IID_PPV_ARGS(&dxgiDebug)
    );
    assert(SUCCEEDED(result));

    ID3D12Device *device = nullptr;
    result = D3D12CreateDevice(
        adapter,
        D3D_FEATURE_LEVEL_11_0,
        IID_PPV_ARGS(&device)
    );
    assert(SUCCEEDED(result));

    checkFeatureSupport(device);

    auto commandQueue = createCommandQueue(device);

    if (!glfwInit())
    {
        return -1;
    }

    glfwSetErrorCallback(error_callback);

    int major, minor, revision;
    glfwGetVersion(&major, &minor, &revision);

    std::cout << "Running against GLFW " << major << "." << minor << "." << revision << std::endl;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow *window = glfwCreateWindow(640, 480, "D3D12 ditty", NULL, NULL);

    auto swapChain = createSwapChain(640, 480, commandQueue, factory, glfwGetWin32Window(window));

    auto [rtHeap, rtHeapSize, renderTargets] = createRenderTargetViews(device, swapChain);

    auto commandAllocators = createCommandAllocators(device);

    auto commandList = createCommandList(device, commandAllocators);

    auto [fenceEvent, fenceValue, fence] = createFences(device);

    dxgiDebug->ReportLiveObjects(
        DXGI_DEBUG_ALL,
        DXGI_DEBUG_RLO_ALL
    );

    while (!glfwWindowShouldClose(window))
    {
        auto frameIndex = updatePipeline(
            swapChain,
            fence,
            fenceValue,
            fenceEvent,
            commandAllocators,
            commandList,
            renderTargets,
            rtHeap,
            rtHeapSize
        );
        render(commandList, commandQueue, frameIndex, fence, fenceValue);
        HRESULT hr = swapChain->Present(0, 0);
        if (FAILED(hr))
        {
            // TODO?
            //Running = false;
        }
        glfwPollEvents();
    }

    flushGpu(fenceValue, fence, fenceEvent, commandQueue);

    for (auto f : fence)
    {
        f->Release();
    }
    CloseHandle(fenceEvent);

    commandList->Release();

    for (auto ca : commandAllocators)
    {
        ca->Release();
    }

    for (auto rt : renderTargets)
    {
        rt->Release();
    }
    rtHeap->Release();

    swapChain->Release();

    glfwDestroyWindow(window);

    glfwTerminate();

    commandQueue->Release();
    device->Release();
    dxgiDebug->Release();
    adapter->Release();
    factory->Release();
    debugInterface->Release();

    return 0;
}
