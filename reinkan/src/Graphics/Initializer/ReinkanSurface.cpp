#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

namespace Reinkan::Graphics
{
    void ReinkanApp::CreateSurface()
    {
        if (glfwCreateWindowSurface(appInstance, appWindow, nullptr, &appSurface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
    }
}