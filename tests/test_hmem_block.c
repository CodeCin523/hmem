#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdalign.h>

#define HMEM_IMPLEMENTATION
#include <hmem/hmem_book.h>
#include <hmem/hmem_block.h>


static void test_block_basic_single_page(void)
{
    hmem_book_t book;
    assert(hmem_setup_book(&book, 1));

    alignas(max_align_t) unsigned char buffer[256];

    hmem_page_t page;
    assert(hmem_setup_page(&page, buffer, sizeof(buffer)));

    assert(hmem_book_push(&book, &page));

    hmem_block_t block;
    assert(hmem_setup_block(&block, &book, 32, 8));

    void *a = hmem_block_alloc_single(&block);
    void *b = hmem_block_alloc_single(&block);

    assert(a != NULL);
    assert(b != NULL);
    assert(a != b);

    assert((uintptr_t)b > (uintptr_t)a);

    assert((uintptr_t)a >= (uintptr_t)buffer);
    assert((uintptr_t)a <  (uintptr_t)(buffer + sizeof(buffer)));

    assert((uintptr_t)b >= (uintptr_t)buffer);
    assert((uintptr_t)b <  (uintptr_t)(buffer + sizeof(buffer)));

    hmem_teardown_block(&block);
    hmem_teardown_book(&book);

    printf("test_block_basic_single_page OK\n");
}


static void test_block_alignment(void)
{
    hmem_book_t book;
    assert(hmem_setup_book(&book, 1));

    alignas(16) unsigned char buffer[512];

    hmem_page_t page;
    assert(hmem_setup_page(&page, buffer, sizeof(buffer)));

    assert(hmem_book_push(&book, &page));

    hmem_block_t block;
    assert(hmem_setup_block(&block, &book, 1, 16));

    void *a = hmem_block_alloc_single(&block);
    void *b = hmem_block_alloc_single(&block);
    void *c = hmem_block_alloc_single(&block);

    assert(a != NULL);
    assert(b != NULL);
    assert(c != NULL);

    assert(((uintptr_t)a % 16) == 0);
    assert(((uintptr_t)b % 16) == 0);
    assert(((uintptr_t)c % 16) == 0);

    hmem_teardown_block(&block);
    hmem_teardown_book(&book);

    printf("test_block_alignment OK\n");
}


static void test_block_free_and_reuse(void)
{
    hmem_book_t book;
    assert(hmem_setup_book(&book, 1));

    alignas(max_align_t) unsigned char buffer[128];

    hmem_page_t page;
    assert(hmem_setup_page(&page, buffer, sizeof(buffer)));

    assert(hmem_book_push(&book, &page));

    hmem_block_t block;
    assert(hmem_setup_block(&block, &book, 16, 8));

    void *a = hmem_block_alloc_single(&block);
    void *b = hmem_block_alloc_single(&block);

    assert(a != NULL);
    assert(b != NULL);
    assert(a != b);

    hmem_block_free_single(&block, a);
    hmem_block_free_single(&block, b);

    void *c = hmem_block_alloc_single(&block);
    void *d = hmem_block_alloc_single(&block);

    assert(c != NULL);
    assert(d != NULL);

    assert(c == a || c == b);
    assert(d == a || d == b);
    assert(c != d);

    hmem_teardown_block(&block);
    hmem_teardown_book(&book);

    printf("test_block_free_and_reuse OK\n");
}


static void test_block_exhaustion(void)
{
    hmem_book_t book;
    assert(hmem_setup_book(&book, 1));

    alignas(max_align_t) unsigned char buffer[64];

    hmem_page_t page;
    assert(hmem_setup_page(&page, buffer, sizeof(buffer)));

    assert(hmem_book_push(&book, &page));

    hmem_block_t block;
    assert(hmem_setup_block(&block, &book, 16, 8));

    /*
        64-byte page
        16-byte blocks
        => 4 allocations maximum
    */

    void *ptrs[4];

    for (size_t i = 0; i < 4; ++i) {
        ptrs[i] = hmem_block_alloc_single(&block);
        assert(ptrs[i] != NULL);
    }

    void *extra = hmem_block_alloc_single(&block);
    assert(extra == NULL);

    hmem_teardown_block(&block);
    hmem_teardown_book(&book);

    printf("test_block_exhaustion OK\n");
}


static void test_block_full_reuse(void)
{
    hmem_book_t book;
    assert(hmem_setup_book(&book, 1));

    alignas(max_align_t) unsigned char buffer[128];

    hmem_page_t page;
    assert(hmem_setup_page(&page, buffer, sizeof(buffer)));

    assert(hmem_book_push(&book, &page));

    hmem_block_t block;
    assert(hmem_setup_block(&block, &book, 16, 8));

    void *ptrs[8];

    for (size_t i = 0; i < 8; ++i) {
        ptrs[i] = hmem_block_alloc_single(&block);
        assert(ptrs[i] != NULL);
    }

    assert(hmem_block_alloc_single(&block) == NULL);

    for (size_t i = 0; i < 8; ++i) {
        hmem_block_free_single(&block, ptrs[i]);
    }

    for (size_t i = 0; i < 8; ++i) {
        void *p = hmem_block_alloc_single(&block);
        assert(p != NULL);
    }

    assert(hmem_block_alloc_single(&block) == NULL);

    hmem_teardown_block(&block);
    hmem_teardown_book(&book);

    printf("test_block_full_reuse OK\n");
}


int main(void)
{
    test_block_basic_single_page();
    test_block_alignment();
    test_block_free_and_reuse();
    test_block_exhaustion();
    test_block_full_reuse();

    printf("ALL BLOCK TESTS PASSED\n");
    return 0;
}