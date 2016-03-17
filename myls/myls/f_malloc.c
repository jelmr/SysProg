#include "myheap.h"

static size_t align_long(size_t i);
static void release_block(header_t **list, header_t *current, UINTPTR_T start,
        UINTPTR_T end, UINTPTR_T new_start, UINTPTR_T new_end);

/**
* Adds the header to the list. The memory addresses of headers in this list
* are monotonically increasing.
*/
void add_to_list(header_t *header, header_t **list)
{
    header_t *current_block;

    header->next = header->previous = 0;
    current_block = *list;
    if (!current_block)
        *list = header;
    else if (header < *list)
    {
        header->next = *list;
        *list = header;
    }
    else
    {
        while (current_block->next && header > current_block->next)
            current_block = current_block->next;

        if (current_block->next)
        {
            current_block->next->previous = header;
            header->next = current_block->next;
        }
        header->previous = current_block;
        current_block->next = header;
    }
}
/**
* Shrink block 'block' to size 'sz'. This function assumes that 'sz' is not
* greater than the size of block. The rest will be added to the supplied
* list 'list' and is thus marked as free. Nothing will happen if the rest
* is smaller than the size of a header.
*/
void shrink_block(header_t *block, header_t **list, size_t sz)
{
    header_t *next_header_start;
    size_t   next_header_size;

    next_header_start = (header_t *)((UINTPTR_T)block +
            (align_long(sizeof (header_t) + sz)));

    next_header_size = ((UINTPTR_T)block + block->size) -
            (UINTPTR_T)next_header_start;
    if (next_header_size > sizeof (header_t))
    {
        next_header_start->size = next_header_size;
        next_header_start->previous = next_header_start->next = 0;
        block->size = (size_t)((UINTPTR_T)next_header_start -
                (UINTPTR_T)block);
        add_to_list(next_header_start, list);
    }
}


/**
* Removes a block from the list, indicating it is no longer free.
*/
void remove_block(header_t *current_block, header_t **block)
{
    if (current_block->next && current_block->previous)
    {
        current_block->next->previous = current_block->previous;
        current_block->previous->next = current_block->next;
    }
    else if (current_block->next)
    {
        *block = current_block->next;
        current_block->next->previous = 0;
    }
    else if (current_block->previous)
    {
        current_block->previous->next = 0;
    }
    else
    {
        *block = 0;
    }
}


/**
* Attempts to unmap blocks or pieces thereof. Will only unmap aligned pages.
* Remainders (if any) before- or after these pages will be split up into
* new headers and added to the free list.
*/
void release_pages(header_t **list)
{
    header_t  *current;
    UINTPTR_T start;
    UINTPTR_T end;
    UINTPTR_T new_start;
    UINTPTR_T new_end;

    current = *list;
    while (current)
    {
        new_start = start = (UINTPTR_T)current;
        new_end = end = (UINTPTR_T)current + current->size;
        if (start % PAGE_SIZE != 0)
            new_start = ((start + sizeof (header_t) + 1) / PAGE_SIZE + 1) *
                    PAGE_SIZE;
        if (end % PAGE_SIZE != 0)
            new_end = ((end - sizeof (header_t) + 1) / PAGE_SIZE) * PAGE_SIZE;
        if (new_end > new_start)
        {
            release_block(list, current, start, end, new_start, new_end);
            current = *list;
        }
        else
            current = current->next;
    }
}

/**
* Merges headers in list 'list' the immediately succeed each other in memory.
*/
void merge_headers(header_t **list)
{
    header_t *current;

    current = *list;

    while (current && current->next)
    {
        if (current->next == (header_t *)((UINTPTR_T)current + current->size))
        {
            current->size += current->next->size;

            if (current->next->next)
            {
                current->next->next->previous = current;
                current->next = current->next->next;
            }
            else
                current->next = 0;
            current = *list;
        }
        else
        {
            current = current->next;
        }
    }
}


/**
* Returns the smallest multiple of long-size that can store 'i' bytes.
*/
static size_t align_long(size_t i)
{
    return (i % sizeof (long) == 0) ? i : (i / sizeof (long) + 1)
            * sizeof (long);
}

static void release_block(header_t **list, header_t *current, UINTPTR_T start,
        UINTPTR_T end, UINTPTR_T new_start, UINTPTR_T new_end)
{
    header_t *start_header;
    header_t *end_header;

    remove_block(current, list);

    if (start % PAGE_SIZE != 0)
    {
        start_header = (header_t *)start;
        start_header->size = new_start - start;
        start_header->next = start_header->previous = 0;
        start_header->list = list;
        add_to_list(start_header, list);
    }
    if (end % PAGE_SIZE != 0)
    {
        end_header = (header_t *)new_end;
        end_header->size = end - new_end;
        end_header->next = end_header->previous = 0;
        end_header->list = list;
        add_to_list(end_header, list);
    }
    munmap((header_t *)new_start, new_end - new_start);
}
