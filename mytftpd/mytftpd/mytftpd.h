#ifndef MYTFTPD_H
#define MYTFTPD_H

    #include <stdbool.h>
    #include <stdint.h>

    #define TFTP_RRQ 1
    #define TFTP_WRQ 2
    #define TFTP_DATA 3
    #define TFTP_ACK 4
    #define TFTP_ERR 5
    #define TFTP_OACK 6

    #define TFTP_DEFAULT_BLKSZ 512
    #define TFTP_MAX_PACKET_SZ 512


    #define TFTP_ERR_UNDEFINED 0
    #define TFTP_ERR_UNKNOWN_FILE 1
    #define TFTP_ERR_ACCES_VIOLATION 2
    #define TFTP_ERR_ILLEGAL_OP 4
    #define TFTP_ERR_UNKNOWN_ID 5

    struct tftp_ack_t
    {
        uint16_t opcode;
        uint16_t block;
    };

    struct tftp_data_t
    {
        uint16_t opcode;
        uint16_t block;
        char     data[1];
    };

    struct tftp_oack_t
    {
        uint16_t opcode;
        char     data[512];
    };

#endif
