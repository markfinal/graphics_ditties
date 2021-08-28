#include <d3d11.h>
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"
#include <cassert>
#include <iostream>

static void error_callback(int code, const char* description)
{
    std::cerr << "glfw error code: " << code << " (" << description << ")" << std::endl;
}

static float randomNumber()
{
    return float(rand()) / float(RAND_MAX);
}

static void render(ID3D11DeviceContext *context, ID3D11RenderTargetView *rtv)
{
    const FLOAT color[4] = {randomNumber(), randomNumber(), randomNumber(), 1.0f};
    context->ClearRenderTargetView(rtv, color);
}

int main()
{
    if (!glfwInit())
    {
        return -1;
    }

    glfwSetErrorCallback(error_callback);

    int major, minor, revision;
    glfwGetVersion(&major, &minor, &revision);

    std::cout << "Running against GLFW " << major << "." << minor << "." << revision << std::endl;

    GLFWwindow *window = glfwCreateWindow(640, 480, "D3D11 ditty", NULL, NULL);

    HRESULT result;

    D3D_FEATURE_LEVEL feature_level;
    UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
    flags |= D3D11_CREATE_DEVICE_DEBUG;

    DXGI_SWAP_CHAIN_DESC swap_chain_descr = { 0 };
    swap_chain_descr.BufferDesc.RefreshRate.Numerator = 0;
    swap_chain_descr.BufferDesc.RefreshRate.Denominator = 1;
    swap_chain_descr.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    swap_chain_descr.SampleDesc.Count = 1;
    swap_chain_descr.SampleDesc.Quality = 0;
    swap_chain_descr.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_descr.BufferCount = 1;
    swap_chain_descr.OutputWindow = glfwGetWin32Window(window);
    swap_chain_descr.Windowed = true;

    ID3D11Device *device_ptr = nullptr;
    ID3D11DeviceContext *device_context_ptr = nullptr;
    IDXGISwapChain *swap_chain_ptr = nullptr;

    result = D3D11CreateDeviceAndSwapChain(
        NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        flags,
        NULL,
        0,
        D3D11_SDK_VERSION,
        &swap_chain_descr,
        &swap_chain_ptr,
        &device_ptr,
        &feature_level,
        &device_context_ptr
    );
    assert(SUCCEEDED(result));

    ID3D11RenderTargetView *render_target_view_ptr = nullptr;
    {
        ID3D11Texture2D* framebuffer;
        result = swap_chain_ptr->GetBuffer(
            0,
            __uuidof(ID3D11Texture2D),
            (void**)&framebuffer
        );
        assert(SUCCEEDED(result));

        result = device_ptr->CreateRenderTargetView(
            framebuffer,
            0,
            &render_target_view_ptr
        );
        assert(SUCCEEDED(result));
        framebuffer->Release();
    }

    while (!glfwWindowShouldClose(window))
    {
        render(device_context_ptr, render_target_view_ptr);
        swap_chain_ptr->Present(0, 0);
        glfwPollEvents();
    }

    render_target_view_ptr->Release();
    swap_chain_ptr->Release();
    device_context_ptr->Release();
    device_ptr->Release();

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;

    return 0;
}
