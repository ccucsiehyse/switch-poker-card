#include <stdio.h>
#include "./SDL2/SDL.h"
#include "./SDL2/SDL_image.h"
#undef main

int sequence[5]={0,1,2,3,4};
int picture_xywh[5][4]={{275, 247, 150, 225},
                      {475, 247, 150, 225},
                      {675, 247, 150, 225},
                      {875, 247, 150, 225},
                      {1075, 247, 150, 225}};

void set_picture(int x, int y, int w, int h, SDL_Renderer* renderer, SDL_Texture* imageTexture){
    SDL_Rect destRect;
    destRect.x = x;  // x-coordinate of the destination rectangle
    destRect.y = y;  // y-coordinate of the destination rectangle
    destRect.w = w;  // width of the destination rectangle
    destRect.h = h;  // height of the destination rectangle

    // Copy texture to renderer
    SDL_RenderCopy(renderer, imageTexture, NULL, &destRect);
}

void print_poker(SDL_Renderer* renderer, SDL_Texture* imageTexture[], int unprint){
    SDL_RenderClear(renderer);
    set_picture(0, 0, 1500, 720, renderer, imageTexture[5]); //background
    if(unprint!=0) set_picture(picture_xywh[0][0], picture_xywh[0][1], picture_xywh[0][2], picture_xywh[0][3], renderer, imageTexture[sequence[0]]);
    if(unprint!=1) set_picture(picture_xywh[1][0], picture_xywh[1][1], picture_xywh[1][2], picture_xywh[1][3], renderer, imageTexture[sequence[1]]);
    if(unprint!=2) set_picture(picture_xywh[2][0], picture_xywh[2][1], picture_xywh[2][2], picture_xywh[2][3], renderer, imageTexture[sequence[2]]);
    if(unprint!=3) set_picture(picture_xywh[3][0], picture_xywh[3][1], picture_xywh[3][2], picture_xywh[3][3], renderer, imageTexture[sequence[3]]);
    if(unprint!=4) set_picture(picture_xywh[4][0], picture_xywh[4][1], picture_xywh[4][2], picture_xywh[4][3], renderer, imageTexture[sequence[4]]);
}

void change_sequence(int change_num, int place){
    int tmp=sequence[change_num];
    if(change_num>=place) for(int i=change_num; i>place; i--) sequence[i]=sequence[i-1];
    else for(int i=change_num; i<place; i++) sequence[i]=sequence[i+1];
    sequence[place]=tmp;
}

int main(int argc, char** argv) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Failed to initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize SDL_image
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        fprintf(stderr, "Failed to initialize SDL_image: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    // Create window
    SDL_Window* window = SDL_CreateWindow("Poker game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1500, 720, SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Load image as texture
    char *picture_name[6]={"./picture/spade_A.png", "./picture/spade_3.png", "./picture/spade_7.png", "./picture/spade_9.png", "./picture/spade_10.png", "./picture/background.png"};
    SDL_Texture* imageTexture[6];
    for(int i=0; i<6; i++){
        imageTexture[i] = IMG_LoadTexture(renderer, picture_name[i]);
        if (!imageTexture[i]) {
            fprintf(stderr, "Failed to load image: %s\n", IMG_GetError());
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            IMG_Quit();
            SDL_Quit();
            return 1;
        }
    }

    // Clear renderer
    SDL_RenderClear(renderer);

    //init
    print_poker(renderer, imageTexture, -1);

    // Update renderer
    SDL_RenderPresent(renderer);

    // Wait for user to quit
    SDL_Event event;
    int mouse_status=0/*buttonUp=0, buttonDown=1*/, choose=-1 , mouse_x, mouse_y, mx_origin, my_origin; 
    while (SDL_WaitEvent(&event)) {
        if (event.type == SDL_QUIT) {
            break;
        }
        if(event.type==SDL_MOUSEMOTION) SDL_GetMouseState(&mouse_x, &mouse_y);
        if(event.type == SDL_MOUSEBUTTONDOWN) mouse_status=1;
        else if(event.type == SDL_MOUSEBUTTONUP){
            mouse_status=0;
            if(choose!=-1){
                if(mouse_x-mx_origin<picture_xywh[0][0]) change_sequence(choose, 0);
                else{
                    for(int i=4; i>=0; i--){
                        if(mouse_x-mx_origin>picture_xywh[i][0]){
                            change_sequence(choose, choose<=i ? i : i+1);
                            break;
                        }
                    }
                }
                print_poker(renderer, imageTexture, -1);
                choose=-1;
            }
            
        }
        if(mouse_status==1 && choose==-1){
            for(int i=0; i<5; i++){
                if(mouse_y>=picture_xywh[0][1] && mouse_y<=picture_xywh[0][1]+picture_xywh[0][3]){
                    if(mouse_x>=picture_xywh[i][0] && mouse_x<=picture_xywh[i][0]+picture_xywh[0][2]){
                        choose=i;
                        mx_origin=mouse_x-picture_xywh[i][0];
                        my_origin=mouse_y-picture_xywh[i][1];
                        break;
                    }
                }
            }
        }
        if(choose>=0 && choose<=4){
            print_poker(renderer, imageTexture, choose);
            set_picture(mouse_x-mx_origin, mouse_y-my_origin, picture_xywh[0][2], picture_xywh[0][3], renderer, imageTexture[sequence[choose]]);
        }
        SDL_RenderPresent(renderer);
    }

    // Destroy texture, renderer, and window
    for(int i=0; i<6; i++) SDL_DestroyTexture(imageTexture[i]);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    // Quit SDL_image and SDL
    IMG_Quit();
    SDL_Quit();

    return 0;
  }