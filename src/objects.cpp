#include "objects.h"

ship::ship(int screen_w, int screen_h, float ship_width_rel, SDL_Renderer* renderer, std::string files[], int arr_len, int ship_animation_speed)
{
    w = screen_w;
    h = screen_h;
    ani = ship_animation_speed;
    len = arr_len;
    rend = renderer;
    for (int i = 0; i < len; i++){
        fmt::print("Loading {}...\n", files[i]);
        SDL_Surface* surface;
        surface = IMG_Load(files[i].c_str());
        tex.push_back(SDL_CreateTextureFromSurface(rend, surface));
        SDL_FreeSurface(surface);
    }

    SDL_QueryTexture(tex.at(now), NULL, NULL, &dest.w, &dest.h);

    float ship_aspect = (float)dest.h / (float)dest.w;
    dest.w = ship_width_rel * screen_w;
    dest.h = dest.w * ship_aspect;
    dest.x = (screen_w - dest.w) / 2;
    dest.y = screen_h - (screen_h - dest.h) / 10;
    original_w = dest.w;
    dstx = (float)dest.x;
    dsty = (float)dest.y;
}

ship::~ship()
{
    for (int i = 0; i <= len; i++)
        SDL_DestroyTexture(tex.at(i));
}

void ship::render(int millis)
{
    if(millis - last_switch > ani)
    {
        last_switch = millis;
        now += 1;
        if(now == len)
            now = 0;
    }
    SDL_RenderCopyEx(rend, tex.at(now), nullptr, &dest, angle, 0, SDL_FLIP_NONE);
}

void ship::move(float amount, int apperent_speed, float div)
{
    float x = (float)angle*(3.14159/180);
    amount = (last_amount*(1-div) + amount*div);
    last_amount = amount;

    dsty += amount*cos(x);
    dstx -= (amount+apperent_speed)*sin(angle*(3.14159/180));
    if(dstx > w-dest.w)
        dstx = w-dest.w;
    else if(dstx < 0)
        dstx = 0;
    if(dsty > h-dest.h)
        dsty = h-dest.h;
    else if(dsty < 0)
        dsty = 0;
    dest.x = (int)dstx;
    dest.y = (int)dsty;
}

void ship::rotate(double angle_delta, int max_rotation, float div)
{
    angle = ((float)angle*(1-div) + angle_delta*div);
    if(angle > max_rotation)
        angle = max_rotation;
    if(angle < -max_rotation)
        angle = -max_rotation;
    dest.w = original_w * (1-(abs(angle)/(max_rotation/0.1)));
}

bkg::bkg(int num_stars, SDL_Renderer* renderer, int screen_w, int screen_h)
{
    std::srand(time(0));
    rend = renderer;
    w = screen_w;
    h = screen_h;
    n = num_stars;
    for (int i = 0; i < num_stars; i++)
    {
        SDL_Rect rect;
        int size = w/480;
        rect.x = (std::rand()%(w+size*2))-size;
        rect.y = (std::rand()%(h+size*2))-size;
        rect.w = w/480;
        rect.h = w/480;
        rects.push_back(rect);
    }
}

void bkg::draw(int bkg_speed)
{
    SDL_SetRenderDrawColor(rend, 155, 155, 155, 255);
    for (int i = 0; i < rects.size(); i++)
    {
        SDL_Rect* rect = &rects.at(i);
        rect->y += bkg_speed;
        if(rect->y > h)
        {
            rect->y = - rect->w;
            rect->x = (std::rand()%(w+rect->w*2))-rect->w;
        }
        SDL_RenderFillRect(rend, rect);
    }
    SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
}

bkg::~bkg()
{

}
