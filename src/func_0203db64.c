typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

extern void CpuCopy8(void *dst, void *src, int n);
extern void func_0205a588(void *dst, int c, int n);
extern void func_02042778(void);
extern void func_0203df40(void);
extern int func_02040a94(void);
extern void func_02040724(void);
extern void OS_SleepThread(u16 *self);
extern void func_0203d854(void);
extern int func_0203d81c(void);
extern void func_0203d918(void);
extern int func_0203d8fc(void);
extern void func_02059eb0(void *dst);
extern void func_02059f2c(void *p);

typedef struct {
    u8 lo : 4;
    u8 hi : 4;
    u8 b1;
} NetCfg;

extern u8 data_020a0ef0;
extern u8 data_020a1064[];
extern u8 data_020a0fec[];
extern u16 data_020a1040[];
extern u8 data_020a0fb8[];
extern u8 data_020a0fa0[];
extern NetCfg data_020a10fc;
extern NetCfg data_020a10a4;
extern u8 data_020a1154[];
extern u8 data_020a0f08;
extern u16 data_020a0f1c;
extern u16 data_020a0f30;
extern u16 data_020a0f28;
extern u8 data_020a0f10;
extern u8 data_02099e18;
extern u8 data_020a0f04;
extern u8 data_020a10fe[];
extern u8 data_020a10a6[];
extern u8 data_020a0f00;

typedef struct {
    u8 unk0;
    u8 unk1;
    u8 unk2[2];
    u8 unk4[0x14];
    u16 unk18;
    u8 unk1A[0x3A];
} SpBuf;

void func_0203db64(void *arg0, void *arg1)
{
    SpBuf buf;
    int b;

    data_020a0ef0 = 0;
    CpuCopy8(arg0, data_020a1064, 0x40);
    CpuCopy8(arg1, data_020a0fec, 0x1c);
    func_0205a588(data_020a1040, 0, 0x24);
    func_0205a588(data_020a0fb8, 0, 0x18);
    func_0205a588(data_020a0fa0, 0, 0x18);
    func_0205a588(&data_020a10fc, 0, 0x58);
    func_0205a588(&data_020a10a4, 0, 0x58);
    func_0205a588(data_020a1154, 0, 0x90);
    data_020a0f08 = 0;
    data_020a0f1c = 0;
    data_020a0f30 = 0;
    data_020a0f28 = 0;
    b = 1;
    data_02099e18 = 1;
    data_020a0f10 = 0;
    data_020a0f04 = 0;
    if (*(u16 *)0x27ffc40 != 2) {
        b = 0;
    }
    if (b) {
        int count;

        data_020a0f04 = 2;
        count = 0x4b0;
        while (!(func_02040a94() == 2 && data_02099e18 == data_020a0f08 && data_020a0f08 != 0) && count != 0) {
            data_020a0f1c |= 0x2000;
            func_02042778();
            func_0203df40();
            count--;
        }
        if (count == 0) {
            data_020a0f1c |= 1;
            func_02040724();
            OS_SleepThread(0);
        }

        count = 0x258;
        data_020a1040[6] |= 1;
        while ((data_020a1040[6] & 1) && count != 0) {
            data_020a0f1c |= 0x2000;
            func_02042778();
            func_0203df40();
            count--;
        }
        if (count == 0) {
            data_020a0f1c |= 1;
            func_02040724();
            OS_SleepThread(0);
        }

        func_0203d854();
        count = 0x258;
        while (func_0203d81c() == 0 && count != 0) {
            data_020a0f1c |= 0x2000;
            func_02042778();
            func_0203df40();
            count--;
        }
        if (count == 0) {
            data_020a0f1c |= 1;
            func_02040724();
            OS_SleepThread(0);
        }

        func_0203d918();
        count = 0x258;
        while (func_0203d8fc() == 0 && count != 0) {
            data_020a0f1c |= 0x2000;
            func_02042778();
            func_0203df40();
            count--;
        }
        if (count == 0) {
            data_020a0f1c |= 1;
            func_02040724();
            OS_SleepThread(0);
        }
    } else {
        func_02059eb0(&buf);
        func_0205a588(&data_020a10fc, 0, 0x16);
        data_020a10fc.lo = buf.unk1;
        data_020a10fc.b1 = buf.unk18;
        data_020a10a4.lo = data_020a10fc.lo;
        data_020a10fc.hi = 0;
        data_020a10a4.hi = data_020a10fc.hi;
        data_020a10a4.b1 = data_020a10fc.b1;
        CpuCopy8(buf.unk4, data_020a10fe, buf.unk18 * 2);
        CpuCopy8(buf.unk4, data_020a10a6, buf.unk18 * 2);
        data_020a0f00 = buf.unk0;
        func_02059f2c(data_020a0fb8);
        func_02059f2c(data_020a0fa0);
    }
    data_020a0ef0 = 1;
}
