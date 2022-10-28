#include <SDL2/SDL.h>

class EventManager
{
	private:
        SDL_Event event;
        bool status[5];     // [close, up, left, right, down, right]

	public:
		bool* check();
        EventManager();
        ~EventManager();
};
