#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif
#include <SDL3/SDL.h>
#include <glm/glm.hpp>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <cstdint>

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
        SDL_Window* window{};
        vk::raii::Context context;
        vk::raii::Instance instance = nullptr;

        void initWindow()
        {
                constexpr uint32_t WIDTH{800};
                constexpr uint32_t HEIGHT{600};
                constexpr char TITLE[]{"My Vulkan window"};

                SDL_Init(SDL_INIT_VIDEO);

                window = SDL_CreateWindow(TITLE, WIDTH, HEIGHT, SDL_WINDOW_VULKAN);


        }


        void createInstance()
        {
                constexpr vk::ApplicationInfo appInfo{  .pApplicationName       = "Hello Triangle",
                                                        .applicationVersion     = VK_MAKE_VERSION(0, 1, 0),
                                                        .pEngineName            = "No Engine",
                                                        .engineVersion          = VK_MAKE_VERSION(0, 1, 0),
                                                        .apiVersion             = vk::ApiVersion14                             
                };
                vk::InstanceCreateInfo instanceInfo{    .pApplicationInfo = &appInfo};
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
        try {
                RenderingClass renderer{};
                renderer.runRenderer();
        }
        catch (const std::exception& e)
        {
                std::cerr << e.what() << '\n';
                return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
}