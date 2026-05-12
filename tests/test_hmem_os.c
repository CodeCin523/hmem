#include <stdio.h>
#include <stdint.h>

#define HMEM_IMPLEMENTATION
#include "hmem/hmem_os.h"

int main(void) {
    size_t page_size = hmem_os_page_size();

    if (page_size == 0) {
        fprintf(stderr, "Failed to get OS page size\n");
        return 1;
    }

    printf("OS page size: %zu bytes\n", page_size);

    /* Allocate one page */
    void *ptr = hmem_os_alloc_pages(page_size);

    if (!ptr) {
        fprintf(stderr, "Failed to allocate pages\n");
        return 1;
    }

    printf("Allocated %zu bytes at %p\n", page_size, ptr);

    /* Test memory access */
    uint8_t *bytes = (uint8_t *)ptr;

    for (size_t i = 0; i < page_size; ++i) {
        bytes[i] = (uint8_t)(i & 0xFF);
    }

    /* Verify memory */
    for (size_t i = 0; i < page_size; ++i) {
        if (bytes[i] != (uint8_t)(i & 0xFF)) {
            fprintf(stderr, "Memory verification failed at byte %zu\n", i);

            hmem_os_free_pages(ptr, page_size);
            return 1;
        }
    }

    printf("Memory verification successful\n");

    /* Free pages */
    hmem_os_free_pages(ptr, page_size);

    printf("Freed pages successfully\n");

    return 0;
}