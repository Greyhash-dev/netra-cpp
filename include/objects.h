#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <vector>
#include <fmt/format.h>

#include "tools.h"

class hitbox_label
{
    private:
        std::string label_text;
    public:
        std::array<Uint8, 3> rgb = {255, 255, 255};
        std::array<int, 2> ray_pos = {0, 0};
        SDL_Texture *texture;
        SDL_Rect rect;
        hitbox_label(std::string label, int pos_x, int pos_y, SDL_Renderer* renderer, TTF_Font *font, std::array<Uint8, 3> rgb);
        void draw(SDL_Renderer* renderer);
        std::string get_text();
};

struct hitbox
{
    std::string hitbox_type;
    hitbox_label* hitbox_ptr;
    SDL_Rect rect;
    void draw_hitbox(SDL_Renderer* renderer, std::vector<hitbox_label>* hitbox_types, TTF_Font *font, int w, int h);
    void generate_labels(SDL_Renderer* renderer, std::vector<hitbox_label>* hitbox_labels, TTF_Font *font, int w, int h, std::string new_type);
    std::array<float, 2> hit_rotation_point = {0, 0};
    float rot = 0;
};

class flasche: public hitbox
{
    private:
        std::string img_path;
        SDL_Texture* tex;
        SDL_Renderer* rend;
    public:
        SDL_Point rotation_point;
        float spin;
        flasche(std::string path, float width_rel, int screen_w, int screen_h, int spin_speed, SDL_Renderer* renderer);
        void render(int micros);
        ~flasche();
};

class ship: public hitbox
{
	private:
        std::vector<SDL_Texture*> tex;
        SDL_Renderer* rend;
        int w;
        int h;
        int len;
        int now = 0;
        int last_switch = 0;
        int ani;
        int original_w;
        float last_amount = 0;
        float angle;

	public:
        ship(int screen_w, int screen_h, float ship_width_rel, SDL_Renderer* renderer, std::string files[], int arr_len, int ship_animation_speed);
        ~ship();
        void render(int millis);
        void move(float amount, int apperent_speed, float div);
        float rotate(double angle_delta, int max_rotation, float div);
};

class harald: public hitbox
{
    private:
        SDL_Renderer* rend;
        SDL_Texture* body;
        SDL_Texture* arm;
        float arm_rotation;
        SDL_Point arm_rotation_point;
        int throw_speed;
    public:
        flasche* flask = nullptr;
        SDL_Rect rect_arm;
        void render(int micros);
        harald(int screen_w, int screen_h, float harald_width_rel, SDL_Renderer* renderer, int speed);
        ~harald();
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

struct bullet: public hitbox
{
    float velocity;
    float cur_rotation;
    bullet(int x, int y, float v, float r){
        hitbox_type = "bullet";
        rect.x = (double)x;
        rect.y = (double)y;
        velocity = v;
        cur_rotation = r;
    }
};

class bulletmgr
{
    private:
        std::vector<bullet> bullets;
        SDL_Renderer* rend;
        int w;
        int h;
    public:
        bulletmgr(SDL_Renderer* renderer, int screen_w, int screen_h);
        ~bulletmgr();
        void spawn(int x, int y, float vel, float rot);
        void update(uint32_t last_frametime);
        void draw_hitbox(SDL_Renderer* renderer, std::vector<hitbox_label>* hitbox_types, TTF_Font *font, int w, int h);
};
