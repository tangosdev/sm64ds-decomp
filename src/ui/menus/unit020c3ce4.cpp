//cpp
/* unit020c3ce4 -- ov007 unit 0x020c3ce4..0x020c3e4c: 7 functions, .text only.
 *
 * The title menu's memory hooks. ov007 is the title and file-select menu
 * program that dScDSMT_c hosts; outside dScDSMT_c it has no ROM RTTI or
 * vtable, so no class name is claimed and the file keeps the unit<addr>
 * name.
 *
 * What the code does. The menu does not call a heap directly: the init
 * dispatcher, func_ov007_020b7138, installs three hooks through
 * func_ov007_020c3e1c, 020c3e2c and 020c3e3c, and every module allocates
 * through func_ov007_020c3df4 (the alloc hook, data_ov007_02103258), frees
 * through func_ov007_020c3d1c (the free hook, data_ov007_02103254) and asks
 * func_ov007_020c3ce4 for the third hook's answer (data_ov007_02104c14,
 * 0 while it is unset). func_ov007_020c3d40 allocates and zero-fills,
 * picking the store routine by the size's low bits. func_ov007_020c3df4's
 * first argument (the callers pass a heap-name string) is not forwarded.
 *
 * Edges: the two .data hook words 0x02103254 / 0x02103258 and the bss word
 * 0x02104c14 are referenced only by these functions. The next function,
 * func_ov007_020c3e4c, has no ov007 caller or data. The left edge is
 * medium: func_ov007_020c3bc8 stores a magic word into the neighbouring
 * .data word 0x02103250 and may belong here, but the colour blend
 * func_ov007_020c3be0 between them references no data at all.
 *
 * Emission order: mwccarm 2004/b56 emits one .text section per function in
 * the reverse of source order, so the highest-address function is written
 * first here and the functions come out in ROM order.
 *
 * The functions keep their ROM names under C linkage.
 */

extern "C" {

typedef int (*Ov007QueryHook)(void);
typedef void (*Ov007FreeHook)(void *);
typedef int (*Ov007AllocHook)(void *);

extern Ov007QueryHook data_ov007_02104c14;
extern Ov007FreeHook data_ov007_02103254;
extern Ov007AllocHook data_ov007_02103258;

extern void MultiStore32Bytes(unsigned val, int *dst, int len);
extern void MultiStore_Int(int val, int *dst, int len);
extern void MultiStore16(unsigned short val, char *dst, int nbytes);
extern void func_0205a588(int a, int b, int c);

// @symbol func_ov007_020c3e3c
void func_ov007_020c3e3c(int v) {
    data_ov007_02103258 = reinterpret_cast<Ov007AllocHook>(v);
}

// @symbol func_ov007_020c3e2c
void func_ov007_020c3e2c(int v) {
    data_ov007_02103254 = reinterpret_cast<Ov007FreeHook>(v);
}

// @symbol func_ov007_020c3e1c
void func_ov007_020c3e1c(int v) {
    data_ov007_02104c14 = reinterpret_cast<Ov007QueryHook>(v);
}

// @symbol func_ov007_020c3df4
int func_ov007_020c3df4(int a, void *b) {
    return data_ov007_02103258(b);
}

// @symbol func_ov007_020c3d40
int func_ov007_020c3d40(int a, int n) {
    int *r = (int *)func_ov007_020c3df4(a, (void *)n);
    volatile unsigned short v16;
    volatile unsigned v32;
    volatile int vint;
    switch (n - (n & ~3)) {
    case 0:
        if (n - (n & ~0x1f) != 0) {
            v32 = 0;
            MultiStore32Bytes(v32, r, n);
        } else {
            vint = 0;
            MultiStore_Int(vint, r, n);
        }
        break;
    case 2:
        v16 = 0;
        MultiStore16(v16, (char *)r, n);
        break;
    case 1:
    case 3:
        func_0205a588((int)r, 0, n);
        break;
    }
    return (int)r;
}

// @symbol func_ov007_020c3d1c
/* NOT a Player method: it reads no fields, it only calls the free hook. */
void func_ov007_020c3d1c(void *self) {
    data_ov007_02103254(self);
}

// @symbol func_ov007_020c3ce4
int func_ov007_020c3ce4(void) {
    if (data_ov007_02104c14 == 0)
        return 0;
    return data_ov007_02104c14();
}
}
