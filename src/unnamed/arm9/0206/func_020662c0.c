#include "types.h"
#include "private/arm9_a9db8_mb_parent.h"
extern void func_0205a588(void *dst, int val, int len);
extern u8 *func_02065d5c(void *src, void *dst);
extern void CpuCopy8(void *dst, void *src, int len);
extern int func_02067f2c(u32 a, u32 b, u32 c);

extern char *data_020a9db8;
extern u8 data_0209a078;

typedef struct { u8 a; u8 pad; s16 b; s16 pad2; } Rec;

int func_020662c0(void)
{
    Rec local;
    u8 buf[16];
    char found = -1;
    u16 n;
    u16 mask = 0;
    u8 c;
    u8 k;
    u8 *ret;

    func_0205a588(buf, mask, 16);

    for (n = 1; n <= 15; n++) {
        if (((MbParentWork *)data_020a9db8)->mChildState[n - 1] == 5) {
            s8 t = ((MbParentWork *)data_020a9db8)->mChildFile[n - 1];
            buf[t]++;
        }
    }

    c = data_0209a078;
    for (k = 0; k < 16; k++) {
        c = (c + 1) % 16;
        if (((MbParentWork *)data_020a9db8)->mFiles[c].mActive != 0) {
            if (buf[c] != 0) {
                found = c;
                break;
            }
        }
    }

    if (found == -1)
        return 0x15;

    data_0209a078 = found;
    for (n = 1; n <= 15; n++) {
        if (((MbParentWork *)data_020a9db8)->mChildState[n - 1] == 5) {
            if (found == ((MbParentWork *)data_020a9db8)->mChildFile[n - 1])
                mask |= 1 << n;
        }
    }

    local.a = 3;
    local.b = found;
    ret = func_02065d5c(&local, data_020a9db8);
    if (ret != 0)
        CpuCopy8(((MbParentWork *)data_020a9db8)->mFiles[found].mHeader, ret, 0xe4);

    return func_02067f2c(0xea, mask, (u32)data_020a9db8);
}
