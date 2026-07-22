//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern "C" {
int func_0203da3c(void);
u32 func_0206045c(void);
int func_02057020(void);
void func_0205ff80(u16 a);
void func_0205ff70(u16 a);
void func_02057078(u16 a);
int func_02060558(u32 addr, void* buf, u32 len, int d, int e, int f);
extern u8 data_020a4b40[];
}

struct SaveData {
    static int ReadDataFromCart(char* buf, u32 len, u32 slot);
};

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
    half = func_0206045c() >> 1;
    off = slot << 7;
    if (half <= off)
        return 1;
    if (len > half - off - 10)
        return 1;
    lock = func_02057020();
    if (lock == -3)
        return 1;
    func_0205ff80((u16)lock);
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
    func_0205ff70((u16)lock);
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
                func_02057078((u16)lock);
                return 0;
            }
        }
    }
    func_0205ff80((u16)lock);
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
            func_0205ff70((u16)lock);
            func_02057078((u16)lock);
            return 1;
        }
    }
    func_0205ff70((u16)lock);
    func_02057078((u16)lock);
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
