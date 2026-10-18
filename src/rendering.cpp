// #define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS

#include "rendering.h"

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <glm/glm.hpp>

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <ranges>
#include <string_view>

static SDL_Window* currentWindow {nullptr};
static VkInstance instance {nullptr};

static std::vector<const char*> VALIDATION_LAYER_NAME {"VK_LAYER_KHRONOS_validation"};

void isErrorVkResultPrint(std::string_view flavText, VkResult resultOperation);
void initWindow();
void initVulkan();
void createInstance();
void rendererLoop();
void rendererCleanup();

void isErrorVkResultPrint(std::string_view flavText, VkResult resultOperation)
{

        if ((resultOperation == VK_SUCCESS) || (resultOperation == VK_INCOMPLETE))
        {
                std::cout << flavText << string_VkResult(resultOperation) << '\n';
        }
        else
        {
                throw std::runtime_error(std::string(flavText) + '\n' + string_VkResult(resultOperation));
        }
}

void createInstance()
{
        VkApplicationInfo applicationInfo{};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.apiVersion = VK_API_VERSION_1_4;
        applicationInfo.pApplicationName = "My Vulkan";
        applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.pEngineName = "Astar Field";
        applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

        uint32_t layerCount{0};
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        VkLayerProperties layerPropertiesList[]{};
        VkResult isEnumerateLayers {vkEnumerateInstanceLayerProperties(&layerCount, layerPropertiesList)};
        isErrorVkResultPrint("Instance Layer Enumeration: ", isEnumerateLayers);

        std::cout << layerPropertiesList[1].layerName << '\n';

        std::vector<const char*> supportedLayer{};

        // Checks for requested layers if it's supported through simple for loops
        for (const char* validationLayer : VALIDATION_LAYER_NAME)
        {
                bool isFoundLayer{false};

                for (int i{0}; i < layerCount; ++i)
                {
                        const auto layerProperties{layerPropertiesList[i]};

                        if (strcmp(validationLayer, layerProperties.layerName) == 0)
                        {
                                isFoundLayer = true;
                        }
                        
                        supportedLayer.push_back(layerProperties.layerName);
                }
                
                if (!isFoundLayer)
                {
                        throw std::runtime_error("Validation Layer Not Found");
                }
        }

        // Compare if the required instance extensions is unsupported
        uint32_t extensionSDLCount{};
        auto extensions {SDL_Vulkan_GetInstanceExtensions(&extensionSDLCount)};
        if (extensions == nullptr) throw std::runtime_error("SDL_Vulkan_GetInstanceExtensions failed to get extensions");

        uint32_t extensionVkCount{};
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionVkCount, nullptr);

        uint32_t extensionCount {extensionSDLCount + extensionVkCount};

        VkExtensionProperties extensionPropertiesList[VK_MAX_EXTENSION_NAME_SIZE]{};
        VkResult isEnumerateExtensions {vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionPropertiesList)};
        isErrorVkResultPrint("Instance Extension Enumeration: ", isEnumerateExtensions);

        for (int i{0}; i < extensionCount; ++i)
        {
                const auto& extensionProperties {extensionPropertiesList[i]};
                const auto& SDLExtension {extensions[i]};

                if (!(strcmp(extensionProperties.extensionName, SDLExtension) == 0))
                {
                     std::cout << "Unsupported extension: " << extensionProperties.extensionName << '\n';   
                }
        }

        VkInstanceCreateInfo instanceInfo{};
        instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceInfo.pApplicationInfo = &applicationInfo; 
        instanceInfo.ppEnabledExtensionNames = extensions;

        VkResult isInstanceCreation = vkCreateInstance(&instanceInfo, nullptr, &instance);
        isErrorVkResultPrint("Instance Creation: ", isInstanceCreation);
}

void renderingMain()
{
        initWindow();
        initVulkan();
        rendererLoop();
        rendererCleanup();
}

void initWindow()
{
        const char* TITLE{"So, my window"};
        constexpr int WIDTH{800};
        constexpr int HEIGHT{800};

        SDL_Init(SDL_INIT_VIDEO);

        currentWindow = SDL_CreateWindow(TITLE, WIDTH, HEIGHT, SDL_WINDOW_VULKAN);
        
}

void initVulkan() 
{
        createInstance();
}

void rendererLoop()
{
        bool isRunning{true};
        while (isRunning)
        {
                SDL_Event event;
                while (SDL_PollEvent(&event))
                {
                        switch (event.type)
                        {
                                case SDL_EVENT_QUIT:
                                isRunning = false;
                                break;

                        }

                        switch(event.key.key)
                        {
                                case SDLK_ESCAPE:
                                isRunning = false;
                                break;
                        }
                }
        }
}

void rendererCleanup()
{
        vkDestroyInstance(instance, nullptr);

        SDL_DestroyWindow(currentWindow);
        currentWindow = nullptr;
        SDL_Quit();
}
