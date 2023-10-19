#include <raylib.h>
#include<bits/stdc++.h>
#include<fstream>
using namespace std;


Texture2D backgroundTexture;
int current_level;
class Fruits;
struct Size;
struct Rect;
int jump_up, jump_down, fruitcount;
int endd = 0;

struct Size
{
    float width;
    float height;
};

struct Rect
{
    Vector2 position;
    Size size;
};

class Fruits
{
public:
    Size fruitSize;
    Vector2 position;

    Fruits(Size size, Vector2 pos)
    {
        fruitSize = size;
        position = pos;
    }
    Vector2 centre()
    {
        return Vector2{(position.x + fruitSize.width/2), (position.y + fruitSize.height/2)};
    }
};

class Player
{
 private:
    int life;
    float speed;
 public:
    Vector2 position;
    Size playerSize;

    Player()       //Default Constructor
    {
        speed = 50;
        life = 3;
    }

    Player(Size size) // Parameterized Constructor
    {
        playerSize = size;
        life = 1;
        position = {0, 583};
        speed = 100;
    }
    void move_right() { position.x += 0.06*speed; }
    void move_left() { position.x -= 0.1*speed; }
    void move_up() { position.y -= 0.1*speed; }
    void move_down() { position.y += 0.1*speed; }
    void jump()
    {
        jump_up = 1;
        jump_down = 0;
    }
    bool collide(Fruits fruit)
    {
         return is_point_in_rect(fruit.centre(), Rect{position, playerSize});
    }
    bool collide(Rectangle a, Rectangle b)  ///Function Oveloading
    {
        bool condition = CheckCollisionRecs(a, b);
        return condition;
    }
    bool is_point_in_rect(Vector2 point, Rect rectangle)
    {
        bool condition_x = (point.x > rectangle.position.x && point.x < (rectangle.position.x + rectangle.size.width));
        bool condition_y = (point.y > rectangle.position.y && point.y < (rectangle.position.y + rectangle.size.height));
        return condition_x && condition_y;
    }
    friend bool birdVSplayer(Player p, Rectangle b);
};

class Bullet
{
public:
    Vector2 position;
    float speed;
    Bullet(Vector2 pos)
    {
        position = pos;
        speed = 80;
    }
    void move()
    {
        position.x += 0.1 * speed;
    }
};


class Enemy
{
public:
    Vector2 position;
    float speed;
    Size size;

    virtual void attack() = 0;  //Pure Virtual Function
};

class Pumpkin: virtual public Enemy  //Inheritance
{
public:
    Pumpkin(Vector2 pos, Size s)
    {
        position = pos;
        size = s;
        speed = 30;
    }
    void attack() //Function Overriding(Polymorphism)
    {
        position.x -= 0.1*speed;
    }
    bool operator+(Player& obj) //Operator Overloading
    {
        Rectangle a = {position.x, position.y, size.width, size.height};
        Rectangle b = {obj.position.x, obj.position.y, obj.playerSize.width, obj.playerSize.height};
        bool condition = CheckCollisionRecs(a, b);
        return condition;
    }

};

class Bird: virtual public Enemy
{
    public:
    Bird(Vector2 pos)
    {
        position = pos;
        speed = 40;
    }
    void attack() //Function overriding(polymorphism)
    {
        position.x -= 0.1*speed;
    }

    friend bool birdVSplayer(Player p, Rectangle b);
};

//Friend Function
bool birdVSplayer(Player p, Rectangle b)
{
    Rectangle a = {p.position.x, p.position.y, p.playerSize.width, p.playerSize.height};
    bool condition = CheckCollisionRecs(a, b);
    return condition;

}

//Using Template
//Checking Collision betweenn Bullet and (pumpkin or bird)
template<class T>
bool is_collision(Rectangle r, T obj, float x, float y)
{
    Rectangle tempP = {obj.position.x, obj.position.y, x, y};
    bool collision = CheckCollisionRecs(r, tempP);
    return collision;
}

///

vector<Fruits> banana;
vector<Fruits> apple;
vector<Fruits> grapes;
vector<Bullet> bullets;
vector<Bird> bird_enemies;
vector<Pumpkin> pumpkin_enemies;

bool vanish_banana[1000] = {false};
bool vanish_apple[1000] = {false};
bool play_sound_apple[1000] = {false};
bool play_sound_banana[1000] = {false};
bool vanish_grapes[1000] = {false};
bool play_sound_grapes[1000] = {false};
bool vanish_bullet[1000] = {false};
bool vanish_pumpkin[1000] = {false};
bool vanish_bird[1000] = {false};

//bool playgain;

//Reseting everything
void reset()
{
    int i, j;
    for(i = 0; i<1000; i++)
    {
        vanish_apple[i] = false;
        vanish_banana[i] = false;
        vanish_grapes[i] = false;
        vanish_bird[i] = false;
        vanish_pumpkin[i] = false;
        vanish_bullet[i] = false;
        play_sound_apple[i] = false;
        play_sound_banana[i] = false;
        play_sound_grapes[i] = false;
    }
    bullets.clear();
    endd = 0;
    fruitcount = 0;
    //playgain = 0;
}

void pumpkinset(float a, float b)
{
    string s1, s2;
    float temp_x, temp_y;
    pumpkin_enemies.clear();
    ifstream pumpkinout("pumpkinfile.txt");
    while(!pumpkinout.eof())    //File Handling
    {
        pumpkinout >> s1;
        pumpkinout >> s2;
        temp_x = stoi(s1);
        temp_y = stoi(s2);
        pumpkin_enemies.push_back(Pumpkin{Vector2{temp_x, temp_y}, Size{a, b}});
    }
    pumpkinout.close();
}

void birdset(float a, float b)
{
    string s1, s2;
    float temp_x, temp_y;
    bird_enemies.clear();
    ifstream birdout("birdfile.txt");
    while(!birdout.eof())        //File Handling
    {
        birdout >> s1;
        birdout >> s2;
        temp_x = stoi(s1);
        temp_y = stoi(s2);
        bird_enemies.push_back(Bird{Vector2{temp_x, temp_y}});
    }
    birdout.close();
}

int main()
{
    // Window
    const int screenWidth = 1500;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "Level - 1");
    SetWindowSize(screenWidth, screenHeight);
    SetWindowState(FLAG_WINDOW_MAXIMIZED); 
    InitAudioDevice();

    // Background
    Image backgroundImage;

    float scaleX = static_cast<float>(screenWidth) / backgroundTexture.width;
    float scaleY = static_cast<float>(screenHeight) / backgroundTexture.height;

    // Player
    Image playerImage = LoadImage("player1.png");
    Texture2D playerTexture = LoadTextureFromImage(playerImage);
    UnloadImage(playerImage);
    Player p = Player{Size{playerTexture.width, playerTexture.height}};
    float players_ground;

    //Variables
    int temp_x, temp_y;
    current_level = 0;
    string num;
    string s1, s2;

    //Banana
    Texture2D bananaTexture = LoadTexture("banana.png");
    ifstream bananaout("bananafile.txt");
    while(!bananaout.eof())
    {
        bananaout >> s1;
        bananaout >> s2;
        temp_x = stoi(s1);
        temp_y = stoi(s2);
        banana.push_back(Fruits{Size{bananaTexture.width, bananaTexture.height}, Vector2{temp_x, temp_y}});
    }
    bananaout.close();

    //Apple
    Texture2D appleTexture = LoadTexture("apple.png");
    ifstream appleout("applefile.txt");
    while(!appleout.eof())
    {
        appleout >> s1;
        appleout >> s2;
        temp_x = stoi(s1);
        temp_y = stoi(s2);
        apple.push_back(Fruits{Size{appleTexture.width, appleTexture.height}, Vector2{temp_x, temp_y}});
    }
    appleout.close();

    //Grapes
    Texture2D grapesTexture = LoadTexture("grapes.png");
    ifstream grapesout("grapesfile.txt");
    while(!grapesout.eof())
    {
        grapesout >> s1;
        grapesout >> s2;
        temp_x = stoi(s1);
        temp_y = stoi(s2);
        grapes.push_back(Fruits{Size{grapesTexture.width, grapesTexture.height}, Vector2{temp_x, temp_y}});
    }

    //Bullet
    Texture2D bulletTexture = LoadTexture("bullet.png");
    
    //Enemy : -
    //Pumpkin
    Texture2D pumpkinTexture = LoadTexture("enpumpkin.png");
    Texture2D strawTexture = LoadTexture("cherry.png");
    //Bird
    Texture2D birdtexture = LoadTexture("bird.png");

    //Sound
    Sound fruitsound = LoadSound("coin.wav"); 
    Sound bullet_sound = LoadSound("weird.wav");
    Sound die_sound = LoadSound("spring.wav");


    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if(current_level == 0)
        {
            if(IsKeyPressed(KEY_ENTER))
            {
                reset();
                pumpkinset(pumpkinTexture.width, pumpkinTexture.height);
                birdset(birdtexture.width, birdtexture.height);
                current_level = 1;
                continue;
            }
            backgroundImage = LoadImage("forest.png");
            backgroundTexture = LoadTextureFromImage(backgroundImage);
            UnloadImage(backgroundImage);
            BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawTexturePro(backgroundTexture,
                            { 0, 0, backgroundTexture.width, backgroundTexture.height }, 
                            { 0, 0, screenWidth, screenHeight },        
                            { 0, 0 },                                     
                            0.0f,                                          
                            WHITE); 
            EndDrawing(); 
        }
        if(current_level == 1){

            ///LEVEL 1

            players_ground = (screenHeight - playerTexture.height)/p.position.y;

            backgroundImage = LoadImage("back7.png");
            backgroundTexture = LoadTextureFromImage(backgroundImage);
            UnloadImage(backgroundImage);

            if(p.position.x >= screenWidth - playerTexture.width)
            {
                current_level = 2;
                p.position = {0, 583};
                continue;
            }
            if(p.position.y <= 100) {jump_up = 0, jump_down = 1;}
            if(players_ground <= 1.2) { jump_down = 0;}

            if(jump_up) { p.position.y -= 20;} 
            if(jump_down) { p.position.y += 15;}

            //Handling keyboard input
            if (IsKeyUp(KEY_RIGHT) && p.position.x < screenWidth - playerTexture.width) p.move_right();
            if (IsKeyDown(KEY_LEFT) && p.position.x > 0) p.move_left();
            if(IsKeyPressed(KEY_UP)) p.jump();

            cout << "players position = " << p.position.x << " " << p.position.y << endl;
            
            //For Banana
            for(int i = 0; i<banana.size(); i++)
            {
                if(p.collide(banana[i]) && !play_sound_banana[i])
                {
                    PlaySound(fruitsound);
                    vanish_banana[i] = true;
                    fruitcount++;
                    play_sound_banana[i] = true;
                }
            }
            //For apple
            for(int i=0; i<apple.size(); i++)
            {
                if(p.collide(apple[i])){
                    if(!play_sound_apple[i]){
                        PlaySound(fruitsound);
                        play_sound_apple[i] = true;
                        fruitcount++;
                    }
                    vanish_apple[i] = 1;
                }
            }

            //For Grapes
            for(int i = 0; i<grapes.size(); i++)
            {
                if(p.collide(grapes[i]) && !play_sound_grapes[i])
                {
                    PlaySound(fruitsound);
                    vanish_grapes[i] = true;
                    fruitcount++;
                    play_sound_grapes[i] = true;
                }
            }

            // Drawing the background image
            BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawTexturePro(backgroundTexture,
                            { 0, 0, backgroundTexture.width, backgroundTexture.height }, // Source rectangle
                            { 0, 0, screenWidth, screenHeight },         // Destination rectangle
                            { 0, 0 },                                     // Origin (rotation pivot)
                            0.0f,                                          // Rotation angle (unused)
                            WHITE);                                        // Tint color

            // Drawing the player
            DrawTexture(playerTexture, static_cast<int>(p.position.x),
                        static_cast<int>(p.position.y), WHITE);


            //Drawing apple
            for(int i=0; i<apple.size(); i++)
            {
                if(!vanish_apple[i])
                    DrawTexture(appleTexture, apple[i].position.x, apple[i].position.y, WHITE);
            }
            //Drawing Banana
            for(int i=0; i<banana.size(); i++)
            {
                if(!vanish_banana[i])
                    DrawTexture(bananaTexture, banana[i].position.x, banana[i].position.y, WHITE);
            }

            //Drawing Grapes
            for(int i=0; i<grapes.size(); i++)
            {
                if(!vanish_grapes[i])
                    DrawTexture(grapesTexture, grapes[i].position.x, grapes[i].position.y, WHITE);
            }

            //Drawing fruitcount
            char numberText[16]; 
            snprintf(numberText, sizeof(numberText), "Fruits: %d", fruitcount);
            DrawText(numberText, 10, 10, 20, DARKBLUE);


            EndDrawing(); 
        }
        if(current_level == 2)
        {
            ///LEVEL 2

            if(endd)
            {
                current_level = 3;
                p.position = {0, 583};
                continue;
            }

            if(p.position.x >= screenWidth - playerTexture.width)
            {
                current_level = 0;
                p.position = {0, 583};
                continue;
            }

            cout << p.position.x << "  " << p.position.y << endl;
            backgroundImage = LoadImage("enback.png");
            backgroundTexture = LoadTextureFromImage(backgroundImage);
            UnloadImage(backgroundImage);

            players_ground = (screenHeight - playerTexture.height)/p.position.y;

            if(p.position.x >= screenWidth - playerTexture.width)
            {
                current_level = 3;
                continue;
            }
            if(p.position.y <= 100) {jump_up = 0, jump_down = 1;}
            if(players_ground <= 1.2) { jump_down = 0;}

            if(jump_up) { p.position.y -= 10;}
            if(jump_down) { p.position.y += 15;}

            if (IsKeyUp(KEY_RIGHT) && p.position.x < screenWidth - playerTexture.width) p.move_right();
            if (IsKeyDown(KEY_LEFT) && p.position.x > 0) p.move_left();
            if(IsKeyPressed(KEY_UP)) p.jump();
            if (IsKeyPressed(KEY_B) && fruitcount)
            {
                PlaySound(bullet_sound);
                bullets.push_back(Bullet{p.position});
                fruitcount--;
            }

            //Strawberry
            Rectangle tempA, tempB;
            bool vanishStraw = 0;
            bool playgain = 0;
            tempA = {p.position.x, p.position.y, p.playerSize.width, p.playerSize.height};
            tempB = {1400, 400, strawTexture.width, strawTexture.height};
            bool gain = p.collide(tempA, tempB);
            if(gain && !playgain)
            {
                PlaySound(fruitsound);
                vanishStraw = 1;
                playgain = 1;
            }

            //for enemy
            //pumpkin
            for(int i = 0; i<pumpkin_enemies.size()-1; i++)
                pumpkin_enemies[i].attack();

            //bird
            for(int i=0; i<bird_enemies.size(); i++)
                bird_enemies[i].attack();

            //Collision with pumpkin
            for(int i = 0; i < pumpkin_enemies.size(); i++)
            {
                if(!vanish_pumpkin[i]){
                    bool die = pumpkin_enemies[i] + p;
                    if(die){
                        endd = 1;
                        PlaySound(die_sound);
                    }
                }
            }

            //Collision with Bird
            for(int i = 0; i < bird_enemies.size(); i++)
            {
                if(!vanish_bird[i]){
                    Rectangle temp = {bird_enemies[i].position.x, bird_enemies[i].position.y, birdtexture.width, birdtexture.height};
                    bool die = birdVSplayer(p, temp);
                    if(die){
                        endd = 1;
                        PlaySound(die_sound);
                    }
                }
            }

            ///Checking if bullets hit pumpkins or birds
            for(int i = 0; i < bullets.size(); i++)
            {
                bullets[i].move();
                if(!vanish_bullet[i]){
                    Rectangle tempB = {bullets[i].position.x, bullets[i].position.y, bulletTexture.width, bulletTexture.height};
                    for(int j = 0; j<pumpkin_enemies.size(); j++)
                    {
                        bool collision = is_collision(tempB, pumpkin_enemies[j], pumpkinTexture.width, pumpkinTexture.height);
                        if(collision && !vanish_pumpkin[j]){
                            vanish_pumpkin[j] = true;
                            vanish_bullet[i] = true;
                            break;
                        }
                    }
                    if(!vanish_bullet[i]){
                        for(int j = 0; j<bird_enemies.size(); j++)
                        {
                            bool collision = is_collision(tempB, bird_enemies[j], birdtexture.width, birdtexture.height);
                            if(collision && !vanish_bird[j]){
                                vanish_bird[j] = true;
                                vanish_bullet[i] = true;
                                break;
                            }
                        }
                    }
                }
            }
            

            BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawTexturePro(backgroundTexture,
                            { 0, 0, backgroundTexture.width, backgroundTexture.height }, // Source rectangle
                            { 0, 0, screenWidth, screenHeight },         // Destination rectangle
                            { 0, 0 },                                     // Origin (rotation pivot)
                            0.0f,                                          // Rotation angle (unused)
                            WHITE);                                        // Tint color
            //Drawing player
            DrawTexture(playerTexture, static_cast<int>(p.position.x),
                        static_cast<int>(p.position.y), WHITE);

            //Drawing Strawberry;
            if(!vanishStraw)
                DrawTexture(strawTexture, static_cast<int>(1400),
                        static_cast<int>(400), WHITE);
            //drawing pumpkin
            for(int i = 0; i < pumpkin_enemies.size(); i++){
                if(!vanish_pumpkin[i]){
                DrawTexture(pumpkinTexture, static_cast<int>(pumpkin_enemies[i].position.x),
                            static_cast<int>(pumpkin_enemies[i].position.y), WHITE);
                }
            }
            //drawing bird
            for(int i = 0; i < bird_enemies.size(); i++){
                if(!vanish_bird[i]){
                    DrawTexture(birdtexture, static_cast<int>(bird_enemies[i].position.x),
                                static_cast<int>(bird_enemies[i].position.y), WHITE);
                }

            }

            //Drawing the Bullet
            for(int i = 0; i < bullets.size(); i++){
                if(!vanish_bullet[i]){
                    DrawTexture(bulletTexture, static_cast<int>(bullets[i].position.x),
                                static_cast<int>(bullets[i].position.y), WHITE);
                }
            }


            EndDrawing();
        }

        if(current_level == 3)
        {
            if(IsKeyPressed(KEY_ENTER))
            {
                reset();
                pumpkinset(pumpkinTexture.width, pumpkinTexture.height);
                birdset(birdtexture.width, birdtexture.height);
                current_level = 1;
                continue;
            }
            backgroundImage = LoadImage("glow.png");
            backgroundTexture = LoadTextureFromImage(backgroundImage);
            UnloadImage(backgroundImage);
            BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawTexturePro(backgroundTexture,
                            { 0, 0, backgroundTexture.width, backgroundTexture.height }, 
                            { 0, 0, screenWidth, screenHeight },        
                            { 0, 0 },                                     
                            0.0f,                                          
                            WHITE); 
            EndDrawing(); 
        }
    }

    // Cleanup
    UnloadTexture(backgroundTexture);
    UnloadTexture(playerTexture);
    UnloadTexture(bananaTexture);
    UnloadTexture(appleTexture);
    UnloadSound(fruitsound); 
    CloseWindow();
    CloseAudioDevice(); 

    return 0;
}


