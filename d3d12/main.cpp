#include <d3d12.h>
#include <cassert>

int main()
{
    HRESULT result;
    ID3D12Device *device = nullptr;
    result = D3D12CreateDevice(
        nullptr,
        D3D_FEATURE_LEVEL_9_3,
        IID_PPV_ARGS(&device)
    );
    assert(S_OK == result);
    // TODO: how to destroy device?
    return 0;
}
