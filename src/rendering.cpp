// #define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#define GLFW_INCLUDE_VULKAN

#include "rendering.h"

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
// #include <GLFW/glfw3.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <glm/glm.hpp>

#include <iostream>
#include <stdexcept>
#include <cstring>
#include <vector>
#include <array>
#include <ranges>
#include <string_view>

static SDL_Window *currentWindow{nullptr};
static VkInstance instance{nullptr};
static VkDevice device{nullptr};

void isErrorVkResultPrint(std::string_view flavText, const VkResult *resultOperation);
const std::vector<const char *> getInstanceLayer();
const std::vector<const char *> getInstanceExtension();
void initWindow();
void initVulkan();
void createInstance();
void createDevice();
void rendererLoop();
void rendererCleanup();

void isErrorVkResultPrint(std::string_view flavText, const VkResult *resultOperation)
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

const std::vector<const char *> getInstanceLayer()
{
        const std::vector<const char *> LAYER_NAMES{"VK_LAYER_KHRONOS_validation"};

        uint32_t vulkanLayerPropertiesCount{};
        VkResult isLayerCounted{vkEnumerateInstanceLayerProperties(&vulkanLayerPropertiesCount, nullptr)};
        isErrorVkResultPrint("Instance Layer Counted: ", &isLayerCounted);

        std::vector<VkLayerProperties> layerPropertiesList{};
        layerPropertiesList.resize(vulkanLayerPropertiesCount);
        VkResult isLayerEnumerated{vkEnumerateInstanceLayerProperties(&vulkanLayerPropertiesCount, layerPropertiesList.data())};
        isErrorVkResultPrint("Instance Layer Enumerated: ", &isLayerEnumerated);

        std::vector<const char *> supportedLayer{};
        for (const char *layerName : LAYER_NAMES)
        {
                bool isFound{false};
                for (VkLayerProperties layerProperties : layerPropertiesList)
                {
                        // VkLayerProperties layerProperties {layerPropertiesList[i]};
                        if (strcmp(layerName, layerProperties.layerName) == 0)
                        {
                                supportedLayer.push_back(layerProperties.layerName);
                                isFound = true;
                                break;
                        }
                }

                if (!isFound)
                {
                        std::cout << "Unsupported Required Instance Layer: " << layerName << '\n';
                }
        }
        std::cout << "Successfully enumerate layer\n";

        return supportedLayer;
}

const std::vector<const char *> getInstanceExtension()
{
        uint32_t requiredExtensionCount{};
        auto requiredExtensions{SDL_Vulkan_GetInstanceExtensions(&requiredExtensionCount)};
        if (requiredExtensions == nullptr)
                throw std::runtime_error("Failed to acquire SDL Required Extension");

        // std::vector<const char*> requiredExtensions{"VK_EXT_debug_report"};
        // for (int i{0}; i < extensionSDLCount; ++i)
        // {
        //         const char* extensionName {SDLRequiredExtensions[i]};
        //         requiredExtensions.push_back(extensionName);
        // }

        /*
        TODO:
        Implement flexible way to get custom extensions may be needed
        Along with adding flexible way, probably also need to merge the count, but that doesn't matter
        */

        uint32_t vulkanExtensionPropertiesCount{};
        VkResult isCountedExtensionProperties{vkEnumerateInstanceExtensionProperties(nullptr, &vulkanExtensionPropertiesCount, nullptr)};
        isErrorVkResultPrint("Instance Extension Properties Counted: ", &isCountedExtensionProperties);

        std::vector<VkExtensionProperties> extensionPropertiesList{};
        VkResult isEnumerateExtension{vkEnumerateInstanceExtensionProperties(nullptr, &vulkanExtensionPropertiesCount, extensionPropertiesList.data())};
        isErrorVkResultPrint("Instance Extension Enumerated: ", &isEnumerateExtension);

        // std::cout << extensionPropertiesList.size() << '\n';
        std::vector<const char *> supportedExtension{};
        for (int i{0}; i < requiredExtensionCount; ++i)
        {
                auto requiredExtension{requiredExtensions[i]};
                bool isFound{false};
                for (VkExtensionProperties extensionProperties : extensionPropertiesList)
                {
                        // VkExtensionProperties extensionProperties {extensionPropertiesList[j]};
                        if (strcmp(requiredExtension, extensionProperties.extensionName) == 0)
                        {
                                supportedExtension.push_back(extensionProperties.extensionName);
                                isFound = true;
                                break;
                        }
                }

                if (!isFound)
                {
                        std::cout << "Unsupported Required Instance Extension: " << requiredExtension << '\n';
                }
        }
        std::cout << "Successfully enumerate extensions\n";

        return supportedExtension;
}

void createInstance()
{
        VkApplicationInfo applicationInfo{};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.apiVersion = VK_API_VERSION_1_3;
        applicationInfo.pApplicationName = "My Vulkan";
        applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.pEngineName = "Astar Field";
        applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

        std::vector<const char *> supportedLayer{getInstanceLayer()};
        std::vector<const char *> supportedExtension{getInstanceExtension()};

        VkInstanceCreateInfo instanceInfo{};
        instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceInfo.pApplicationInfo = &applicationInfo;
        instanceInfo.enabledLayerCount = static_cast<uint32_t>(supportedLayer.size());
        instanceInfo.ppEnabledLayerNames = supportedLayer.data();
        instanceInfo.enabledExtensionCount = static_cast<uint32_t>(supportedExtension.size());
        instanceInfo.ppEnabledExtensionNames = supportedExtension.data();

        VkResult isInstanceCreation = vkCreateInstance(&instanceInfo, nullptr, &instance);
        isErrorVkResultPrint("Instance Creation: ", &isInstanceCreation);
}

void createDevice()
{
        VkPhysicalDevice physicalDevice{};

        VkDeviceCreateInfo deviceInfo{};

        vkCreateDevice(physicalDevice, &deviceInfo, nullptr, &device);
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
        const char *TITLE{"So, my window"};
        constexpr int WIDTH{800};
        constexpr int HEIGHT{800};

        bool isInit{SDL_Init(SDL_INIT_VIDEO)};
        if (!isInit)
                throw std::runtime_error("Failed to initialize SDL Video");

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
                SDL_Event event{};
                while (SDL_PollEvent(&event))
                {
                        switch (event.type)
                        {
                        case SDL_EVENT_QUIT:
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
