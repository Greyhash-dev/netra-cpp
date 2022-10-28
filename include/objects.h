#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <vector>
#include <fmt/format.h>

class ship
{
	private:
        SDL_Rect dest;
        std::vector<SDL_Texture*> tex;
        SDL_Renderer* rend;
        double angle;
        int len;
        int now = 0;
        int last_switch = 0;
        int ani;
        float dstx;
        float dsty;
        int h;
        int w;
        int original_w;
        float last_amount = 0;

	public:
        ship(int screen_w, int screen_h, float ship_width_rel, SDL_Renderer* renderer, std::string files[], int arr_len, int ship_animation_speed);
        ~ship();
        void render(int millis);
        void move(float amount, int apperent_speed, float div);
        void rotate(double angle_delta, int max_rotation, float div);
};

class bkg
{
    private:
        SDL_Renderer* rend;
        int w;
        int h;
        int n;
        std::vector<SDL_Rect> rects;
    public:
        bkg(int num_stars, SDL_Renderer* renderer, int screen_w, int screen_h);
        ~bkg();
        void draw(int bkg_speed);
};
