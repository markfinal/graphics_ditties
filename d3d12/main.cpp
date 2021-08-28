#include <d3d12.h>
#include <dxgi1_4.h>
#include <dxgidebug.h>
#include <cassert>

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

    dxgiDebug->ReportLiveObjects(
        DXGI_DEBUG_ALL,
        DXGI_DEBUG_RLO_ALL
    );

    device->Release();
    dxgiDebug->Release();
    adapter->Release();
    factory->Release();
    debugInterface->Release();

    return 0;
}
