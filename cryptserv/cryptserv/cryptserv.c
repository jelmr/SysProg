#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include "cryptserv.h"


static int die(const char* msg);
static int prepare_socket(char *path);
static void accept_connection(int sd, fd_set *active_fd_set, crypt_conn_t **connections, unsigned key);
static int handle_connection(fd_set *fd_set, crypt_conn_t *conn, crypt_conn_t **connections);
static void process_arguments(int argc, char *argv[], int sockets[], unsigned *key);
static void store_connection(int fd, crypt_conn_t **list, unsigned key);
static int close_connection(crypt_conn_t *conn, crypt_conn_t **list, fd_set *fd_set);
static void initialize_fd_set(int socket_count, int sockets[], fd_set *active_fd_set);
static void process_requests(int sockets[], int socket_count, crypt_conn_t **connections, unsigned key);
static int send_encrypted(crypt_conn_t *conn, char str[], ssize_t size);


int main(int argc, char *argv[])
{
    int          sockets[(argc - 2 < 0) ? 0 : argc - 2];
    unsigned     key;
    crypt_conn_t *connections;

    process_arguments(argc, argv, sockets, &key);
    connections = 0;

    process_requests(sockets, argc - 2, &connections, key);

    return 0;
}

static void process_requests(int sockets[], int socket_count, crypt_conn_t **connections, unsigned key)
{
    crypt_conn_t *current;
    fd_set       active_fd_set;
    fd_set       read_fd_set;
    int          i;

    initialize_fd_set(socket_count, sockets, &active_fd_set);

    printf("Waiting for client...\n");
    while (true)
    {
        read_fd_set = active_fd_set;
        if (select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0)
            die("select");

        for (current = *connections; current; current = current->next)
            if (FD_ISSET (current->fd, &read_fd_set))
                if (handle_connection(&active_fd_set, current, connections))
                    break;

        for (i = 0; i < socket_count; i++)
            if (FD_ISSET (sockets[i], &read_fd_set))
                accept_connection(sockets[i], &active_fd_set, connections, key);

    }
}

static void initialize_fd_set(int socket_count, int sockets[], fd_set *active_fd_set)
{
    int i;

    FD_ZERO (active_fd_set);

    for (i = 0; i < socket_count; i++)
        FD_SET (sockets[i], active_fd_set);
}

static void process_arguments(int argc, char *argv[], int sockets[], unsigned *key)
{
    int j;

    if (argc < 3)
    {
        printf("Must supply a key and at least one unix domain socket.\n");
        exit(1);
    }

    *key = (unsigned) strtoul(argv[1], 0, 0);

    for (j = 2; j < argc; j++)
        sockets[j - 2] = prepare_socket(argv[j]);
}

static int handle_connection(fd_set *fd_set, crypt_conn_t *conn, crypt_conn_t **connections)
{
    char    str[CRYPT_CHUNK_SZ + 1];
    ssize_t bytes_received;

    bytes_received = recv(conn->fd, str, CRYPT_CHUNK_SZ, 0);
    if (bytes_received < 0)
        return die("recv");
    else if (bytes_received != 0)
        return send_encrypted(conn, str, bytes_received);
    else
        return close_connection(conn, connections, fd_set);
}

static int send_encrypted(crypt_conn_t *conn, char str[], ssize_t size)
{
    int i;
    setstate(conn->random_state);

    for(i = 0; i < size; i++)
        str[i] = (str[i]) ^ (random() & 0xff);


    if (send(conn->fd, str, (size_t)size, 0) < 0)
        die("send");

    return 0;
}


static void accept_connection(int sd, fd_set *active_fd_set, crypt_conn_t **connections, unsigned key)
{
    struct sockaddr_un remote;
    socklen_t          socklen;
    int fd;

    printf("---Accepting new client\n");
    socklen = sizeof(remote);
    if ((fd = accept(sd, (struct sockaddr *)&remote, &socklen)) == -1)
        die("accept");

    FD_SET(fd, active_fd_set);

    store_connection(fd, connections, key);

}

static int prepare_socket(char *path)
{
    int                sd;
    struct sockaddr_un sock;
    socklen_t          len;

    if ((sd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
        die("socket");

    unlink(path);
    sock.sun_family = AF_UNIX;
    strcpy(sock.sun_path, path);

    int optval;
    optval=1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    len = (socklen_t)(strlen(sock.sun_path) + sizeof(sock.sun_family) + 1);

    if (bind(sd, (struct sockaddr *)&sock, len) == -1)
        die("bind");

    if (listen(sd, CRYPT_BACKLOG) == -1)
        die("listen");
    return sd;
}


static int die(const char* msg)
{
    perror(msg);
    exit(1);
    return 1;
}


static void store_connection(int fd, crypt_conn_t **list, unsigned key)
{
    crypt_conn_t *new_conn;

    new_conn = malloc(sizeof (crypt_conn_t) + 1);
    new_conn->fd = fd;

    new_conn->random_state = malloc(256 + 1);

    initstate(key, new_conn->random_state, 256);

    new_conn->next = *list;
    *list = new_conn;

}

static int close_connection(crypt_conn_t *conn, crypt_conn_t **list, fd_set *fd_set)
{
    crypt_conn_t *current;

    current = *list;
    if (!current)
        die("list-close");

    if (current->fd == conn->fd)
        *list = conn->next;
    else
    {
        while (current->next && current->next->fd != conn->fd)
            current = current->next;

        if (current->next->fd == conn->fd)
           current->next = current->next->next;
        else
            die("list");
    }

    printf(">>>Done with client.\n");
    close(conn->fd);
    FD_CLR(conn->fd, fd_set);
    free(conn->random_state);
    free(conn);
    return 1;
}