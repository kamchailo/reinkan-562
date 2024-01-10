#include "pch.h"

#include "Core/ReinkanEngine.h"

#include "Graphics/ReinkanVulkan.h"
#include "Graphics/ResourceLoader/ReinkanModelLoader.h"
#include "Graphics/Structure/ModelData.h"

#include <glm/gtc/matrix_transform.hpp>

int main() 
{
    Reinkan::Core::ReinkanEngine engine;

    engine.Init(1920, 1080);
    
    engine.Update();

    engine.Destroy();

    return 0;
}

// Johnathan Blow Slerp for slerp