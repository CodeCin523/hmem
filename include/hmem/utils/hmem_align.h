#ifndef HMEM_UTILS_ALIGN_H
#define HMEM_UTILS_ALIGN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <hmem/utils/hmem_check.h>

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>


/**
 * Check if a value is a power-of-2 (>0).
 * Returns 1 if true, 0 otherwise.
 */
static inline bool hmem_is_pow2(uintptr_t v) {
    return v != 0 && (v & (v - 1)) == 0;
}

/**
 * Check if value is aligned to `align`.
 * Returns 1 if true, 0 otherwise.
 * `align` must be a power of 2.
 */
static inline bool hmem_is_aligned(uintptr_t v, uintptr_t align) {
    HMEM_CHECK_ARGS(hmem_is_pow2(align), 0);
    return (v & (align - 1)) == 0;
}

/**
 * Align a value UP to the next multiple of `align`.
 * Returns the next multiple of `align` >= value.
 * `align` must be >0 and a power-of-2.
 * Returns 0 on invalid input or overflow.
 */
static inline uintptr_t hmem_align_up(uintptr_t v, uintptr_t align) {
    HMEM_CHECK_ARGS(hmem_is_pow2(align), 0);

    const uintptr_t mask = align - 1;

    HMEM_CHECK_ARGS(v <= UINTPTR_MAX - mask, 0);

    return (v + mask) & ~mask;
}

/**
 * Align a value DOWN to the previous multiple of `align`.
 * Returns the last multiple of `align` <= value.
 * `align` must be >0 and a power-of-2.
 * Returns 0 on invalid input.
 */
static inline uintptr_t hmem_align_down(uintptr_t v, uintptr_t align) {
    HMEM_CHECK_ARGS(hmem_is_pow2(align), 0);

    return v & ~(align - 1);
}

/**
 * Clamp value between min and max.
 */
static inline size_t hmem_clamp(size_t v, size_t min, size_t max) {
    const size_t t = v < min ? min : v;
    return t > max ? max : t;
}


#ifdef __cplusplus
}
#endif

#endif /* HMEM_UTILS_ALIGN_H */