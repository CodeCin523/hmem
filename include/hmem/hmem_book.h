#ifndef HMEM_BOOK_H
#define HMEM_BOOK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>


typedef struct hmem_page hmem_page_t;
typedef struct hmem_book hmem_book_t;

struct hmem_page {
    void *pool;
    size_t capacity;
};
struct hmem_book {
    hmem_page_t *pages;
    uint8_t current;
    uint8_t capacity;
};


bool hmem_setup_page(hmem_page_t *page, void *pool, size_t capacity);
void hmem_teardown_page(hmem_page_t *page);

bool hmem_setup_book(hmem_book_t *book, uint8_t capacity);
void hmem_teardown_book(hmem_book_t *book);

void hmem_book_push(hmem_book_t *book, hmem_page_t *page);
bool hmem_book_pop(hmem_book_t *book, hmem_page_t *outPage);


#ifdef __cplusplus
}
#endif

#endif /* HMEM_BOOK_H */


#define HMEM_IMPLEMENTATION
#if defined(HMEM_IMPLEMENTATION) && !defined(HMEM_BOOK_IMPLEMENTATION)
#define HMEM_BOOK_IMPLEMENTATION

#ifdef __cplusplus
extern "C" {
#endif

#include <hmem/utils/hmem_align.h>

#include <stdlib.h>


bool hmem_setup_page(hmem_page_t *page, void *pool, size_t capacity) {
    HMEM_CHECK_ARGS(page && pool && capacity != 0, false);

    page->pool = pool;
    page->capacity = capacity;

    return true;
}
void hmem_teardown_page(hmem_page_t *page) {
    HMEM_CHECK_ARGS(page, /*void*/);

    page->pool = NULL;
    page->capacity = 0;
}

bool hmem_setup_book(hmem_book_t *book, uint8_t capacity) {
    HMEM_CHECK_ARGS(book && capacity != 0, false);

    book->pages = calloc(capacity, sizeof(hmem_page_t));
    HMEM_CHECK_ALLOC(book->pages, false);

    book->current = 0;
    book->capacity = capacity;

    return true;
}
void hmem_teardown_book(hmem_book_t *book) {
    HMEM_CHECK_ARGS(book, /*void*/);

    if(book->pages)
        free(book->pages);

    book->pages = NULL;
    book->current = 0;
    book->capacity = 0;
}

void hmem_book_push(hmem_book_t *book, hmem_page_t *page) {
    HMEM_CHECK_ARGS(book && page, /*void*/);
    
    if(book->current >= book->capacity) {
        HMEM_CHECK_STATE(book->capacity < UINT8_MAX, /*void*/);

        void *npages = NULL;
        size_t nsize = (size_t)book->capacity * 2;
        if (nsize > UINT8_MAX)
            nsize = UINT8_MAX;

        npages = realloc(book->pages, nsize * sizeof(hmem_page_t));
        HMEM_CHECK_ALLOC(npages, /*void*/);

        book->pages = npages;
        book->capacity = nsize;
    }

    book->pages[book->current++] = *page;
}
bool hmem_book_pop(hmem_book_t *book, hmem_page_t *outPage) {
    HMEM_CHECK_ARGS(book, false);
    
    if(book->current > 0) {
        --book->current;

        if(outPage)
            *outPage = book->pages[book->current];
        return true;
    }
    return false;
}

void *hmem_book_resolve(hmem_book_t *book, uintptr_t offset) {
    for(uint8_t i = 0; i < book->current; ++i) {
        if(offset < book->pages[i].capacity) {
            return (void *) ((uintptr_t)book->pages[i].pool + offset);
        }
        offset -= book->pages[i].capacity;
    }
    return NULL;
}
size_t hmem_book_remaining(hmem_book_t *book, uintptr_t offset) {
    for(uint8_t i = 0; i < book->current; ++i) {
        if(offset < book->pages[i].capacity) {
            return book->pages[i].capacity - offset;
        }

        offset -= book->pages[i].capacity;
    }

    return 0;
}


#ifdef __cplusplus
}
#endif

#endif /* HMEM_BOOK_IMPLEMENTATION */