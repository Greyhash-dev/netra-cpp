#include <bits/stdc++.h>
#include <SDL2/SDL.h>

class EventManager
{
	private:
        SDL_Event event;
        bool status[6] = {0,0,0,0,0,0};

	public:
        const std::array<std::string, 6> labels = {"Close", "Up", "Left", "Down", "Right", "Space"};
		bool* check();
        EventManager();
        ~EventManager();
};
