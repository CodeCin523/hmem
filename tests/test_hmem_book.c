#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HMEM_IMPLEMENTATION
#include <hmem/hmem_book.h>


static void test_basic_page_setup(void) {
    hmem_page_t page;
    char buffer[128];

    hmem_setup_page(&page, buffer, sizeof(buffer));

    assert(page.pool == buffer);
    assert(page.capacity == sizeof(buffer));

    hmem_teardown_page(&page);

    assert(page.pool == NULL);
    assert(page.capacity == 0);

    printf("test_basic_page_setup OK\n");
}


static void test_book_push_pop(void) {
    hmem_book_t book;
    hmem_setup_book(&book, 2);

    char a[64], b[64];

    hmem_page_t p1 = { a, sizeof(a) };
    hmem_page_t p2 = { b, sizeof(b) };

    hmem_book_push(&book, &p1);
    hmem_book_push(&book, &p2);

    assert(book.current == 2);

    hmem_book_pop(&book, NULL);
    assert(book.current == 1);

    hmem_book_pop(&book, NULL);
    assert(book.current == 0);

    hmem_teardown_book(&book);

    printf("test_book_push_pop OK\n");
}


static void test_book_resolve_single_page(void) {
    hmem_book_t book;
    hmem_setup_book(&book, 1);

    char buffer[100];

    hmem_page_t p = { buffer, sizeof(buffer) };
    hmem_book_push(&book, &p);

    buffer[42] = 123;

    void *ptr = hmem_book_resolve(&book, 42);

    assert(ptr == &buffer[42]);
    assert(*(unsigned char *)ptr == 123);

    hmem_teardown_book(&book);

    printf("test_book_resolve_single_page OK\n");
}


static void test_book_resolve_multi_page(void) {
    hmem_book_t book;
    hmem_setup_book(&book, 2);

    char p1[50];
    char p2[50];

    memset(p1, 0, sizeof(p1));
    memset(p2, 0, sizeof(p2));

    p2[10] = 77;

    hmem_page_t page1 = { p1, sizeof(p1) };
    hmem_page_t page2 = { p2, sizeof(p2) };

    hmem_book_push(&book, &page1);
    hmem_book_push(&book, &page2);

    // offset 60 → page2[10]
    void *ptr = hmem_book_resolve(&book, 60);

    assert(ptr == &p2[10]);
    assert(*(unsigned char *)ptr == 77);

    // remaining should match page2
    size_t rem = hmem_book_remaining(&book, 60);
    assert(rem == sizeof(p2) - 10);

    hmem_teardown_book(&book);

    printf("test_book_resolve_multi_page OK\n");
}


int main(void) {
    test_basic_page_setup();
    test_book_push_pop();
    test_book_resolve_single_page();
    test_book_resolve_multi_page();

    printf("ALL TESTS PASSED\n");
    return 0;
}