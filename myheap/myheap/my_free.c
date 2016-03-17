#include "myheap.h"

void my_free(void *ptr)
{
    header_t *header;

    if (ptr)
    {
        header = (header_t *)((UINTPTR_T)ptr - sizeof (header_t));
        add_to_list(header, header->list);
        merge_headers(header->list);
        release_pages(header->list);
    }
}
