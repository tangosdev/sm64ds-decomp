//cpp
/* arm9/SaveData2 -- the save-file manager's cart I/O, .text
 * 0x02042804..0x02042fe4. SaveData::SaveDataToCart writes a block with an
 * 8-byte magic and a 16-bit rolling checksum to a primary and a mirror copy;
 * SaveData::ReadDataFromCart reads one back with the same checks, falling
 * back to the mirror. func_02042f68 sets the backup device up and stores the
 * magic both of them use. mwccarm emits .text in reverse source order, so
 * the definitions below run ROM-descending; the roster reads ROM-ascending.
 *
 *   ReadDataFromCart 0x02042804
 *   SaveDataToCart   0x02042c3c
 *   func_02042f68    0x02042f68
 */
#include "types.h"
#include "decl_common.h"
#include "SaveData.h"
#include "nitro/card/card_common.h"
#include "nitro/os/spinLock.h"

extern "C" {
int func_0203da3c(void);
u32 CARD_GetBackupTotalSize(void);
int func_02060558(u32 addr, void* buf, u32 len, int d, int e, int f);
int func_02060484(int a, int b, int c, int d, int e, int f);
/* local extern: no include/ header declares the CARD backup setup. */
void CARD_Init(void);
void CARD_IdentifyBackup(int type);
}

/* func_02042f68(type, magic) at 0x02042f68 -- the cart I/O pair's setup.
 *
 * NitroSDK CARD_Init, then CARD_IdentifyBackup(type), then keep `magic` up to
 * its NUL or 8 bytes, zero-padded, in data_020a4b40: the 8-byte magic
 * SaveDataToCart writes with each block and ReadDataFromCart checks. Nothing
 * else in arm9 or the overlays touches data_020a4b40. func_0201a054 calls
 * it at boot with 0xd01 (CARD_BACKUP_TYPE_EEPROM_64KBITS: device 1, 2^13
 * bytes) and "ds mario". It returns at once in link mode 2
 * (func_0203da3c), the mode func_0203db64 gives a download-play child, but
 * never does at that call site: data_020a0f04 is still 0 then.
 */
// @symbol func_02042f68
extern "C" void func_02042f68(int type, unsigned char *magic) {
    int i;
    unsigned char *d;
    if (func_0203da3c() == 2)
        return;
    CARD_Init();
    CARD_IdentifyBackup(type);
    func_0205a588(data_020a4b40, 0, 8);
    d = data_020a4b40;
    for (i = 0; i < 8; i++) {
        unsigned char c = magic[i];
        if (c == 0)
            return;
        *d++ = c;
    }
}

/* SaveData::SaveDataToCart(char* data, u32 size, u32 fileID) at 0x02042c3c -- static.
 *
 * Writes one block to the cart's backup memory with an 8-byte magic and a 16-bit
 * rolling checksum, to a primary and a mirror copy. Returns 0 on success -- the
 * opposite of the 1-is-success convention its callers use, which is why every one
 * of them inverts it.
 */
// @symbol _ZN8SaveData14SaveDataToCartEPcjj
int SaveData::SaveDataToCart(char* data, u32 size, u32 fileID)
{
    int lockID;
    u16 sum = 0;
    int freeVal;
    int retry;
    int v14, v18, v10;
    int hdrOff, dataOff;
    int need;
    int newBase;
    int v20, v24, v1c;
    int hdrOff2, dataOff2;
    int retry2;

    if (func_0203da3c() == 2)
        return 1;

    freeVal = (int)((u32)CARD_GetBackupTotalSize() >> 1);
    need = (int)(fileID << 7);
    if ((u32)freeVal <= (u32)need)
        return 1;
    if (size > (u32)(freeVal - need - 10))
        return 1;

    sum = 0;
    {
        int i = 0;
        u16 acc = 0;
        const u8* t = data_020a4b40;
        do {
            u8 c = *t;
            i++;
            acc = (u16)(acc + c);
            t++;
        } while (i < 8);
        sum = acc;
    }

    {
        u32 i;
        for (i = 0; i < size; i++) {
            sum = (u16)(((sum << 1) & 0xfffe) | ((sum >> 15) & 1));
            sum = (u16)(sum ^ (((u8)data[i]) & 0xff));
        }
    }

    lockID = OS_GetLockID();
    if (lockID == -3)
        return 1;

    CARD_LockBackup((u16)lockID);

    retry = 0;
    v14 = 0;
    v18 = 0;
    v10 = 0;
    hdrOff = need + 2;
    dataOff = need + 10;
    while (func_02060484(need, (int)&sum, 2, v10, v10, v10) != 1 ||
           func_02060484(hdrOff, (int)data_020a4b40, 8, v14, v14, v14) != 1 ||
           func_02060484(dataOff, (int)data, (int)size, v18, v18, v18) != 1) {
        retry++;
        if (retry > 0) {
            CARD_UnlockBackup((u16)lockID);
            OS_ReleaseLockID((u16)lockID);
            return 1;
        }
    }

    CARD_UnlockBackup((u16)lockID);
    CARD_LockBackup((u16)lockID);

    retry2 = 0;
    newBase = freeVal + need;
    v20 = 0;
    v24 = 0;
    v1c = 0;
    hdrOff2 = newBase + 2;
    dataOff2 = newBase + 10;
    while (func_02060484(newBase, (int)&sum, 2, v1c, v1c, v1c) != 1 ||
           func_02060484(hdrOff2, (int)data_020a4b40, 8, v20, v20, v20) != 1 ||
           func_02060484(dataOff2, (int)data, (int)size, v24, v24, v24) != 1) {
        retry2++;
        if (retry2 > 0) {
            CARD_UnlockBackup((u16)lockID);
            OS_ReleaseLockID((u16)lockID);
            return 1;
        }
    }

    CARD_UnlockBackup((u16)lockID);
    OS_ReleaseLockID((u16)lockID);
    return 0;
}

/* SaveData::ReadDataFromCart(char* buf, u32 len, u32 slot) at 0x02042804 -- static.
 *
 * Reads a block from the cart's backup memory, checking an 8-byte magic and a
 * 16-bit rolling checksum, and falling back to the mirror copy at +half if the
 * primary fails either check. Three-valued: 0 read good, 2 no valid data on the
 * cart at all, 1 read failed.
 */
// @symbol _ZN8SaveData16ReadDataFromCartEPcjj
int SaveData::ReadDataFromCart(char* buf, u32 len, u32 slot)
{
    u16 sum;
    int lock;
    u32 half;
    int ok;
    u16 crc;
    u8 magic[8];
    int fa, fb, fc;
    int ga, gb, gc;
    int retry;
    u32 a2;
    u32 a10;
    u32 off;
    u32 off2;
    u32 b2;
    int retry2;
    u32 b10;
    int i;
    u32 j;
    const u8* pd;
    const u8* pm;

    crc = 0;
    if (func_0203da3c() == 2)
        return 1;
    half = CARD_GetBackupTotalSize() >> 1;
    off = slot << 7;
    if (half <= off)
        return 1;
    if (len > half - off - 10)
        return 1;
    lock = OS_GetLockID();
    if (lock == -3)
        return 1;
    CARD_LockBackup((u16)lock);
    retry = 0;
    fb = 0;
    fc = 0;
    fa = 0;
    a2 = off + 2;
    a10 = off + 10;
    while (func_02060558(off, &crc, 2, fa, fa, fa) != 1 ||
           func_02060558(a2, magic, 8, fb, fb, fb) != 1 ||
           func_02060558(a10, buf, len, fc, fc, fc) != 1) {
        retry++;
        if (retry > 0)
            break;
    }
    CARD_UnlockBackup((u16)lock);
    if (retry <= 0) {
        ok = 1;
        pd = data_020a4b40;
        pm = magic;
        for (i = 0; i < 8; i++, pd++, pm++) {
            if (*pm != *pd) {
                ok = 0;
                break;
            }
        }
        if (ok == 1) {
            sum = 0;
            pd = data_020a4b40;
            for (i = 0; i < 8; i++, pd++)
                sum = sum + *pd;
            for (j = 0; j < len; j++) {
                sum = (u16)(((sum << 1) & 0xFFFE) |
                            ((sum >> 15) & 1)) ^
                      ((u8)buf[j] & 0xFF);
            }
            if (sum == crc) {
                OS_ReleaseLockID((u16)lock);
                return 0;
            }
        }
    }
    CARD_LockBackup((u16)lock);
    retry2 = 0;
    off2 = half + off;
    gb = 0;
    gc = 0;
    ga = 0;
    b2 = off2 + 2;
    b10 = off2 + 10;
    while (func_02060558(off2, &crc, 2, ga, ga, ga) != 1 ||
           func_02060558(b2, magic, 8, gb, gb, gb) != 1 ||
           func_02060558(b10, buf, len, gc, gc, gc) != 1) {
        retry2++;
        if (retry2 > 0) {
            CARD_UnlockBackup((u16)lock);
            OS_ReleaseLockID((u16)lock);
            return 1;
        }
    }
    CARD_UnlockBackup((u16)lock);
    OS_ReleaseLockID((u16)lock);
    pd = data_020a4b40;
    pm = magic;
    for (i = 0; i < 8; i++, pd++, pm++) {
        if (*pm != *pd)
            return (ok == 1) ? 1 : 2;
    }
    sum = 0;
    pm = magic;
    for (i = 0; i < 8; i++, pm++)
        sum = sum + *pm;
    for (j = 0; j < len; j++) {
        sum = (u16)(((sum << 1) & 0xFFFE) |
                    ((sum >> 15) & 1)) ^
              ((u8)buf[j] & 0xFF);
    }
    if (sum == crc)
        return 0;
    return 1;
}
