#include "mytftpd.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/fcntl.h>
#include <limits.h>

static void process_arguments(int argc, char *argv[], uint16_t *port_nr,
        char **addr);
static int prepare_socket(char *addr, uint16_t port_nr);
static void set_timeout(int sd, int timeout);
static void serve_requests(int master_sd, char *addr);
static void handle_request(int opcode, int master_sd, char *addr,
        char *buffer, struct sockaddr_in *client);
static bool in_path(char *filename);
static uint16_t get_blksize(char *buffer, char *filename);
static void send_blksize_ack(int sd, struct sockaddr_in *client,
        uint16_t blksize);

static void send_file(char *filename, struct sockaddr_in *client, char *addr,
        int master_sd, uint16_t blksize);
static bool rrq_options_negotiated(struct sockaddr_in *client,
        uint16_t blksize, int sd, uint16_t *block_nr);
static void process_send_packets(struct sockaddr_in *client, uint16_t blksize,
        int file, int sd, char *message);
static ssize_t send_data(struct sockaddr_in *client, uint16_t blksize,
        int file, int sd, uint16_t block_nr);
static bool received_ack(int sd, char *message, uint16_t *block_nr);

static void receive_file(char *filename, struct sockaddr_in *client,
        char *addr, int master_sd, uint16_t blksize);
static void process_receive_packets(struct sockaddr_in *client,
        uint16_t blksize, struct sockaddr_in *current_client, FILE *file,
        int sd);
static bool process_receive_response(FILE *file, int sd, char *data,
        ssize_t bytes_received, struct sockaddr_in *client,
        struct sockaddr_in *current_client, int blksize, int *block_nr);
static void send_ack(int sd, struct sockaddr_in *client, uint16_t block);

static void send_error(struct sockaddr_in *client, uint16_t code,
        char *message, int sd);
static void die(const char* msg);
static void* xmalloc(size_t n);

int main(int argc, char *argv[])
{
    // SET TO 69
    // SET TO 69
    // SET TO 69
    // SET TO 69
    // SET TO 69
    // SET TO 69
    // SET TO 69
    uint16_t port;
    char     *addr;
    int      master_sd;

    port = 1234;
    addr = 0;
    process_arguments(argc, argv, &port, &addr);
    master_sd = prepare_socket(addr, port);
    serve_requests(master_sd, addr);

    return 0;
}

static void process_arguments(int argc, char *argv[], uint16_t *port_nr,
        char  **addr)
{
    char *end;

    if (argc == 2)
        *addr = argv[1];
    else if (argc == 3)
    {
        *addr = argv[1];
        *port_nr = (uint16_t)strtol(argv[2], &end, 10);
        if (end == argv[2] || *end != '\0' || errno == ERANGE)
            die("Invalid port");
    }
    else if (argc != 1)
        die("Too many arguments.");
}

static int prepare_socket(char *addr, uint16_t port_nr)
{
    int                sd;
    struct sockaddr_in socket_addr;

    sd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (-1 == sd)
        die("socket");

    memset(&socket_addr, 0, sizeof (socket_addr));
    socket_addr.sin_family = AF_INET;
    socket_addr.sin_port = htons(port_nr);
    socket_addr.sin_addr.s_addr = addr ? inet_addr(addr) : INADDR_ANY;

    if (bind(sd, (struct sockaddr *)&socket_addr, sizeof (socket_addr)) < 0)
        die("bind");

    if (port_nr != 0)
    {
        if (addr)
            printf("Listening at %s:%hu\n", addr, port_nr);
        else
            printf("Listening on all interfaces at port %hu\n", port_nr);
    }

    return sd;
}

static void set_timeout(int sd, int timeout)
{
    struct timeval tv;

    tv.tv_sec = timeout;
    tv.tv_usec = 0;
    if (setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof (tv)) < 0)
        perror("Error");
}

static void serve_requests(int master_sd, char *addr)
{
    char               *buffer;
    ssize_t            l;
    struct sockaddr_in client;
    socklen_t          len;
    uint16_t           opcode;

    buffer = xmalloc(TFTP_MAX_PACKET_SZ);
    len = sizeof (client);
    while (true)
    {
        l = recvfrom(master_sd, buffer, TFTP_MAX_PACKET_SZ, 0,
                (struct sockaddr *)&client, &len);
        if (l == 0 || (l == -1 && errno == EAGAIN))
            continue;
        else if (l == -1)
            die("recv");
        opcode = ((uint16_t)(buffer[0] << 8));
        opcode += buffer[1];
        if (opcode == TFTP_RRQ || opcode == TFTP_WRQ)
            handle_request(opcode, master_sd, addr, buffer + 2, &client);
        else if (opcode == TFTP_ACK || opcode == TFTP_DATA)
            send_error(&client, TFTP_ERR_UNKNOWN_ID,
                    (char *)"Unknown transfer ID.", master_sd);
        else if (opcode != TFTP_ERR)
            send_error(&client, TFTP_ERR_ILLEGAL_OP,
                    (char *)"Illegal TFTP operation.", master_sd);
    }
}

static void handle_request(int opcode, int master_sd, char *addr,
        char *buffer, struct sockaddr_in *client)
{
    char     *filename;
    uint16_t blksize;

    filename = buffer;
    if (!in_path(filename))
    {
        send_error(client, TFTP_ERR_ACCES_VIOLATION,
                (char *)"Requested file outside of working dir.", master_sd);
        return;
    }
    buffer += strnlen(buffer, 508) + 1;
    if (strncmp(buffer, "octet", 5) != 0)
    {
        send_error(client, TFTP_ERR_UNDEFINED,
                (char *)"Transfer mode not supported.", master_sd);
        return;
    }
    buffer += 6;
    blksize = get_blksize(buffer, filename);

    if (opcode == TFTP_RRQ)
        send_file(filename, client, addr, master_sd, blksize);
    else
        receive_file(filename, client, addr, master_sd, blksize);
}

static bool in_path(char *filename)
{
    char   *real_path;
    char   *cwd;
    char   *real_cwd;
    size_t real_path_len;
    size_t real_cwd_len;
    bool   result;

    real_path = xmalloc(PATH_MAX + 1);
    realpath(filename, real_path);
    cwd = xmalloc(PATH_MAX + 1);
    real_cwd = xmalloc(PATH_MAX + 1);
    getcwd(cwd, PATH_MAX);
    realpath(cwd, real_cwd);
    real_path_len = strnlen(real_path, PATH_MAX);
    real_cwd_len = strnlen(real_cwd, PATH_MAX);

    if (real_path_len < real_cwd_len)
        result = false;
    else
        result = (strncmp(real_cwd, real_path, real_cwd_len) == 0);

    free(real_cwd);
    free(real_path);
    free(cwd);
    return result;
}

static uint16_t get_blksize(char *buffer, char *filename)
{
    uint16_t      blksize;
    unsigned long option_one_length;
    unsigned long option_two_length;
    unsigned long delta;

    delta = (unsigned long)(filename - buffer);
    blksize = TFTP_DEFAULT_BLKSZ;

    while (*buffer && delta < 508
            && strncmp(buffer, "blksize", delta + 508) != 0)
    {
        option_one_length = strnlen(buffer, delta + 508);
        option_two_length = strnlen(buffer, delta + 508 - option_one_length);
        buffer += option_one_length + option_two_length;
        delta += option_one_length + option_two_length;
    }

    if (strncmp(buffer, "blksize", 7) == 0)
    {
        buffer += 8;
        blksize = (uint16_t) strtoul(buffer, 0, 10);
    }
    return blksize;
}

static void send_blksize_ack(int sd,
        struct sockaddr_in *client, uint16_t blksize)
{
    struct tftp_oack_t *ack;
    char               str[6];

    ack = xmalloc(sizeof (struct tftp_oack_t));
    ack->opcode = htons(TFTP_OACK);
    strncpy(ack->data, "blksize", 8);
    memset(str, '\0', 6);
    sprintf(str, "%d", blksize);
    strncpy(ack->data + 8, str, 6);

    sendto(sd, ack, 11 + strlen(str), 0, (struct sockaddr *)client,
            sizeof (struct sockaddr));

    free(ack);
}

static void send_file(char *filename, struct sockaddr_in *client,
        char *addr, int master_sd, uint16_t blksize)
{
    char client_name[INET_ADDRSTRLEN];
    int  file;
    int  sd;
    char *message;

    message = xmalloc(4);
    memset(message, '\0', 4);

    if ((file = open(filename, O_RDONLY)) == -1)
    {
        send_error(client, TFTP_ERR_UNKNOWN_FILE,
                (char *)"File not found.", master_sd);
        return;
    }
    if (inet_ntop(AF_INET, &client->sin_addr, client_name,
            sizeof (client_name)))
        printf("Uploading %s to %s:%hu\n", filename, client_name,
                ntohs(client->sin_port));
    sd = prepare_socket(addr, 0);
    set_timeout(sd, 1);
    process_send_packets(client, blksize, file, sd, message);
    printf("Done.\n\n");
    close(sd);
    free(message);
    close(file);
}

static bool rrq_options_negotiated(struct sockaddr_in *client,
        uint16_t blksize, int sd, uint16_t *block_nr)
{
    struct sockaddr_in client_current;
    char               *message;
    ssize_t            l;
    socklen_t          len;
    uint16_t           block;
    uint16_t           opcode;

    send_blksize_ack(sd, client, blksize);
    len = sizeof (client);
    message = xmalloc(4);
    l = recvfrom(sd, message, 4, 0, (struct sockaddr *)&client_current, &len);
    if (l == 0 || (l == -1 && errno == EAGAIN))
        return false;
    else if (l == -1)
        die("recv");
    opcode = (uint16_t)((((unsigned char)message[0]) << 8)
            | ((unsigned char)message[1]));
    block = (uint16_t)((((unsigned char)message[2]) << 8)
            | ((unsigned char)message[3]));
    free(message);
    if (opcode == TFTP_ACK && block == 0)
    {
        (*block_nr)++;
        return true;
    }
    return false;
}

static void process_send_packets(struct sockaddr_in *client, uint16_t blksize,
        int file, int sd, char *message)
{
    short    attempts;
    ssize_t  bytes_sent;
    uint16_t block_nr;

    block_nr = (uint16_t)((blksize == TFTP_DEFAULT_BLKSZ) ? 1 : 0);
    attempts = 1;
    while (1)
    {
        if (attempts >= 5)
        {
            send_error(client, TFTP_ERR_UNDEFINED,
                    (char *)"Too many timeouts.", sd);
            break;
        }
        if (blksize != TFTP_DEFAULT_BLKSZ && block_nr == 0
                && !rrq_options_negotiated(client, blksize, sd, &block_nr))
            continue;
        bytes_sent = send_data(client, blksize, file, sd, block_nr);
        attempts++;
        if (!received_ack(sd, message, &block_nr))
            continue;
        if (bytes_sent < blksize)
            break;
        attempts = 1;
    }
}

static ssize_t send_data(struct sockaddr_in *client, uint16_t blksize,
        int file, int sd, uint16_t block_nr)
{
    struct tftp_data_t *data;
    ssize_t            bytes_read;

    data = xmalloc(sizeof (struct tftp_data_t) + blksize);
    memset(data, '\0', sizeof (struct tftp_data_t) + blksize);
    data->opcode = (uint16_t)(TFTP_DATA >> 8) | (uint16_t)(TFTP_DATA << 8);
    data->block = (uint16_t)(block_nr >> 8) | (uint16_t)(block_nr << 8);
    bytes_read = pread(file, data->data, blksize, (block_nr - 1) * blksize);

    if (bytes_read < 0)
    {
        send_error(client, TFTP_ERR_UNDEFINED, (char *)"Error reading file.",
                sd);
        free(data);
        return 0;
    }

    sendto(sd, data, 2 * sizeof (uint16_t) + (size_t)bytes_read, 0,
            (struct sockaddr *)client, sizeof (struct sockaddr));
    free(data);
    return bytes_read;
}

static bool received_ack(int sd, char *message, uint16_t *block_nr)
{
    struct sockaddr_in client_current;
    socklen_t          len;
    ssize_t            bytes_read;
    uint16_t           opcode;
    uint16_t           block;

    len = sizeof (client_current);
    bytes_read = recvfrom(sd, message, 4, 0,
            (struct sockaddr *)&client_current, &len);
    if (bytes_read == 0 || (bytes_read == -1 && errno == EAGAIN))
        return false;
    else if (bytes_read == -1)
        die("recv");
    opcode = (uint16_t)((((unsigned char)message[0]) << 8)
            | ((unsigned char)message[1]));
    block = (uint16_t)((((unsigned char)message[2]) << 8)
            | ((unsigned char)message[3]));

    if (opcode == TFTP_ACK && block == *block_nr)
        (*block_nr)++;
    else if (opcode == TFTP_ERR)
        return false;

    return true;
}

static void receive_file(char *filename, struct sockaddr_in *client,
        char *addr, int master_sd, uint16_t blksize)
{
    char               client_name[INET_ADDRSTRLEN];
    struct sockaddr_in current_client;
    FILE               *file;
    int                sd;

    if (!(file = fopen(filename, "w")))
    {
        send_error(client, TFTP_ERR_ACCES_VIOLATION,
                (char *)"Unable to write to file.", master_sd);
        return;
    }
    if (inet_ntop(AF_INET, &client->sin_addr, client_name,
                sizeof (client_name)))
        printf("Downloading %s from %s:%hu\n", filename, client_name,
                ntohs(client->sin_port));
    sd = prepare_socket(addr, 0);
    set_timeout(sd, 1);
    if (blksize == TFTP_DEFAULT_BLKSZ)
        send_ack(sd, client, 0);
    else
        send_blksize_ack(sd, client, blksize);
    process_receive_packets(client, blksize, &current_client, file, sd);
    printf("Done.\n\n");
    close(sd);
    fclose(file);
}

static void process_receive_packets(struct sockaddr_in *client,
        uint16_t blksize, struct sockaddr_in *current_client, FILE *file,
        int sd)
{
    char      *data;
    int       block_nr;
    socklen_t len;
    ssize_t   bytes_received;
    int       attempts;

    block_nr = 1;
    len = sizeof ((*current_client));
    data = xmalloc(blksize + 4);
    attempts = 1;
    while (1)
    {
        if (attempts >= 5)
        {
            send_error(client, TFTP_ERR_UNDEFINED,
                    (char *)"Too many timeouts.", sd);
            break;
        }
        memset(data, '\0', blksize + 4);
        bytes_received = recvfrom(sd, data, blksize + 4, 0,
                (struct sockaddr *)current_client, &len);
        if (bytes_received == 0 || (bytes_received == -1 && errno == EAGAIN))
            continue;
        else if (bytes_received == -1)
            die("recv");
        attempts++;
        if (process_receive_response(file, sd, data, bytes_received, client,
                current_client, blksize, &block_nr))
            break;
        attempts = 1;
    }
    free(data);
}

static bool process_receive_response(FILE *file, int sd, char *data,
        ssize_t bytes_received, struct sockaddr_in *client,
        struct sockaddr_in *current_client, int blksize, int *block_nr)
{
    uint16_t opcode;
    uint16_t block;

    if (client->sin_port != current_client->sin_port)
    {
        send_error(current_client, TFTP_ERR_UNKNOWN_ID,
                (char *)"Invalid TID.", sd);
        return false;
    }
    opcode = (uint16_t)(((unsigned char)(*data++)) << 8);
    opcode |= (uint16_t)((unsigned char)(*data++));
    if (opcode == TFTP_DATA)
    {
        block = (uint16_t)(((unsigned char)(*data++)) << 8);
        block |= ((unsigned char)(*data++));
        if (*block_nr != block)
            return false;
        (*block_nr)++;
        fwrite(data, 1, (size_t)(bytes_received - 4), file);
        send_ack(sd, current_client, block);
        if (bytes_received < blksize + 4)
            return true;
    }
    else if (opcode == TFTP_ERR)
    {
        printf("An error occured. Transfer aborted.\n");
        return true;
    }
    else
        send_error(current_client, TFTP_ERR_UNDEFINED,
                (char *)"Expected data packet.", sd);
    return false;
}

static void send_ack(int sd, struct sockaddr_in *client, uint16_t block)
{
    struct tftp_ack_t *ack;

    ack = xmalloc(sizeof (struct tftp_ack_t));
    ack->opcode = htons(TFTP_ACK);
    ack->block = htons(block);

    sendto(sd, ack, 2 * sizeof (uint16_t), 0,
            (struct sockaddr *)client, sizeof (struct sockaddr));
    free(ack);
}



static void send_error(struct sockaddr_in *client, uint16_t code,
        char *message, int sd)
{
    const size_t      len = strlen(message);
    struct tftp_ack_t *ack;

    ack = xmalloc(sizeof (struct tftp_ack_t) + len);
    ack->opcode = htons(TFTP_ERR);
    ack->block = htons(code);
    memcpy((char *)ack + sizeof (struct tftp_ack_t), message, len + 1);

    sendto(sd, ack, sizeof (struct tftp_ack_t) + len + 1, 0,
            (struct sockaddr *)client, sizeof (struct sockaddr));
    free(ack);
}


static void die(const char* msg)
{
    perror(msg);
    exit(1);
}

static void *xmalloc(size_t n)
{
    void *p;

    p = malloc(n);
    if (!p)
        die("malloc");

    return p;
}
