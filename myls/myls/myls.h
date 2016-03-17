#ifndef MYLS_H
#define MYLS_H

    #include <stdbool.h>

    #define MAX_PATH_LENGTH 4096
    #define TERMINAL_WIDTH 80u


    typedef enum
    {
        SORT_ALPHABETICALLY,
        SORT_DATE_MODIFIED,
        SORT_SIZE,
        SORT_NONE
    } sort_mode_t;

    typedef struct ls_mode
    {
        bool        recurse;
        bool        show_hidden;
        bool        long_listing;
        bool        force_columns;
        sort_mode_t sort_mode;
    }               ls_mode_t;

    typedef struct file
    {
        struct stat *stat;
        const char        *path;
        const char        *file;
    }               file_t;

    typedef struct ls_width
    {
        unsigned name;
        unsigned links;
        unsigned uid;
        unsigned gid;
        unsigned size;
    }          ls_width_t;

    void myls();

#endif
