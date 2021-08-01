#include <d3d12.h>
#include <dxgi1_4.h>
#include <cassert>

int main()
{
    HRESULT result;

    UINT dxgiFactoryFlags = 0;
    dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;

    IDXGIFactory4* factory = nullptr;
    result = CreateDXGIFactory2(
        dxgiFactoryFlags,
        IID_PPV_ARGS(&factory)
    );
    assert(S_OK == result);

    ID3D12Debug *debugInterface = nullptr;
    result = D3D12GetDebugInterface(
        IID_PPV_ARGS(&debugInterface)
    );
    assert(S_OK == result);
    debugInterface->EnableDebugLayer();

    ID3D12Device *device = nullptr;
    result = D3D12CreateDevice(
        nullptr,
        D3D_FEATURE_LEVEL_11_0,
        IID_PPV_ARGS(&device)
    );
    assert(S_OK == result);

    device->Release();
    debugInterface->Release();
    factory->Release();

    return 0;
}
