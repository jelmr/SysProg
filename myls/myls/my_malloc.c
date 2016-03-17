#include "myheap.h"


static header_t *find_header(header_t **list, size_t sz);
static size_t determine_size(unsigned long x);

void *my_malloc(unsigned long x)
{
    static header_t *free = 0;
    header_t        *block;

    block = find_header(&free, x);
    shrink_block(block, &free, x);
    block->list = &free;

    return (void *)((char *)block + sizeof (header_t));
}

/**
* Returns the minimum amount of bytes needed to store x bytes and a header,
* rounded up to the nearest page.
*/
static size_t determine_size(unsigned long x)
{
    size_t base_size;

    base_size = x + sizeof (header_t);

    if (base_size % PAGE_SIZE == 0)
        return base_size;
    else
        return (base_size / PAGE_SIZE + 1) * PAGE_SIZE;
}

/**
* Attempts to find a free, suitable block. If no such block exists,
* creates one.
*/
static header_t *find_header(header_t **list, size_t sz)
{
    header_t *current_block;
    size_t   total_size;

    current_block = *list;
    total_size = determine_size(sz);

    while (current_block && current_block->size < (sz + sizeof (header_t)))
        current_block = current_block->next;

    if (!current_block)
    {
        current_block = mmap(0, total_size, PROT_READ | PROT_WRITE,
                MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
        current_block->size = total_size;
    }
    else
        remove_block(current_block, list);

    current_block->next = current_block->previous = 0;
    return current_block;
}


