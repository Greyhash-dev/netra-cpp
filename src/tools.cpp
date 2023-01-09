#include "tools.h"

using namespace std;
std::array<int, 3> HSVtoRGB(float H, float S,float V)
{
    std::array<int, 3> ret = {0,0,0};
    if(H>360 || H<0 || S>100 || S<0 || V>100 || V<0){
        return ret;
    }
    float s = S/100;
    float v = V/100;
    float C = s*v;
    float X = C*(1-abs(fmod(H/60.0, 2)-1));
    float m = v-C;
    float r,g,b;
    if(H >= 0 && H < 60){
        r = C,g = X,b = 0;
    }
    else if(H >= 60 && H < 120){
        r = X,g = C,b = 0;
    }
    else if(H >= 120 && H < 180){
        r = 0,g = C,b = X;
    }
    else if(H >= 180 && H < 240){
        r = 0,g = X,b = C;
    }
    else if(H >= 240 && H < 300){
        r = X,g = 0,b = C;
    }
    else{
        r = C,g = 0,b = X;
    }
    ret[0] = (int)(r+m)*255;
    ret[1] = (int)(g+m)*255;
    ret[2] = (int)(b+m)*255;
    return ret;
}

int getIndex(std::vector<std::string> v, std::string key)
{
    std::vector<std::string>::iterator itr = std::find(v.begin(), v.end(), key);
    if (itr != v.cend()) {
        return std::distance(v.begin(), itr);
    }
    else
    {
        return -1;
    }
}

void get_text_and_rect(SDL_Renderer *renderer, int x, int y, const char *text, TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect, std::array<Uint8, 3> rgb)
{
    int text_width;
    int text_height;
    SDL_Surface *surface;
    SDL_Color textColor = {rgb.at(0), rgb.at(1), rgb.at(2), 0};

    surface = TTF_RenderText_Solid(font, text, textColor);
    *texture = SDL_CreateTextureFromSurface(renderer, surface);
    text_width = surface->w;
    text_height = surface->h;
    SDL_FreeSurface(surface);
    rect->x = x;
    rect->y = y;
    rect->w = text_width;
    rect->h = text_height;
}

std::array<int, 2> rotate_point(std::array<int, 2> rotatation_point, std::array<int, 2> point, float amount)
{
    float px = point.at(0);
    float py = point.at(1);
    float ox = rotatation_point.at(0);
    float oy = rotatation_point.at(1);
    float x = cos(amount) * (px-ox) - sin(amount) * (py-oy) + ox;
    float y = sin(amount) * (px-ox) + cos(amount) * (py-oy) + oy;
    return {(int)x, (int)y};
}
