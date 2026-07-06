// MATCHING (abverify MATCH, mwccarm 1.2/sp2p3, canonical flags)
/* func_ov006_02115b0c at 0x02115b0c (ov006), size 0x18bc (6,332 bytes, 1583 insns)
 * Compiler mwccarm 1.2/sp2p3, flags:
 * -O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 *
 * Minigame-board teardown+rebuild: 7 inlined-destructor loops over object
 * slot arrays (double vtable store + Memory::operator_delete2 + slot=0),
 * 3 single-slot deletes, language-selected asset loads (func_02057d00 +
 * DecompressLZ16 to VRAM/tilemaps), per-tile-type scan loops over the two
 * 0x300-entry tilemaps at +0x4954/+0x5154 collecting spawn positions
 * ((i%32)<<15, mapA: ((i/32)*8-0xf0)<<12 / mapB: (i/32)<<15), spawn loops
 * (_Znwj + per-type init, V2 passed via pointer to a caller-local copy),
 * a virtual-init pass (vtbl[2](this) on every spawned object), bulk zeroing.
 *
 * POOL-WORD RESOLUTION (was flagged ambiguous in the lossy disasm text):
 *   +0x59c scan mask = 0x3ff (confirmed byte-exact);
 *   +0x5ac tile ID for the +0x74c object type = 0x3d2 (NOT 0x2d2/0x1d2 -
 *   verified against ROM bytes via abverify; the published text was lossy).
 *
 * Match levers (in order of impact):
 *  - Spawn callees take V2* + explicit caller-local copy (V2 tmp = pos[j];
 *    f(p,c,j,&tmp)). By-value V2 in C mode splits across r3/[sp] with
 *    dynamic sp adjustment, which pins a frame pointer the ROM lacks.
 *    _Znwj result flows through p into the slot store (no separate r var).
 *  - u64-mask laundering on the two 0x46bc spawn slot stores -> pooled
 *    0x46bc + str r0,[r1,rK]! writeback (also the 9th live value that pulls
 *    fp into the push set; frame 0x124 with 9 saved regs).
 *  - BOTH vtable stores in every teardown site are volatile ints: forces
 *    the per-loop pool reload of the shared base vtable (0213ed10) that
 *    mwccarm otherwise CSEs function-wide, and pins the preamble emission
 *    order (mov #0 before the pool loads).
 *  - Counter increments written as inline volatile RMWs
 *    ((*(volatile int*)(c+0x4670))++): rematerializes the pooled counter
 *    address per loop instead of CSEing it across the loop pair.
 *  - In the 12 counted scan loops the pos[k] stores are volatile and the
 *    final shift is folded into the y-store (y kept un-shifted): gives the
 *    ROM texture str x / add rFresh / lsl into dead-k / str. The 6 k-local
 *    (k++) scan loops keep plain stores with fully-shifted y locals.
 *  - Every loop counter is block-scoped (for (int i = ...)): stabilizes the
 *    per-loop register set (i=r4, const=r5, zero=r6, vtblB=r7, vtblA=sb,
 *    slot=sl) instead of rotating.
 *  - Virtual init calls pass this: ((void(**)(void*))(*(int*)p))[2](p)
 *    keeps the object in r0 and the vptr chain in r1.
 *  - Second 0x46bc spawn loop copies the V2 via int temps loaded y-then-x
 *    (int ty = pos[n].y; int tx = pos[n].x;) to match its load order.
 *  - #pragma opt_strength_reduction off (as in the draft) keeps every loop
 *    un-strength-reduced; does not break the div/mod-by-32 shift idioms.
 */
#pragma opt_strength_reduction off
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct V2 { int x, y; } V2;

extern void func_ov004_020adb1c(int a);
extern void _ZN6Memory16operator_delete2EPv(void *p);
extern void func_0207328c(void *p, int a, int b, void *cb);
extern void func_ov004_020b04d0(int a);
extern int GetOwnerLanguage(void);
extern void func_02057d00(void *dst, void *src, int flag);
extern void *func_020adc74(void *p);
extern void DecompressLZ16(void *src, void *dst);
extern void func_ov004_020adc5c(void *p);
extern u16 *_ZN2G212GetBG2ScrPtrEv(void);
extern u16 *_ZN3G2S12GetBG2ScrPtrEv(void);
extern void *func_02054fb0(void);
extern u16 *_ZN3G2S12GetBG3ScrPtrEv(void);
extern void func_020733a8(void *p, int a, int b, void *cb1, void *cb2);
extern void *_Znwj(u32 size);
extern void *func_ov006_02111b40(void *p, char *c, int i, V2 *pos);
extern void *func_ov006_02111774(void *p, char *c, int i, V2 *pos);
extern void *func_ov006_02114548(void *p, char *c, int i, V2 *pos);
extern void *func_ov006_02110bc0(void *p, char *c, int i, V2 *pos);
extern void *func_ov006_02110874(void *p, char *c, int i, V2 *pos);
extern void *func_ov006_02111220(void *p, char *c, int i, V2 *pos);
extern void *func_ov006_021101bc(void *p, char *c, int i, V2 *pos);
extern void *func_ov006_0210f9f8(void *p, char *c, int i, V2 *pos);
extern void *func_ov006_0210e480(void *p, char *c, int i, V2 *pos);
extern void *func_ov006_0210e0d0(void *p, char *c, int i, V2 *pos);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void SetSubBg0Offset(int x, int y);
extern void func_ov006_02114dd0(char *c);
extern void func_0203d47c(void);
extern void func_0203d738(void);

extern int data_ov006_0213ec98[];
extern int data_ov006_0213ed10[];
extern int data_ov006_0213ed4c[];
extern int data_ov006_0213ed88[];
extern int data_ov006_0213ece8[];
extern int data_ov006_0213ecac[];
extern int data_ov006_0213ed9c[];
extern int data_ov006_0213ed38[];
extern int data_ov006_0213ed74[];
extern int data_ov006_0213ed60[];
extern int data_ov006_0213ed24[];
extern int data_ov006_02142c40[];
extern int data_ov006_0213ef8c[];
extern int data_ov006_0213efb8[];
extern int data_ov006_0213efe4[];
extern int data_ov006_0213f010[];
extern int data_ov006_0213f03c[];
extern int data_ov006_0213f068[];
extern int data_ov006_0213f090[];
extern int data_ov006_0213f0c0[];
extern int data_ov006_0213f0f0[];
extern int data_ov006_0213f120[];
extern int data_ov006_0213f150[];
extern int data_ov006_0213f17c[];
extern int data_ov006_0213f1a8[];
extern int data_ov006_0213f1d8[];
extern int data_ov006_0213f208[];
extern int data_ov006_0213f234[];

void func_ov006_02115b0c(char *c)
{
    V2 pos[25];

    func_ov004_020adb1c(0);

    for (int i = 0; i < 0xd; i++) {
        int **slot = (int **)(((long long)(int)(c + i * 4 + 0x4688)) & 0xFFFFFFFFFFFFFFFFLL);
        int *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)data_ov006_0213ec98;
                *(int volatile *)p = (int)data_ov006_0213ed10;
                _ZN6Memory16operator_delete2EPv(p);
            }
            *slot = 0;
        }
    }
    for (int i = 0; i < 8; i++) {
        int **slot = (int **)(((long long)(int)(c + i * 4 + 0x4720)) & 0xFFFFFFFFFFFFFFFFLL);
        int *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)data_ov006_0213ed4c;
                *(int volatile *)p = (int)data_ov006_0213ed10;
                _ZN6Memory16operator_delete2EPv(p);
            }
            *slot = 0;
        }
    }
    for (int i = 0; i < 0x19; i++) {
        int **slot = (int **)(((long long)(int)(c + i * 4 + 0x46bc)) & 0xFFFFFFFFFFFFFFFFLL);
        int *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)data_ov006_0213ed88;
                *(int volatile *)p = (int)data_ov006_0213ed10;
                _ZN6Memory16operator_delete2EPv(p);
            }
            *slot = 0;
        }
    }
    for (int i = 0; i < 3; i++) {
        int **slot = (int **)(((long long)(int)(c + i * 4 + 0x4740)) & 0xFFFFFFFFFFFFFFFFLL);
        int *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)data_ov006_0213ece8;
                *(int volatile *)p = (int)data_ov006_0213ed10;
                _ZN6Memory16operator_delete2EPv(p);
            }
            *slot = 0;
        }
    }
    for (int i = 0; i < 6; i++) {
        int **slot = (int **)(((long long)(int)(c + i * 4 + 0x474c)) & 0xFFFFFFFFFFFFFFFFLL);
        int *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)data_ov006_0213ecac;
                *(int volatile *)p = (int)data_ov006_0213ed10;
                _ZN6Memory16operator_delete2EPv(p);
            }
            *slot = 0;
        }
    }
    for (int i = 0; i < 3; i++) {
        int **slot = (int **)(((long long)(int)(c + i * 4 + 0x4764)) & 0xFFFFFFFFFFFFFFFFLL);
        int *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)data_ov006_0213ed9c;
                *(int volatile *)p = (int)data_ov006_0213ed10;
                _ZN6Memory16operator_delete2EPv(p);
            }
            *slot = 0;
        }
    }
    for (int i = 0; i < 2; i++) {
        int **slot = (int **)(((long long)(int)(c + i * 4 + 0x4770)) & 0xFFFFFFFFFFFFFFFFLL);
        int *p = *slot;
        if (p != 0) {
            if (p != 0) {
                *(int volatile *)p = (int)data_ov006_0213ed38;
                *(int volatile *)p = (int)data_ov006_0213ed10;
                _ZN6Memory16operator_delete2EPv(p);
            }
            *slot = 0;
        }
    }

    {
        int *p = *(int **)(c + 0x4000 + 0x778);
        if (p != 0) {
            if (p != 0) {
                *p = (int)data_ov006_0213ed74;
                func_0207328c((char *)p + 0x4c, 3, 8, func_0203d47c);
                func_0207328c((char *)p + 0x34, 3, 8, func_0203d47c);
                *(int volatile *)p = (int)data_ov006_0213ed10;
                _ZN6Memory16operator_delete2EPv(p);
            }
            *(int **)(c + 0x4000 + 0x778) = 0;
        }
    }
    {
        int *p = *(int **)(c + 0x4000 + 0x77c);
        if (p != 0) {
            if (p != 0) {
                *p = (int)data_ov006_0213ed60;
                *(int volatile *)p = (int)data_ov006_0213ed10;
                _ZN6Memory16operator_delete2EPv(p);
            }
            *(int **)(c + 0x4000 + 0x77c) = 0;
        }
    }
    {
        int *p = *(int **)(c + 0x4000 + 0x780);
        if (p != 0) {
            if (p != 0) {
                *p = (int)data_ov006_0213ed24;
                *(int volatile *)p = (int)data_ov006_0213ed10;
                _ZN6Memory16operator_delete2EPv(p);
            }
            *(int **)(c + 0x4000 + 0x780) = 0;
        }
    }

    func_ov004_020b04d0(0x30);

    if (GetOwnerLanguage() == 5) {
        func_02057d00(data_ov006_02142c40, data_ov006_0213ef8c, *(int *)(c + 0xbc) & 1);
    } else if (GetOwnerLanguage() == 4) {
        func_02057d00(data_ov006_02142c40, data_ov006_0213efb8, *(int *)(c + 0xbc) & 1);
    } else if (GetOwnerLanguage() == 3) {
        func_02057d00(data_ov006_02142c40, data_ov006_0213efe4, *(int *)(c + 0xbc) & 1);
    } else if (GetOwnerLanguage() == 2) {
        func_02057d00(data_ov006_02142c40, data_ov006_0213f010, *(int *)(c + 0xbc) & 1);
    } else if ((*(int *)(c + 8) & 0xff) == 0) {
        func_02057d00(data_ov006_02142c40, data_ov006_0213f03c, *(int *)(c + 0xbc) & 1);
    } else {
        func_02057d00(data_ov006_02142c40, data_ov006_0213f068, *(int *)(c + 0xbc) & 1);
    }
    {
        void *d = func_020adc74(data_ov006_02142c40);
        DecompressLZ16(d, (void *)0x6400000);
        DecompressLZ16(d, (void *)0x6600000);
        func_ov004_020adc5c(d);
    }
    func_02057d00(data_ov006_02142c40, data_ov006_0213f090, *(int *)(c + 0xbc) & 1);
    {
        void *d = func_020adc74(data_ov006_02142c40);
        DecompressLZ16(d, c + 0x4954);
        func_ov004_020adc5c(d);
    }
    func_02057d00(data_ov006_02142c40, data_ov006_0213f0c0, *(int *)(c + 0xbc) & 1);
    {
        void *d = func_020adc74(data_ov006_02142c40);
        DecompressLZ16(d, c + 0x5154);
        func_ov004_020adc5c(d);
    }
    if ((*(int *)(c + 8) & 0xff) == 0) {
        void *d = func_020adc74(data_ov006_0213f0f0);
        DecompressLZ16(d, _ZN2G212GetBG2ScrPtrEv());
        func_ov004_020adc5c(d);
        d = func_020adc74(data_ov006_0213f120);
        DecompressLZ16(d, _ZN3G2S12GetBG2ScrPtrEv());
        func_ov004_020adc5c(d);
        d = func_020adc74(data_ov006_0213f150);
        DecompressLZ16(d, func_02054fb0());
        func_ov004_020adc5c(d);
        d = func_020adc74(data_ov006_0213f17c);
        DecompressLZ16(d, _ZN3G2S12GetBG3ScrPtrEv());
        func_ov004_020adc5c(d);
    } else {
        void *d = func_020adc74(data_ov006_0213f1a8);
        DecompressLZ16(d, _ZN2G212GetBG2ScrPtrEv());
        func_ov004_020adc5c(d);
        d = func_020adc74(data_ov006_0213f1d8);
        DecompressLZ16(d, _ZN3G2S12GetBG2ScrPtrEv());
        func_ov004_020adc5c(d);
        d = func_020adc74(data_ov006_0213f208);
        DecompressLZ16(d, func_02054fb0());
        func_ov004_020adc5c(d);
        d = func_020adc74(data_ov006_0213f234);
        DecompressLZ16(d, _ZN3G2S12GetBG3ScrPtrEv());
        func_ov004_020adc5c(d);
    }

    *(int *)(c + 0x4000 + 0x668) = 0;
    *(int *)(c + 0x4000 + 0x66c) = 0;
    *(int *)(c + 0x4000 + 0x670) = 0;
    *(int *)(c + 0x4000 + 0x678) = 0;
    *(int *)(c + 0x4000 + 0x67c) = 0;
    *(int *)(c + 0x4000 + 0x674) = 0;
    *(int *)(c + 0x4000 + 0x680) = 0;

    func_020733a8(pos, 0x19, 8, func_0203d738, func_0203d47c);

    {
        int mask = 0x3ff;
        for (int i = 0; i < 0x300; i++) {
            int k = *(int *)(c + 0x4000 + 0x670);
            if (k >= 0x19) break;
            if ((*(u16 *)(c + i * 2 + 0x4954) & mask) == 0x2e8) {
                int x = (i % 0x20) << 0xf;
                int y = (i / 0x20) * 8 - 0xf0;
                *(volatile int *)&pos[k].x = x;
                *(volatile int *)&pos[k].y = y << 0xc;
                (*(volatile int *)(c + 0x4670))++;
            }
        }
        for (int i = 0; i < 0x300; i++) {
            int k = *(int *)(c + 0x4000 + 0x670);
            if (k >= 0x19) break;
            if ((*(u16 *)(c + i * 2 + 0x5154) & mask) == 0x2e8) {
                int x = (i % 0x20) << 0xf;
                int y = i / 0x20;
                *(volatile int *)&pos[k].x = x;
                *(volatile int *)&pos[k].y = y << 0xf;
                (*(volatile int *)(c + 0x4670))++;
            }
        }
    }
    {
        int n = *(int *)(c + 0x4000 + 0x670);
        int j;
        for (j = 0; j < n; j++) {
            void *p = _Znwj(0x40);
            if (p != 0) {
                V2 tmp = pos[j];
                p = func_ov006_02111b40(p, c, j, &tmp);
            }
            {
                void **slot = (void **)(((long long)(int)(c + j * 4 + 0x46bc)) & 0xFFFFFFFFFFFFFFFFLL);
                *slot = p;
                if (*slot != 0)
                    *(int *)((char *)*slot + 0x34) = 0;
            }
        }
        {
            int mask = 0x3ff;
                for (int i = 0; i < 0x300; i++) {
                int k = *(int *)(c + 0x4000 + 0x670);
                if (k >= 0x19) break;
                if ((*(u16 *)(c + i * 2 + 0x4954) & mask) == 0x328) {
                    int x = (i % 0x20) << 0xf;
                    int y = (i / 0x20) * 8 - 0xf0;
                    *(volatile int *)&pos[k].x = x;
                    *(volatile int *)&pos[k].y = y << 0xc;
                    (*(volatile int *)(c + 0x4670))++;
                }
            }
            for (int i = 0; i < 0x300; i++) {
                int k = *(int *)(c + 0x4000 + 0x670);
                if (k >= 0x19) break;
                if ((*(u16 *)(c + i * 2 + 0x5154) & mask) == 0x328) {
                    int x = (i % 0x20) << 0xf;
                    int y = i / 0x20;
                    *(volatile int *)&pos[k].x = x;
                    *(volatile int *)&pos[k].y = y << 0xf;
                    (*(volatile int *)(c + 0x4670))++;
                }
            }
        }
        for (; n < *(int *)(c + 0x4000 + 0x670); n++) {
            void *p = _Znwj(0x40);
            if (p != 0) {
                V2 tmp;
                int ty = pos[n].y;
                int tx = pos[n].x;
                tmp.x = tx;
                tmp.y = ty;
                p = func_ov006_02111b40(p, c, n, &tmp);
            }
            {
                void **slot = (void **)(((long long)(int)(c + n * 4 + 0x46bc)) & 0xFFFFFFFFFFFFFFFFLL);
                *slot = p;
                if (*slot != 0)
                    *(int *)((char *)*slot + 0x34) = 1;
            }
        }
    }

    {
        int mask = 0x3ff;
        int tile = 0x2cb;
        for (int i = 0; i < 0x300; i++) {
            int k = *(int *)(c + 0x4000 + 0x66c);
            if (k >= 8) break;
            if ((*(u16 *)(c + i * 2 + 0x4954) & mask) == tile) {
                int x = (i % 0x20) << 0xf;
                int y = (i / 0x20) * 8 - 0xf0;
                *(volatile int *)&pos[k].x = x;
                *(volatile int *)&pos[k].y = y << 0xc;
                (*(volatile int *)(c + 0x466c))++;
            }
        }
        for (int i = 0; i < 0x300; i++) {
            int k = *(int *)(c + 0x4000 + 0x66c);
            if (k >= 8) break;
            if ((*(u16 *)(c + i * 2 + 0x5154) & mask) == tile) {
                int x = (i % 0x20) << 0xf;
                int y = i / 0x20;
                *(volatile int *)&pos[k].x = x;
                *(volatile int *)&pos[k].y = y << 0xf;
                (*(volatile int *)(c + 0x466c))++;
            }
        }
        {
            int n = *(int *)(c + 0x4000 + 0x66c);
            int j;
            for (j = 0; j < n; j++) {
                void *p = _Znwj(0x38);
                if (p != 0) {
                    V2 tmp = pos[j];
                    p = func_ov006_02111774(p, c, j, &tmp);
                }
                *(void **)(c + j * 4 + 0x4000 + 0x720) = p;
                n = *(int *)(c + 0x4000 + 0x66c);
            }
        }
    }

    if ((*(int *)(c + 8) & 0xff) == 0) {
        *(int *)(c + 0x4000 + 0x668) = 0xc;
    } else {
        *(int *)(c + 0x4000 + 0x668) = 0xd;
    }
    {
        int j;
        for (j = 0; j < *(int *)(c + 0x4000 + 0x668); j++) {
            void *p;
            pos[j].x = ((1 - j) << 0x10) + 0xc0000;
            pos[j].y = _ZN4cstd4fdivEii((int)(((long long)pos[j].x * 0xa000 + 0x800) >> 12), 0xd8000) + 0x90000;
            p = _Znwj(0x12c);
            if (p != 0) {
                V2 tmp = pos[j];
                p = func_ov006_02114548(p, c, j, &tmp);
            }
            *(void **)(c + j * 4 + 0x4000 + 0x688) = p;
        }
    }

    {
        int mask = 0x3ff;
        int tile = 0x3d2;
        int zero = 0;
        for (int i = 0; i < 0x300; i++) {
            u16 raw;
            int k = *(int *)(c + 0x4000 + 0x678);
            if (k >= 6) break;
            raw = *(u16 *)(c + i * 2 + 0x4954);
            if ((raw & mask) == tile && (raw & 0x800) == 0) {
                int x = (i % 0x20) << 0xf;
                int y = zero;
                pos[k].x = x;
                pos[k].y = y;
                (*(volatile int *)(c + 0x4678))++;
            }
        }
        for (int i = 0; i < 0x300; i++) {
            u16 raw;
            int k = *(int *)(c + 0x4000 + 0x678);
            if (k >= 6) break;
            raw = *(u16 *)(c + i * 2 + 0x5154);
            if ((raw & mask) == tile && (raw & 0x800) == 0) {
                int x = (i % 0x20) << 0xf;
                int y = zero;
                pos[k].x = x;
                pos[k].y = y;
                (*(volatile int *)(c + 0x4678))++;
            }
        }
        {
            int n = *(int *)(c + 0x4000 + 0x678);
            int j;
            for (j = 0; j < n; j++) {
                void *p = _Znwj(0x34);
                if (p != 0) {
                    V2 tmp = pos[j];
                    p = func_ov006_02110bc0(p, c, j, &tmp);
                }
                *(void **)(c + j * 4 + 0x4000 + 0x74c) = p;
                n = *(int *)(c + 0x4000 + 0x678);
            }
        }
    }

    {
        int mask = 0x3ff;
        int tile = 0x292;
        for (int i = 0; i < 0x300; i++) {
            int k = *(int *)(c + 0x4000 + 0x67c);
            if (k >= 3) break;
            if ((*(u16 *)(c + i * 2 + 0x4954) & mask) == tile) {
                int x = (i % 0x20) << 0xf;
                int y = (i / 0x20) * 8 - 0xf0;
                *(volatile int *)&pos[k].x = x;
                *(volatile int *)&pos[k].y = y << 0xc;
                (*(volatile int *)(c + 0x467c))++;
            }
        }
        for (int i = 0; i < 0x300; i++) {
            int k = *(int *)(c + 0x4000 + 0x67c);
            if (k >= 3) break;
            if ((*(u16 *)(c + i * 2 + 0x5154) & mask) == tile) {
                int x = (i % 0x20) << 0xf;
                int y = i / 0x20;
                *(volatile int *)&pos[k].x = x;
                *(volatile int *)&pos[k].y = y << 0xf;
                (*(volatile int *)(c + 0x467c))++;
            }
        }
        {
            int n = *(int *)(c + 0x4000 + 0x67c);
            int j;
            for (j = 0; j < n; j++) {
                void *p = _Znwj(0x3c);
                if (p != 0) {
                    V2 tmp = pos[j];
                    p = func_ov006_02110874(p, c, j, &tmp);
                }
                *(void **)(c + j * 4 + 0x4000 + 0x764) = p;
                n = *(int *)(c + 0x4000 + 0x67c);
            }
        }
    }

    {
        int mask = 0x3ff;
        for (int i = 0; i < 0x300; i++) {
            int k = *(int *)(c + 0x4000 + 0x674);
            if (k >= 3) break;
            if ((*(u16 *)(c + i * 2 + 0x4954) & mask) == 0x84) {
                int x = (i % 0x20) << 0xf;
                int y = (i / 0x20) * 8 - 0xf0;
                *(volatile int *)&pos[k].x = x;
                *(volatile int *)&pos[k].y = y << 0xc;
                (*(volatile int *)(c + 0x4674))++;
            }
        }
        for (int i = 0; i < 0x300; i++) {
            int k = *(int *)(c + 0x4000 + 0x674);
            if (k >= 3) break;
            if ((*(u16 *)(c + i * 2 + 0x5154) & mask) == 0x84) {
                int x = (i % 0x20) << 0xf;
                int y = i / 0x20;
                *(volatile int *)&pos[k].x = x;
                *(volatile int *)&pos[k].y = y << 0xf;
                (*(volatile int *)(c + 0x4674))++;
            }
        }
        {
            int n = *(int *)(c + 0x4000 + 0x674);
            int j;
            for (j = 0; j < n; j++) {
                void *p = _Znwj(0x78);
                if (p != 0) {
                    V2 tmp = pos[j];
                    p = func_ov006_02111220(p, c, j, &tmp);
                }
                *(void **)(c + j * 4 + 0x4000 + 0x740) = p;
                n = *(int *)(c + 0x4000 + 0x674);
            }
        }
    }

    {
        int k = 0;
        int mask = 0x3ff;
        int j;
        for (j = 0; j < 0x300; j++) {
            if (k >= 1) break;
            if ((*(u16 *)(c + j * 2 + 0x4954) & mask) == 0x57) {
                int x = (j % 0x20) << 0xf;
                int y = ((j / 0x20) * 8 - 0xf0) << 0xc;
                pos[k].x = x;
                pos[k].y = y;
                k++;
            }
        }
        {
            int mask2 = 0x3ff;
            for (j = 0; j < 0x300; j++) {
                if (k >= 1) break;
                if ((*(u16 *)(c + j * 2 + 0x5154) & mask2) == 0x57) {
                    int x = (j % 0x20) << 0xf;
                    int y = (j / 0x20) << 0xf;
                    pos[k].x = x;
                    pos[k].y = y;
                    k++;
                }
            }
        }
        if (k >= 1) {
            void *p = _Znwj(0x88);
            if (p != 0) {
                V2 tmp = pos[0];
                p = func_ov006_021101bc(p, c, 0, &tmp);
            }
            *(void **)(c + 0x4000 + 0x778) = p;
        }
    }

    {
        int k = 0;
        int mask = 0x3ff;
        int j;
        for (j = 0; j < 0x300; j++) {
            if (k >= 9) break;
            if ((*(u16 *)(c + j * 2 + 0x4954) & mask) == 0x4a) {
                int x = (j % 0x20) << 0xf;
                int y = ((j / 0x20) * 8 - 0xf0) << 0xc;
                pos[k].x = x;
                pos[k].y = y;
                k++;
            }
        }
        {
            int mask2 = 0x3ff;
            for (j = 0; j < 0x300; j++) {
                if (k >= 9) break;
                if ((*(u16 *)(c + j * 2 + 0x5154) & mask2) == 0x4a) {
                    int x = (j % 0x20) << 0xf;
                    int y = (j / 0x20) << 0xf;
                    pos[k].x = x;
                    pos[k].y = y;
                    k++;
                }
            }
        }
        if (k >= 9) {
            void *p = _Znwj(0x9c);
            if (p != 0) {
                V2 tmp = pos[4];
                p = func_ov006_0210f9f8(p, c, 0, &tmp);
            }
            *(void **)(c + 0x4000 + 0x77c) = p;
        }
    }

    {
        int mask = 0x3ff;
        for (int i = 0; i < 0x300; i++) {
            int k = *(int *)(c + 0x4000 + 0x680);
            if (k >= 2) break;
            if ((*(u16 *)(c + i * 2 + 0x4954) & mask) == 0x50) {
                int x = (i % 0x20) << 0xf;
                int y = (i / 0x20) * 8 - 0xf0;
                *(volatile int *)&pos[k].x = x;
                *(volatile int *)&pos[k].y = y << 0xc;
                (*(volatile int *)(c + 0x4680))++;
            }
        }
        for (int i = 0; i < 0x300; i++) {
            int k = *(int *)(c + 0x4000 + 0x680);
            if (k >= 2) break;
            if ((*(u16 *)(c + i * 2 + 0x5154) & mask) == 0x50) {
                int x = (i % 0x20) << 0xf;
                int y = i / 0x20;
                *(volatile int *)&pos[k].x = x;
                *(volatile int *)&pos[k].y = y << 0xf;
                (*(volatile int *)(c + 0x4680))++;
            }
        }
        {
            int n = *(int *)(c + 0x4000 + 0x680);
            int j;
            for (j = 0; j < n; j++) {
                void *p = _Znwj(0x38);
                if (p != 0) {
                    V2 tmp = pos[j];
                    p = func_ov006_0210e480(p, c, j, &tmp);
                }
                *(void **)(c + j * 4 + 0x4000 + 0x770) = p;
                n = *(int *)(c + 0x4000 + 0x680);
            }
        }
    }

    {
        int k = 0;
        int mask = 0x3ff;
        int j;
        for (j = 0; j < 0x300; j++) {
            if (k >= 1) break;
            if ((*(u16 *)(c + j * 2 + 0x4954) & mask) == 0xab) {
                int x = (j % 0x20) << 0xf;
                int y = ((j / 0x20) * 8 - 0xf0) << 0xc;
                pos[k].x = x;
                pos[k].y = y;
                k++;
            }
        }
        {
            int mask2 = 0x3ff;
            for (j = 0; j < 0x300; j++) {
                if (k >= 1) break;
                if ((*(u16 *)(c + j * 2 + 0x5154) & mask2) == 0xab) {
                    int x = (j % 0x20) << 0xf;
                    int y = (j / 0x20) << 0xf;
                    pos[k].x = x;
                    pos[k].y = y;
                    k++;
                }
            }
        }
        if (k >= 1) {
            void *p = _Znwj(0x88);
            if (p != 0) {
                V2 tmp = pos[0];
                p = func_ov006_0210e0d0(p, c, 0, &tmp);
            }
            *(void **)(c + 0x4000 + 0x780) = p;
        }
    }

    for (int i = 0; i < *(int *)(c + 0x4000 + 0x66c); i++) {
        void *p = *(void **)(c + i * 4 + 0x4000 + 0x720);
        if (p != 0)
            ((void (**)(void *))(*(int *)p))[2](p);
    }
    for (int i = 0; i < *(int *)(c + 0x4000 + 0x670); i++) {
        void *p = *(void **)(c + i * 4 + 0x4000 + 0x6bc);
        if (p != 0)
            ((void (**)(void *))(*(int *)p))[2](p);
    }
    for (int i = 0; i < *(int *)(c + 0x4000 + 0x668); i++) {
        void *p = *(void **)(c + i * 4 + 0x4000 + 0x688);
        if (p != 0)
            ((void (**)(void *))(*(int *)p))[2](p);
    }
    {
        void *p = *(void **)(c + 0x4000 + 0x684);
        ((void (**)(void *))(*(int *)p))[2](p);
    }
    for (int i = 0; i < *(int *)(c + 0x4000 + 0x674); i++) {
        void *p = *(void **)(c + i * 4 + 0x4000 + 0x740);
        if (p != 0)
            ((void (**)(void *))(*(int *)p))[2](p);
    }
    for (int i = 0; i < *(int *)(c + 0x4000 + 0x678); i++) {
        void *p = *(void **)(c + i * 4 + 0x4000 + 0x74c);
        if (p != 0)
            ((void (**)(void *))(*(int *)p))[2](p);
    }
    for (int i = 0; i < *(int *)(c + 0x4000 + 0x67c); i++) {
        void *p = *(void **)(c + i * 4 + 0x4000 + 0x764);
        if (p != 0)
            ((void (**)(void *))(*(int *)p))[2](p);
    }
    for (int i = 0; i < *(int *)(c + 0x4000 + 0x680); i++) {
        void *p = *(void **)(c + i * 4 + 0x4000 + 0x770);
        if (p != 0)
            ((void (**)(void *))(*(int *)p))[2](p);
    }
    {
        void *p = *(void **)(c + 0x4000 + 0x778);
        if (p != 0)
            ((void (**)(void *))(*(int *)p))[2](p);
    }
    {
        void *p = *(void **)(c + 0x4000 + 0x77c);
        if (p != 0)
            ((void (**)(void *))(*(int *)p))[2](p);
    }
    {
        void *p = *(void **)(c + 0x4000 + 0x780);
        if (p != 0)
            ((void (**)(void *))(*(int *)p))[2](p);
    }

    *(int *)(c + 0x4000 + 0x664) = 0;
    *(int *)(c + 0x4000 + 0x784) = 0;
    *(int *)(c + 0x4000 + 0x788) = 0;
    *(int *)(c + 0x5000 + 0x954) = 0;
    *(int *)(c + 0x5000 + 0x958) = 0;
    *(u8 *)(c + 0x5000 + 0x95c) = 0;
    *(u8 *)(c + 0x5000 + 0x95d) = 0;
    for (int i = 0; i < 4; i++)
        *(int *)(c + i * 4 + 0x5000 + 0x968) = 0;
    *(int *)(c + 0x5000 + 0x964) = 0;
    *(int *)(c + 0x5000 + 0x978) = 0;
    *(int *)(c + 0x5000 + 0x97c) = 0;
    *(int *)(c + 0x5000 + 0x980) = 0;
    *(int *)(c + 0x5000 + 0x984) = 0;
    for (int i = 0; i < 3; i++)
        *(int *)(c + i * 4 + 0x5000 + 0x988) = 0;
    *(int *)(c + 0x5000 + 0x994) = 0;
    *(int *)(c + 0x5000 + 0x960) = 0;
    for (int i = 0; i < 5; i++) {
        *(int *)(c + i * 4 + 0x4000 + 0x78c) = 0;
        *(int *)(c + i * 4 + 0x4000 + 0x7a0) = 0;
        *(int *)(c + i * 4 + 0x4000 + 0x7b4) = 0;
        *(int *)(c + i * 8 + 0x4000 + 0x7c8) = 0;
        *(int *)(c + i * 8 + 0x4000 + 0x7cc) = 0;
        *(int *)(c + i * 4 + 0x4000 + 0x7f0) = 0;
    }
    for (int i = 0; i < 0x10; i++) {
        *(u8 *)(c + i + 0x4000 + 0x804) = 0;
        *(int *)(c + i * 4 + 0x4000 + 0x814) = 0;
        *(int *)(c + i * 8 + 0x4000 + 0x854) = 0;
        *(int *)(c + i * 8 + 0x4000 + 0x858) = 0;
        *(int *)(c + i * 8 + 0x4000 + 0x8d4) = 0;
        *(int *)(c + i * 8 + 0x4000 + 0x8d8) = 0;
    }
    *(int *)(c + 0x4000 + 0x660) = 0;
    *(int *)(c + 0x5000 + 0x998) = 0x100;
    SetSubBg0Offset(*(int *)(c + 0x5000 + 0x998), 0);
    func_ov006_02114dd0(c);
    func_0207328c(pos, 0x19, 8, func_0203d47c);
}
