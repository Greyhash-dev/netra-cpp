#include <SDL2/SDL.h>

class EventManager
{
	private:
        SDL_Event event;
        bool status[6] = {0,0,0,0,0,0};     // [close, up, left, right, down, right, space]

	public:
		bool* check();
        EventManager();
        ~EventManager();
};
