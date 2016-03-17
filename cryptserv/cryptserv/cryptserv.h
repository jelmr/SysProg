#ifndef CRYPTSERV_H
#define CRYPTSERV_H

    #include <stdbool.h>
    #include <stdint.h>

    #define CRYPT_CHUNK_SZ 128
    #define CRYPT_BACKLOG 10

    typedef struct crypt_conn {
        int               fd;
        char              *random_state;
        struct crypt_conn *next;
    } crypt_conn_t;

#endif
