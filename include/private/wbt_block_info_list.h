#ifndef PRIVATE_WBT_BLOCK_INFO_LIST_H
#define PRIVATE_WBT_BLOCK_INFO_LIST_H

/* NitroSDK WBTBlockInfoList, a node of the registered-block list whose head
 * func_02065ae0 returns (data_020a94d0). func_02065a94 searches it by block
 * id (the WBT_RegisterBlock key), func_02065970 counts the nodes and
 * func_02063bc4 reads block_type. Only the fields those read are placed; the
 * node's total size is not evidenced. */
struct WBTBlockInfoList {
    int block_id; /* 0x00 */
    /* 0x04 -- block size in bytes: func_0206491c divides it by the packet
     * size to bound the packet number; func_020658c0 stores its fifth
     * argument here */
    int block_size;
    /* 0x08 -- the SDK's user_id: the 0x20 bytes of user data
     * WBT_RegisterBlock (func_020658c0) copies in */
    char user_id[0x20];
    struct WBTBlockInfoList *next; /* 0x28 */
    /* 0x2c -- the block's data; func_02064888 indexes it by packet */
    char *data_ptr;
    /* 0x30 -- WBT_RegisterBlock's last argument, the SDK's permission_bmp
     * (the AIDs allowed to request the block); every caller passes 0 */
    unsigned short permission_bmp;
    /* 0x32 -- WBT_RegisterBlock (func_020658c0) sets 0 (COMMON) when it is
     * given the block's data and 1 (USER) when it is not, so each packet has
     * to be asked for with PREPARE_SEND_DATA */
    unsigned short block_type;
};

#endif
