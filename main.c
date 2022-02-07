// Cengiz Cimen MONKEY GAME OOOH OOOH AHHH AHHH

#include "include/raylib.h"
#include "include/queue.h"
#include "include/level.h"
#include "include/hashmap.h"
#include "raylib-aseprite.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

// Raylib aseprite implementation <- poggers library btw
#define RAYLIB_ASEPRITE_IMPLEMENTATION
#include "include/raylib-aseprite.h"

#define RAYGUI_IMPLEMENTATION
#include "include/raygui.h"

#define GUI_SIMPLE_GUI_IMPLEMENTATION
#include "include/gui.h"

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif

// Change the following for debugging purposes
#define ACTIVE_ENEMIES 0    // change to 1 to enable enemies
#define CHUNK_DEBUG 1       // change to see all chunks


int main(void)
{
    //----------------------------------------------------
    /////////////      INITIALISATION      ///////////////
    //----------------------------------------------------

    const int WIDTH = 1920, HEIGHT = 1080;                          //
    InitWindow(WIDTH, HEIGHT, "MONKEY");                            //
    SetWindowState(FLAG_MSAA_4X_HINT | FLAG_WINDOW_UNDECORATED | FLAG_VSYNC_HINT);
    SetTargetFPS(60);                                               // Initialise the window

    int tex_index = 0;                          // the current texture index for the paint tool
    uint8_t puddle_index = 0;                   // keep track of current puddles - limits size
    uint8_t dialog_enab = 0;                    // disable and enable updating/rendering of dialogue
    uint8_t collision_edit_mode = 0;

    Camera2D mainCam = {{0,0}, {0,0}, 0, 1};    // mainCam name used in macros: do not change name
    Circle puddles[6] = {0};                    // array for circle puddles

    struct hashmap_s tilemap;                   //
    hashmap_create(8, &tilemap);                // 
    RetrieveTilemap(&tilemap);                  // tilemap for every tile and it's indexed texture

    Aseprite tilesheet = LoadAseprite("resources/aseprite/TileSheet.aseprite");  // load the raw aseprite tilesheet
    Texture2D tiletex = GetAsepriteTexture(tilesheet);                  // load the texture from the aseprite (need to draw parts of ase)
    Texture2D player_dialog = LoadTexture("resources/images/why3.png");

    RenderTexture2D target_rtxt = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());   // target (main) render texture for the game
    RenderTexture2D dialog_rtxt = LoadRenderTexture(GetScreenWidth(), 400);
    RenderTexture2D water_rtxt = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    RenderTexture2D coll_rtxt = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    GuiSimpleGuiState gui = InitGuiSimpleGui(); // GUI initialisation


    // Experimental shaders ------ YAY IT WORKS :)))))) -- TEMP
    // Red outline shader
    Shader outline_shader = LoadShader(0, "resources/shaders/outline.fs");
    // Outline Color
    Vector4 color = {255.0, 0.0, 0.0,255.0};
    int colorIndex = GetShaderLocation(outline_shader, "color");
    SetShaderValue( outline_shader, colorIndex, &color, SHADER_UNIFORM_VEC4 );
    // Thickness
    float size = 0.2;
    int thickness = GetShaderLocation(outline_shader, "thickness");
    SetShaderValue( outline_shader, thickness, &size, SHADER_UNIFORM_FLOAT );

    // Water shader
    Shader water_shader = LoadShader(0, TextFormat("resources/shaders/glsl%i/wave.fs", GLSL_VERSION));
    int secondsLoc = GetShaderLocation(water_shader, "secondes");
    int freqXLoc = GetShaderLocation(water_shader, "freqX");
    int freqYLoc = GetShaderLocation(water_shader, "freqY");
    int ampXLoc = GetShaderLocation(water_shader, "ampX");
    int ampYLoc = GetShaderLocation(water_shader, "ampY");
    int speedXLoc = GetShaderLocation(water_shader, "speedX");
    int speedYLoc = GetShaderLocation(water_shader, "speedY");

    // Shader uniform values that can be updated at any time
    float freqX = 25.0f;
    float freqY = 25.0f;
    float ampX = 1.0f;
    float ampY = 1.0f;
    float speedX = 2.0f;
    float speedY = 2.0f;

    float screenSize[2] = { (float)GetScreenWidth(), (float)GetScreenHeight() };
    SetShaderValue(water_shader, GetShaderLocation(water_shader, "size"), &screenSize, SHADER_UNIFORM_VEC2);
    SetShaderValue(water_shader, freqXLoc, &freqX, SHADER_UNIFORM_FLOAT);
    SetShaderValue(water_shader, freqYLoc, &freqY, SHADER_UNIFORM_FLOAT);
    SetShaderValue(water_shader, ampXLoc, &ampX, SHADER_UNIFORM_FLOAT);
    SetShaderValue(water_shader, ampYLoc, &ampY, SHADER_UNIFORM_FLOAT);
    SetShaderValue(water_shader, speedXLoc, &speedX, SHADER_UNIFORM_FLOAT);
    SetShaderValue(water_shader, speedYLoc, &speedY, SHADER_UNIFORM_FLOAT);

    //----------------------------------------------------
    ////////////////      MAIN LOOP      /////////////////
    //----------------------------------------------------

    while (!WindowShouldClose())
    {   
        float seconds = GetTime();
        SetShaderValue(water_shader, secondsLoc, &seconds, SHADER_UNIFORM_FLOAT);

        //----------------------------------------------------
        // The following are simply tools and shortcuts to edit the world when NOT interacting with a GUI
        // Read the respective comments below for more information
        if (!GuiMouseCollision(gui.layoutRecs, 5)) {   
            // Simple zoom using the scroll wheel
            if (GetMouseWheelMove()) mainCam.zoom += GetMouseWheelMove()/20;
            
            // Pan by holding down the middle mouse button and draggin
            if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
                mainCam.target.x -= GetMouseDelta().x * 10;
                mainCam.target.y -= GetMouseDelta().y * 10;
            }
            
            // Texture dropper and (other) texture selection <== very very useful TO-DO: an accompying texture viewer in the GUI
            if (IsKeyDown(KEY_LEFT_ALT) && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) tex_index = GetIndexOfCell(xChunk, yChunk, xtile, ytile, &tilemap);
            // Change the currently selected tile using the right and left arrow keys
            if (IsKeyPressed(KEY_RIGHT)) tex_index++;
            else if (IsKeyPressed(KEY_LEFT)) tex_index--;
            if (tex_index < 0) tex_index = tilesheet.ase->slice_count;
            if (tex_index > tilesheet.ase->slice_count) tex_index = 0;
            
            // Paint a tile with the current texture, adds a new chunk to the file if it doesn't already exist
            else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                NewChunk(xChunk, yChunk, &tilemap);
                Chunk *selected = ReturnChunk(xChunk, yChunk, &tilemap);
                if (collision_edit_mode && selected != NULL) selected->tiles[xtile][ytile].solid = 1;
                else if (GetIndexOfCell(xChunk, yChunk, xtile, ytile, &tilemap) == -1) selected->active++;
                selected->tiles[xtile][ytile].index = tex_index + 1;
            }
            
            // Deletes the chunk from the level file
            if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && IsKeyPressed(KEY_LEFT_CONTROL)) {
                RemoveChunk(xChunk, yChunk, &tilemap);
            }

            // Removes the currently painted tile
            else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                Chunk *selected = ReturnChunk(xChunk, yChunk, &tilemap);
                if (selected != NULL && collision_edit_mode) selected->tiles[xtile][ytile].solid = 0;
                else if (selected != NULL && (GetIndexOfCell(xChunk, yChunk, xtile, ytile, &tilemap) != -1)) {
                    selected->tiles[xtile][ytile].index = 0;
                    selected->active--;
                    if (!selected->active) {
                        RemoveChunk(xChunk, yChunk, &tilemap);
                    }    
                }
            }

            // SHORTCUTS FOR DEBUGGING -- TEMP
            // Water ripple
            if (IsKeyPressed(KEY_P) && GetIndexOfCell(xChunk, yChunk, xtile, ytile, &tilemap) == 3) {
                puddles[puddle_index].radius = 1;
                puddles[puddle_index].center = MOUSE_WORLD_VEC;
                puddle_index ++;
                if (puddle_index > 5) puddle_index = 0;
            }
            // Enables/disables dialogue
            if (IsKeyDown(KEY_D)? (dialog_enab = 1) : (dialog_enab = 0));

            if (IsKeyPressed(KEY_C)) {
                if (collision_edit_mode ? (collision_edit_mode = 0) : (collision_edit_mode = 1));
                if (collision_edit_mode ? (printf("Collisions mode\n")) : (printf("Paintbrush mode\n")));
            }
            if (IsKeyPressed(KEY_SPACE)) {
                Chunk *selected = ReturnChunk(xChunk, yChunk, &tilemap);
                    if (selected != NULL) {
                    if (selected->tiles[xtile][ytile].solid ? (printf("Tile is solid\n")):(printf("Tile is not solid\n")));
                } 
            }
        }

        // The following is for updating values before drawing
        // TO-DO: add proper GUI interaction
        UpdatePuddles(puddles);
    
        //----------------------------------------------------
        ////////////////     DRAW CALLS      /////////////////
        //----------------------------------------------------
        BeginDrawing();                 // BEGIN drawing the scene
        {
            BeginTextureMode(target_rtxt);  // DRAW the GAME world to the (main) rtxt
            {
                ClearBackground(BLANK);     // Create a BLANK canvas
                BeginMode2D(mainCam);       // BEGIN 2D mode
                    // DRAW all the surrounding chunks that around the mouse coordinates - saves on draw calls
                    for (int x = xChunk - 6; x < xChunk + 6; x++) {
                        for (int y = yChunk - 6; y < yChunk + 6; y++) {
                            DrawChunk(x, y, &tilemap, &tilesheet, &tiletex);                  
                        }
                    }
                EndMode2D();                // END 2D mode

            }
            EndTextureMode();               // END drawing to the (main) rtxt
            
            BeginTextureMode(dialog_rtxt);
            {   
                if (dialog_enab) {
                    ClearBackground(BLACK);
                    DrawTextureEx(player_dialog, Vector2(0,0), 0, 4.1,WHITE);
                    DrawText("Raylib is cool.", 430, 150, 70, WHITE);
                    DrawRectangleLinesEx(Rectangle(0,0,400,400), 10, WHITE);
                }                  
                else ClearBackground(BLANK);
            }
            EndTextureMode();
            
            BeginTextureMode(water_rtxt);
            {
                ClearBackground(BLANK);     // Create a BLANK canvas
                BeginMode2D(mainCam);       // BEGIN 2D mode

                // DRAW all the surrounding chunks that around the mouse coordinates - saves on draw calls
                for (int x = xChunk - 6; x < xChunk + 6; x++) {
                    for (int y = yChunk - 6; y < yChunk + 6; y++) {
                        DrawChunkWaterLayer(x, y, &tilemap, &tilesheet, &tiletex);                  
                    }
                }
                // DRAW puddle ripples if there are any
                for (int i = 0; i < 6; i++) {
                    if (puddles[i].radius) DrawCircleLines(puddles[i].center.x,puddles[i].center.y,puddles[i].radius, BLACK);
                }
                EndMode2D();  
                
            }
            EndTextureMode();

            BeginTextureMode(coll_rtxt); 
            {
                ClearBackground(BLANK);     // Create a BLANK canvas
                BeginMode2D(mainCam);       // BEGIN 2D mode

                // DRAW all the surrounding chunks that around the mouse coordinates - saves on draw calls
                for (int x = xChunk - 6; x < xChunk + 6; x++) {
                    for (int y = yChunk - 6; y < yChunk + 6; y++) {
                        DrawChunkCollLayer(x, y, &tilemap, &tilesheet, &tiletex);    
                    }
                }
                EndMode2D();   
            } 
            EndTextureMode();
                
                ClearBackground(BLACK);
                BeginShaderMode(outline_shader); {
                    DrawTextureRec(target_rtxt.texture, Rectangle(0,0,GetScreenWidth(),-GetScreenHeight()), Vector2(0,0), WHITE);
                } EndShaderMode();
                BeginShaderMode(water_shader); {
                    DrawTextureRec(water_rtxt.texture, Rectangle(0,0,GetScreenWidth(),-GetScreenHeight()), Vector2(0,0), WHITE);
                } EndShaderMode();
                
                //DrawTextureRec(target_rtxt.texture, Rectangle(0,0,GetScreenWidth(),-GetScreenHeight()), Vector2(0,0), WHITE); // DRAW the (main) rtxt
                //DrawTextureRec(water_rtxt.texture, Rectangle(0,0,GetScreenWidth(),-GetScreenHeight()), Vector2(0,0), WHITE);
                
                BeginMode2D(mainCam);
                    // DRAW the outlines of the current chunk and cell - visual aid
                    DrawCellOutline(xCell, yCell, mainCam);
                    DrawChunkOutline(xChunk, yChunk, mainCam);
                EndMode2D();
                

                if (collision_edit_mode) (DrawTextureRec(coll_rtxt.texture, Rectangle(0,0,GetScreenWidth(), -GetScreenHeight()), Vector2(0,0), WHITE));

                DrawTextureRec(dialog_rtxt.texture, Rectangle(0,0,GetScreenWidth(),-400), Vector2(0,0), WHITE);


                //DrawFPS(100, 100);          // Display the FPS - simple performance observation
                GuiSimpleGui(&gui);         // Overlay the GUI - TO-DO: add proper GUI interaction
                DrawTextureRec( tiletex,    
                        (Rectangle){
                            tilesheet.ase->slices[tex_index].origin_x,
                            tilesheet.ase->slices[tex_index].origin_y,
                            TILE_SIZE,TILE_SIZE}, 
                        (Vector2){
                            1700, 
                            330}, 
                        GRAY
                );                          // DRAW the (grayed out) current tile texture that is being used


            }
        EndDrawing();                   // END drawing the scene
    }
  
    //----------------------------------------------------
    //////////////    DE-INITIALISATION      /////////////
    //----------------------------------------------------

    StoreTilemap(&tilemap);
    hashmap_destroy(&tilemap);
    UnloadTexture(tiletex);
    UnloadRenderTexture(target_rtxt);
    UnloadRenderTexture(dialog_rtxt);
    UnloadRenderTexture(water_rtxt);
    UnloadRenderTexture(coll_rtxt);
    UnloadAseprite(tilesheet);
    UnloadTexture(player_dialog);

    // Experimental
    UnloadShader(outline_shader);
    UnloadShader(water_shader);

    CloseWindow(); // Close the window

    return 0;
}
