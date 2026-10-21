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

std::vector<const char*> LAYER_NAMES{"VK_LAYER_KHRONOS_validation"};

void isErrorVkResultPrint(std::string_view flavText, const VkResult* resultOperation);
void initWindow();
void initVulkan();
void createInstance();
void rendererLoop();
void rendererCleanup();

void isErrorVkResultPrint(std::string_view flavText, const VkResult* resultOperation)
{

        if ((*resultOperation == VK_SUCCESS) || (*resultOperation == VK_INCOMPLETE))
        {
                std::cout << flavText << string_VkResult(*resultOperation) << '\n';
        }
        else
        {
                throw std::runtime_error(std::string(flavText) + '\n' + string_VkResult(*resultOperation));
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

        // Acquire required instance layer
        uint32_t vulkanLayerPropertiesCount{};
        VkResult isLayerCounted {vkEnumerateInstanceLayerProperties(&vulkanLayerPropertiesCount, nullptr)};
        isErrorVkResultPrint("Instance Layer Counted: ", &isLayerCounted);

        VkLayerProperties layerPropertiesList[vulkanLayerPropertiesCount]{};
        VkResult isLayerEnumerated {vkEnumerateInstanceLayerProperties(&vulkanLayerPropertiesCount, layerPropertiesList)};
        isErrorVkResultPrint("Instance Layer Enumerated: ", &isLayerEnumerated);
        
        std::vector<const char*> supportedLayer{};
        for (const char* layerName : LAYER_NAMES)
        {
                for (int i{0}; i < vulkanLayerPropertiesCount; ++i)
                {
                        VkLayerProperties layerProperties {layerPropertiesList[i]};
                        if (strcmp(layerName, layerProperties.layerName) == 0)
                        {
                                supportedLayer.push_back(layerProperties.layerName);
                        }
                        else
                        {
                                std::cout << "Unsupported instance layer: " << layerProperties.layerName << '\n';
                        }
                }
        }

        uint32_t extensionSDLCount {};
        auto SDLRequiredExtensions {SDL_Vulkan_GetInstanceExtensions(&extensionSDLCount)};
        if (SDLRequiredExtensions == nullptr) throw std::runtime_error("Failed to acquire SDL Required Extension");

        /*
        TODO:
        Implement flexible way to get custom extensions may be needed, incorporating into the vkEnumerateInstanceExtensionProperties pLayerName
        Along with adding flexible way, probably also need to merge the count, but that doesn't matter
        */

        uint32_t vulkanExtensionPropertiesCount {};
        VkResult isCountedExtensionProperties {vkEnumerateInstanceExtensionProperties(nullptr, &vulkanExtensionPropertiesCount, nullptr)};
        isErrorVkResultPrint("Instance Extension Properties Counted: ", &isCountedExtensionProperties);

        VkExtensionProperties extensionPropertiesList[vulkanExtensionPropertiesCount]{};
        VkResult isEnumerateExtension {vkEnumerateInstanceExtensionProperties(nullptr, &vulkanExtensionPropertiesCount, extensionPropertiesList)};
        isErrorVkResultPrint("Instance Extension Enumerated: ", &isEnumerateExtension);

        std::vector<const char*> supportedExtension{};
        for (int i{0}; i < extensionSDLCount; ++i)
        {
                auto requiredExtension {SDLRequiredExtensions[i]};
                for (int j{0}; j < vulkanExtensionPropertiesCount; ++j)
                {
                        VkExtensionProperties extensionProperties {extensionPropertiesList[j]};
                        if (strcmp(requiredExtension, extensionProperties.extensionName) == 0)
                        {
                                supportedExtension.push_back(extensionProperties.extensionName);
                        }
                        else
                        {
                                std::cout << "Unsupported instance extension: " << extensionProperties.extensionName << '\n';
                        }
                }
        }
        std::cout << "Successfully enumerate extensions\n";

        VkInstanceCreateInfo instanceInfo{};
        instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceInfo.pApplicationInfo = &applicationInfo;
        instanceInfo.enabledLayerCount = static_cast<uint32_t> (supportedLayer.size());
        instanceInfo.ppEnabledLayerNames = supportedLayer.data();
        instanceInfo.enabledExtensionCount = static_cast<uint32_t> (supportedExtension.size());
        instanceInfo.ppEnabledExtensionNames = supportedExtension.data();

        VkResult isInstanceCreation = vkCreateInstance(&instanceInfo, nullptr, &instance);
        isErrorVkResultPrint("Instance Creation: ", &isInstanceCreation);
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
