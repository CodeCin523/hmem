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