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
            glm::mat4 modelTr = glm::mat4(1.0f);
            /*modelTr = glm::scale(modelTr, glm::vec3(0.3));
            modelTr = glm::translate(modelTr, glm::vec3(18.0f, 18.0f, 1.5f));
            modelTr = glm::rotate(modelTr, glm::radians(90.0f), glm::vec3(0,0,1));*/
            ReadAssimpFile("../assets/models/wheel.obj",
                modelTr,
                modelDatas,
                vulkanApp->GetAppMaterialPool(),
                vulkanApp->GetAppTexturePool(),
                vulkanApp->GetAppMaterialPool().size());

            modelTr = glm::mat4(1);
            modelTr = glm::scale(modelTr, glm::vec3(2.0f));
            ReadAssimpFile("../assets/models/plane.obj",
                modelTr,
                modelDatas,
                vulkanApp->GetAppMaterialPool(),
                vulkanApp->GetAppTexturePool(),
                vulkanApp->GetAppMaterialPool().size());

            // ----------- SKY SPHERE -----------
            modelTr = glm::mat4(1);
            modelTr = glm::scale(modelTr, glm::vec3(-700.0f));
            //modelTr = glm::rotate(modelTr, 0.5f, glm::vec3(0, 1, 0)); 
            modelTr = glm::translate(modelTr, glm::vec3(0.0f, 0.0f, 0.0f));
            ReadAssimpFile("../assets/models/smoothSphere.obj",
                modelTr,
                modelDatas,
                vulkanApp->GetAppMaterialPool(),
                vulkanApp->GetAppTexturePool(),
                vulkanApp->GetAppMaterialPool().size());

            modelTr = glm::mat4(1);
            modelTr = glm::scale(modelTr, glm::vec3(0.3f));
            modelTr = glm::rotate(modelTr, 0.5f, glm::vec3(0, 1, 0));
            modelTr = glm::translate(modelTr, glm::vec3(0.0f, 1.5f, 0.0f));
            ReadAssimpFile("../assets/models/smoothSphere.obj",
                modelTr,
                modelDatas,
                vulkanApp->GetAppMaterialPool(),
                vulkanApp->GetAppTexturePool(),
                vulkanApp->GetAppMaterialPool().size());
            
            modelTr = glm::mat4(1);
            modelTr = glm::scale(modelTr, glm::vec3(0.3f));
            modelTr = glm::rotate(modelTr, 0.0f, glm::vec3(0, 1, 0));
            modelTr = glm::translate(modelTr, glm::vec3(3.0f, 1.5f, 0.0f));
            ReadAssimpFile("../assets/models/halfShinySphere.obj",
                modelTr,
                modelDatas,
                vulkanApp->GetAppMaterialPool(),
                vulkanApp->GetAppTexturePool(),
                vulkanApp->GetAppMaterialPool().size());

            modelTr = glm::mat4(1);
            modelTr = glm::scale(modelTr, glm::vec3(0.3f));
            modelTr = glm::rotate(modelTr, 0.0f, glm::vec3(0, 1, 0));
            modelTr = glm::translate(modelTr, glm::vec3(6.0f, 1.5f, 0.0f));
            ReadAssimpFile("../assets/models/shinySphere.obj",
                modelTr,
                modelDatas,
                vulkanApp->GetAppMaterialPool(),
                vulkanApp->GetAppTexturePool(),
                vulkanApp->GetAppMaterialPool().size());

            modelTr = glm::mat4(1);
            modelTr = glm::scale(modelTr, glm::vec3(0.3f));
            modelTr = glm::rotate(modelTr, 0.0f, glm::vec3(0, 1, 0));
            modelTr = glm::translate(modelTr, glm::vec3(-3.0f, 1.5f, 0.0f));
            ReadAssimpFile("../assets/models/metalSphere.obj",
                modelTr,
                modelDatas,
                vulkanApp->GetAppMaterialPool(),
                vulkanApp->GetAppTexturePool(),
                vulkanApp->GetAppMaterialPool().size());
            
            modelTr = glm::mat4(1);
            modelTr = glm::scale(modelTr, glm::vec3(0.3f));
            modelTr = glm::rotate(modelTr, 0.0f, glm::vec3(0, 1, 0));
            modelTr = glm::translate(modelTr, glm::vec3(-6.0f, 1.5f, 0.0f));
            ReadAssimpFile("../assets/models/torus.obj",
                modelTr,
                modelDatas,
                vulkanApp->GetAppMaterialPool(),
                vulkanApp->GetAppTexturePool(),
                vulkanApp->GetAppMaterialPool().size());

            /*
            ReadAssimpFile("../assets/models/simpleShapesIco.obj",
                modelTr,
                modelDatas,
                vulkanApp->GetAppMaterialPool(),
                vulkanApp->GetAppTexturePool(),
                vulkanApp->GetAppMaterialPool().size());
            */
            

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
            //vulkanApp->AppendLight({ glm::vec3(-1.0, 2.0, 1.5), glm::vec3(1.0,0.0,0.0), 1.0, 1.0 });
            //vulkanApp->AppendLight({ glm::vec3(0.0, 2.0, 1.5), glm::vec3(0.0,1.0,0.0), 1.0, 1.0 });
            //vulkanApp->AppendLight({ glm::vec3(1.0, 2.0, 1.5), glm::vec3(0.0,1.0,0.0), 1.0, 1.0 });

            //vulkanApp->AppendLight({ glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0,0.9,1.0), 1.0, 1.0 });
            
            float space = 0.25f;

            int width = 2;
            
            float colorOffset = (1.0f / width);

            glm::vec3 positionOffset{ -width * space / 2.0f, 0.1f, -width * space / 2.0f};

            for (int i = 0; i < width; ++i)
            {
                for (int j = 0; j < width; ++j)
                {
                    float randOffsetX = (static_cast<float>(std::rand()) / _I16_MAX - 0.5f) * space * 2.0f;
                    float randOffsetZ = (static_cast<float>(std::rand()) / _I16_MAX - 0.5f) * space * 2.0f;
                    //randOffsetX = 0;
                    //randOffsetZ = 0;

                    glm::vec3 position{ i * space + randOffsetX, 0.0f, j * space + randOffsetZ };

                    glm::vec3 color{ 1.0, 1.0, 0.5 };
                    color.r -= i * colorOffset;
                    color.g -= j * colorOffset;

                    float intensity = 0.05f;
                    float radius = 0.5f;

                    vulkanApp->AppendLight({ positionOffset + position, color, intensity, radius });
                }
            }

            // Light Meshes
            std::vector<ModelData>  lightMeshModelDatas;
            std::vector<Material>   lightMeshesMaterialPoolDummy;
            std::vector<std::string>     lightMeshesTexturePoolDummy;

            ReadAssimpFile("../assets/models/sphereico.obj",
                            glm::mat4(1),
                            lightMeshModelDatas,
                            lightMeshesMaterialPoolDummy,
                            lightMeshesTexturePoolDummy,
                            0);
            
            for (auto lightMeshModelData : lightMeshModelDatas)
            {
                vulkanApp->AppendLightMesh(lightMeshModelData);
            }

            // HDR Image
            vulkanApp->AddHDRImagePath("../assets/hdrTextures/NatureLabFront_IBL_Ref");
            vulkanApp->AddHDRImagePath("../assets/hdrTextures/Alexs_Apt_2k");

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

