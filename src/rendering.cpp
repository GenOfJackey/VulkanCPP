// #define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
// #define GLFW_INCLUDE_VULKAN

#include "rendering.h"

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
// #include <GLFW/glfw3.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <glm/glm.hpp>

#include <iostream>
#include <stdexcept>
#include <vector>
#include <string_view>

static SDL_Window *currentWindow{nullptr};
static VkInstance instance{nullptr};
static VkPhysicalDevice currentPhysicalDevice{nullptr};

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
                                supportedLayer.push_back(layerName);
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
        uint32_t requiredSDLExtensionCount{};
        auto requiredSDLExtensions{SDL_Vulkan_GetInstanceExtensions(&requiredSDLExtensionCount)};
        if (requiredSDLExtensions == nullptr)
                throw std::runtime_error("Failed to acquire SDL Required Extension");

        std::vector<const char *> requiredExtensions{};
        for (int i{0}; i < requiredSDLExtensionCount; ++i)
        {
                requiredExtensions.push_back(requiredSDLExtensions[i]);
        }

        // std::vector<const char*> requiredExtensions{"VK_EXT_debug_report"};
        // for (int i{0}; i < extensionSDLCount; ++i)
        // {
        //         const char* extensionName {SDLRequiredExtensions[i]};
        //         requiredExtensions.push_back(extensionName);
        // }

        uint32_t vulkanExtensionPropertiesCount{};
        VkResult isCountedExtensionProperties{vkEnumerateInstanceExtensionProperties(nullptr, &vulkanExtensionPropertiesCount, nullptr)};
        isErrorVkResultPrint("Instance Extension Properties Counted: ", &isCountedExtensionProperties);

        std::vector<VkExtensionProperties> extensionPropertiesList{};
        extensionPropertiesList.resize(vulkanExtensionPropertiesCount);
        VkResult isEnumerateExtension{vkEnumerateInstanceExtensionProperties(nullptr, &vulkanExtensionPropertiesCount, extensionPropertiesList.data())};
        isErrorVkResultPrint("Instance Extension Enumerated: ", &isEnumerateExtension);

        // std::cout << extensionPropertiesList.size() << '\n';
        std::vector<const char *> supportedExtension{};
        for (const char *requiredExtension : requiredExtensions)
        {
                bool isFound{false};
                for (VkExtensionProperties extensionProperties : extensionPropertiesList)
                {
                        // VkExtensionProperties extensionProperties {extensionPropertiesList[j]};
                        if (strcmp(requiredExtension, extensionProperties.extensionName) == 0)
                        {
                                supportedExtension.push_back(requiredExtension);
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
        // Select the best physical device required for graphics
        uint32_t physicalDeviceCount{};
        VkResult isPhysicalDeviceCounted{vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr)};
        isErrorVkResultPrint("phyDevice Counted: ", &isPhysicalDeviceCounted);

        std::vector<VkPhysicalDevice> physicalDeviceList{};
        VkResult isPhysicalDeviceEnumerated{vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDeviceList.data())};
        isErrorVkResultPrint("phyDevice Enumerated: ", &isPhysicalDeviceEnumerated);

        /*
        TODO:
        Finish the selection ranking for the best Physical Device available for GPU computing, refer to
        Vulkan Tutorial for important features since I don't know stuff
        */
        for (VkPhysicalDevice physicalDevice : physicalDeviceList)
        {
                VkPhysicalDeviceProperties2 physicalDeviceProperties{};
                physicalDeviceProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
                vkGetPhysicalDeviceProperties2(physicalDevice, &physicalDeviceProperties);

                VkPhysicalDeviceFeatures2 physicalDeviceFeatures{};
                physicalDeviceFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
                vkGetPhysicalDeviceFeatures2(physicalDevice, &physicalDeviceFeatures);

                uint32_t score{0};

                if (physicalDeviceProperties.properties.apiVersion < VK_API_VERSION_1_3)
                        continue;

                if (physicalDeviceFeatures.features.geometryShader != VK_TRUE)
                        continue;
                switch (physicalDeviceProperties.properties.deviceType)
                {
                case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                        score += 100;
                        break;

                case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                        score += 50;
                        break;

                case VK_PHYSICAL_DEVICE_TYPE_CPU:
                        score += 20;
                        break;

                default:
                        continue;
                }
        }
/*
TODO:
- Get Queue family for Graphics
hypothesis: The queue family is for the chosen current physical device, not for other physical devices.

- Implement pNext dynamic rendering for physical devices where it could propagate, refer to
Vulkan reference to know about the pNext functionality.
*/
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
        createDevice();
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
