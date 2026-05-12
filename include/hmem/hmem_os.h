#ifndef HMEM_OS_H
#define HMEM_OS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>


size_t hmem_os_page_size(void);

void *hmem_os_alloc_pages(size_t size);
void hmem_os_free_pages(void *ptr, size_t size);


#ifdef __cplusplus
}
#endif

#endif /* HMEM_OS_H */


#if defined(HMEM_IMPLEMENTATION) && !defined(HMEM_OS_IMPLEMENTATION)
#define HMEM_OS_IMPLEMENTATION

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#elif defined(__linux__)
#include <sys/mman.h>
#include <unistd.h>
#endif


#if defined(_WIN32) || defined(_WIN64)
size_t hmem_os_page_size(void) {
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return (size_t)si.dwPageSize;
}

void *hmem_os_alloc_pages(size_t size) {
    return VirtualAlloc(
        NULL,
        size,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );
}
void hmem_os_free_pages(void *ptr, size_t size) {
    VirtualFree(ptr, 0, MEM_RELEASE);
}
#elif defined(__linux__)
size_t hmem_os_page_size(void) {
    long sz = sysconf(_SC_PAGESIZE);
    return (sz > 0) ? (size_t)sz : 0;
}

void *hmem_os_alloc_pages(size_t size) {
    void* ptr = mmap(
        NULL,
        size,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1,
        0
    );

    if(ptr == MAP_FAILED)
        return NULL;

    return ptr;
}
void hmem_os_free_pages(void *ptr, size_t size) {
    munmap(ptr, size);
}
#endif


#ifdef __cplusplus
}
#endif

#endif /* HMEM_OS_IMPLEMENTATION */