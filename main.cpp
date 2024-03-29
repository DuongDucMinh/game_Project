#include <queue>
#include <cstdlib> // for rand() and srand()
#include <ctime>   // for time()
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include "graphics.h"
#include "def.h"
#include "game.h"
#include "column.h"
#include "Collition.h"

SDL_Renderer* Graphics::renderer = nullptr;
SDL_Texture* Graphics::col1 = nullptr;
SDL_Texture* Graphics::col2 = nullptr;

std::queue<Column*> nyc;

using namespace std;

void waitUntilKeyPressed()
{
    SDL_Event e;
    while (true) {
        if ( SDL_PollEvent(&e) != 0 &&
             (e.type == SDL_KEYDOWN || e.type == SDL_QUIT) )
            return;
        SDL_Delay(100);
    }
}

int main(int argc, char *argv[])
{
    Graphics graphics;
    graphics.init();

    // KHỞI TẠO NHẠC CHƠI GAME
    //Mix_Music *gMusic = graphics.loadMusic("assets\\RunningAway.mp3");
    //graphics.play(gMusic);

    // KHỞI TẠO TIẾNG KÊU KHI NHẢY
    Mix_Chunk *gJump = graphics.loadSound("sound\\flappy_whoosh.mp3");

    // KHỞI TẠO TIẾNG TING KHI CHIM QUA CỘT
    Mix_Music *fpass = graphics.loadMusic("sound\\sound_ting.mp3");

    // KHỞI TẠO HẠT GIỐNG NGẪU NHIÊN
    srand(time(NULL));

    // KHỞI TẠO VỊ TRÍ BAN ĐẦU CON CHIM HIỆN RA
    Mouse mouse(SCREEN_WIDTH / 3, SCREEN_HEIGHT / 3);

    // KHỞI TẠO CỘT
    Column* colu = new Column();


    // KHAI BÁO KHỞI TẠO NỀN TRÔI
    ScrollingBackground background;
    background.setTexture(graphics.loadTexture("picture\\background_night.png"));

    // KHAI BÁO CÁI CỘT
    Graphics::col1 = graphics.loadTexture("picture\\pipe_up.png");
    Graphics::col2 = graphics.loadTexture("picture\\pipe_down.png");
    Uint32 startTime = SDL_GetTicks();

    // KHAI BÁO CON CHIM
    Sprite flappy_bird;
    SDL_Texture* FL_Bird_Texture = graphics.loadTexture("picture\\bird.png");
    flappy_bird.init(FL_Bird_Texture, FL_BIRD_FRAMES, FL_BIRD_CLIPS);


    bool quit = false;
    SDL_Event event;

    while (!quit && !gameOver1(mouse)) {

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) quit = true;
        }

        const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

        mouse.turnSouth();
        mouse.move();

        if (currentKeyStates[SDL_SCANCODE_UP] || currentKeyStates[SDL_SCANCODE_SPACE]) {
            graphics.play(gJump);
            mouse.jump();
            mouse.move();
        }

        // TẠO NỀN TRÔI
        background.scroll(1);
        graphics.render_back(background);


        // TẠO CỘT
        Uint32 currentTime = SDL_GetTicks();
        Uint32 elapsedTime = currentTime - startTime;
        if (elapsedTime > delayTime) {
            colu = new Column();
            colu->destRect1.y = rand() % 165 - 245;
            colu->destRect2.y = colu->destRect1.y + 500;
            nyc.push(colu);
            startTime = currentTime;
        }

        while (!nyc.empty()) {
            nyc.front()->update();
            if (mouse.touch(nyc.front())) quit = true; //if (gameOver2(mouse, )) quit = true;
            nyc.front()->render();

            if (nyc.front()->destRect1.x + COLUMN_WIDTH < 0) {
                nyc.pop();
            } else {
                break;
            }
        }

        // TIENG TING KHI CHIM QUA COT - chưa làm đc
        if ((nyc.front()->destRect1.x ) == 116) graphics.play(fpass);

        // TẠO CON CHIM
        flappy_bird.tick();
        graphics.render( mouse.rect.x , mouse.rect.y , flappy_bird);

        graphics.presentScene();
        SDL_Delay(15);
    }

    //if (gMusic != nullptr) Mix_FreeMusic( gMusic );
    if (gJump != nullptr) Mix_FreeChunk( gJump);
    if (fpass != nullptr) Mix_FreeMusic(fpass);

    // PHÁ HỦY CON CHIM
    SDL_DestroyTexture ( FL_Bird_Texture );
    FL_Bird_Texture = nullptr;

    // PHÁ HỦY NỀN TRÔI
    SDL_DestroyTexture( background.texture );

    graphics.quit();
    return 0;
}






