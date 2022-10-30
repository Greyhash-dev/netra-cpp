#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <fmt/format.h>
#include <string>

#include "events.h"
#include "timer.h"
#include "objects.h"
#include "ringbuffer.h"


// Game setup
float ship_width_rel = 0.06;        // ship_w = screen_w * ship_width_rel
int ship_speed_rel = 3000000;       // time in uS to move across the screen vertically
int rotation_speed_rel = 2000000;   // time in uS to make 1 rotation
int ship_animation_speed = 100;     // Ship animation time in mS
int max_rotation = 20;              // Max rotation (+ and - degrees)
int acceleration = 1000;            // Time in mS to accelerate (and decellerate)
int bkg_speed = 7000000;            // Time for a star to tavel from the top of the screen to the bottom
int accel_time_const = 300000;     // Acceleration time in uS to get to 100% speed (or steer)

// INIT Globals
uint32_t framestart;
uint32_t last_frametime = 0;
uint32_t gameloop_time = 0; 

void ToggleFullscreen(SDL_Window* Window) {
    Uint32 FullscreenFlag = SDL_WINDOW_FULLSCREEN;
    bool IsFullscreen = SDL_GetWindowFlags(Window) & FullscreenFlag;
    SDL_SetWindowFullscreen(Window, IsFullscreen ? 0 : FullscreenFlag);
    SDL_ShowCursor(IsFullscreen);
}

int main(int argc, char *argv[])
{

	// returns zero on success else non-zero
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("error initializing SDL: %s\n", SDL_GetError());
	}
	SDL_Window* win = SDL_CreateWindow("GAME", // creates a window
									SDL_WINDOWPOS_CENTERED,
									SDL_WINDOWPOS_CENTERED,
                                    1920, 1080, 0);

	ToggleFullscreen(win);

	// triggers the program that controls
	// your graphics hardware and sets flags
	Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;

	// creates a renderer to render our images
	SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);

    int w = SDL_GetWindowSurface(win)->w;
    int h = SDL_GetWindowSurface(win)->h;
	int close = 0;

	fmt::print("Witdh: {}, Height: {}\n", w, h); // C++ with libfmt, <fmt/format.h>
	EventManager ev;

    std::string ship_files[5] = {"res/ship_1.png", "res/ship_2.png", "res/ship_3.png", "res/ship_4.png", "res/ship_5.png"};
    
    ship ship_1 = ship(w, h, ship_width_rel, rend, ship_files, 5, ship_animation_speed);
    bkg bg = bkg(1000, rend, w, h);
    bool debug = true;
    Ringbuffer frame_time = Ringbuffer(500);
    Ringbuffer game_time = Ringbuffer(500);
    //int frame_time[200];
    //int game_time[200];

	// animation loop
	while (!close) {
        framestart = micros();


        bool* event_status = ev.check();
        if(event_status[0] == 1)
        {
            close = 1;
        }
        float mov = ((float)last_frametime/(float)ship_speed_rel)*h;
        float mov_bkg = ((float)last_frametime/(float)bkg_speed)*h;
        int rot = 0;
        float move = 0;

        if(event_status[1])
            move = -mov;
        if(event_status[3])
            move = mov;
        if(event_status[2])
            rot = -max_rotation;
        if(event_status[4])
            rot = max_rotation;
        float div = (float)last_frametime / (float)accel_time_const;
        ship_1.rotate(rot, max_rotation, div);
        ship_1.move(move, -mov*3, div);

		// clears the screen
		SDL_RenderClear(rend);

        bg.draw(mov_bkg);
        ship_1.render(millis());

        if(debug)
        {
            int div = 50;
            int step = 10;
            int begin = 30;
            int end = 120;
            int pos_x = 10;
            SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
            for(int y = begin; y <= end; y += step)
                SDL_RenderDrawLine(rend, pos_x, (1/(float)y)*(1000000/div), frame_time.buffer.size(), (1/(float)y)*(1000000/div));
            SDL_SetRenderDrawColor(rend, 0, 255, 0, 255);
            for(int i = 0; i < (frame_time.buffer.size()-1) and frame_time.buffer.size() != 0; i++)
                SDL_RenderDrawLine(rend, pos_x+i, frame_time.buffer.at(i)/div, pos_x+1+i, frame_time.buffer.at(i+1)/div);
            SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
            for(int i = 0; i < (game_time.buffer.size()-1) and game_time.buffer.size() != 0; i++)
                SDL_RenderDrawLine(rend, pos_x+i, game_time.buffer.at(i)/div, pos_x+1+i, game_time.buffer.at(i+1)/div);
            SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
        }

		// triggers the double buffers
		// for multiple rendering
        gameloop_time = micros() - framestart;
		SDL_RenderPresent(rend);

		// calculates to set fps
        last_frametime = micros() - framestart;
        frame_time.add(last_frametime);
        game_time.add(gameloop_time);
        fmt::print("Frame Time: {} \t Gameloop Time {} \t> {}%\n", last_frametime, gameloop_time, (float)gameloop_time/(float)last_frametime);
	}

	// destroy texture
	//SDL_DestroyTexture(tex);

	// destroy renderer
	SDL_DestroyRenderer(rend);

	// destroy window
	SDL_DestroyWindow(win);
	
	// close SDL
	SDL_Quit();

	return 0;
}

