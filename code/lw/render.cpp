
void clearscreen(texture* bb)
{
    // set this as background texture later maybe idk
    uint32* pointer = (uint32*)bb->memory;
    uint32 count = bb->width*bb->height/2;
    
    // floor
    for(uint32 ii = 0; ii < count; ++ii)
    {
        *pointer = 0x001a1b34;
        pointer++;
    }
    
    // ceiling
    for(uint32 ii = 0; ii < count; ++ii)
    {
        *pointer = 0x003d4265;
        pointer++;
    }
}

void drawsquare(texture* bb, int x, int y, int w, int h, uint32 color)
{
    // TODO: do this with start/end shit
    uint32* screenpointer = (uint32*)bb->memory + x + y*bb->width;
    
	for(int yy = 0; yy < h; ++yy)
    {
        for(int xx = 0; xx < w; ++xx)
        {
            *screenpointer = color;
            screenpointer++;
        }
        screenpointer += bb->width - w;
    }
    
    return;
}

int drawtext(texture* bb, spritesheet* ss, int x, int y, char* text, uint32 color)
{
    uint32* screenpointer = 0;
	int32 characterindex = 0;
    
    while(characterindex >= 0)
    {
        char character = text[characterindex];
        if(character == 0)
        {
            characterindex = -1;
            break;
        }
        
        char imagecharacter = character - 32;
        
        int icx = imagecharacter % ss->xcount;
        int icy = imagecharacter / ss->xcount;
        
        for(int yy = 0; yy < ss->sheight; ++yy)
        {
            screenpointer = ((uint32*)bb->memory) + ((y+yy))*(bb->pitch/bb->bpp) + x + (characterindex*(ss->swidth/2));
            uint8* imagepointer = (uint8*)ss->tex.memory + (icx*ss->swidth) + ((icy*ss->sheight+yy)*ss->tex.pitch);
            
            for(int xx = 0; xx < ss->swidth; ++xx)
            {
                
                
                color = (color & 0x00FFFFFF) | ( *imagepointer << 24 );
                
                
                if(*imagepointer > 0)
                {
                    *screenpointer = color;
                }
                //*screenpointer = blend(*screenpointer, color);
                
                screenpointer++;
                imagepointer++;
            }            
        }
        
        characterindex++;
    }
    
    
    return 0;
}

// this is probably stupid
#define C_TL 0
#define C_BL 1
#define C_TR 2
#define C_BR 3

void drawentiresprite(texture* bb, spritesheet* ss, int x, int y, uint8 corner)
{
    switch(corner) // take care of the corner cases
    {
        case C_TL:
        {
            // do nothing actually
        } break;
        case C_BL:
        {
            y = bb->height - y - ss->tex.height;
        } break;
        case C_TR:
        {
            x = bb->height - x - ss->tex.width;
        } break;
        case C_BR:
        {
            x = bb->height - x - ss->tex.width;
            y = bb->height - y - ss->tex.height;
        } break;
        default:
        break;
    }
    
    int starty = y < 0 ? 0 : y;
    int endy = y + ss->tex.height > bb->height ? bb->height : y + ss->tex.height;
    
    int startx = x < 0 ? 0 : x;
    int endx = x + ss->tex.width > bb->width ? bb->width : x + ss->tex.width;
    
    uint32* screenpointer = (uint32*)bb->memory + x + (starty)*bb->width;
    uint32* imagepointer = (uint32*)ss->tex.memory;
    
    for(int yy = starty; yy < endy; ++yy)
    {
        for( int xx = startx; xx < endx; ++xx)
        {
            // kinda bad alpha blending (again)
            uint8 a = ((*imagepointer & 0xFF000000) >> 24);
            uint32 br = (a*((*imagepointer & 0x00FF00FF)) >> 8) + ((256-a)*((*screenpointer & 0x00FF00FF)) >> 8);
            uint32 g = (a*((*imagepointer & 0xFF00FF00)) >> 8) + ((256-a)*((*screenpointer & 0xFF00FF00)) >> 8);
            
            *screenpointer = (br & 0x00FF00FF) | (g & 0xFF00FF00);
            
            screenpointer++;
            imagepointer++;
        }
        screenpointer += bb->width - (endx-startx);
        imagepointer += ss->tex.width - (endx-startx);
    }
}

// sprites are assumed to be rotated 90 degrees and have colors in correct output format
void drawspritestrip(texture* bb, spritesheet* ss, int x, int y, float t, int spriteheight)
{
    int starty = y < 0 ? 0 : y;
    int endy = y + spriteheight > bb->height ? bb->height : y + spriteheight;
    
    // how far left or right in UNrotated image
    int startheight = t*(ss->tex.height);
    
    float err = (float)ss->tex.width / (float)spriteheight; // can be >1 or <1
    
#if 0
    if(ss->optimizations != 0)
    {
        // TODO: fix this (misalignment?)
        // these fudge 1s seem to work almost perfectly... weird
        int tempstart = y + 1 + (ss->optimizations[startheight*2])/err;
        int tempend = y + 1 + (ss->optimizations[startheight*2+1])/err;
        
        starty = tempstart < starty ? starty : tempstart;
        endy = tempend > endy ? endy : tempend;
    }
#endif
    
    int offset = err*(starty - y); // screenspace->imagespace
    float acc = err*(starty - y) - offset;
    
    uint32* screenpointer = (uint32*)bb->memory + x + starty*bb->width;
    uint32* imagepointer = (uint32*)ss->tex.memory + (startheight)*ss->tex.width + offset;
    
    for(int yy = starty; yy < endy; ++yy)
    {
        // kinda bad alpha blending
        uint8 a = ((*imagepointer & 0xFF000000) >> 24);
        uint32 br = (a*((*imagepointer & 0x00FF00FF)) >> 8) + ((256-a)*((*screenpointer & 0x00FF00FF)) >> 8);
        uint32 g = (a*((*imagepointer & 0xFF00FF00)) >> 8) + ((256-a)*((*screenpointer & 0xFF00FF00)) >> 8);
        
        *screenpointer = (br & 0x00FF00FF) | (g & 0xFF00FF00);
        screenpointer += bb->width;
        
        // too much math?
        acc += err;
        int sign = (int)acc;
        imagepointer += sign;
        acc -= sign;
    }
}

// walls are assumed to be rotated 90 degrees and have colors in the correct output format
void drawwallstrip(texture* bb, spritesheet* ss, int screen_x, float t, int wallheight, float dark)
{
    int onscreenheight = wallheight < 0 ? 0 : wallheight < bb->height-1 ? wallheight : bb->height - 1;
    
    uint32* screenpointer = ((uint32*)bb->memory) + (((bb->height/2)-onscreenheight/2)*(bb->width) + screen_x);
    
    float err = (ss->tex.width)/(float)wallheight;
    int offset = err*(wallheight-onscreenheight)/2;
    float acc = err*(wallheight-onscreenheight)/2.0f + offset;
    
    uint32* imagepointer = (uint32*)(ss->tex.memory) + (uint32)(t*ss->tex.width)*ss->tex.height  - offset;
    
    for(int yy = 0; yy < onscreenheight; ++yy)
    {
        *screenpointer = *imagepointer;
        screenpointer += bb->width;
        
        acc += err;
        int sign = (int)acc;
        imagepointer += sign;
        acc -= sign;
    }
}

void drawminimap(texture* bb, game_state* gs, int x, int y, uint8 size)
{
    map_data* map = &gs->currentmap;
    player_data* player = &gs->player;
    uint32* screenpointer = 0;
    
    // draw the minimap
    for(uint32 yy = 0; yy < map->height; ++yy)
    {
        for(uint8 ys = 0; ys < size; ++ys)
        {
            
            screenpointer = ((uint32*)bb->memory) + ((y+ys+size*yy))*(bb->width) + x;
            
            for(uint32 xx = 0; xx < map->width; ++xx)
            {   
                uint32 color;
                switch(map->memory[xx+yy*map->width])
                {
                    case 0:
                    color = 0x00333333;
                    break;
                    case 1:
                    color = 0x00000000;
                    break;
                    case 2:
                    color = 0x00ff0000;
                    break;
                    default:
                    color = 0x00ff00ff;
                    break;
                }
                
                for(int xs = 0; xs < size; ++xs)
                {
                    *screenpointer = color;
                    
                    screenpointer++;
                }
            }
        }
    }
    
    // draw the player
    uint32 playercolor = 0x00AAAAAA;
    screenpointer = ((uint32*)bb->memory) + (int)(x + 1 + size*(player->position.x)) + (int)((y+size*(map->height + 1 - player->position.y)))*bb->width;
    
    *screenpointer = playercolor;
    *(screenpointer+1) = playercolor;
    *(screenpointer-1) = playercolor;
    *(screenpointer + (bb->pitch/bb->bpp))= playercolor;
    *(screenpointer - (bb->pitch/bb->bpp))= playercolor;
    
    // draw the projectiles too fuck it
    for(int ii = 0; ii < gs->entities; ++ii)
    {
        switch(gs->entity[ii].entitytype)
    	{
            case ET_PROP:
            {
                screenpointer = ((uint32*)bb->memory) + (int)(x + 1 + size*(gs->entity[ii].position.x)) + (int)((y+size*(map->height + 1 - gs->entity[ii].position.y)))*bb->width;
                *screenpointer = 0x0000ff00;
            } break;
            case ET_ENEMY:
            {
                screenpointer = ((uint32*)bb->memory) + (int)(x + 1 + size*(gs->entity[ii].position.x)) + (int)((y+size*(map->height + 1 - gs->entity[ii].position.y)))*bb->width;
                *screenpointer = 0x00ff0000;
            } break;
            case ET_PROJECTILE:
            {
                screenpointer = ((uint32*)bb->memory) + (int)(x + 1 + size*(gs->entity[ii].position.x)) + (int)((y+size*(map->height + 1 - gs->entity[ii].position.y)))*bb->width;
                *screenpointer = 0x00ff0000;
            } break;
            default: break;
        }
    }
}


void shittydrawline(texture* bb, int x, int y, float angle, int steps, int stepdist, uint32 color)
{
    uint32* screenpointer = (uint32*)bb->memory + x + (bb->height-y)*bb->width;
    
    for(int ii = 0; ii < steps; ++ii)
    {
        if(screenpointer > bb->memory )
        {
            *(screenpointer + (int)(ii*stepdist*cos(angle)) + (int)(ii*stepdist*sin(-angle))*((bb->pitch/bb->bpp))) = color;
        }
        
    }
}

void castraynew(texture* bb, int screen_x, map_data* map, v2 startpos, v2 direction, float* depthbuffer)
{
    int xsquare = (int)startpos.x;
    int ysquare = (int)startpos.y;
    
    float xdistnext = 0;
    float ydistnext = 0;
    
    float xstepdist = abs(1.0f/direction.x);
    float ystepdist = abs(1.0f/direction.y);
    
    int xsign = 0;
    int ysign = 0;
    
    int hit = false;
    int side = 0; // shitty hack
    
    int wallspriteindex = 1;
    
    if(direction.x < 0)
    {
        xsign = -1;
        xdistnext = (startpos.x - xsquare)*xstepdist;
    } else {
        xsign= 1;
        xdistnext = (xsquare + (1.0f - startpos.x))*xstepdist;
    }
    
    if(direction.y < 0)
    {
        ysign = -1;
        ydistnext = (startpos.y - ysquare)*ystepdist;
    } else {
        ysign = 1;
        ydistnext = (ysquare + (1.0f - startpos.y))*ystepdist;
    }
    
    while(hit == 0)
    {
        if(xdistnext < ydistnext)
        {
            xdistnext += xstepdist;
            xsquare += xsign;
            side = 0;
        } else {
            ydistnext += ystepdist;
            ysquare += ysign;
            side = 1;
        }
        
        int tilevalue = map->memory[xsquare + (map->height-ysquare)*map->width];
        if(tilevalue > 0)
        {
            hit = 1;
            if(tilevalue == 2)
            {
                wallspriteindex = 3;
            }
        }
        
    }
    
    // distance calc
    float distance = 0;
    float texx = 0;
    if (side == 0)
    {
        distance = (xsquare - startpos.x + ((1-xsign)/2))/direction.x;
        //texx = abs(fmod(startpos.y + distance*direction.y, 1.0f));
        float t = startpos.y + distance*direction.y;
        texx = t - floor(t);
    } else {
        distance = (ysquare - startpos.y + ((1-ysign)/2))/direction.y;
        // texx = abs(fmod(startpos.x + distance*direction.x, 1.0f));
        float t = startpos.x + distance*direction.x;
        texx = t - floor(t);
    }
    int wallheight = (int)(bb->height/distance);
    
    // texture x
    float darkness = side*0.3 + (100-distance)/100;
    
    depthbuffer[screen_x] = distance;
    
    drawwallstrip(bb, &map->sprites[wallspriteindex], screen_x, texx, wallheight, darkness);
}


inline uint32* getscreenpos(texture* bb, int x, int y)
{
    return (uint32*)bb->memory + x +  y*bb->width;
}

void debug_inputdisplay(texture* bb, input_state* is, int x, int y, uint8 size)
{
    uint32* screenpointer = 0;
    
    int buttons = 8;
    
    u8 buttonstate[] = { 
        is->l1.d, is->up.d, is->r1.d,
    	is->left.d, is->down.d, is->right.d,
        is->buttons[0].d, is->buttons[1].d};
    
    // qwe asd space ctrl
    uint32* buttonpixels[] = { getscreenpos(bb, x, y), getscreenpos(bb, x+size+1, y), getscreenpos(bb, x+2*size+2, y),
        getscreenpos(bb, x, y+size+1), getscreenpos(bb, x+size+1, y+size+1), getscreenpos(bb, x+2*size+2, y+size+1),
        getscreenpos(bb, x+2*size+2, y+2*size+2), getscreenpos(bb, x, y+2*size+2)};
    
    for(int ii = 0; ii < buttons; ++ii)
    {
        screenpointer = buttonpixels[ii];
        uint32 color = buttonstate[ii] > 0 ? 0x00ffffff : 0x00000000;
        
        for(int xx = 0; xx < size; ++xx)
        {
            for(int yy = 0; yy < size; ++yy)
            {
                *(screenpointer + xx + yy*bb->width) = color;
            }
        }
    }
    
}
