// #define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS

#include "rendering.h"

#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>

#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

static SDL_Window* currentWindow {nullptr};
static VkInstance* instance {nullptr};

void initWindow();
void initVulkan();
void createInstance();
void rendererLoop();
void rendererCleanup();

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
        const int WIDTH{800};
        const int HEIGHT{800};

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
        SDL_DestroyWindow(currentWindow);
        currentWindow = nullptr;
        SDL_Quit();
}


void createInstance()
{
        constexpr VkApplicationInfo applicationInfo{
                .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                .pNext = nullptr,
                .pApplicationName = "My Vulkan",
                .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
                .pEngineName = "Astar Field",
                .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                .apiVersion = VK_API_VERSION_1_4
        };

        VkInstanceCreateInfo instanceInfo{
                .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                .pNext = nullptr,
                .flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
                .pApplicationInfo = &applicationInfo
        };

        VkResult instanceCreationResult = vkCreateInstance(&instanceInfo, nullptr, instance);

        if (instanceCreationResult != VK_SUCCESS)
        {
                throw std::runtime_error(std::string("Instance Create Failed\n") + std::to_string(instanceCreationResult));
        }
}