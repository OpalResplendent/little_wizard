#ifndef PLATFORM
#include <stdint.h> // int aliases

typedef int8_t int8;
typedef int32_t int32;

typedef uint8_t uint8;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t i8;
typedef int32_t i32;

typedef uint8_t u8;
typedef uint32_t u32;
typedef uint64_t u64;

typedef char c8;
typedef unsigned char uc8;

typedef float r32;
typedef float f32;

typedef uint8 b8;
typedef uint32 b32;

typedef float real;
typedef float real32;
typedef float float32;

typedef uint8_t bool8;
typedef uint32_t bool32;

#include "sge/sge_math.h"
#include "sge/sge_random.cpp"

#define KB(Value) ((Value)*1024LL)
#define MB(Value) (KB(Value)*1024LL)
#define GB(Value) (MB(Value)*1024LL)
#define TB(Value) (GB(Value)*1024LL)

#define GAME_MEMORY_SIZE MB(32);
#define TEMP_MEMORY_SIZE MB(128);

#define MAX_SPRITES 128

#define MOUSE_BUTTONS 5
#define DIRECTION_BUTTONS 4
#define BUTTONS 12
#define MAX_CONTROLLERS 4
#define CONTROLLER_BUTTONS (DIRECTION_BUTTONS + BUTTONS)
//#define TOTAL_BUTTONS (((1+MAX_CONTROLLERS)*CONTROLLER_BUTTONS) + MOUSE_BUTTONS)

#define DIRECTIONS_CANCEL 1
//#define INACTIVE_INPUT 0
#define INACTIVE_KEYBOARD_INPUT 0
#define INACTIVE_CONTROLLER_INPUT 1 

struct input_buttonstate
{
	union{
		struct{uint8 down, downprevious, wentdown, wentup;};
		struct{uint8 d,dp,wd,wu;};
	};
    
};

struct controller_state
{
	union{
		input_buttonstate button[CONTROLLER_BUTTONS];
		struct{
			union{
				input_buttonstate directions[DIRECTION_BUTTONS];
				struct{input_buttonstate up, down, left, right;};
			};
			union{
				input_buttonstate buttons[BUTTONS];
				struct{input_buttonstate a, b, x, y, lb, rb, lt, rt, back, start, ls, rs;};
				struct{input_buttonstate db, rb, lb, ub, l1, r1, l2, r2, select, start, l3, r3;};
			};
		};
	};
    real32 lax, lay, rax, ray, lat, rat;
};

struct input_state
{
    union{
        controller_state virtualcontroller;
        union{
            input_buttonstate button[CONTROLLER_BUTTONS];
            struct{
                union{
                    input_buttonstate directions[DIRECTION_BUTTONS];
                    struct{input_buttonstate up, down, left, right;};
                    struct{input_buttonstate u, d, l, r;};
                };
                union{
                    input_buttonstate buttons[BUTTONS];
                    struct {input_buttonstate a, b, x, y, lb, rb, lt, rt, back, start, ls, rs;};
                    struct{input_buttonstate a, b, x, y, l1, r1, l2, r2, select, start, l3, r3;};
                };
                struct{
                    real32 lax, lay, rax, ray, lat, rat;
                };
            };
            
        };
    };
    controller_state controllers[MAX_CONTROLLERS];
    
    union{
        input_buttonstate mousebutton[MOUSE_BUTTONS]; // L, R, M, X1, X2
        struct{input_buttonstate LM, RM, MM, X1, X2;};
    };
	
    
	union{
		struct{int mousex, mousey, mousexprevious, mouseyprevious;};
		struct{int mx, my, mxp, myp;};
	};
	int8 mousewheel;
	uint8 activecontrollers;
};

//#define RO_COLOR union{uint32 color;struct {uint8 r, g, b, a;};}

union texture {
    struct {
        void* memory;
        int width;
        int height;
        int pitch;
        int bpp;
    };
    struct {
        void* m;
        int w;
        int h;
        int p;
        int b;
    };
};

// TODO: this should probably have a name and a pointer to a texture instead
struct spritesheet {
    texture tex;
    int swidth;
    int sheight;
    int xcount;
    int ycount;
    uint8* optimizations;
};

struct game_memory {
    bool initialized;
    bool swappedgamecode;
    bool shuttingdown;
    uint64 timer;
    
    int SCREEN_WIDTH;
    int SCREEN_HEIGHT;
    
    uint32 persistsize;
    uint8 *persist;
    
    uint32 tempmemsize;
    uint8 *tempmem;
    
    // no count on the sprites D:
    spritesheet* sprites;
    char debug_fps[16];
};

#define GAME_UPDATE_AND_RENDER(name) void name(game_memory* GameMemory, input_state* InputState, texture* bb)
typedef GAME_UPDATE_AND_RENDER(game_update_and_render);

#define PLATFORM
#endif
