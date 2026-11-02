#ifndef PRIVATE_CARD_BACKUP_SPEC_H
#define PRIVATE_CARD_BACKUP_SPEC_H

/* One 0x28-byte entry of the NitroSDK CARD backup-spec table
 * (data_020867bc). func_02060398 searches the table for the entry whose
 * type matches, ending at a zero type; func_02060364 selects it for
 * CARD_IdentifyBackup and publishes the halfword at 0x0e. The first word is
 * what CARD_GetCurrentBackupType returns and the second what
 * CARD_GetBackupTotalSize returns. The halfword at 0x0e has no reader in src/
 * beyond that copy, so it keeps a plain offset name. */
struct CardBackupSpec {
    int type;      /* 0x00 -- CARD backup type; 0 ends the table */
    int totalSize; /* 0x04 -- what CARD_GetBackupTotalSize returns */
    char pad_08[0x6];
    unsigned short h0e; /* 0x0e -- copied to data_020a8764 by func_02060364 */
    char pad_10[0x18];
};

#ifndef SM64DS_PLATFORM_PC
typedef char CardBackupSpec_size_must_be_0x28[sizeof(struct CardBackupSpec) == 0x28 ? 1 : -1];
#endif

#endif
