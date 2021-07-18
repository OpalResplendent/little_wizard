// intrin.h should be called before this

// no idea where this is used
extern "C" double _hypot(double x, double y)
{
    double result = x*x + y*y;
    __m128d t = _mm_load_pd(&result);
    _mm_sqrt_pd(t);
    _mm_store_pd(&result, t);
    return result;
}

// time has to work to seed RNG
extern "C" __time64_t _time64(__time64_t *timer) {return *timer;}

// hack to get the shit to start in MSVC
void WinMainCRTStartup()
{
    int Result = WinMain(GetModuleHandle(0), 0, 0, 0);
    ExitProcess(Result);
}

// these have to work for live code reloading
extern "C" const char* strrchr(const char *s, int c)
{
    const char* ret=0;
    do {
        if( *s == (char)c )
            ret=s;
    } while(*s++);
    return ret;
}

#pragma warning(push)
#pragma warning(disable: 4706)
extern "C" char *strncpy(char *dest, const char *src, size_t n)
{
    char *ret = dest;
    do {
        if (!n--)
            return ret;
    } while (*dest++ = *src++);
    while (n--)
        *dest++ = 0;
    return ret;
}
#pragma warning(pop)



// --- me ---
// calloc
// sprintf

// --- stbi ---
// free
// malloc
// reaclloc
// fclose
// feof
// fopen_s
// fread
// fseek
// ftell
// wassert


// -- fine now??
// strncmp
// strtol
