// NONMATCHING: near-miss, size 0xd8 vs 0xdc (one insn short). Everything matches
// except the tail four zero-stores: ROM materializes `mov r2,#0` TWICE (once per
// store pair: e670/e66c then e664/e668, each pair as ldr addr1; mov #0; ldr addr2;
// str; str), while every candidate shares ONE zero across all four stores and
// hoists pair2's first pool ldr one slot early. Levers tried and inert on the
// zero-remat: volatile stores (fixed store ORDER and pair1 to byte-exact),
// chain vs separate statements, 0u / (void*)0 / 0.0f typed zeros, block-scoped
// zero temps, CLEAR2 macro, goto label EBB split, C++ frontend, static inline
// C++ helper, #pragma opt_common_subs/opt_propagation/opt_lifetimes off.
// Logic verified vs ROM. Counts as decompiled, not matched.
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct OAMEntry { u32 a; u16 b; u16 pad; } OAMEntry;

extern void MultiCopy_Int(void* dst, void* src, int len);
extern void MultiCopy32Bytes(void* src, void* dst, int len);

extern unsigned char data_0209e660;
extern OAMEntry data_0209e674[];
extern int data_0209e67c[];
extern int data_0209e694[];
extern int data_0209ea74[];
extern volatile int data_0209e670;
extern volatile int data_0209e66c;
extern volatile int data_0209e664;
extern volatile int data_0209e668;

void _ZN3OAM5ResetEv(void) {
    int i;
    if (data_0209e660 != 0) {
        i = 0;
        do {
            data_0209e674[i].a = 0xc0;
            data_0209e674[i].b = 0;
        } while (++i < 0x80);
    } else {
        data_0209e674[0].a = 0xc0;
        data_0209e674[0].b = 0;
        MultiCopy_Int(data_0209e674, data_0209e67c, 0x18);
    }
    MultiCopy32Bytes(data_0209e674, data_0209e694, 0x3e0);
    MultiCopy32Bytes(data_0209e674, data_0209ea74, 0x400);
    data_0209e670 = 0;
    data_0209e66c = 0;
    data_0209e664 = 0;
    data_0209e668 = 0;
}
