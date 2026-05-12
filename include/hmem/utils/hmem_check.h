#ifndef HMEM_UTILS_CHECK_H
#define HMEM_UTILS_CHECK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>


/* #if defined(HMEM_NO_CHECKS)

    #define HMEM_CHECK(expr, ret) \
        do { (void)sizeof(expr); } while (0)

#elif defined(HMEM_DEBUG)

    #include <assert.h>

    #define HMEM_CHECK(expr, ret) \
        do { \
            assert(expr); \
        } while (0)

#else

    #define HMEM_CHECK(expr, ret) \
        do { \
            if (!(expr)) \
                return ret; \
        } while (0)

#endif*/

#define HMEM_PACK_ARGS(...) __VA_ARGS__

#define HMEM_CHECK_ARGS(expr, ret)  \
    do {                            \
        if(!(expr)) return ret;     \
    } while(0)

#define HMEM_CHECK_ALLOC(expr, ret) \
    do {                            \
        assert(expr);               \
    } while(0)

#define HMEM_CHECK_STATE(expr, ret) \
    do {                            \
        assert(expr);               \
    } while(0)


#ifdef __cplusplus
}
#endif

#endif /* HMEM_UTILS_CHECK_H */