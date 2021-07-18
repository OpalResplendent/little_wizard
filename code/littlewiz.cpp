#if NOCRT
#include "nocrt/crt_float.cpp"
#include "nocrt/crt_math.cpp"
#include "lw/nocrt.cpp"
#endif

#include "platform.h"
#include "lw/littlewiz.h"
#include "lw/render.cpp"

// TODO: Make the depth buffer scale with screen size!

// TODO: unicode support is working?

// TODO: fix the fucking movement
// TODO: floor and ceiling rendering
// TODO: fix collision
// TODO: use the enemy/projectile arrays to speed stuff up
// TODO: properly support animations
// TODO: fix raw input
// TODO: alt for sidestepping
// TODO: text events? ui events

uint8 testmap[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

uint8 coolertestmap[] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,2,1,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

uint8 level2[] = {
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,0,1,1,1,1,0,0,0,0,0,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,0,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,0,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,0,0,1,0,0,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,0,0,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

v2 DIRECTION = {1.0, 0};  // right unit vector
v2 VIEWPLANE = {0, -0.5}; // negative half-vector for left of screen

// will return the _highest_ value
int movewithcollision(map_data* map, v2* position, v2* velocity, float radius)
{
    int firstcollided = 0;
    int secondcollided = 0;
    // fuck it, x, then y movement
    
    int xsign = velocity->x < 0 ? -1 : 1;
    int xnext = (int)(position->x + velocity->x + xsign*radius);
    int xcur = (int)(position->x);
    
    int ysign = velocity->y < 0 ? -1 : 1;
    int ynext = (int)(position->y + velocity->y + ysign*radius);
    int ycur = (int)(position->y);
    
    // fix this garbage
    
    // x
    if(map->memory[xnext + (map->height - ycur)*map->width] != 0)
    {
        firstcollided = map->memory[xnext + (map->height - ycur)*map->width];
        float xoffset = velocity->x < 0 ? radius : 1.0f - radius;
        
        position->x = xcur + xoffset;
        
        velocity->x = 0;
    }
    
    // y
    if(map->memory[xcur + (map->height - ynext)*map->width] != 0)
    {
        secondcollided = map->memory[xcur + (map->height - ynext)*map->width];
        float yoffset = velocity->y < 0 ? radius : 1.0f - radius;
        
        position->y = ycur + yoffset;
        
        velocity->y = 0;
    }
    
    // move shit that didn't hit
    *position += *velocity;
    
    return (firstcollided > secondcollided ? firstcollided : secondcollided);
}

void removeentity(game_state* gs, int ei)
{
    switch(gs->entity[ei].entitytype)
    {
    	case ET_PROJECTILE:
        {
            for(int ii = 0; ii < gs->projectiles; ++ii)
            {
                if(gs->projectile[ii] == ei)
                {
                    // swap current with last minus one
                    gs->projectile[ii] = gs->projectile[gs->projectiles-1];
                    ii = gs->projectiles;
                    gs->projectiles--;
                    
                }
            }
        } break;
        
        case ET_ENEMY:
    	{
            for(int ii = 0; ii < gs->enemies; ++ii)
            {
                if(gs->enemy[ii] == ei)
                {
                    gs->enemies--;
                    gs->enemy[ii] = gs->enemy[gs->enemies];
                    ii = gs->enemies;
                }
            }
        } break;
        
        default: break;
    }
    
    gs->entities--;
    gs->entity[ei] = gs->entity[gs->entities];
}

void killenemy(game_state* gs, uint8 ei)
{
    entity_data* e = &gs->entity[ei];
    
    for(int ii = 0; ii < gs->enemies; ++ii)
    {
        if(gs->enemy[ii] == ei)
        {
            gs->enemy[ii] = gs->enemy[gs->enemies];
            ii = gs->enemies;
            gs->enemies--;
        }
    }
    
    v2 temppos = e->position;
    *e = {};
	e->entitytype = ET_PROP;
    e->position = temppos;
    e->animationoffset = pcg_urand(&gs->rng, 255);
}

void addprop(game_state* gs, uint8 type, v2 position)
{
    entity_data* e = &gs->entity[gs->entities];
    
    *e = entity_data{};
    
    e->entitytype = ET_PROP;
    e->type = type;
    e->position = position;
    e->animationoffset = pcg_urand(&gs->rng, 255);
    
    gs->entities++;
}

void addprojectile(game_state* gs, uint8 type, v2 position, v2 velocity)
{
    entity_data* e = &gs->entity[gs->entities];
    
    *e = entity_data{};
    
    e->entitytype = ET_PROJECTILE;
    e->type = type;
    e->position = position;
    e->velocity = velocity;
    e->value = 1;
    e->animationoffset = pcg_urand(&gs->rng, 255);
    e->height = 1.0f;
    
    gs->projectile[gs->projectiles] = gs->entities;
    
    gs->projectiles++;
    gs->entities++;
}

void addenemy(game_state* gs, float x, float y, uint8 type)
{
    entity_data* e = &gs->entity[gs->entities];
    
    *e = entity_data{};
    
    e->entitytype = ET_ENEMY;
    e->type = type;
    e->position = v2{x,y};
    e->friction = 0.6;
    e->value = 1;
    e->animationoffset = pcg_urand(&gs->rng, 255);
    
    gs->enemy[gs->enemies] = gs->entities;
    
    gs->enemies++;
    gs->entities++;
}

void initialize(game_memory* gm)
{
	game_state *gs = (game_state*)gm->persist;
	memory_arena *tm = (memory_arena*)gm->tempmem;
    
    // set the FOV based on the aspect ratio
    float aspectratio = gm->SCREEN_WIDTH/(float)gm->SCREEN_HEIGHT;
    aspectratio *= 3.0f/4.0f; // correct to get 90 degrees at 4:3 ratio
    VIEWPLANE.y = -(aspectratio/2.0f); // negative half-vector
    
    pcg_state *rng = &gs->rng;
    if(&rng == 0)
    {
        pcg_seed(rng);
    }
    
    gs->memory = {gm->tempmemsize,0,gm->tempmem};
    
    gs->currentmap = {0, 33, 31, gm->sprites};
    
    gs->player = {0};
    gs->player.position = v2{gs->currentmap.width/2 + 0.6, gs->currentmap.width/2 + 0.1};
    gs->player.rotation = M_PI;
    gs->player.position = v2{27.1,25.5};
    
    gs->entities = 0;
    
    addprop(gs, 3, v2{23.2, 25.5});
    #if 1
    addprop(gs, 4, v2{27.2, 27.7});
    
    addprop(gs, 4, v2{12.2, 26.8});
    
    addprop(gs, 5, v2{19.5, 23.5}); // HAIL ORB!
    addprop(gs, 2, v2{17.5, 22});
    addprop(gs, 2, v2{17.5, 24.5});
    addprop(gs, 2, v2{21, 24.5});
    addprop(gs, 2, v2{21, 22});
    
    addenemy(gs, 13.7, 29, 0);
    addprop(gs, 1, v2{14.5, 29.5});
    addprop(gs, 1, v2{12.5, 29.5});
    
    addenemy(gs, 8, 25.5, 0);
    addenemy(gs, 8, 23, 0);
    addprop(gs, 1, v2{9, 21.5});
    addprop(gs, 1, v2{7, 21.5});
    
    addenemy(gs, 11.8, 16, 0);
    addenemy(gs, 12.5, 15.4, 0);
    
    addenemy(gs, 17.5, 10.4, 0);
    addenemy(gs, 19.5, 8.3, 0);
    addenemy(gs, 19.9, 5.4, 0);
    addprop(gs, 3, v2{19.96, 9.3});
    addprop(gs, 1, v2{17.2, 5.5});
    #endif
    gs->currentmap.memory = pusharray(&gs->memory, gs->currentmap.width*gs->currentmap.height, uint8);
    
    for(uint32 ii = 0; ii < gs->currentmap.height*gs->currentmap.width; ++ii)
    {
        gs->currentmap.memory[ii] = level2[ii];
    }
    
    gm->initialized = true;
}

extern "C" void GameUpdateAndRender(game_memory* gm, input_state* is, texture* bb)
{
    game_state *gs = (game_state*)gm->persist;

	pcg_state *rng = &gs->rng;
    
    memory_arena *tm = (memory_arena*)gm->tempmem;
    
    if(gm->initialized == 0)
    {
        initialize(gm);
        gs->ui.staffposition = v2{bb->width/2+32,bb->height-206}; // staff tex height - 50
        gs->ui.staffrestposition = gs->ui.staffposition;
    }
    
    if(gm->swappedgamecode == 1)
    {
        initialize(gm);
        
        // fov keeps getting reset... delete this later
        float aspectratio = gm->SCREEN_WIDTH/(float)gm->SCREEN_HEIGHT;
        aspectratio *= 3.0f/4.0f;
        VIEWPLANE.y = -(aspectratio/2.0f);
    }
    
    // ---------------- gameplay (player) ------------------------------ //
    
    b8 strafetoggle = (!!is->buttons[1].d);
    
    // rotation
    i8 turning = strafetoggle ? (!!is->l1.d - !!is->r1.d) : (!!is->left.d - !!is->right.d);
    
    float rotationspeed = 0.013;
    float rotationfriction = (turning == 0) ? 0.99 : 0.16;
    rotationfriction = 1.0f - rotationfriction;
        
    gs->player.rotationvelocity += turning*rotationspeed;
    
    gs->player.rotation += gs->player.rotationvelocity;
    gs->player.rotationvelocity *= rotationfriction;
    
    // player rotation trig functions for this frame
    real32 spr = sin(gs->player.rotation);
    real32 cpr = cos(gs->player.rotation);
    
	// movement
    i8 forward = (!!is->up.d - !!is->down.d);
    i8 sideways = strafetoggle ? (!!is->left.d - !!is->right.d) : (!!is->l1.d - !!is->r1.d);
    i8 moving = abs(forward) + abs(sideways);
    
    float acc = 0.07; // acc
    float friction = 0.4f;
    friction = 1.0f - friction;
        
    // move forward/back
    gs->player.velocity.x += acc*forward*cpr;
    gs->player.velocity.y += acc*forward*spr;
    
    // strafe left/right
    gs->player.velocity.x += (acc/2.0f)*sideways*(-spr);
    gs->player.velocity.y += (acc/2.0f)*sideways*cpr;
    
    // move the player
	if(movewithcollision(&gs->currentmap, &gs->player.position, &gs->player.velocity, 0.15) == 2)
    {
        gm->shuttingdown = 1;
    }
    
    // TODO: friction should be based on magnitude, also break it into a function
    gs->player.velocity *= friction;
    
    // setup player direction and viewplane vectors
    float tempsin = sin(gs->player.rotation);
    float tempcos = cos(gs->player.rotation);
    v2 rotationvec = { DIRECTION.x*tempcos - DIRECTION.y*tempsin,
        DIRECTION.x*tempsin + DIRECTION.y*tempcos };
    v2 viewplanevec = { VIEWPLANE.x*tempcos - VIEWPLANE.y*tempsin,
        VIEWPLANE.x*tempsin + VIEWPLANE.y*tempcos };
    
    // fire a ball of fireball
    if(is->buttons[0].wd)
    {
        gs->ui.staffvelocity.y += pcg_frand(rng, 40, 120);
        gs->ui.staffvelocity.x += pcg_frand(rng, -13, 55);
        
        v2 tempmovevec = rotationvec/4.0f;
        
        addprojectile(gs, 0, gs->player.position + tempmovevec, rotationvec/4.0f);
        
        movewithcollision(&gs->currentmap, &gs->entity[gs->entities].position, &tempmovevec, 0.05);
    }
    
    // ---------------------- gameplay (tick) --------------------------------- //
    
    gs->ui.staffvelocity.x += gs->player.rotationvelocity*100;
    gs->ui.staffvelocity.y -= abs(gs->player.rotationvelocity)*10;
    gs->ui.staffvelocity += (gs->ui.staffrestposition - gs->ui.staffposition)/20;
    v2 staffmovevel = v2{(-gs->player.velocity.x*tempsin + gs->player.velocity.y*tempcos)*80,
        			(gs->player.velocity.x*tempcos + gs->player.velocity.y*tempsin)*15};
    gs->ui.staffvelocity += staffmovevel;
    gs->ui.staffposition += gs->ui.staffvelocity;
    gs->ui.staffvelocity *= 0.4;
    
    // order should be:
    // move stuff and resolve projectile vs wall
    // resolve active projectiles
    // resolve enemies
    
    // update projectiles
    for(int ii = 0; ii < gs->projectiles; ++ii)
    {
        entity_data* p = &gs->entity[gs->projectile[ii]];

        if(p->animation < 2)
        {
            uint8 exploded = 0;
            uint8 enemiestokill[MAX_ENTITIES] = {0};
            
            for(int jj = 0; jj < gs->enemies; ++jj)
            {    
                entity_data* e = &gs->entity[gs->enemy[jj]];
                
                if(distancev2(p->position, e->position) < 0.09)
                {
                    enemiestokill[exploded] = gs->enemy[jj];
                    exploded++;
                }
            }
            
            if(exploded > 0)
            {
                
                for(int jj = 0; jj < exploded; ++jj)
                {
                    killenemy(gs, enemiestokill[jj]);
                }
                
	            p->animation = 2;
	            p->velocity = {0};
            	p->animationtimer = 0;
            }
        } else if(p->animationtimer > 24) {
            removeentity(gs, gs->projectile[ii]);
            ii--;
        }
    }
    
    // update entities
    for(int ii = 0; ii < gs->entities; ++ii)
    {
        entity_data* e = &gs->entity[ii];
        
        switch(e->entitytype)
    	{
            case ET_PROP:
            {
                if(e->type == 0
                   && e->animation == 0)
                {
                    e->animationtimer++;
                    if(e->animationtimer > 50)
                    {
                        e->animation = 1;
                    }
                } else if(e->type == 5) {
                	if(e->state == 1)
                    {
                        e->animationtimer++;
                        if(e->animationtimer > 50)
                        {
                            removeentity(gs, ii);
                            ii--;
                        }
                    } else if(distancev2(e->position, gs->player.position) < 1.0f)
                    {
                        e->state = 1;
                    }
                        
                }
                    
            } break;
            case ET_ENEMY:
            {
                int collisionvalue = movewithcollision(&gs->currentmap, &e->position, &e->velocity, 0.2);
                
                switch(e->state)
                {
                    case 0: // hanging out
                    {
                        if(distancev2(e->position, gs->player.position) < 5.4)
                        {
                            e->state = 1;
                            e->animation = 1;
                        }
                    } break;
                    case 1: // chasing player
                    {
                        float distance = distancev2(e->position, gs->player.position);
                        
                        if(distance < 0.8)
                        {
                            e->state = 2;
                            e->animation = 2;
                            e->animationtimer = 0;
                        } else {
	                        v2 relativeposition = gs->player.position - e->position;
	                        e->velocity += relativeposition/distance;
	                        clamp_v2magnitude(gs->entity[ii].velocity, 0.2);
                        	e->velocity *= 1.0f - gs->entity[ii].friction;
                        }
                    } break;
                    case 2: // attacking
                    {
                        e->velocity = v2{0,0};
                        
                        if(e->animationtimer > 60)
                        {
                            if(distancev2(e->position, gs->player.position) > 15)
                            {
                                e->state = 0;
                                e->animationtimer = 0;
                                e->animation = 0;
                            } else {
                                e->state = 1;
                                e->animation = 1;
                            }
                        }
                    }
                    default: break;
                }
                
                e->animationtimer++;
            } break;
            case ET_PROJECTILE:
            {
                v2 adjustedvel = e->velocity;
                if(e->animation == 0)
                {
                    
                    adjustedvel *= 0.2;
                    adjustedvel += gs->player.velocity;
                }
                
                
                
                int collisionvalue = movewithcollision(&gs->currentmap, &e->position, &adjustedvel, 0.05);
                
	            if(collisionvalue != 0)
	            {
                    e->animation = 2;
                    e->velocity = {0};
                    e->animationtimer = 0;
	            }
                
                if(e->animation == 0
                   && e->animationtimer > 18)
                {
                    e->animation++;
                    e->animationtimer = 0;
                } else {
                    e->animationtimer++;
                }
                
            } break;
            default: break;
        }
    }
    
    // ---------------------- render ------------------------------------------ //
    uint32* firstpixel = (uint32*)bb->memory;
	uint32* screenpointer = firstpixel;
    	
    // TODO: this shoudn't be needed after doing floor/ceiling
    clearscreen(bb);
    
    // TODO: depthbuffer is not dynamic!!
    float depthbuffer[1920] = {0};
    
    // cast dem rays
    float xval = 0;
    v2 direction = {0};
    
    texture* obb = bb;
    
	for(int xx = 0; xx < bb->width; ++xx)
    {
        xval = (2.0f*xx)/((float)bb->width) - 1.0f;
        direction.x = rotationvec.x + xval*viewplanevec.x;
    	direction.y = rotationvec.y + xval*viewplanevec.y;
        
        castraynew(bb, xx, &gs->currentmap, gs->player.position, direction, depthbuffer);
        bb = obb;
    }
    
    // render entities
    uint8 e_order[MAX_ENTITIES] = {0};
    uint8 e_visible = 0;
    v2 e_spos[MAX_ENTITIES] = {0};
    float inverseplane = 1.0f/(viewplanevec.x*rotationvec.y - viewplanevec.y*rotationvec.x);
    
    // grab all entity screen positions (y is distance)
    for(int ii = 0; ii < gs->entities; ++ii)
    {
        v2 relpos = gs->entity[ii].position - gs->player.position;
        e_spos[ii] = { inverseplane*(rotationvec.y*relpos.x - rotationvec.x*relpos.y),
            inverseplane*(-viewplanevec.y*relpos.x + viewplanevec.x*relpos.y) };
        
        // very simple visibility culling for things in front of the camera
        if(e_spos[ii].y > 0)
        {
            e_order[e_visible] = ii;
			e_visible++;
        }
    }
    
    // bad sort for back-to-front rendering
    // heapsort is good, but combsort is simpler...
    // ... so copy/paste from wiki psuedocode
    
    uint8 gap = e_visible;
    float shrink = 1.3; // magic roughly optimal number
    uint8 sorted = false;
    
    while(!sorted)
    {
        int gap = (int)(gap/shrink); // free floor
        if(gap < 1)
        {
            gap = 1;
            sorted = true;
        }
        
        int ii = 0;
        while(ii + gap < e_visible)
        {
            if(e_spos[e_order[ii]].y < e_spos[e_order[ii+gap]].y)
            {
                float temp = e_order[ii];
                e_order[ii] = e_order[ii+gap];
                e_order[ii+gap] = temp;
                sorted = false;
            }
            ii++;
         }
     }
	
    uint8 hailingorb = false;
	uint32 linesexpected = 0;;
    uint32 linesdrawn = 0;
    
    // actually draw them
    for(int ii = 0; ii < e_visible; ++ii)
    {
        uint8 ei = e_order[ii];
        entity_data* e = &gs->entity[ei];
        v2 scrpos = e_spos[ei];
        
        float scale = 4.0f/scrpos.y;
        int width = scale*gm->sprites[2].tex.width;
        int scrx = (int)((bb->width/2)*(1+(scrpos.x/scrpos.y)))-width/2;

        if(scrx + width > 0 && scrx < bb->width)
        {
            int scry = (bb->height + (1.0/(scrpos.y))*bb->height)/2;
            int height = scale*(gm->sprites[2].tex.height);
            int xstart = scrx < 0 ? 0 : scrx;
            int xend = scrx + width > bb->width ? bb->width : scrx + width;
            
            linesexpected = xend - xstart;
            
            for(int xx = xstart; xx < xend; ++xx)
            {
                float t = (xx-scrx)/((float)width);
	            
                if(depthbuffer[xx] > scrpos.y)
                {
                    spritesheet* sprite = 0;
                    
                    switch(e->entitytype)
                    {
                        case ET_PROP:
                        {
                            switch(e->type)
                            {
                            	case 0:
                                {
                                    if(e->animation == 0)
                                    {
                                    	int index = e->animationtimer/12;
                                    	sprite = &gm->sprites[22+index];
                                    } else {
										sprite = &gm->sprites[9];
                                    }
                                    
                                } break;
                                case 1:
                                case 2:
                                case 3:
                                case 4:                   
                                {
                                    sprite = &gm->sprites[e->type+26];
                                } break;
                                case 5:
                                {
                                    sprite = &gm->sprites[31];
                                    if(e->state == 1)
                                    {
                                        hailingorb = true;
                                    }
                                } break;
                                case 255:
                                {
                                    
                                } break;
                                default: break;
                            }
                        } break;
                        case ET_ENEMY:
                        {
                            switch(e->animation)
                            {
                            	case 0:
                                {
			                    	sprite = &gm->sprites[2];
                                } break;
                                case 1:
                                {
                                    int hold = 4;
                                    int frames = 3;
                                    int timer = gm->timer;
                                    int offset = e->animationoffset;
                                    // left mid right
                                    int si = ((timer+offset)%(frames*hold))/hold;
                                    
                                    si = si == 0 ? 2 : si + 19;
                                    
                                    sprite = &gm->sprites[si];
                                } break;
                                case 2:
                                {
                                    if(e->animationtimer < 30)
                                    {
                                        sprite = &gm->sprites[21];
                                    } else if(e->animationtimer < 40)
                                    {
                                        sprite = &gm->sprites[20];
                                    } else {
                                        sprite = &gm->sprites[2];
                                    }
                                } break;
                                default: break;
                            }
                        } break;
                        case ET_PROJECTILE:
                        {
                            // ghetto animation for now
                            int si = e->animation*3;
                            int hold = 6;
                            int frames = e->animation == 2 ? 4 : 3;
                            int offset = e->animation == 1 ? e->animationoffset : 0;
                            
                            si += ((e->animationtimer+offset)%(frames*hold))/hold;
                            
                            sprite = &gm->sprites[si+10];
                        } break;
                        default:
                        {
	                        sprite = &gm->sprites[0];
                        } break;
                    }
                    drawspritestrip(bb, sprite, xx, scry-height, t, height);
                    linesdrawn++;
                }
            }
		}
    }

    int drawingtext = 0;
    if(drawingtext > 0)
    {
	    char text[] = "this is a test";
	    drawtext(bb, &gm->sprites[0], 201, 200, text, 0xFF000000);
	    drawtext(bb, &gm->sprites[0], 199, 200, text, 0xFF000000);
	    drawtext(bb, &gm->sprites[0], 200, 199, text, 0xFF000000);
	    drawtext(bb, &gm->sprites[0], 200, 201, text, 0xFF000000);
    	
	    drawtext(bb, &gm->sprites[0], 201, 201, text, 0xFF000000);
    	drawtext(bb, &gm->sprites[0], 199, 199, text, 0xFF000000);
	    drawtext(bb, &gm->sprites[0], 201, 199, text, 0xFF000000);
	    drawtext(bb, &gm->sprites[0], 199, 201, text, 0xFF000000);
	    
    	drawtext(bb, &gm->sprites[0], 200, 200, "this is a test", 0xFFFFFFFF);
    }
    
    // draw ui
    int uiheight = bb->height;
    drawentiresprite(bb, &gm->sprites[4],0,0,1);
    drawentiresprite(bb, &gm->sprites[5],0,0,1);
    drawentiresprite(bb, &gm->sprites[7],92,136,1);
    drawentiresprite(bb, &gm->sprites[7],120,124,1);
    drawentiresprite(bb, &gm->sprites[7],136,96,1);
    drawentiresprite(bb, &gm->sprites[6],16,20,1);
    
    // draw the staff
    drawentiresprite(bb, &gm->sprites[8], gs->ui.staffposition.x, gs->ui.staffposition.y, 0);
    
    gs->ui.minimap = 1;
    
    // draw the minimap
    if(gs->ui.minimap > 0)
    {
	    uint8 mmsize = 5;
	    uint8 mmoffset = mmsize;
	    int minimapx = bb->width - (mmoffset + mmsize*(gs->currentmap.width));
		int minimapy = bb->height - (mmoffset + mmsize*(gs->currentmap.height));
	    
	    drawminimap(bb, gs, minimapx, minimapy, mmsize);
    }
	
    if(hailingorb)
    {
        drawsquare(bb, bb->width/2-110, bb->height/2+95, 160, 40, 0x00000000);
        drawtext(bb, &gs->currentmap.sprites[0], bb->width/2-100, bb->height/2+100, "HAIL ORB", 0x00FFFFFF);
    }
    
    debug_inputdisplay(bb, is, 160, bb->height-40, 10);
    drawtext(bb, &gs->currentmap.sprites[0], 86, 4, "fps", 0x00FFFFFF);
    drawtext(bb, &gs->currentmap.sprites[0], 0, 2, gm->debug_fps, 0x00FFFFFF);
    
    return;
}