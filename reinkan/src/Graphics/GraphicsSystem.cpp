#include "pch.h"
#include "GraphicsSystem.h"

#include <random>
#include <glm/gtx/transform.hpp>

#include "ResourceLoader/ReinkanModelLoader.h"

namespace Reinkan::Graphics
{
    void GraphicsSystem::Init(uint32_t width, uint32_t height)
    {
        ////////////////////////////////////////
        //          Vulkan Init
        ////////////////////////////////////////
        vulkanApp = std::make_shared<ReinkanApp>(width, height);

        vulkanApp->Init();

        ////////////////////////////////////////
        //          Load & Bind Resources
        ////////////////////////////////////////
        {   // Indent to free model data after load

            std::vector<ModelData> modelDatas;

            //Reinkan::ModelData model;
            glm::mat4 modelTr = glm::mat4(1);
            /*modelTr = glm::scale(modelTr, glm::vec3(0.3));
            modelTr = glm::translate(modelTr, glm::vec3(18.0f, 18.0f, 1.5f));
            modelTr = glm::rotate(modelTr, glm::radians(90.0f), glm::vec3(0,0,1));*/
            ReadAssimpFile("../assets/models/wheel.obj",
                modelTr,
                modelDatas,
                vulkanApp->GetAppMaterialPool(),
                vulkanApp->GetAppTexturePool(),
                vulkanApp->GetAppMaterialPool().size());

            modelTr = glm::scale(modelTr, glm::vec3(0.3));
            modelTr = glm::rotate(modelTr, 0.5f, glm::vec3(0, 1, 0));
            modelTr = glm::translate(modelTr, glm::vec3(0.0f, 2.0f, 0.0f));

            
            ReadAssimpFile("../assets/models/simpleShapesIco.obj",
                modelTr,
                modelDatas,
                vulkanApp->GetAppMaterialPool(),
                vulkanApp->GetAppTexturePool(),
                vulkanApp->GetAppMaterialPool().size());
            

            //ModelData sponza;
            ReadAssimpFile("../assets/models/sponza.obj",
            
                glm::mat4(0.01),
                modelDatas,
                vulkanApp->GetAppMaterialPool(),
                vulkanApp->GetAppTexturePool(),
                vulkanApp->GetAppMaterialPool().size());
 
            for (int i = 0; i < modelDatas.size(); ++i)
            {
                vulkanApp->LoadModel(modelDatas[i], glm::mat4(1.0f));
            }

            // Lights
            vulkanApp->AppendLight({ glm::vec3(-1.0, 2.0, 0.0), glm::vec3(1.0,0.0,0.0), 2.0, 1.0 });
            vulkanApp->AppendLight({ glm::vec3(0.0, 2.0, 0.0), glm::vec3(0.0,1.0,0.0), 2.0, 1.0 });
            vulkanApp->AppendLight({ glm::vec3(1.0, 2.0, 0.0), glm::vec3(0.0,1.0,0.0), 2.0, 1.0 });
            
            vulkanApp->BindResources();
        }

        ////////////////////////////////////////
        //          Game Object Control
        ////////////////////////////////////////
        //vulkanApp->SetEyePosition(0.0, 3.0, 2.0);

        // set binding to action
        // CreateAction 
        // action call camera

	}

	void GraphicsSystem::Update()
	{
        ////////////////////////////////////////
        //          Vulkan Main Program
        ////////////////////////////////////////
        vulkanApp->UpdateCamera();

        vulkanApp->ReinkanUpdate();
	}
	
	void GraphicsSystem::Destroy()
	{
        vulkanApp->Cleanup();
	}

    ReinkanApp* GraphicsSystem::GetVulkanApp() const
    {
        return vulkanApp.get();
    }
}

