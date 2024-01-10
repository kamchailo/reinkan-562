#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

namespace Reinkan::Graphics
{
    void ReinkanApp::AppendLight(const LightObject& lightObject)
    {
        appLightObjects.push_back(lightObject);
    }
}