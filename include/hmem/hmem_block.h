#ifndef HMEM_BLOCK_H
#define HMEM_BLOCK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <hmem/hmem_book.h>


typedef struct hmem_block hmem_block_t;

struct hmem_block {
    hmem_book_t *book;
    void *top;
    uintptr_t block_align;
    size_t block_size;
    // size_t block_count; // do not make sense in my current system
    uint8_t book_last_current;
};


bool hmem_setup_block(hmem_block_t *block, hmem_book_t *book, size_t block_size, uintptr_t block_align);
void hmem_teardown_block(hmem_block_t *block);

void hmem_block_update(hmem_block_t *block);

void *hmem_block_alloc_single(hmem_block_t *block);
void hmem_block_free_single(hmem_block_t *block, void *addr);

// void *hmem_block_alloc_run(hmem_block_t *block, size_t len);
// void hmem_block_free_run(hmem_block_t *block, void *addr, size_t len);


#ifdef __cplusplus
}
#endif

#endif /* HMEM_BLOCK_H */


#if defined(HMEM_IMPLEMENTATION) && !defined(HMEM_BLOCK_IMPLEMENTATION)
#define HMEM_BLOCK_IMPLEMENTATION

#ifdef __cplusplus
extern "C" {
#endif

#include <hmem/utils/hmem_align.h>

#include <stdalign.h>


struct hmem_block_meta {
    struct hmem_block_meta *next;
    size_t count;
};


bool hmem_setup_block(hmem_block_t *block, hmem_book_t *book, size_t block_size, uintptr_t block_align) {
    HMEM_CHECK_ARGS(block && book && block_size != 0 && hmem_is_pow2(block_align), false);

    if (block_size < sizeof(struct hmem_block_meta))
        block_size = sizeof(struct hmem_block_meta);

    // Ensure alignment is at least as big as meta requires
    uintptr_t meta_align = alignof(struct hmem_block_meta);
    if (block_align < meta_align)
        block_align = meta_align;

    block_size = hmem_align_up(block_size, block_align);

    block->book = book;
    block->block_size = block_size;
    block->block_align = block_align;

    // Technically not needed if update works
    // struct hmem_block_meta *top = book->pages[0].pool;
    // top->next = NULL;
    // top->count = book->pages[0].capacity / block->block_size;

    // block->top = (void *)top;
    // block->book_last_current = 1;
    block->book_last_current = 0;
    block->top = NULL;
    hmem_block_update(block);

    return true;
}
void hmem_teardown_block(hmem_block_t *block) {
    HMEM_CHECK_ARGS(block, /*void*/);

    block->book = NULL;
    block->top = NULL;
    block->block_align = 0;
    block->block_size = 0;
    block->book_last_current = 0;
}

void hmem_block_update(hmem_block_t *block) {
    HMEM_CHECK_ARGS(block && block->book, /*void*/);

    hmem_book_t *book = block->book;

    for(size_t i = block->book_last_current; i < book->current; ++i) {
        hmem_page_t *page = &book->pages[i];

        uintptr_t raw_addr = (uintptr_t)page->pool;
        uintptr_t pool_addr = hmem_align_up(raw_addr, block->block_align);
        size_t skipped = (size_t)(pool_addr - raw_addr);

        if(skipped >= page->capacity)
            continue;

        size_t usable = page->capacity - skipped;
        size_t pool_block_count = usable / block->block_size;

        if(pool_block_count == 0)
            continue;

        struct hmem_block_meta *meta = (struct hmem_block_meta *)pool_addr;

        meta->next = NULL;
        meta->count = pool_block_count;

        struct hmem_block_meta **p = (struct hmem_block_meta **)&block->top;

        while(*p && (uintptr_t)(*p) < pool_addr)
            p = &(*p)->next;

        meta->next = *p;
        *p = meta;
    }

    block->book_last_current = book->current;
}

void *hmem_block_alloc_single(hmem_block_t *block) {
    HMEM_CHECK_ARGS(block, NULL);

    if(block->top == NULL)
        return NULL;

    struct hmem_block_meta *meta = (struct hmem_block_meta *) block->top;

    void *result = (void*)meta;

    if(meta->count == 1) {
        block->top = (void *)meta->next;
    } else { // bigger than 1
        struct hmem_block_meta *next = (struct hmem_block_meta *)((uintptr_t)meta + block->block_size);
        next->next = meta->next;
        next->count = --meta->count;
        block->top = (void *)next;
    }

    meta->next = NULL;
    meta->count = 0; // allocated block is now considered “in use”
    
    return result;
}
void hmem_block_free_single(hmem_block_t *block, void *addr) {
    HMEM_CHECK_ARGS(block && addr, /*void*/);

    struct hmem_block_meta *meta = (struct hmem_block_meta *)addr;
    meta->count = 1;
    meta->next = NULL;

    struct hmem_block_meta **p = (struct hmem_block_meta **)&block->top;
    struct hmem_block_meta *prev = NULL;

    while(*p && (uintptr_t)(*p) < (uintptr_t)meta) {
        prev = *p;
        p = &(*p)->next;
    }

    meta->next = *p;
    *p = meta;

    // Merge with next if contiguous
    if (meta->next && (uintptr_t)meta + block->block_size * meta->count == (uintptr_t)meta->next) {
        meta->count += meta->next->count;
        meta->next = meta->next->next;
    }

    // Merge with previous if contiguous
    if (prev && (uintptr_t)prev + block->block_size * prev->count == (uintptr_t)meta) {
        prev->count += meta->count;
        prev->next = meta->next;
    }
}


#ifdef __cplusplus
}
#endif

#endif /* HMEM_BLOCK_IMPLEMENTATION */