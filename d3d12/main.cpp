#include <d3d12.h>
#include <dxgi1_4.h>
#include <dxgidebug.h>
#include <cassert>

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

    IDXGIDebug *dxgiDebug = nullptr;
    result = DXGIGetDebugInterface1(
        0,
        IID_PPV_ARGS(&dxgiDebug)
    );
    assert(SUCCEEDED(result));

    ID3D12Device *device = nullptr;
    result = D3D12CreateDevice(
        nullptr,
        D3D_FEATURE_LEVEL_11_0,
        IID_PPV_ARGS(&device)
    );
    assert(SUCCEEDED(result));

    dxgiDebug->ReportLiveObjects(
        DXGI_DEBUG_ALL,
        DXGI_DEBUG_RLO_ALL
    );

    device->Release();
    dxgiDebug->Release();
    factory->Release();
    debugInterface->Release();

    return 0;
}
