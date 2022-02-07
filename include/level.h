// Intended for managing/encoding/decoding the level data
#pragma once

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <math.h>

#define RAYLIB_ASEPRITE_IMPLEMENTATION
#include "raylib-aseprite.h"
#include "hashmap.h"

#define max(a, b)						((a) > (b) ? (a) : (b))
#define min(a, b) 						((a) < (b) ? (a) : (b))
#define Vector2(x,y)					(Vector2) {x,y}
#define Vector3(x,y,z)					(Vector3) {x,y,z}
#define Vector4(x,y,w,h)				(Vector4) {x,y,w,h}
#define Rectangle(x,y,width,heigth)		(Rectangle) {x,y,width,heigth}
#define Color(red,green,blue,alpha)		(Color) {red,green,blue,alpha}

//////////  CUSTOM DEFINES AND MACROS
#define GRID_SIZE 32    // width and height of chunk
#define TILE_SIZE 32    // width and height of grid
#define CHUNK_WIDTH (GRID_SIZE*TILE_SIZE)
// Mouse relative to the world
#define MOUSE_WORLD_VEC GetScreenToWorld2D(GetMousePosition(), mainCam)
// get the inworld chunk relative to the target Vector
#define xChunk (floor(MOUSE_WORLD_VEC.x / CHUNK_WIDTH))
#define yChunk (floor(MOUSE_WORLD_VEC.y / CHUNK_WIDTH))
// get tue inworld cell relative to target Vector
#define xCell (floor(MOUSE_WORLD_VEC.x / TILE_SIZE))
#define yCell (floor(MOUSE_WORLD_VEC.y / TILE_SIZE))
// get the cell relative to the chunk
#define xtile (int)(xCell - xChunk * GRID_SIZE)
#define ytile (int)(yCell - yChunk * GRID_SIZE)

// Enemy related constants:
#define MAX_ENEMY 3     // Phase out for modular enemy size
#define WALKSPEED 400
#define ENEMY_DETECTION_RANGE 800
#define ENEMY_SPEED 200

////////////////// TYPE DECLARATIONS //////////////////
typedef struct Enemy {
    Rectangle rec;
    char *type;
    int counter;
    bool active;
} Enemy;

typedef struct Gun {
    float offset;
    Rectangle rec;
    Rectangle bullet;
    float scale;
    float reloadTime;
} Gun;

typedef struct Player {
    Vector2 pos;
    Aseprite ase;
    Gun gun;
} Player;

typedef struct Tile {
    int8_t index; // if -1 it is empty
    int8_t solid; // keeps track of whether tile is solid or not
    // TO-DO: add more information to tiles
} Tile;

typedef struct Chunk {
    Tile tiles[GRID_SIZE][GRID_SIZE];
    int active;
} Chunk;

typedef struct Circle {
    Vector2 center;
    float radius;
} Circle;


// The tags used for Player animations - must be adjusted
typedef enum {
    RIGHT_WALK = 0,
    LEFT_WALK = 1,
    IDLE = 2,
} playerAnimTag;

// Velocity vector
typedef Vector2 Velocity2;


//////////////////////////////  FUNCTIONS DECLARATIONS  //////////////////////////////
// Miscellaneous
int GuiMouseCollision(Rectangle *recs, int nrecs);
void UpdatePuddles(Circle circles[6]);

// Chunk data management
Chunk *ChunkInit(void);
char *ChunkKey(int x, int y);
Chunk *ReturnChunk(int x, int y, struct hashmap_s *const m);
int NewChunk(int x, int y, struct hashmap_s *const m);
int RemoveChunk(int x, int y, struct hashmap_s *const m);

void DrawChunkOutline(int x, int y, Camera2D cam);
void DrawCellOutline(int x, int y, Camera2D cam);
void DrawChunk(int x, int y, struct hashmap_s *const m, Aseprite *ase, Texture2D *text);
void DrawChunkWaterLayer(int x, int y, struct hashmap_s *const m, Aseprite *ase, Texture2D *text);
void DrawChunkCollLayer(int x, int y, struct hashmap_s *const m, Aseprite *ase, Texture2D *text);
int GetIndexOfCell(int chunkx, int chunky, int cellx, int celly, struct hashmap_s *const m);
int GetSelectedSlice(Texture2D *ttx, Camera2D texCam);

void StoreTilemap(struct hashmap_s *const hashmap);
void RetrieveTilemap(struct hashmap_s *const hashmap);

// Player data management
void storePos(Vector2 playerDat);
Vector2 getPlayerPos(void);
Enemy *FetchEnemyData(void);
void StoreEntityData(Enemy enem[MAX_ENEMY]);
Player loadPlayer(void);

///////////////////////////////////////////////////////////////////////////////////////

// QOL: Checks if the mouse is hovering over the GUI and not the world so
// as to avoid accidental interaction
int GuiMouseCollision(Rectangle *recs, int nrecs) {
    for (int i = 0; i < nrecs; i ++) {
        if (CheckCollisionPointRec(GetMousePosition(), recs[i])) return 1;
    }
    return 0;
}

// Updates the radius of the circle over frames (TO-DO: constant time)
void UpdatePuddles(Circle circles[6]) {
    for (int i = 0; i < 6; ++i) {
        if (circles[i].radius) {
            circles[i].radius++;
            if (circles[i].radius > 32) {
                circles[i].radius = 0;
            }
        }
    }
}


////////////////// CHUNK RELATED FUNCTIONS //////////////////
int GetIndexOfCell(int chunkx, int chunky, int cellx, int celly, struct hashmap_s *const m) {
    if (ReturnChunk(chunkx, chunky, m) == NULL) return -1;
    if (ReturnChunk(chunkx, chunky, m)->tiles[cellx][celly].index) return ReturnChunk(chunkx, chunky, m)->tiles[cellx][celly].index - 1;
    return -1;
}

void DrawChunkOutline(int x, int y, Camera2D cam)
{
    int thick = 2;
    if (cam.zoom < 0.8) thick = 4/cam.zoom;
    DrawRectangleLinesEx((Rectangle){x * CHUNK_WIDTH, y * CHUNK_WIDTH, 
    CHUNK_WIDTH, CHUNK_WIDTH}, thick, WHITE);
}

void DrawCellOutline(int x, int y, Camera2D cam) {
    int thick = 2;
    if (cam.zoom < 0.8) thick = 4/cam.zoom;
    DrawRectangleLinesEx((Rectangle){x * TILE_SIZE, y * TILE_SIZE, 
    TILE_SIZE, TILE_SIZE}, thick, WHITE);
}

void DrawChunk(int x, int y, struct hashmap_s *const m, Aseprite *ase, Texture2D *text) {
    if (ReturnChunk(x, y, m) == NULL) return;
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            uint8_t tileIndex = ReturnChunk(x, y, m)->tiles[i][j].index;
            if (tileIndex && tileIndex != 4) {      // Excludes the water tiles
                tileIndex--;
                DrawTextureRec( *text, 
                    (Rectangle){
                        ase->ase->slices[tileIndex].origin_x,
                        ase->ase->slices[tileIndex].origin_y,
                        TILE_SIZE,TILE_SIZE }, 
                    (Vector2){
                        x*CHUNK_WIDTH + i*TILE_SIZE, 
                        y*CHUNK_WIDTH + TILE_SIZE*j}, 
                    WHITE
                );
            }
        }
    }
}

void DrawChunkWaterLayer(int x, int y, struct hashmap_s *const m, Aseprite *ase, Texture2D *text) {
    if (ReturnChunk(x, y, m) == NULL) return;
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            uint8_t tileIndex = ReturnChunk(x, y, m)->tiles[i][j].index;
            if (tileIndex && tileIndex == 4) {      // Includes the water tiles
                tileIndex--;
                DrawTextureRec( *text, 
                    (Rectangle){
                        ase->ase->slices[tileIndex].origin_x,
                        ase->ase->slices[tileIndex].origin_y,
                        TILE_SIZE,TILE_SIZE }, 
                    (Vector2){
                        x*CHUNK_WIDTH + i*TILE_SIZE, 
                        y*CHUNK_WIDTH + TILE_SIZE*j}, 
                    WHITE
                );
            }
        }
    }
}

void DrawChunkCollLayer(int x, int y, struct hashmap_s *const m, Aseprite *ase, Texture2D *text) {
    if (ReturnChunk(x, y, m) == NULL) return;
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            int8_t coll = ReturnChunk(x, y, m)->tiles[i][j].solid;
            if (coll != 0) {      // Includes the water tiles
                DrawRectangle( x*CHUNK_WIDTH + i*TILE_SIZE, y*CHUNK_WIDTH + TILE_SIZE*j, TILE_SIZE, TILE_SIZE, (Color){ 253, 249, 0, 200 } );
            }
        }
    }
}


Chunk *ChunkInit(void) {
    Chunk *new = malloc(sizeof(Chunk));
    Chunk dat = (Chunk){ 0 };
    memcpy(new, &dat, sizeof(Chunk));
    return new;
}

char *ChunkKey(int x, int y) {
    char *key;
    __mingw_asprintf(&key, "%d %d", x, y);
    return key;
}

Chunk *ReturnChunk(int x, int y, struct hashmap_s *const m) {
    char *key = ChunkKey(x, y);
    Chunk *element = hashmap_get(m, key, strlen(key));
    free(key);
    return element;
}

int NewChunk(int x, int y, struct hashmap_s *const m) {
    if (ReturnChunk(x, y, m) != NULL) return 0;
    hashmap_put(m, ChunkKey(x, y), strlen(ChunkKey(x, y)), ChunkInit());
    return 0;
}

int RemoveChunk(int x, int y, struct hashmap_s *const m) {
    char *key = ChunkKey(x, y);
    hashmap_remove(m, key, strlen(key));
    free(key);
    return 0;
}

// TO-DO: Implement with a GUI
int GetSelectedSlice(Texture2D *ttx, Camera2D texCam) {
    int xslice = floor(GetWorldToScreen2D(GetMousePosition(), texCam).x/(TILE_SIZE));
    int yslice = floor(GetWorldToScreen2D(GetMousePosition(), texCam).y/(TILE_SIZE));
    int texWidth = ceil(ttx->width / TILE_SIZE);
    return xslice + texWidth*yslice - 44;   // TO-DO: why is there an additional 44??
}


////////////////// PLAYER RELATED FUNCTIONS //////////////////

// Store the position of the player character
void storePos(Vector2 playerDat) {
    FILE *level = fopen("data/level_1p.txt", "rw+");
    fprintf(level, "%f %f", playerDat.x, playerDat.y);
    fclose(level);
} 

// Fetch the player position from the level file
Vector2 getPlayerPos(void) {
    Vector2 playerPos;
    playerPos.x = 0;
    playerPos.y = 0;
    FILE *level = fopen("data/level_1p.txt", "r");
    fscanf(level, "%f %f", &playerPos.x, &playerPos.y);
    fclose(level);
    return playerPos;
}

// Fetch the enemy position from the level file
Enemy *FetchEnemyData(void) {
    Enemy *enem = malloc(sizeof(Enemy) * MAX_ENEMY);
    char buf[30];
    FILE *level = fopen("data/level_1e.txt", "r");
    for (int i = 0; i < MAX_ENEMY && fgets(buf, 30, level) != NULL; i++) {
        sscanf(buf, "%f %f\n", &enem[i].rec.x, &enem[i].rec.y);
        enem[i].rec.height = 70;
        enem[i].rec.width = 70;
    }
    fclose(level);
    return enem;
}

// Store the entity data 
void StoreEntityData(Enemy enem[MAX_ENEMY]) {
    // TO-DO:

}

// Load the player data
Player loadPlayer(void) {
    Player newPlayer;
    newPlayer.pos = getPlayerPos();
    newPlayer.ase = LoadAseprite("resources/newGeorge.aseprite");

    // Attach gun
    newPlayer.gun.rec = (Rectangle){ newPlayer.pos.x - newPlayer.ase.ase->w/2, 
    newPlayer.pos.y - newPlayer.ase.ase->h/2, 20, 20 };
    newPlayer.gun.offset = 100;
    newPlayer.gun.reloadTime = 10;
    newPlayer.gun.scale = 1;

    return newPlayer;
}


// TILEMAP SERIALIZE/DESERIALISE
void StoreTilemap(struct hashmap_s *const hashmap) {
    FILE *f = fopen("data/map.txt", "w");
    fprintf(f, "%u\n", hashmap->size);
    for (int i = 0; i < hashmap->table_size; i++) {
        if (hashmap->data[i].in_use) {
            fprintf(f, "%s,", hashmap->data[i].key);
            Chunk *element = hashmap->data[i].data;
            fprintf(f, "%d,", element->active);
            for (int j = 0; j < GRID_SIZE; j++) {
                for (int l = 0; l < GRID_SIZE; l++) {
                    fputc(element->tiles[j][l].index, f);
                    fputc(element->tiles[j][l].solid,f);
                }
            }
        }
    }
    fclose(f);
}


void RetrieveTilemap(struct hashmap_s *const hashmap) {
    FILE *f = fopen("data/map.txt", "r");
    unsigned int table_size;
    fscanf(f, "%u\n", &table_size);
    for (int i = 0; i < table_size; i++) {
        int x, y;
        fscanf(f, "%d %d,", &x, &y);
        Chunk *element = malloc(sizeof(Chunk));
        fscanf(f, "%d,", &element->active);
        for (int j = 0; j < GRID_SIZE; j++) {
            for (int l = 0; l < GRID_SIZE; l++) {
                element->tiles[j][l].index = fgetc(f);
                element->tiles[j][l].solid = fgetc(f);
            }
        }
        hashmap_put(hashmap, ChunkKey(x, y), strlen(ChunkKey(x, y)), element);
    }
    fclose(f);
}