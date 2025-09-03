#include <raylib.h>
#include <iostream>
using namespace std;

class Platform {
public:
    bool dead;
    bool ready;
    struct PlatData {
        float height, x;
    };
    Texture2D Pipe;
    Texture2D Extend;
    Texture2D Flipped;
    Texture2D OldTent;
    PlatData arr[6];
    Rectangle piperect[6];

    void updateCollision() {
        for (int i = 0; i < 3; i++) {
            piperect[i * 2]     = { (float)(arr[i].x + 5), (float)(arr[i].height + 230), 60, 320 };
            piperect[i * 2 + 1] = { (float)(arr[i].x + 5), (float)(arr[i].height - 915), 60, 960 };
        }
    }

    void init() {
        for (int i = 0; i < 3; i++) {
            arr[i].height = GetRandomValue(10, 400);
            arr[i].x = i * 250 + 500;
        }
    }

    void draw() {
        for (int i = 0; i < 3; i++) {
            DrawTextureEx(Extend, { arr[i].x + 67, arr[i].height - 320 }, 180.0f, 1.3f, WHITE);
            DrawTextureEx(OldTent, { arr[i].x, 1024 - (800 - arr[i].height - 320) }, 0.0f, 1.3f, WHITE);
            DrawTextureEx(Flipped, { arr[i].x + 67, arr[i].height + 50 }, 180.0f, 1.3f, WHITE);
            DrawTextureEx(Pipe, { arr[i].x, 1024 - (800 - arr[i].height) }, 0.0f, 1.3f, WHITE);
        }
    }

    void update() {
        for (int i = 0; i < 3; i++) {
            if (!dead && !ready) arr[i].x -= 2.5;
            if (arr[i].x < -150) {
                arr[i].x = 576;
                arr[i].height = GetRandomValue(200, 500);
            }
        }
    }
};

class Player {
public:
    Texture2D bird, up, down, play, gameover;
    Sound wing, point, hit, die;
    double x, y, width = 60, height = 60, velocity, gravity, rotation;
    int frame = 0;
    bool dead = false;
    bool ready = true;

    void draw() {
        int speed = 5;
        int count = (frame / speed) % 3;
        Texture2D currentTexture = (dead) ? down : (count == 0 ? up : (count == 1 ? bird : down));
        frame++;

        DrawTexturePro(
            currentTexture,
            { 0, 0, (float)currentTexture.width, (float)currentTexture.height },
            { (float)GetScreenWidth() / 2, (float)y, currentTexture.width * 1.6f, currentTexture.height * 1.6f },
            { currentTexture.width * 1.6f / 2, currentTexture.height * 1.6f / 2 },
            rotation,
            WHITE
        );
    }

    void update() {
        if (ready == false) {
            velocity += gravity;
            y += velocity;
        }

        if (velocity > 0 && velocity < 5) rotation += velocity / 5;
        else if (velocity < 0) rotation += velocity;
        else if (velocity > 0) rotation += velocity / 3;

        if (rotation > 90) rotation = 90;
        if (rotation < -45) rotation = -45;

        if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_SPACE)) && !ready && !dead) {
            PlaySound(wing);
            velocity = -10;
        }

        if (y >= 780) y = 780;
    }
};

Platform Plat;
Player player;

int main() {
    InitWindow(576, 1024, "Flappy Bird Raylib");
    Image icon = LoadImage("favicon.ico");
    SetWindowIcon(icon);
    SetTargetFPS(60);
    InitAudioDevice();

    int points = 0;
    int high = 0;
    int hightemp = 0;
    double xval = 0;
    int waitval = 0;
    bool finaljump = true;
    bool ready = true;
    
    double startvel = -10;

    player.bird = LoadTexture("assets/bluebird.png");
    player.up = LoadTexture("assets/blueup.png");
    player.down = LoadTexture("assets/bluedown.png");
    player.play = LoadTexture("assets/playbutton.png");
    player.gameover = LoadTexture("assets/gameover.png");
    player.wing = LoadSound("audios/wing.wav");
    player.point = LoadSound("audios/point.ogg");
    player.hit = LoadSound("audios/hit.wav");
    player.die = LoadSound("audios/die.ogg");

    Plat.Pipe = LoadTexture("assets/pipe.png");
    Plat.Flipped = LoadTexture("assets/flipped.png");
    Plat.OldTent = LoadTexture("assets/oldtend.png");
    Plat.Extend = LoadTexture("assets/extender.png");

    Texture2D bg = LoadTexture("assets/bg.png");
    Texture2D base = LoadTexture("assets/base.png");
    Texture2D Readdy = LoadTexture("assets/GetReady.png");

    Font Flappy = LoadFont("fonts/flappy.ttf");

    Image favicon = LoadImage("favicon.png");
    SetWindowIcon(favicon);

    player.x = GetScreenWidth() / 2 - 107;
    player.y = GetScreenHeight() / 2;
    player.velocity = 10;
    player.gravity = 0.5;
    player.rotation = 90;

    Plat.dead = false;
    Plat.ready = true;

    Plat.init();

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(WHITE);

        DrawTextureEx(bg, { 0, 0 }, 0.0f, 2.0f, WHITE);

        player.update();
        Plat.draw();
        Plat.update();
        Plat.updateCollision();

        // collision box DrawRectangle(player.x + 85, player.y - 22, player.width - 15, player.height - 15, WHITE );
        Rectangle birdRect = { (float)player.x + 85, (float)player.y - 22, (float)player.width - 15, (float)player.height - 15 };

        for (int i = 0; i < 6; i++) {
            if (CheckCollisionRecs(birdRect, Plat.piperect[i]) || player.y >= 780) {
                if (finaljump) {
                    PlaySound(player.hit);
                    player.velocity = -10;
                    finaljump = false;
                }
                player.dead = true;
                Plat.dead = true;
            }
        }



        if (player.dead && Plat.dead) {
            DrawTextPro(Flappy, "PRESS C TO CONTINUE", (Vector2){90, (float)GetScreenHeight()/2}, (Vector2){40, 40}, 0, 45, 2, WHITE);
            DrawTextureEx(player.gameover, { GetScreenWidth() / 2 - (player.gameover.width * 3.0f) / 2, (float)GetScreenHeight() / 2 - 175 }, 0.0f, 3.0f, WHITE);

            high = points;


            if (IsKeyPressed(KEY_C)) {
                points = 0;
                Plat.init();
                player.y = GetScreenHeight() / 2;
                player.velocity = -10;
                player.dead = false;
                Plat.dead = false;
                finaljump = true;
            }
        }

        for (int i = 0; i < 6; i++) {
            if (birdRect.x > Plat.piperect[i].x + 10 && birdRect.x < Plat.piperect[i].x + 20) {
                PlaySound(player.point);
                waitval++;
                if (waitval == 8) {
                    points++;
                    waitval = 0;
                }
            }
        }

        if (!player.dead && ready == false) xval -= 3.5;

        DrawTextureEx(base, { (float)xval, 800 }, 0.0f, 2.0f, WHITE);
        if (xval <= -49) xval = 0;

        if (ready == true) {
            DrawTextureEx(Readdy, { (float)GetScreenWidth() / 2 - 161, (float)GetScreenHeight() / 2 - 140 }, 0.0f, 1.75f, WHITE);


            player.y += startvel;

            if (player.y > 524) {
                startvel = -1;
            } else if (player.y < 495) {
                startvel = 1;
                
            }



            // player.y is 512

            player.rotation = 0;
            if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_SPACE))) {
                ready = false;
                player.ready = false;
                Plat.ready = false;
                PlaySound(player.wing);
                player.velocity = -10;
            }
            
        }

        player.draw();
        DrawTextPro(Flappy, to_string(points).c_str(), (Vector2){75, 70}, (Vector2){40, 40}, 0, 100, 12, WHITE);
        DrawTextPro(Flappy, "HI", (Vector2){75, 160}, (Vector2){40, 40}, 0, 30, 4, WHITE);
        DrawTextPro(Flappy, to_string(high).c_str(), (Vector2){120, 160}, (Vector2){40, 40}, 0, 30, 4, WHITE);

        DrawTextPro(Flappy, "A JOSH WALTERS PRODUCTION", (Vector2){50, 1035}, (Vector2){40, 40}, 0, 15, 2, WHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
