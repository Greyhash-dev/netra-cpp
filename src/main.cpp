#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <fmt/format.h>
#include <string>

#include "events.h"
#include "timer.h"
#include "objects.h"
#include "ringbuffer.h"
#include "tools.h"


// Game setup
float ship_width_rel = 0.06;        // ship_w = screen_w * ship_width_rel
int ship_speed_rel = 3000000;       // time in uS to move across the screen vertically
int rotation_speed_rel = 2000000;   // time in uS to make 1 rotation
int ship_animation_speed = 100000;  // Ship animation time in uS
int max_rotation = 20;              // Max rotation (+ and - degrees)
int acceleration = 1000;            // Time in mS to accelerate (and decellerate)
int bkg_speed = 7000000;            // Time for a star to tavel from the top of the screen to the bottom
int bullet_speed = 800000;          // Time in uS for a bullet to travel across the scren vertically
int accel_time_const = 300000;      // Acceleration time in uS to get to 100% speed (or steer)
int bullet_cooldown = 400000;       // Bullet shooting cooldown in uS
int multishot_max = 2;              // Max multishot
int multishot_delay = 50000;        // Multishot delay in uS
int harald_throw_delay = 1500000;   // Harald throw speed in uS
int flask_spin_speed = 500000;      // Flask spin speed in uS

// INIT Globals
// TIMING Stuff
uint32_t framestart;
uint32_t last_frametime = 0;
uint32_t gameloop_time = 0; 
uint32_t last_shoottime = 0;
uint32_t last_multishot = 0;
// OTHER
int multishot_no = 1;

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

    // FONT
    TTF_Init();
    TTF_Font *font = TTF_OpenFont("res/hack.ttf", 24);
    TTF_Font *debug_font = TTF_OpenFont("res/hack.ttf", 16);
    if (font == NULL) {
        fmt::print("ERROR loading font! File not found!");
        exit(EXIT_FAILURE);
    }

    int w = SDL_GetWindowSurface(win)->w;
    int h = SDL_GetWindowSurface(win)->h;
	int close = 0;

	fmt::print("Witdh: {}, Height: {}\n", w, h); // C++ with libfmt, <fmt/format.h>
	EventManager ev;

    std::string ship_files[5] = {"res/ship_1.png", "res/ship_2.png", "res/ship_3.png", "res/ship_4.png", "res/ship_5.png"};
    
    ship ship_1 = ship(w, h, ship_width_rel, rend, ship_files, 5, ship_animation_speed);
    harald harald_1 = harald(w, h, 0.1, rend, harald_throw_delay);
    std::vector<flasche*> flaschen;
    //flasche flask = flasche("res/rum.png", 0.035, w, h, flask_spin_speed, rend);
    //flaschen.push_back(&flask);
    bkg bg = bkg(1000, rend, w, h);
    bulletmgr bmgr = bulletmgr(rend, w, h);
    bool debug = true;
    Ringbuffer frame_time = Ringbuffer(500);
    Ringbuffer game_time = Ringbuffer(500);
    SDL_Rect rect_debug_fps;
    SDL_Rect rect_debug_keys;
    SDL_Texture *texture_debug_fps;
    SDL_Texture *texture_debug_keys;
    std::string bla = "NONE";
    get_text_and_rect(rend, 0, 0, bla.c_str(), font, &texture_debug_fps, &rect_debug_fps, {255, 255, 255});
    std::vector<hitbox_label> hitbox_labels;

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
        if(event_status[1] and event_status[3])
            move = 0;
        if(event_status[2])
            rot = -max_rotation;
        if(event_status[4])
            rot = max_rotation;
        if(event_status[2] and event_status[4])
            rot = 0;
        float div = (float)last_frametime / (float)accel_time_const;
        float angle = ship_1.rotate(rot, max_rotation, div);
        ship_1.move(move, -mov*3, div);

        if(event_status[5])
        {
            if((uint32_t)micros() - last_shoottime > bullet_cooldown){
                multishot_no = 1;
                last_shoottime = micros();
                last_multishot = micros();
                angle = angle*(3.14159/180);
                bmgr.spawn(ship_1.rect.x+ship_1.rect.w/4, ship_1.rect.y+ship_1.rect.h/2-(ship_1.rect.w/4)*sin(angle), bullet_speed/h, angle);
                bmgr.spawn(ship_1.rect.x+ship_1.rect.w*3/4, ship_1.rect.y+ship_1.rect.h/2+(ship_1.rect.w/4)*sin(angle), bullet_speed/h, angle);
            }
            else if(multishot_no < multishot_max && (uint32_t)micros() - last_multishot > multishot_delay)
            {
                last_multishot = micros();
                angle = angle*(3.14159/180);
                bmgr.spawn(ship_1.rect.x+ship_1.rect.w/4, ship_1.rect.y+ship_1.rect.h/2-(ship_1.rect.w/4)*sin(angle), bullet_speed/h, angle);
                bmgr.spawn(ship_1.rect.x+ship_1.rect.w*3/4, ship_1.rect.y+ship_1.rect.h/2+(ship_1.rect.w/4)*sin(angle), bullet_speed/h, angle);
                multishot_no ++;
            }
        }

		// clears the screen
		SDL_RenderClear(rend);

        bg.draw(mov_bkg);
        ship_1.render(micros());
        harald_1.render(micros());
        for (flasche* flask : flaschen)
            flask->render(micros());
        bmgr.update(last_frametime);

        if(debug)
        {
            int pos_v = 0;
            // FPS DEBUG
            if (frame_time.buffer.size() > 0)
            {
                int fps_now = (int)(1000000./last_frametime);
                int fps_low = (int)(1000000./ *std::max_element(frame_time.buffer.begin(), frame_time.buffer.end()));
                int fps_high = (int)(1000000./ *std::min_element(frame_time.buffer.begin(), frame_time.buffer.end()));
                bla = fmt::format("FPS: | Cur: {} | Min: {} | Max: {} |", fps_now, fps_low, fps_high);
            }
            get_text_and_rect(rend, 0, 0, bla.c_str(), debug_font, &texture_debug_fps, &rect_debug_fps, {150, 150, 150});
            pos_v += rect_debug_fps.h;
            SDL_RenderCopy(rend, texture_debug_fps, NULL, &rect_debug_fps);

            // KEY input debug
            bla = "Events: | ";
            int cnt = 0;
            for (std::string label : ev.labels)
            {
                bla += fmt::format("{} {} | ", label, int(event_status[cnt]));
                cnt ++;
            }
            get_text_and_rect(rend, 0, pos_v, bla.c_str(), debug_font, &texture_debug_keys, &rect_debug_keys, {150, 150, 150});
            pos_v += rect_debug_keys.h;
            SDL_RenderCopy(rend, texture_debug_keys, NULL, &rect_debug_keys);

            // Graph
            int div = 50;
            int step = 10;
            int begin = 30;
            int end = 120;
            int pos_x = 10;
            SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
            SDL_RenderDrawLine(rend, pos_x, pos_v, frame_time.buffer.size(), pos_v);
            for(int y = begin; y <= end; y += step)
                SDL_RenderDrawLine(rend, pos_x, (1/(float)y)*(1000000/div)+pos_v, frame_time.buffer.size(), (1/(float)y)*(1000000/div)+pos_v);
            SDL_SetRenderDrawColor(rend, 0, 255, 0, 255);
            for(int i = 0; i < (frame_time.buffer.size()-1) and frame_time.buffer.size() != 0; i++)
                SDL_RenderDrawLine(rend, pos_x+i, frame_time.buffer.at(i)/div+pos_v, pos_x+1+i, frame_time.buffer.at(i+1)/div+pos_v);
            SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
            for(int i = 0; i < (game_time.buffer.size()-1) and game_time.buffer.size() != 0; i++)
                SDL_RenderDrawLine(rend, pos_x+i, game_time.buffer.at(i)/div+pos_v, pos_x+1+i, game_time.buffer.at(i+1)/div+pos_v);
            SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
            ship_1.draw_hitbox(rend, &hitbox_labels, font, w, h);
            harald_1.draw_hitbox(rend, &hitbox_labels, font, w, h);
            bmgr.draw_hitbox(rend, &hitbox_labels, font, w, h);
            for (flasche* flask : flaschen)
                flask->draw_hitbox(rend, &hitbox_labels, font, w, h);
            if (harald_1.flask != nullptr)
                harald_1.flask->draw_hitbox(rend, &hitbox_labels, font, w, h);
            for (hitbox_label &label : hitbox_labels)
                label.draw(rend);
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

