#ifndef HMEM_ARENA_H
#define HMEM_ARENA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <hmem/hmem_book.h>


typedef struct hmem_arena hmem_arena_t;

struct hmem_arena {
    hmem_book_t *book;
    uintptr_t page_offset;
    uint8_t page_idx;
};


bool hmem_setup_arena(hmem_arena_t *arena, hmem_book_t *book);
void hmem_teardown_arena(hmem_arena_t *arena);

void *hmem_arena_alloc(hmem_arena_t *arena, size_t size, size_t align);
void hmem_arena_reset(hmem_arena_t *arena);


#ifdef __cplusplus
}
#endif

#endif /* HMEM_ARENA_H */


#if defined(HMEM_IMPLEMENTATION) && !defined(HMEM_ARENA_IMPLEMENTATION)
#define HMEM_ARENA_IMPLEMENTATION

#ifdef __cplusplus
extern "C" {
#endif

#include <hmem/utils/hmem_align.h>


bool hmem_setup_arena(hmem_arena_t *arena, hmem_book_t *book) {
    HMEM_CHECK_ARGS(arena && book, false);

    arena->book = book;
    arena->page_offset = 0;
    arena->page_idx = 0;

    return true;
}
void hmem_teardown_arena(hmem_arena_t *arena) {
    HMEM_CHECK_ARGS(arena, /*void*/);

    arena->book = NULL;
    arena->page_offset = 0;
    arena->page_idx = 0;
}

void *hmem_arena_alloc(hmem_arena_t *arena, size_t size, size_t align) {
    HMEM_CHECK_ARGS(arena && size != 0 && hmem_is_pow2(align), NULL);
    
    hmem_book_t *book = arena->book;
    HMEM_CHECK_STATE(arena->page_idx < book->current, NULL);

    hmem_page_t *page = &book->pages[arena->page_idx];

    uintptr_t aligned = hmem_align_up((uintptr_t)page->pool + arena->page_offset, align);
    uintptr_t offset = aligned - (uintptr_t)page->pool;

    if (aligned + size <= (uintptr_t)page->pool + page->capacity) {
        arena->page_offset = offset + size;

        return (void *)aligned;
    } else {
        if(arena->page_idx+1 >= book->current)
            return NULL;

        page = &book->pages[arena->page_idx+1];
        
        aligned = hmem_align_up((uintptr_t)page->pool, align);
        HMEM_CHECK_STATE(aligned + size <= (uintptr_t)page->pool + page->capacity, NULL);

        ++arena->page_idx;
        arena->page_offset = (uintptr_t)aligned + size - (uintptr_t)page->pool;

        return (void *)aligned;
    }
}
void hmem_arena_reset(hmem_arena_t *arena) {
    HMEM_CHECK_ARGS(arena, /*void*/);

    arena->page_offset = 0;
    arena->page_idx = 0;
}


#ifdef __cplusplus
}
#endif

#endif /* HMEM_ARENA_IMPLEMENTATION */