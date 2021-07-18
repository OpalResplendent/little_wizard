#include <cstdio>
#include <windows.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "platform.h"
#include "lwd/littlewizarddata.h"
#include "win32/win32_helpers.cpp"

#define MAXFILES 1024
texture textures[MAXFILES];
spritesheet spritesheets[MAXFILES];
void* texmem[MAXFILES];
void* texopt[MAXFILES];
char filenames[MAXFILES][LWD_MAX_FILENAME_LENGTH]; // ehhh

enum SSOPTIMIZATION {OPT_NONE, OPT_FULL, OPT_ROTATE, OPT_SWIZZLE, OPT_COUNT };

// NOTE: this is now offline file packer

uint8* loadtexture(char* filename, texture* tex)
{
    uint8* pointer = 0;
    
    int x, y, n;
    pointer = stbi_load(filename, &x, &y, &n, 0);
    const char* failure = stbi_failure_reason();
    tex->width = x;
    tex->height = y;
    tex->bpp = n;
    tex->pitch = n*x;        
    
    tex->memory = pointer;
    
    return pointer;
}

void swizzletexture(texture* tex)
{
    uint8* leadingpointer = (uint8*)tex->memory;
    uint8* followingpointer = (uint8*)tex->memory;
    
    for(int ii = 0; ii < tex->height*tex->width; ++ii)
    {
        uint8 r = *leadingpointer++;
        uint8 g = *leadingpointer++;
        uint8 b = *leadingpointer++;
        uint8 a = *leadingpointer++;
        
        *followingpointer++ = b;
        *followingpointer++ = g;
        *followingpointer++ = r;
        *followingpointer++ = a;
    }
}

// rotate anti-clockwise
void rotatespritesheet(spritesheet* ss)
{
    uint8* pointer = (uint8*)malloc(ss->tex.width*ss->tex.height*ss->tex.bpp);
    
    uint8* destination = (uint8*)pointer;
    uint8* source = (uint8*)ss->tex.m;
    
    int temppitch = ss->tex.height*ss->tex.bpp - ss->tex.bpp;
    
    if(ss->tex.bpp == 4)
    {
        for(int yy = 0; yy < ss->tex.height; ++yy)
        {
            destination = (uint8*)pointer + yy*4;
            
            for(int xx = 0; xx < ss->tex.width; ++xx)
            {
                uint8 r = *source++;
                uint8 g = *source++;
                uint8 b = *source++;
                uint8 a = *source++;
                
                *destination++ = b;
                *destination++ = g;
                *destination++ = r;
                *destination++ = a;
                
                destination += temppitch;
            }
        }
    } else { // assuming bpp = 1
        for(int yy = 0; yy < ss->tex.height; ++yy)
        {
            destination = (uint8*)pointer + yy;
            
            for(int xx = 0; xx < ss->tex.width; ++xx)
            {
                *destination++ = *source++;
                
                destination += temppitch;
            }
        }
    }
    
    // record the rotation
    int oldwidth = ss->tex.width;
    int oldheight = ss->tex.height;
    int oldpitch = ss->tex.pitch;
    
    ss->tex.width = oldheight;
    ss->tex.height = oldwidth;
    ss->tex.pitch = ss->tex.bpp*ss->tex.width;
    
    free(ss->tex.m);
    ss->tex.memory = pointer;
}

void createbounds(spritesheet* ss)
{
    if(ss->optimizations != 0)
    {
        free(ss->optimizations);
    }
    
    ss->optimizations = (uint8*)malloc(2*sizeof(uint8)*ss->tex.width);
    
    uint32* pointer = (uint32*)ss->tex.memory;
    uint8* opl = ss->optimizations;
    uint8* oph = ss->optimizations+1;
    
    *opl = 0;
    *oph = 0;
    
    for(int yy = 0; yy < ss->tex.height; ++yy)
    {
        for(int xx = 0; xx < ss->tex.width; ++xx)
        {
            if(*pointer > 0x00FFFFFF)
            {
                if(*opl == 0)
                {
                    *opl = xx;
                }
                *oph = xx;
            }
            pointer++;
        }
        
        // TODO: fix the sprite optimization stuff
        if(*oph > 0)
        {
            *oph += 1;
        }
        
        opl += 2;
        oph += 2;
    }
    
}

// TODO: add sprite sizes or whatever
uint8* loadspritesheet(char* filename, spritesheet* ss, SSOPTIMIZATION optimize)
{
    uint8* pointer = 0;
    
    char fn[MAX_PATH] = {0};
    strcat(fn, filename);
    
    if(getfile(fn))
    {
        pointer = loadtexture(fn, &ss->tex);
    }
    
    // sheet stuff
    ss->swidth = ss->tex.width;
    ss->sheight = ss->tex.height;
    ss->xcount = 1;
    ss->ycount = 1;
    
    switch(optimize)
    {
        case OPT_FULL:
        {
            rotatespritesheet(ss);
            createbounds(ss);
        } break;
        
        case OPT_ROTATE:
        {
            rotatespritesheet(ss);
        } break;
        
        case OPT_SWIZZLE:
        {
            swizzletexture(&ss->tex);
        } break;
        
        default:
        break;
    }
    
    return pointer;
}

int main(int argc, char *argv[])
{
    // Load the data ---------------------------------------------------------
    uint32 filesfound = 0;
    uint32 spritesfound = 0;
    
    uint32 spriteindex = 0;
    
    WIN32_FIND_DATA fd = {0};
    
    char dirsprites[MAX_PATH] = "..\\data\\sprites\\";
    char dirimages[MAX_PATH] = "..\\data\\images\\";
    char dirfonts[MAX_PATH] = "..\\data\\fonts\\";
    
    char searchsprites[MAX_PATH] = "..\\data\\sprites\\*.png";
    char searchimages[MAX_PATH] = "..\\data\\images\\*.png";
    char searchfonts[MAX_PATH] = "..\\data\\fonts\\*.png";
    
    HANDLE sh = 0;
    BOOL foundfile = 0;
    
    // sprites
    sh = FindFirstFile((LPCSTR)searchsprites, &fd);
    foundfile = (sh != INVALID_HANDLE_VALUE) ? 1 : 0;
    
    printf("Sprites:\n", filesfound-1, fd.cFileName);
    while(foundfile != 0)
    {
        filesfound++;
        printf("%3u: %s\n", filesfound-1, fd.cFileName);
        
        strncpy(&filenames[spriteindex][0], fd.cFileName, LWD_MAX_FILENAME_LENGTH);
        
        char fn[MAX_PATH];
        strcpy(&fn[0], dirsprites);
        strcat(&fn[0], fd.cFileName);
        
        loadspritesheet(fn, &spritesheets[spriteindex], OPT_FULL);
        
        texmem[spriteindex] = spritesheets[spriteindex].tex.memory;
        texopt[spriteindex] = spritesheets[spriteindex].optimizations;
        
        spriteindex++;
        foundfile = FindNextFile(sh, &fd);
    }
    FindClose(sh);
    spritesfound = filesfound;
    
    // images
    sh = FindFirstFile((LPCSTR)searchimages, &fd);
    foundfile = (sh != INVALID_HANDLE_VALUE) ? 1 : 0;
    
    printf("Images:\n", filesfound-1, fd.cFileName);
    while(foundfile != 0)
    {
        filesfound++;
        printf("%3u: %s\n", filesfound-1, fd.cFileName);
        
        strncpy(&filenames[spriteindex][0], fd.cFileName, LWD_MAX_FILENAME_LENGTH);
        
        char fn[MAX_PATH];
        strcpy(&fn[0], dirimages);
        strcat(&fn[0], fd.cFileName);
        printf("%s\n", fn);
        
        loadtexture(fn, &textures[spriteindex]);
        swizzletexture(&textures[spriteindex]);
        //loadspritesheet(fd.cFileName, &spritesheets[spriteindex], OPT_SWIZZLE);
        
        texmem[spriteindex] = textures[spriteindex].memory;
        texopt[spriteindex] = 0;
        
        spriteindex++;
        foundfile = FindNextFile(sh, &fd);
    }
    FindClose(sh);
    
    // fonts
    sh = FindFirstFile((LPCSTR)searchfonts, &fd);
    foundfile = (sh != INVALID_HANDLE_VALUE) ? 1 : 0;
    
    printf("Fonts:\n", filesfound-1, fd.cFileName);
    while(foundfile != 0)
    {
        filesfound++;
        printf("%3u: %s\n", filesfound-1, fd.cFileName);
        
        strncpy(&filenames[spriteindex][0], fd.cFileName, LWD_MAX_FILENAME_LENGTH);
        
        char fn[MAX_PATH];
        strcpy(&fn[0], dirfonts);
        strcat(&fn[0], fd.cFileName);
        
        loadtexture(fn, &textures[spriteindex]);
        
        texmem[spriteindex] = textures[spriteindex].memory;
        texopt[spriteindex] = 0;
        
        spriteindex++;
        foundfile = FindNextFile(sh, &fd);
    }
    FindClose(sh);
    
    if(filesfound == 0)
    {
        printf("no files found\n");
        return 1;
    } else {
        printf("Files found: %u", filesfound);
    }
    
    // TODO: calculate offsets
    // Calculate offsets. -----------------------------------------------------
    uint64 headersize = sizeof(littlewizarddataheader);
    uint64 spritesheetsoffset = headersize + filesfound*(LWD_MAX_FILENAME_LENGTH);
    uint64 baseoffset = spritesheetsoffset + filesfound*(sizeof(spritesheet)) + (filesfound-spritesfound)*(sizeof(texture));
    
    uint64 runningoffset = baseoffset;
    
    // sprites
    for(uint32 ii = 0; ii < spritesfound; ++ii)
    {
        spritesheets[ii].tex.memory = (void*)(runningoffset);
        runningoffset += spritesheets[ii].tex.width*spritesheets[ii].tex.height*spritesheets[ii].tex.bpp;
    }
    
    // textures
    for(uint32 ii = spritesfound; ii < filesfound; ++ii)
    {
        textures[ii].memory = (void*)(runningoffset);
        runningoffset += textures[ii].width*textures[ii].height*textures[ii].bpp;
    }
    
    // optimizations
    uint64 optimizationoffset = runningoffset;
    for(uint32 ii = 0; ii < spritesfound; ++ii)
    {
        if(spritesheets[ii].optimizations != 0)
        {
            spritesheets[ii].optimizations = (uint8*)(runningoffset);
            runningoffset += 2*spritesheets[ii].tex.width;
        }
    }
    
    // Write the data out. ----------------------------------------------------
    char filedescriptor[5] = "LWD0";
    
    littlewizarddataheader lwdh = {0};
    strncpy(lwdh.descriptor, filedescriptor, 4);
    lwdh.filesfound = filesfound;
    lwdh.imagedo = baseoffset;
    lwdh.spritedo = spritesheetsoffset;
    lwdh.optimizationsdo = optimizationoffset;
    lwdh.filenamesdo = sizeof(littlewizarddataheader);
    
    // fileheader, filename list, spriteheaders, imagedata, optimzations
    FILE* fp = fopen("littlewizard.lwd", "wb+");
    
    // fileheader
    fwrite(&lwdh, sizeof(littlewizarddataheader), 1, fp);
    
    // filenames
    fwrite(&filenames, LWD_MAX_FILENAME_LENGTH, filesfound, fp);
    
    // spritesheet headers
    fwrite(&spritesheets, sizeof(spritesheet), spritesfound, fp);
    
    // non-sprite texture headers
    fwrite(&textures, sizeof(texture), filesfound - spritesfound, fp);
    
    // sprite texutre data
    for(uint32 ii = 0; ii < filesfound; ++ii)
    {
        uint32 bytestowrite = spritesheets[ii].tex.width*spritesheets[ii].tex.height*spritesheets[ii].tex.bpp;
        fwrite(texmem[ii], bytestowrite, 1, fp);
        printf("\nBytes to Write: %u", bytestowrite);
        printf(" w: %u, h: %u, b: %u", spritesheets[ii].tex.width, spritesheets[ii].tex.height, spritesheets[ii].tex.bpp);
    }
    
    // texture texutre data
    printf("\ntexture writing");
    for(uint32 ii = 0; ii < filesfound; ++ii)
    {
        uint32 bytestowrite = textures[ii].width*textures[ii].height*textures[ii].bpp;
        fwrite(texmem[ii], bytestowrite, 1, fp);
        printf("\nBytes to Write: %u", bytestowrite);
        printf(" w: %u, h: %u, b: %u", spritesheets[ii].tex.width, spritesheets[ii].tex.height, spritesheets[ii].tex.bpp);
    }
    
    // optimizations
    for(uint32 ii = 0; ii < spritesfound; ++ii)
    {
        fwrite(&texopt[ii], sizeof(uint8), 2*spritesheets[ii].tex.width, fp);
    }
    
    printf("\n\nComplete!");
    printf("\nLWDH is %u bytes.", sizeof(littlewizarddataheader));
    printf("\nSprite headers are %u bytes total.", spritesfound*sizeof(spritesheet));
    printf("\nTexture headers are %u bytes total.", (filesfound - spritesfound)*sizeof(texture));
    
    
    // OS takes care of cleanup for us :^)
    fclose(fp); // still be polite and close resources though
    
    return 0;
}