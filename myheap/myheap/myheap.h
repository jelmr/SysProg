#ifndef MYHEAP_H
#define MYHEAP_H

  #include <stddef.h>
  #include <unistd.h>
  #include <sys/mman.h>

  #ifndef MAP_ANONYMOUS
  #  ifdef MAP_ANON
  #    define MAP_ANONYMOUS MAP_ANON
  #  else
  #    define MAP_ANONYMOUS 0x1000
  #  endif
  #endif

  #ifdef HAVE_GETPAGESIZE
  #  define PAGE_SIZE (size_t)getpagesize()
  #else
  #  ifdef _SC_PAGESIZE
  #    define PAGE_SIZE (size_t)sysconf(_SC_PAGESIZE)
  #  else
  #    define PAGE_SIZE (size_t)8192u
  #  endif
  #endif

  #define UINTPTR_T unsigned long long

  typedef struct header
  {
      struct header *next;
      struct header *previous;
      struct header **list;
      size_t        size;
  }                 header_t;

  void remove_block(header_t *current_block, header_t **block);
  void shrink_block(header_t *block, header_t **list, size_t sz);
  void add_to_list(header_t *header, header_t **list);
  void merge_headers(header_t **list);
  void release_pages(header_t **list);
  void *my_malloc(size_t x);
  void my_free(void *ptr);
  void *my_realloc(void *ptr, size_t sz);
#endif
