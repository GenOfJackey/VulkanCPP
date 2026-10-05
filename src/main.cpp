#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif

#ifdef NDEBUG
constexpr bool VALIDATION_LAYER {false};
#else
constexpr bool VALIDATION_LAYER {true};
#endif

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include <stdexcept>
#include <ranges>
#include <cstdlib>
#include <cstdint>

const std::vector<char const *> validationLayers{
    "VK_LAYER_KHRONOS_validation"};

class RenderingClass
{
public:
        void runRenderer()
        {
                initWindow();
                initVulkan();
                rendererLoop();
                renderingCleanup();
        }

private:
        SDL_Window *window{};
        vk::raii::Context context;
        vk::raii::Instance instance = nullptr;

        std::vector<const char *> getRequiredInstanceExtension()
        {
                uint32_t SDLExtensionCount{0};
                auto SDLExtensions{SDL_Vulkan_GetInstanceExtensions(&SDLExtensionCount)};

                std::vector extensions(SDLExtensions, SDLExtensions + SDLExtensionCount);
                return extensions;
        }

        void createInstance()
        {
                // Checks every available extensions
                auto availableExtensions{context.enumerateInstanceExtensionProperties()};
                for (auto ext : availableExtensions)
                {
                        std::cout << "Extension available: " << ext.extensionName
                                  << "\t Extension version: " << ext.specVersion << '\n';
                }

                // Checking if the required extensions are available and
                // supported by Vulkan implementation
                auto requiredExtensions{getRequiredInstanceExtension()};

                auto extensionProperties{context.enumerateInstanceExtensionProperties()};
                auto unsupportedExtension{std::ranges::find_if(
                    requiredExtensions,
                    [&extensionProperties](auto const requiredExtension)
                    {
                            return std::ranges::none_of(
                                extensionProperties,
                                [requiredExtension](auto const &extensionProperty)
                                {
                                        return strcmp(extensionProperty.extensionName, requiredExtension) == 0;
                                });
                    })
                };
                if (unsupportedExtension != requiredExtensions.end())
                {
                        throw std::runtime_error("Required extension are not supported: " + std::string(*unsupportedExtension));
                }

                // Checking if all the requested layers are available and
                // is accepted by Vulkan implementation
                std::vector<const char *> requiredLayers;
                if (VALIDATION_LAYER) requiredLayers.assign(validationLayers.begin(), validationLayers.end());

                auto layerProperties{context.enumerateInstanceLayerProperties()};
                auto unsupportedLayer{
                    std::ranges::find_if(
                        requiredLayers,
                        [&layerProperties](auto const &requiredLayer)
                        {
                                return std::ranges::none_of(
                                        layerProperties,
                                        [requiredLayer](auto const &layerProperty)
                                        { 
                                                return strcmp(layerProperty.layerName, requiredLayer) == 0; 
                                        });
                        })
                };
                if (unsupportedLayer != requiredLayers.end())
                {
                        throw std::runtime_error("Required layer are not supported: " + std::string(*unsupportedLayer));
                }

                // std::vector<char const *> requiredLayers;
                // requiredLayers.assign(validationLayers.begin(), validationLayers.end());

                // auto layerProperties = context.enumerateInstanceLayerProperties();
                // auto unsupportedLayerIt = std::ranges::find_if(requiredLayers,
                //                                                [&layerProperties](auto const &requiredLayer)
                //                                                {
                //                                                        return std::ranges::none_of(layerProperties,
                //                                                                                    [requiredLayer](auto const &layerProperty)
                //                                                                                    { return strcmp(layerProperty.layerName, requiredLayer) == 0; });
                //                                                });
                // if (unsupportedLayerIt != requiredLayers.end())
                // {
                //         throw std::runtime_error("Required layer not supported: " + std::string(*unsupportedLayerIt));
                // }

                constexpr vk::ApplicationInfo appInfo{.pApplicationName = "Hello Triangle",
                                                      .applicationVersion = VK_MAKE_VERSION(0, 1, 0),
                                                      .pEngineName = "No Engine",
                                                      .engineVersion = VK_MAKE_VERSION(0, 1, 0),
                                                      .apiVersion = vk::ApiVersion14};

                // uint32_t sdlExtensionCount{0};
                // auto extensions{SDL_Vulkan_GetInstanceExtensions(&sdlExtensionCount)};

                // Checking if the extension are supported by Vulkan implementation
                // auto SDLExtensionProperties{context.enumerateInstanceExtensionProperties()};
                // for (uint32_t i{0}; i < sdlExtensionCount; ++i)
                // {
                //         if (std::ranges::none_of(
                //                 SDLExtensionProperties,
                //                 [extension = extensions[i]](auto const &extensionProperty)
                //                 { return strcmp(extensionProperty.extensionName, extension) == 0; }))
                //         {
                //                 throw std::runtime_error("Required SDL extension are not supported : " + std::string(extensions[i]));
                //         }
                // }

                vk::InstanceCreateInfo createInfo{
                        .pApplicationInfo = &appInfo,
                        .enabledLayerCount = static_cast<uint32_t> (requiredLayers.size()),
                        .ppEnabledLayerNames = requiredLayers.data(),
                        .enabledExtensionCount = static_cast<uint32_t> (requiredExtensions.size()),
                        .ppEnabledExtensionNames = requiredExtensions.data()
                };

                instance = vk::raii::Instance(context, createInfo);
        }

        void initWindow()
        {
                constexpr uint32_t WIDTH{800};
                constexpr uint32_t HEIGHT{600};
                constexpr char TITLE[]{"My Vulkan window"};

                SDL_Init(SDL_INIT_VIDEO);

                window = SDL_CreateWindow(TITLE, WIDTH, HEIGHT, SDL_WINDOW_VULKAN);
        }

        void initVulkan()
        {
                createInstance();
        }

        void rendererLoop()
        {
                bool loopDone{false};
                while (!loopDone)
                {
                        SDL_Event sdlEvent;
                        while (SDL_PollEvent(&sdlEvent))
                        {
                                switch (sdlEvent.type)
                                {
                                case SDL_EVENT_QUIT:
                                        loopDone = true;
                                        break;
                                }

                                switch (sdlEvent.key.key)
                                {
                                case SDLK_ESCAPE:
                                        loopDone = true;
                                        break;
                                }
                        }
                }
        }

        void renderingCleanup()
        {
                SDL_DestroyWindow(window);
                SDL_Quit();
        }
};

int main()
{
        try
        {
                RenderingClass renderer{};
                renderer.runRenderer();
        }
        catch (const std::exception &e)
        {
                std::cerr << e.what() << '\n';
                return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
}