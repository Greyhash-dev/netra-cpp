#include <bits/stdc++.h>
#include <vector>
#include <SDL2/SDL_ttf.h>

std::array<int, 3> HSVtoRGB(float H, float S,float V);
int getIndex(std::vector<std::string> v, std::string key);
void get_text_and_rect(SDL_Renderer *renderer, int x, int y, const char *text, TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect, std::array<Uint8, 3> rgb);
