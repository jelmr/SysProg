#include "myls.h"
#include "my_conv.h"
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>

static int print_entry(char const *path, ls_mode_t *mode);
static int print_multiple_dirs(int c, char *a[], int i, ls_mode_t *mode);
static int get_mode(int c, char *a[], ls_mode_t **mode);
static void process_flag(ls_mode_t **mode, char const flag);
static int fill_file_array(const char *path, file_t *files, ls_mode_t *mode);
static struct stat *get_stat(char const *path, char *file,
        struct stat *entry_stat);
static void determine_max_widths(size_t const size, file_t files[],
        ls_width_t *widths);
static void print_dir_long(const unsigned size, file_t files[]);
static void print_dir_columns(const unsigned size, file_t files[]);
static void print_dir_regular(const unsigned size, file_t *files);
static int count_files(char const *path, ls_mode_t *mode);
static void print_symlink(const struct file file);
static void get_file_permissions(char *buf, mode_t mode);
static int cmp_date (const void * a, const void * b);
static int cmp_name (const void * a, const void * b);
static int cmp_size (const void * a, const void * b);
static int print_file(file_t *file, ls_mode_t *mode);
static int print_dir(char const *path, ls_mode_t *mode);
static int recurse(const char *path, ls_mode_t *mode, const int size,
        file_t files[]);
static void sort(file_t *files, ls_mode_t *mode, int size);

int main(int c, char *a[])
{
    ls_mode_t *mode;
    int       flags_strings_read;

    flags_strings_read = get_mode(c, a, &mode);

    if (c - flags_strings_read == 0)
        return print_entry(".", mode);
    else
        return print_multiple_dirs(c, a, flags_strings_read, mode);

}

/**
* Prints an entry (dir or regular file) pointed to by 'path' according
* to the values of 'mode'.
*/
static int print_entry(const char *path, ls_mode_t *mode)
{
    struct stat    *entry_stat;
    file_t         *file;

    entry_stat = malloc(sizeof (struct stat));
    lstat(path, entry_stat);

    file = malloc(sizeof (file_t));
    file->stat = entry_stat;
    file->file = file->path = path;

    if (!S_ISDIR(entry_stat->st_mode))
        return print_file(file, mode);
    else
        return print_dir(path, mode);
}

int print_dir(char const *path, ls_mode_t *mode)
{
    const int size = count_files(path, mode);
    file_t    files[size];

    printf("%s:\n", path);

    if (size == -1)
        return 1;

    fill_file_array(path, files, mode);

    if (mode->long_listing)
        print_dir_long((unsigned)size, files);
    else if (mode->force_columns || isatty(1))
        print_dir_columns((unsigned)size, files);
    else
        print_dir_regular((unsigned)size, files);

    return recurse(path, mode, size, files);
}

int recurse(const char *path, ls_mode_t *mode, const int size, file_t files[])
{
    struct file file;
    int         i;
    char        *new_path;

    for (i = 0; i < size; ++i)
    {
        file = files[i];
        if (S_ISDIR(file.stat->st_mode) && mode->recurse &&
                (strncmp(file.file, ".", 2) != 0) &&
                (strncmp(file.file, "..", 3) != 0))
        {
            printf("\n");
            new_path = malloc(strlen(path) + strlen(file.file) + 3);
            if (path[strlen(new_path) - 1] != '/')
                snprintf(new_path, PATH_MAX + 1, "%s%c%s", path, '/',
                        file.file);
            else
                snprintf(new_path, PATH_MAX + 1, "%s%s", path, file.file);
            if (print_entry(new_path, mode) == 1)
            {
                free(new_path);
                return 1;
            }
            free(new_path);
        }
    }
    return 0;
}

static int print_file(file_t *file, ls_mode_t *mode)
{
    if (mode->long_listing)
        print_dir_long(1, file);
    else if (isatty(1))
        print_dir_columns(1, file);
    else
        print_dir_regular(1, file);
    return 0;
}

/**
* Prints multiple dirs, calling 'print_entry' for each dir.
*/
static int print_multiple_dirs(int c, char *a[], int i, ls_mode_t *mode)
{
    for (; i < c; ++i)
    {
        if (print_entry(a[i], mode) == 1)
            return 1;
        if (i + 1 < c)
            printf("\n");
    }

    return 0;
}



/************************
*     HANDLE FLAGS     *
***********************/

/**
* Sets the value of the ls_mode_t 'mode' according to the supplied flags.
* This ls_mode_t will be passed along to other methods, so they know how
* to structure the output.
*
* Note that all flag-arguments must appear before the first
* directory-argument is given.
*/
static int get_mode(int c, char *a[], ls_mode_t **mode)
{
    int i;
    int j;

    i = 1;
    j = 0;

    *mode = malloc(sizeof (ls_mode_t));
    (*mode)->recurse = false;
    (*mode)->show_hidden = false;
    (*mode)->long_listing = false;
    (*mode)->force_columns = false;
    (*mode)->sort_mode = SORT_ALPHABETICALLY;

    while (i < c && a[i][0] == '-')
    {
        while ((a[i][j++]))
            process_flag(mode, a[i][j]);

        j = 0;
        i++;
    }
    return i;
}

static void process_flag(ls_mode_t **mode, char const flag)
{
    if (flag == 'a')
        (*mode)->show_hidden = true;
    else if (flag == 'R')
        (*mode)->recurse = true;
    else if (flag == 'l')
        (*mode)->long_listing = true;
    else if (flag == 'C')
        (*mode)->force_columns = true;
    else if (flag == 't')
        (*mode)->sort_mode = SORT_DATE_MODIFIED;
    else if (flag == 'f')
        (*mode)->sort_mode = SORT_NONE;
    else if (flag == 'S')
        (*mode)->sort_mode = SORT_SIZE;
}


/************************
*     PREPROCESSING    *
***********************/

/**
* Builds an internal representation of the contents of a directory. This
* allows for easy sorting and printing.
*/
static int fill_file_array(char const *path, file_t *files, ls_mode_t *mode)
{
    DIR            *dir;
    struct stat    *entry_stat;
    struct dirent  *entry;
    char           *file;
    int            i;

    i = 0;
    if ((dir = opendir(path)))
    {
        while ((entry = readdir(dir)))
            if (entry->d_name[0] != '.' || mode->show_hidden)
            {
                file = malloc(strlen(entry->d_name) + 1);
                strcpy(file, entry->d_name);
                entry_stat = malloc(sizeof(struct stat));
                files[i].stat = get_stat(path, entry->d_name, entry_stat);
                files[i].file = file;
                files[i].path = path;
                i++;
            }
    }
    else
        return 1;
    sort(files, mode, i);
    return 0;
}

static void sort(file_t *files, ls_mode_t *mode, int size)
{
    if (mode->sort_mode == SORT_DATE_MODIFIED)
        qsort(files, (size_t) size, sizeof (file_t), cmp_date);
    if (mode->sort_mode == SORT_ALPHABETICALLY)
        qsort(files, (size_t) size, sizeof (file_t), cmp_name);
    if (mode->sort_mode == SORT_SIZE)
        qsort(files, (size_t) size, sizeof (file_t), cmp_size);
}

static struct stat *get_stat(char const *path, char *file,
        struct stat *entry_stat)
{
    const unsigned long path_length = strlen(path);
    const unsigned long name_length = strlen(file);
    char                *s = malloc(path_length + name_length + 4);

    strcpy(s, path);
    strcat(s, "/");
    strcat(s, file);
    lstat(s, entry_stat);

    return entry_stat;
}

/**
* Determines the maximum width (most characters) of each variable across all
* files. This is used to determine the amount of columns used for each
* variable in the long-list and column views.
*/
static void determine_max_widths(size_t const size, file_t files[],
        ls_width_t *widths)
{
    unsigned i;
    unsigned name_sz;
    unsigned links_sz;
    unsigned uid_sz;
    unsigned gid_sz;
    unsigned size_sz;

    widths->name = widths->links = widths->uid = widths->gid =
            widths->size = 0;

    for (i = 0; i < (unsigned) size; ++i)
    {
        name_sz = (unsigned)strlen(files[i].file);
        links_sz = (unsigned)f_count_digits_i((int)files[i].stat->st_nlink,
                10);
        uid_sz = (unsigned)f_count_digits_i((int)files[i].stat->st_uid, 10);
        gid_sz = (unsigned)f_count_digits_i((int)files[i].stat->st_gid, 10);
        size_sz = (unsigned)f_count_digits_i((int)files[i].stat->st_size, 10);

        widths->name = (name_sz > widths->name) ? name_sz : widths->name;
        widths->links = (links_sz > widths->links) ? links_sz : widths->links;
        widths->uid = (uid_sz > widths->uid) ? uid_sz : widths->uid;
        widths->gid = (gid_sz > widths->gid) ? gid_sz : widths->gid;
        widths->size = (size_sz > widths->size) ? size_sz : widths->size;
    }
}

/************************
*      PRINT MODES      *
***********************/

/**
* Prints in the long-listing format. Used when flag -l is given.
*/
static void print_dir_long(const unsigned size, file_t files[])
{
    struct file file;
    struct tm   ts;
    unsigned    i;
    ls_width_t  *widths;
    char        time_modified[50];
    char        permission[11];

    widths = malloc(sizeof (ls_width_t));
    determine_max_widths(size, files, widths);

    for (i = 0; i < size; ++i)
    {
        file = files[i];
        ts = *localtime(&file.stat->st_mtime);
        strftime(time_modified, 49, "%Y-%m-%d %H:%M:%S", &ts);
        get_file_permissions(permission, file.stat->st_mode);
        printf("%s %*u %*u %*d %*lldB %s %s", permission, widths->links,
                (unsigned)file.stat->st_nlink, widths->uid, file.stat->st_uid,
                widths->gid, file.stat->st_gid, widths->size,
                (unsigned long long)file.stat->st_size, time_modified,
                file.file);
        if (S_ISLNK(file.stat->st_mode))
            print_symlink(file);
        printf("\n");
    }
}

/**
* Prints using multiple columns. Used when not using long-listing format
* and outputting to a terminal.
*/
static void print_dir_columns(const unsigned size, file_t files[])
{
    ls_width_t *widths;
    unsigned   row;
    unsigned   column;
    unsigned   column_count;
    unsigned   column_width;
    unsigned   row_count;
    unsigned   nr;

    widths = malloc(sizeof (ls_width_t));
    determine_max_widths((size_t)size, files, widths);
    column_count = TERMINAL_WIDTH / (widths->name + 2u);
    column_width = TERMINAL_WIDTH / column_count;
    row_count = size / column_count + 1;

    for (row = 0; row < row_count; ++row)
    {
        for (column = 0; column < column_count; ++column)
        {
            nr = column * row_count + row;
            if (nr < size)
                printf("%-*s", column_width, files[nr].file);
        }
        printf("\n");
    }
}

/**
* Prints using a single column.
*/
static void print_dir_regular(const unsigned size, file_t *files)
{
    unsigned i;

    for (i = 0; i < size; ++i)
        printf("%s\n", files[i].file);
}


/************************
*        MISC.         *
***********************/

/**
* Counts how many files are in a directory.
*/
static int count_files(char const *path, ls_mode_t *mode)
{
    DIR           *dir;
    int           file_counter;
    struct dirent *entry;

    file_counter = 0;

    if ((dir = opendir(path)))
    {
        while ((entry = readdir(dir)))
            if (entry->d_name[0] != '.' || mode->show_hidden)
                file_counter++;

        closedir(dir);
        return file_counter;
    }

    return -1;
}


static void print_symlink(const struct file file)
{
    const unsigned long path_length = strlen(file.path);
    const unsigned long name_length = strlen(file.file);
    char                s[path_length + name_length + 4];
    char                result[PATH_MAX + 1];
    ssize_t             chars_written;

    snprintf(s, path_length + name_length + 4, "%s/%s", file.path, file.file);
    chars_written = readlink(s, result, PATH_MAX);
    result[chars_written] = '\0';
    printf(" -> %s", result);
}


static void get_file_permissions(char *buf, mode_t mode)
{
    buf[0] = (char) ((S_ISDIR(mode)) ? 'd' : '-');
    buf[1] = (char) ((mode & S_IRUSR) ? 'r' : '-');
    buf[2] = (char) ((mode & S_IWUSR) ? 'w' : '-');
    buf[3] = (char) ((mode & S_IXUSR) ? 'x' : '-');
    buf[4] = (char) ((mode & S_IRGRP) ? 'r' : '-');
    buf[5] = (char) ((mode & S_IWGRP) ? 'w' : '-');
    buf[6] = (char) ((mode & S_IXGRP) ? 'x' : '-');
    buf[7] = (char) ((mode & S_IROTH) ? 'r' : '-');
    buf[8] = (char) ((mode & S_IWOTH) ? 'w' : '-');
    buf[9] = (char) ((mode & S_IXOTH) ? 'x' : '-');
    buf[10] = '\0';
}



/**********************
*  COMPARE FUNCTIONS *
**********************/

/**
* Returns -1 and 1 instead of 'difference' for saturation purposes.
*/
static int cmp_date(const void *a, const void *b)
{
    off_t difference;

    difference = ((file_t *)b)->stat->st_mtime - ((file_t *)a)->stat->st_mtime;
    if (difference < 0)
        return -1;
    if (difference > 0)
        return 1;
    return 0;
}

static int cmp_name(const void *a, const void *b)
{
    return strcmp(((file_t *)a)->file, ((file_t *)b)->file);
}

/**
* Returns -1 and 1 instead of 'difference' for saturation purposes.
*/
static int cmp_size(const void *a, const void *b)
{
    off_t difference;

    difference = ((file_t *)b)->stat->st_size - ((file_t *)a)->stat->st_size;
    if (difference < 0)
        return -1;
    if (difference > 0)
        return 1;
    return 0;
}
