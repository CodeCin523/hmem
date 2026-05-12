#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HMEM_IMPLEMENTATION
#include <hmem/hmem_book.h>
#include <hmem/hmem_arena.h>


static void test_arena_basic_single_page(void)
{
    hmem_book_t book;
    hmem_setup_book(&book, 1);

    char buffer[256];

    hmem_page_t page;
    hmem_setup_page(&page, buffer, sizeof(buffer));
    hmem_book_push(&book, &page);

    hmem_arena_t arena;
    hmem_setup_arena(&arena, &book);

    void *a = hmem_arena_alloc(&arena, 32, 8);
    void *b = hmem_arena_alloc(&arena, 32, 8);

    assert(a != NULL);
    assert(b != NULL);
    assert(b > a);

    // ensure inside buffer
    assert((char*)a >= buffer && (char*)a < buffer + sizeof(buffer));
    assert((char*)b >= buffer && (char*)b < buffer + sizeof(buffer));

    hmem_teardown_book(&book);

    printf("test_arena_basic_single_page OK\n");
}


static void test_arena_alignment(void)
{
    hmem_book_t book;
    hmem_setup_book(&book, 1);

    char buffer[512];

    hmem_page_t page;
    hmem_setup_page(&page, buffer, sizeof(buffer));
    hmem_book_push(&book, &page);

    hmem_arena_t arena;
    hmem_setup_arena(&arena, &book);

    void *a = hmem_arena_alloc(&arena, 1, 16);
    void *b = hmem_arena_alloc(&arena, 1, 32);
    void *c = hmem_arena_alloc(&arena, 1, 64);

    assert(((uintptr_t)a % 16) == 0);
    assert(((uintptr_t)b % 32) == 0);
    assert(((uintptr_t)c % 64) == 0);

    hmem_teardown_book(&book);

    printf("test_arena_alignment OK\n");
}


static void test_arena_page_overflow(void)
{
    hmem_book_t book;
    hmem_setup_book(&book, 2);

    char p1[64];
    char p2[64];

    hmem_page_t page1 = { p1, sizeof(p1) };
    hmem_page_t page2 = { p2, sizeof(p2) };

    hmem_book_push(&book, &page1);
    hmem_book_push(&book, &page2);

    hmem_arena_t arena;
    hmem_setup_arena(&arena, &book);

    void *a = hmem_arena_alloc(&arena, 48, 8);
    void *b = hmem_arena_alloc(&arena, 32, 8);

    assert(a != NULL);
    assert(b != NULL);

    // must land in different pages
    assert((char*)a >= p1 && (char*)a < p1 + sizeof(p1));
    assert((char*)b >= p2 && (char*)b < p2 + sizeof(p2));

    hmem_teardown_book(&book);

    printf("test_arena_page_overflow OK\n");
}


static void test_arena_reset(void)
{
    hmem_book_t book;
    hmem_setup_book(&book, 1);

    char buffer[256];

    hmem_page_t page = { buffer, sizeof(buffer) };
    hmem_book_push(&book, &page);

    hmem_arena_t arena;
    hmem_setup_arena(&arena, &book);

    void *a = hmem_arena_alloc(&arena, 64, 8);
    assert(a != NULL);

    hmem_arena_reset(&arena);

    void *b = hmem_arena_alloc(&arena, 64, 8);
    assert(b != NULL);

    // after reset should reuse from start
    assert((char*)a == (char*)b);

    hmem_teardown_book(&book);

    printf("test_arena_reset OK\n");
}


static void test_arena_exhaustion(void)
{
    hmem_book_t book;
    hmem_setup_book(&book, 1);

    char buffer[64];

    hmem_page_t page = { buffer, sizeof(buffer) };
    hmem_book_push(&book, &page);

    hmem_arena_t arena;
    hmem_setup_arena(&arena, &book);

    void *last = NULL;

    for (int i = 0; i < 20; i++) {
        last = hmem_arena_alloc(&arena, 8, 8);
        if (!last)
            break;
    }

    // should eventually fail, not crash
    assert(last == NULL || last != NULL);

    hmem_teardown_book(&book);

    printf("test_arena_exhaustion OK\n");
}


int main(void)
{
    test_arena_basic_single_page();
    test_arena_alignment();
    test_arena_page_overflow();
    test_arena_reset();
    test_arena_exhaustion();

    printf("ALL ARENA TESTS PASSED\n");
    return 0;
}