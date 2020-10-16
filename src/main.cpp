#include "raylib.h"
#include "escapi.h"
#include <stdio.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

struct Pixel {
  unsigned char r;
  unsigned char g;
  unsigned char b;
};

struct PixelESCAPI {
  unsigned char x;
  unsigned char r;
  unsigned char g;
  unsigned char b;
};

Texture2D createTexture(int width, int height){
    Image image = GenImageColor(width, height, BLACK); 
    ImageDrawText(&image, {width/3.0f, height/2.0f}, "Loading Webcam", 12, WHITE);
    ImageFormat(&image, UNCOMPRESSED_R8G8B8);
    Texture2D texture = LoadTextureFromImage(image);
    //free(image.data);
    return texture;
}

int main(int argc, char* argv[]){
    // Initialization
    //--------------------------------------------------------------------------------------
    int devices = setupESCAPI();

    if (devices == 0){
      printf("ESCAPI initialization failure or no devices found.\n");
      return -1;
    }

    struct SimpleCapParams capture;
    capture.mWidth = 320;
    capture.mHeight = 240;
    auto bufferSize = capture.mWidth * capture.mHeight;
    capture.mTargetBuf = new int[bufferSize];

    int screenWidth = capture.mWidth;
    int screenHeight = capture.mHeight;

    InitWindow(screenWidth, screenHeight, "Webcam");

    SetTargetFPS(60);

    Texture2D texture = createTexture(screenWidth, screenHeight);
    //--------------------------------------------------------------------------------------

    if (initCapture(0, &capture) == 0){
      printf("Capture failed - device may already be in use.\n");
      return -2;
    }

    //Request webcam capture
    doCapture(0);

    bool saveNextCapture = false;

    // Main game loop
    while (!WindowShouldClose()){    // Detect window close button or ESC key
        // Update
        if(!saveNextCapture && IsKeyPressed(KEY_ENTER)){
          saveNextCapture = true;
        }

        if(isCaptureDone(0) == 1){
          auto rgb_size = 3 * sizeof(char);

          PixelESCAPI* pixelsCamera = (PixelESCAPI*) capture.mTargetBuf;
          Pixel* pixels = (Pixel*) capture.mTargetBuf;
          for(int i = 0; i < bufferSize; i++){
            pixels[i].g = pixelsCamera[i].r;
            pixels[i].r = pixelsCamera[i].g;
            pixels[i].b = pixelsCamera[i].b;
          }

          if(saveNextCapture){
            printf("Saving capture\n");
            stbi_write_bmp("capture.bmp", capture.mWidth, capture.mHeight, 3, pixels);
            saveNextCapture = false;
          }

          UpdateTexture(texture, pixels);

          //Request a new capture, hopefully ready on the next frame     
          doCapture(0);
        }

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

          ClearBackground(WHITE);
          DrawTexture(texture, 0, 0, WHITE);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------   
    CloseWindow();        // Close window and OpenGL context
    deinitCapture(0);
    //--------------------------------------------------------------------------------------

    return 0;
}