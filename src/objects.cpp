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

    SDL_QueryTexture(tex.at(now), NULL, NULL, &rect.w, &rect.h);

    float ship_aspect = (float)rect.h / (float)rect.w;
    rect.w = ship_width_rel * screen_w;
    rect.h = rect.w * ship_aspect;
    rect.x = (screen_w - rect.w) / 2;
    rect.y = screen_h - (screen_h - rect.h) / 10;
    original_w = rect.w;
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
    SDL_RenderCopyEx(rend, tex.at(now), nullptr, &rect, angle, 0, SDL_FLIP_NONE);
}

void ship::move(float amount, int apperent_speed, float div)
{
    float x = (float)angle*(3.14159/180);
    amount = (last_amount*(1-div) + amount*div);
    last_amount = amount;

    rect.y += amount*cos(x);
    rect.x -= (amount+apperent_speed)*sin(angle*(3.14159/180));
    if(rect.x > w-rect.w)
        rect.x = w-rect.w;
    else if(rect.x < 0)
        rect.x = 0;
    if(rect.y > h-rect.h)
        rect.y = h-rect.h;
    else if(rect.y < 0)
        rect.y = 0;
    rect.x = (int)rect.x;
    rect.y = (int)rect.y;
}

float ship::rotate(double angle_delta, int max_rotation, float div)
{
    angle = ((float)angle*(1-div) + angle_delta*div);
    if(angle > max_rotation)
        angle = max_rotation;
    if(angle < -max_rotation)
        angle = -max_rotation;
    rect.w = original_w * (1-(abs(angle)/(max_rotation/0.1)));
    rect.w = rect.w;
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
    bullets.back().rect.w = -10*sin(rot);
    bullets.back().rect.h = -10*cos(rot);
}

bool rem_bullet(bullet bul)
{
    return bul.rect.x < 0;
}

void bulletmgr::update(uint32_t last_frametime)
{
    SDL_SetRenderDrawColor(rend, 255, 255, 0, 255);
    int idx = 0;
    for (bullet &bul : bullets)
    {
        float mult_x = sin(bul.cur_rotation);
        float mult_y = cos(bul.cur_rotation);
        bul.rect.x += mult_x*(last_frametime/bul.velocity);
        bul.rect.y -= mult_y*(last_frametime/bul.velocity);
        SDL_RenderDrawLine(rend, bul.rect.x, bul.rect.y, bul.rect.x+(10*mult_x), bul.rect.y-(10*mult_y));
        bul.rect.w = -10*mult_x;
        bul.rect.h = -10*mult_y;
        if(bul.rect.y < 0)
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
        
        if (rot==0){
            SDL_RenderDrawLine(renderer, (int)rect.x, (int)rect.y, (int)rect.x+rect.w, (int)rect.y);
            SDL_RenderDrawLine(renderer, (int)rect.x, (int)rect.y, (int)rect.x, (int)rect.y+rect.h);
            SDL_RenderDrawLine(renderer, (int)rect.x+rect.w, (int)rect.y+rect.h, (int)rect.x+rect.w, (int)rect.y);
            SDL_RenderDrawLine(renderer, (int)rect.x+rect.w, (int)rect.y+rect.h, (int)rect.x, (int)rect.y+rect.h);
        }
        else
        {
            std::array<int, 2> rotation_point = {(int)(hit_rotation_point.at(0)+rect.x), (int)(hit_rotation_point.at(1)+rect.y)};
            std::array<int, 2> p1 = rotate_point(rotation_point, {rect.x, rect.y}, rot);
            std::array<int, 2> p2 = rotate_point(rotation_point, {rect.x+rect.w, rect.y}, rot);
            SDL_RenderDrawLine(renderer, (int)p1.at(0), (int)p1.at(1), (int)p2.at(0), (int)p2.at(1));
            p2 = rotate_point(rotation_point, {rect.x, rect.y+rect.h}, rot);
            SDL_RenderDrawLine(renderer, (int)p1.at(0), (int)p1.at(1), (int)p2.at(0), (int)p2.at(1));
            p1 = rotate_point(rotation_point, {rect.x+rect.w, rect.y+rect.h}, rot);
            SDL_RenderDrawLine(renderer, (int)p1.at(0), (int)p1.at(1), (int)p2.at(0), (int)p2.at(1));
            p2 = rotate_point(rotation_point, {rect.x+rect.w, rect.y}, rot);
            SDL_RenderDrawLine(renderer, (int)p1.at(0), (int)p1.at(1), (int)p2.at(0), (int)p2.at(1));
        }
        //SDL_RenderDrawLine(renderer, (int)cur_x+hit_w, (int)cur_y, hitbox_ptr->ray_pos.at(0), hitbox_ptr->ray_pos.at(1));
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

harald::harald(int screen_w, int screen_h, float harald_width_rel, SDL_Renderer* renderer, int speed)
{
    throw_speed = speed;
    hitbox_type = "boss";
    rend = renderer;
    fmt::print("Loading res/harald-arm.png...\n");
    SDL_Surface* surface;
    surface = IMG_Load("res/harald-arm.png");
    arm = SDL_CreateTextureFromSurface(rend, surface);
    fmt::print("Loading res/harald-body.png...\n");
    surface = IMG_Load("res/harald-body.png");
    body = SDL_CreateTextureFromSurface(rend, surface);
    SDL_FreeSurface(surface);
    SDL_QueryTexture(body, NULL, NULL, &rect.w, &rect.h);
    SDL_QueryTexture(arm, NULL, NULL, &rect_arm.w, &rect_arm.h);


    float body_aspect = (float)rect.h / (float)rect.w;
    float arm_aspect = (float)rect_arm.h / (float)rect_arm.w;
    int body_orig_w = rect.w;
    rect.w = harald_width_rel * screen_w;
    rect.h = rect.w * body_aspect;
    rect.x = (screen_w - rect.w) / 2;
    rect.y = screen_h / 2;
    rect_arm.w *= ((float)rect.w / (float)body_orig_w);
    rect_arm.h = rect_arm.w * arm_aspect;
    rect_arm.x = rect.x-rect.w*0.34;
    rect_arm.y = rect.y+rect.h*0.15;

    arm_rotation_point = {(int)(rect.w*0.75), (int)(rect.h*0.3)};

    flask = new flasche("res/rum.png", 0.03, screen_w, screen_h, 0, rend);
    flask->rotation_point = {(int)(rect.w*0.75), (int)(rect.h*0.3)};
    flask->hit_rotation_point = {(float)(rect.w*0.75), (float)(rect.h*0.3)};
}

void harald::render(int micros)
{
    arm_rotation = sin(((float)micros/throw_speed)*3.1415*2);
    flask->rect.x = rect_arm.x;
    flask->rect.y = rect_arm.y;
    flask->rot = arm_rotation*1.91986+1;
    flask->render(micros);
    SDL_RenderCopyEx(rend, arm, nullptr, &rect_arm, arm_rotation*110.+63., &arm_rotation_point, SDL_FLIP_NONE);
    SDL_RenderCopyEx(rend, body, nullptr, &rect, 0, 0, SDL_FLIP_NONE);
}

harald::~harald()
{
    SDL_DestroyTexture(body);
    SDL_DestroyTexture(arm);
    delete flask;
}

flasche::flasche(std::string path, float width_rel, int screen_w, int screen_h, int spin_speed, SDL_Renderer* renderer)
{
    spin = spin_speed;
    rend = renderer;
    hitbox_type = "flasche";
    fmt::print("Loading {}...\n", path);
    SDL_Surface* surface;
    surface = IMG_Load(path.c_str());
    tex = SDL_CreateTextureFromSurface(rend, surface);
    SDL_FreeSurface(surface);
    SDL_QueryTexture(tex, NULL, NULL, &rect.w, &rect.h);

    float aspect = (float)rect.h / (float)rect.w;
    rect.w = width_rel * screen_w;
    rect.h = rect.w * aspect;
    rect.x = 500;
    rect.y = 100;

    rotation_point = {(int)(rect.w*0.5), (int)(rect.h*0.075)};
    hit_rotation_point = {(float)(rect.w*0.5), (float)(rect.h*0.075)};
}

flasche::~flasche()
{
    SDL_DestroyTexture(tex);
}

void flasche::render(int micros)
{
    if (spin != 0)
        rot = ((float)micros / (float)spin)*2*3.1415;
    SDL_RenderCopyEx(rend, tex, nullptr, &rect, rot*(180/3.1415), &rotation_point, SDL_FLIP_NONE);
}
