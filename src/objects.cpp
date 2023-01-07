#include "objects.h"

ship::ship(int screen_w, int screen_h, float ship_width_rel, SDL_Renderer* renderer, std::string files[], int arr_len, int ship_animation_speed)
{
    hitbox_type = "ship";
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
    hit_w = dest.w;
    hit_h = dest.h;
    dest.x = (screen_w - dest.w) / 2;
    dest.y = screen_h - (screen_h - dest.h) / 10;
    original_w = dest.w;
    cur_x = (float)dest.x;
    cur_y = (float)dest.y;
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

    cur_y += amount*cos(x);
    cur_x -= (amount+apperent_speed)*sin(angle*(3.14159/180));
    if(cur_x > w-dest.w)
        cur_x = w-dest.w;
    else if(cur_x < 0)
        cur_x = 0;
    if(cur_y > h-dest.h)
        cur_y = h-dest.h;
    else if(cur_y < 0)
        cur_y = 0;
    dest.x = (int)cur_x;
    dest.y = (int)cur_y;
}

float ship::rotate(double angle_delta, int max_rotation, float div)
{
    angle = ((float)angle*(1-div) + angle_delta*div);
    if(angle > max_rotation)
        angle = max_rotation;
    if(angle < -max_rotation)
        angle = -max_rotation;
    dest.w = original_w * (1-(abs(angle)/(max_rotation/0.1)));
    hit_w = dest.w;
    return angle;
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
            rect->y -= h + rect->w;
            rect->x = (std::rand()%(w+rect->w*2))-rect->w;
        }
        SDL_RenderFillRect(rend, rect);
    }
    SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
}

bkg::~bkg()
{

}

bulletmgr::bulletmgr(SDL_Renderer* renderer, int screen_w, int screen_h)
{
    w = screen_w;
    h = screen_h;
    rend = renderer;
}

void bulletmgr::spawn(int x, int y, float vel, float rot)
{
    bullets.push_back(bullet(x, y, vel, rot));
    bullets.back().hit_w = -10*sin(rot);
    bullets.back().hit_h = -10*cos(rot);
}

bool rem_bullet(bullet bul)
{
    return bul.cur_x < 0;
}

void bulletmgr::update(uint32_t last_frametime)
{
    SDL_SetRenderDrawColor(rend, 255, 255, 0, 255);
    int idx = 0;
    for (bullet &bul : bullets)
    {
        float mult_x = sin(bul.cur_rotation);
        float mult_y = cos(bul.cur_rotation);
        bul.cur_x += mult_x*(last_frametime/bul.velocity);
        bul.cur_y -= mult_y*(last_frametime/bul.velocity);
        SDL_RenderDrawLine(rend, bul.cur_x, bul.cur_y, bul.cur_x+(10*mult_x), bul.cur_y-(10*mult_y));
        bul.hit_w = -10*mult_x;
        bul.hit_h = -10*mult_y;
        if(bul.cur_y < 0)
        {
            bullets.erase(bullets.begin()+idx);
            idx --;
        }
        idx ++;
    }
}

void bulletmgr::draw_hitbox(SDL_Renderer* renderer, std::vector<hitbox_label>* hitbox_types, TTF_Font *font, int w, int h)
{
    for (bullet &bul : bullets)
    {
        bul.draw_hitbox(renderer, hitbox_types, font, w, h);
    }
}

bulletmgr::~bulletmgr()
{
}

void hitbox::draw_hitbox(SDL_Renderer* renderer, std::vector<hitbox_label>* hitbox_labels, TTF_Font *font, int w, int h)
{
    std::vector<std::string> hitbox_types;
    for (hitbox_label &label : *hitbox_labels)
        hitbox_types.push_back(label.get_text());
    if (not (std::find(hitbox_types.begin(), hitbox_types.end(), hitbox_type) != hitbox_types.end()))
        generate_labels(renderer, hitbox_labels, font, w, h, hitbox_type);
    else {
        float idx = (float)getIndex(hitbox_types, hitbox_type)+1;
        hitbox_ptr = &(hitbox_labels->at((int)idx-1));
        float size = (float)hitbox_types.size();
        std::array<int, 3> rgb = HSVtoRGB(360*(idx/size), 100, 100);
        SDL_SetRenderDrawColor(renderer, rgb.at(0), rgb.at(1), rgb.at(2), 255);
        SDL_RenderDrawLine(renderer, (int)cur_x, (int)cur_y, (int)cur_x+hit_w, (int)cur_y);
        SDL_RenderDrawLine(renderer, (int)cur_x, (int)cur_y, (int)cur_x, (int)cur_y+hit_h);
        SDL_RenderDrawLine(renderer, (int)cur_x+hit_w, (int)cur_y+hit_h, (int)cur_x+hit_w, (int)cur_y);
        SDL_RenderDrawLine(renderer, (int)cur_x+hit_w, (int)cur_y+hit_h, (int)cur_x, (int)cur_y+hit_h);
        SDL_RenderDrawLine(renderer, (int)cur_x+hit_w, (int)cur_y, hitbox_ptr->ray_pos.at(0), hitbox_ptr->ray_pos.at(1));
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    }
}

hitbox_label::hitbox_label(std::string label, int pos_x, int pos_y, SDL_Renderer* renderer, TTF_Font *font, std::array<Uint8, 3> rgb)
{
    rgb = rgb;
    label_text = label;
    get_text_and_rect(renderer, pos_x, pos_y, label.c_str(), font, &texture, &rect, rgb);
    rect.x -= rect.w;
    ray_pos.at(0) = rect.x;
    ray_pos.at(1) = pos_y + (rect.h/2);
}

void hitbox_label::draw(SDL_Renderer* renderer)
{
    SDL_RenderCopy(renderer, texture, NULL, &rect);
}

std::string hitbox_label::get_text()
{
    return label_text;
}

void hitbox::generate_labels(SDL_Renderer* renderer, std::vector<hitbox_label>* hitbox_labels, TTF_Font *font, int w, int h, std::string new_type)
{
    float size = (float)hitbox_labels->size();
    float cnt = 0;
    for (hitbox_label &label : *hitbox_labels)
    {
        std::array<int, 3> rgb_new = HSVtoRGB(360*((cnt+1)/(size+1)), 100, 100);
        Uint8 R = rgb_new.at(0);
        Uint8 G = rgb_new.at(1);
        Uint8 B = rgb_new.at(2);
        hitbox_label new_label = hitbox_label(label.get_text(), w, 35*cnt, renderer, font, {R, G, B}); //FIXME
        hitbox_labels->at((int)cnt) = new_label;
        cnt += 1;
    }
    std::array<int, 3> rgb = HSVtoRGB(360, 100, 100);
    hitbox_label label = hitbox_label(new_type, w, 35*(int)size, renderer, font, {(Uint8)rgb.at(0), (Uint8)rgb.at(1), (Uint8)rgb.at(2)}); //FIXME
    hitbox_labels->push_back(label);
}
