#include "myheap.h"
#include "my_memcpy.h"

static void *expand_block(header_t *header, void *ptr, size_t new_sz);

void *my_realloc(void *ptr, unsigned long sz)
{
    header_t *header;

    if (!ptr)
        return my_malloc(sz);

    header = (header_t *)((UINTPTR_T)ptr - sizeof (header_t));

    if (sz > header->size)
        return expand_block(header, ptr, sz);
    else if (sz < header->size)
    {
        shrink_block(header, header->list, sz);
        merge_headers(header->list);
        release_pages(header->list);
    }

    return ptr;
}

static void *expand_block(header_t *header, void *ptr, size_t new_sz)
{
    void *new_block;

    new_block = my_malloc(new_sz);
    my_memcpy(new_block, ptr, (unsigned)((unsigned)header->size -
            sizeof (header_t)));
    my_free(ptr);
    return new_block;
}

