typedef volatile unsigned int vu32;

typedef struct UnkObj UnkObj;
typedef struct UnkVt {
    void *pad0;
    void *pad4;
    int (*func8)(UnkObj *);
} UnkVt;
struct UnkObj {
    UnkVt *vt;
};

extern UnkObj *data_0209d4a8;
extern unsigned char data_0209d45c;
extern unsigned char data_0209d454;
extern short data_0209d468;
extern short data_0209d46c;
extern short data_0209d4a4;
extern short data_0209d4a0;
extern short data_0209d49c;
extern short data_0209d478;
extern short data_0209d48c;
extern short data_0209d490;
extern short data_0209d494;
extern short data_0209d498;
extern short data_0209d484;
extern short data_0209d480;
extern short data_0209d47c;
extern short data_0209d470;
extern short data_0209d474;
extern short data_0209d488;

void _ZN3OAM5FlushEv(void);
void _ZN3OAM4LoadEv(void);
void func_02055454(int v);

void func_02019144(void)
{
    UnkObj *p = data_0209d4a8;
    if (p != 0) {
        if (p->vt->func8(p) == 0)
            return;
    }
    _ZN3OAM5FlushEv();
    _ZN3OAM4LoadEv();
    *(vu32 *)0x4000000 = (*(vu32 *)0x4000000 & ~0x1f00) | (data_0209d45c << 8);
    *(vu32 *)0x4001000 = (*(vu32 *)0x4001000 & ~0x1f00) | (data_0209d454 << 8);
    if (!(*(vu32 *)0x4000000 & 8)) {
        *(vu32 *)0x4000010 = (data_0209d468 & 0x1ff) | (0x1ff0000 & (data_0209d46c << 16));
    } else {
        func_02055454(data_0209d468);
    }
    *(vu32 *)0x4000014 = (data_0209d4a4 & 0x1ff) | (0x1ff0000 & (data_0209d4a0 << 16));
    *(vu32 *)0x4000018 = (data_0209d49c & 0x1ff) | (0x1ff0000 & (data_0209d478 << 16));
    *(vu32 *)0x400001c = (data_0209d48c & 0x1ff) | (0x1ff0000 & (data_0209d490 << 16));
    *(vu32 *)0x4001000;
    *(vu32 *)0x4001010 = (data_0209d494 & 0x1ff) | (0x1ff0000 & (data_0209d498 << 16));
    *(vu32 *)0x4001014 = (data_0209d484 & 0x1ff) | (0x1ff0000 & (data_0209d480 << 16));
    *(vu32 *)0x4001018 = (data_0209d47c & 0x1ff) | (0x1ff0000 & (data_0209d470 << 16));
    *(vu32 *)0x400101c = (data_0209d474 & 0x1ff) | (0x1ff0000 & (data_0209d488 << 16));
}
