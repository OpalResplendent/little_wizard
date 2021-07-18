#include <intrin.h>

void *memset(void *dest, int c, size_t count)
{
    char *bytes = (char *)dest;
    while (count--)
    {
        *bytes++ = (char)c;
    }
    return dest;
}

void *memcpy(void *dest, const void *src, size_t count)
{
    char *dest8 = (char *)dest;
    const char *src8 = (const char *)src;
    while (count--)
    {
        *dest8++ = *src8++;
    }
    return dest;
}

// no idea where this is used, also this is probably bad
extern "C" double _hypot(double x, double y)
{
    double result = x*x + y*y;
    __m128d t = _mm_load_pd(&result);
    _mm_sqrt_pd(t);
    _mm_store_pd(&result, t);
    return result;
}

// time has to work to seed RNG, this seems to make it work
extern "C" __time64_t _time64(__time64_t *timer) {return *timer;}

// this is fine
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

// hack to get the shit to start in MSVC, .dll version
extern "C" int DLLEntry()
{
    return 1;
}
