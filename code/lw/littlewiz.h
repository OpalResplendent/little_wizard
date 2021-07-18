
#define sizeofarray(array) (sizeof(array)/sizeof(array[0]))

struct memory_arena
{
	uint32 size;
	uint32 used;
	uint8* base;
};

#define pushbytes(arena, byte) (type*)pushsize_(arena, bytes)
#define pushstruct(arena, type) (type*)pushsize_(arena, sizeof(type))
#define pusharray(arena, count, type) (type*)pushsize_(arena, count * sizeof(type))
void*
pushsize_(memory_arena *arena, uint32 size)
{
	void* result = arena->base + arena->used;
	arena->used += size;
	return(result);
};

enum entity_type{ET_PROP, ET_ITEM, ET_ENEMY, ET_PROJECTILE, ET_TOTAL};

struct entity_data {
    entity_type entitytype;
    v2 position;
    v2 velocity;
    float friction;
    float rotation;
    float height;
    uint8 type;
    uint8 value; // hp, whatever
    uint8 state; // ai, whatever
    uint8 animation;
    uint8 animationtimer;
    uint8 animationoffset;
    
};

struct player_data {
    v2 position;
    v2 velocity;
    float rotation;
    float rotationvelocity;
    char name[16];
    uint32 points;
    uint32 exp;
    uint8 level;
    uint8 hp;
    uint8 state;
    uint8 weapon;
    uint8 weaponstates[8];
    uint8 ammos[8];
    uint8 artifacts[2];
    uint8 keys;
};

struct map_data {
    uint8* memory;
    uint32 width;
    uint32 height;
    
    // uint32 num_ss;
    spritesheet* sprites;
};

struct ui_data {
    v2 staffposition;
    v2 staffvelocity;
    v2 staffrestposition;
    uint8 portraitstate;
    uint8 minimap;
};

#define MAX_ENTITIES 256

struct game_state
{
    pcg_state rng;
    player_data player;
    map_data currentmap;
    uint8 entities;
    uint8 projectiles;
    uint8 enemies;
    entity_data entity[MAX_ENTITIES]; // should fix this size later
    uint8 projectile[MAX_ENTITIES];
    uint8 enemy[MAX_ENTITIES];
    ui_data ui;
    
    uint32 networkstatus;
    memory_arena memory;
};
