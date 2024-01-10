#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

namespace Reinkan::Graphics
{
    void ReinkanApp::DestroyParallaxOcclusionResources()
    {
        for (auto pyramidalImageWrap : appPyramidalImageWraps)
            pyramidalImageWrap.Destroy(appDevice);
    }
}