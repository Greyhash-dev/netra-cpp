#include "events.h"

bool* EventManager::check()
{
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                status[0] = 1;
                break;

            case SDL_KEYDOWN:
            case SDL_KEYUP:
                switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_W:
                    case SDL_SCANCODE_UP:
                        status[1] = (event.type == SDL_KEYDOWN);
                        break;
                    case SDL_SCANCODE_A:
                    case SDL_SCANCODE_LEFT:
                        status[2] = (event.type == SDL_KEYDOWN);
                        break;
                    case SDL_SCANCODE_S:
                    case SDL_SCANCODE_DOWN:
                        status[3] = (event.type == SDL_KEYDOWN);
                        break;
                    case SDL_SCANCODE_D:
                    case SDL_SCANCODE_RIGHT:
                        status[4] = (event.type == SDL_KEYDOWN);
                        break;
                    case SDL_SCANCODE_SPACE:
                        status[5] = (event.type == SDL_KEYDOWN);
                    default:
                        break;
                }
        }
    }
    return status;

}

EventManager::EventManager()
{

}

EventManager::~EventManager()
{

}
