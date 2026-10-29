//cpp
/* unit020b6eb4 -- ov007 unit 0x020b6eb4..0x020b72a0: 5 functions, .text only.
 *
 * The title menu's entry points. ov007 is the title and file-select menu
 * program that dScDSMT_c hosts; outside dScDSMT_c it has no ROM RTTI or
 * vtable, so no class name is claimed and the file keeps the unit<addr>
 * name.
 *
 * What the code does. These are the five functions dScDSMT_c calls into the
 * menu through. func_ov007_020b7138 brings the menu up: it installs the
 * caller's memory and file hooks, builds every module and seeds the work
 * block from the caller's boot flags. func_ov007_020b7090 runs one frame of
 * input and menu logic and returns the choice once one is made;
 * func_ov007_020b7040 runs the frame's drawing; func_ov007_020b6eb4 runs the
 * frame's display work (the screen fade, a queued display capture or
 * background mode) under a saved ARM math state; func_ov007_020b6f4c fades
 * out, tears every module down and returns the flags to hand back.
 * data_ov007_02103430 is clear while one of the first four is running and
 * set once it is done; func_ov007_020b6eb4 does nothing until it is set.
 *
 * Edges: the bss word 0x02103430 is referenced by these five functions and
 * nothing else in the overlay, and the range is the one build/tu_map.json
 * cuts between its unattributed ov007 units 0x020ad660..0x020b6eb4 and
 * 0x020b72a0..0x020cc028. The previous function, func_ov007_020b6d40, is one
 * of the init dispatcher's builders; the next, func_ov007_020b72a0, is the
 * display hook func_ov007_020b6eb4 calls.
 *
 * Emission order: mwccarm 2004/b56 emits one .text section per function in
 * the reverse of source order, so the highest-address function is written
 * first here and the functions come out in ROM order.
 *
 * The functions keep their ROM names under C linkage.
 */
#include "types.h"
#include "G2x.h"
#include "Ov007Work.h"

extern "C" {

/* func_ov007_020b7138's first argument: the hooks dScDSMT_c hands the menu
 * (data_ov007_02103290). */
typedef struct Ov007MenuHooks {
    int alloc;               /* 0x00: installed by func_ov007_020c3e3c */
    int free;                /* 0x04: func_ov007_020c3e2c */
    int query;               /* 0x08: func_ov007_020c3e1c */
    int fileLoad;            /* 0x0c: func_ov007_020b4100 */
    int fileRelease;         /* 0x10: func_ov007_020b4100 */
    int (*callback)(int id); /* 0x14: becomes the work block's callback */
} Ov007MenuHooks;

/* The 0x3c-byte record func_ov007_020b6a84 allocates; only the slots this
 * file touches. */
typedef struct Ov007Settings {
    /* 0x00: func_ov007_020b7138's second argument, whose first word holds
     * the boot flags */
    int *bootArgs;
    char pad_04[0x36];
    unsigned char language; /* 0x3a: func_ov007_020b6a84 seeds it from the owner-info language */
} Ov007Settings;

typedef struct ARMMathState {
    int pad[7];
} ARMMathState;

extern char *data_ov007_0210342c;
/* set while no entry point is running */
extern int data_ov007_02103430;

/* The extern keeps the tree's char * spelling; the typed view is reached
 * through a cast macro. */
#define WORK ((Ov007Work *)data_ov007_0210342c)

extern void ARMMathSaveState(ARMMathState *s);
extern void ARMMathLoadState(ARMMathState *s);
extern void func_02045d9c(void);
extern void func_ov007_020aed98(void *thiz);
extern void func_ov007_020b2e64(u16 *buf);
extern void func_ov007_020b3050(void);
extern void func_ov007_020b30b0(void);
extern void func_ov007_020b40c0(void);
extern void func_ov007_020b4100(int a, int b);
extern void func_ov007_020b6544(void);
extern void func_ov007_020b65e0(void);
extern void func_ov007_020b68e8(void);
extern void func_ov007_020b6c54(void);
extern void func_ov007_020b72a0(void);
extern void func_ov007_020b7764(void);
extern void func_ov007_020b782c(void);
extern void func_ov007_020b8690(void);
extern void func_ov007_020b8714(int a0, int a1, int a2);
extern void func_ov007_020bc9c8(int x);
extern void func_ov007_020bcf90(void);
extern void func_ov007_020bd4e8(void);
extern void func_ov007_020bd500(void);
extern void func_ov007_020bd5a8(void);
extern void func_ov007_020bd648(int mode);
extern void func_ov007_020bd960(void);
extern void *func_ov007_020bdebc(void);
extern void func_ov007_020bdf60(void);
extern void func_ov007_020be00c(void);
extern void func_ov007_020be47c(void);
extern void func_ov007_020be50c(void);
extern void func_ov007_020bf690(void);
extern void func_ov007_020bfc54(void);
extern void func_ov007_020bfcec(void);
extern void func_ov007_020c1db0(void *c, u16 *in);
extern void func_ov007_020c20b8(int a, void *b);
extern void func_ov007_020c3bc8(void);
extern void func_ov007_020c3e1c(int v);
extern void func_ov007_020c3e2c(int v);
extern void func_ov007_020c3e3c(int v);

// @symbol func_ov007_020b7138
int func_ov007_020b7138(Ov007MenuHooks *a, int *b) {
    Ov007Work *p;
    int flags;

    data_ov007_02103430 = 0;
    func_ov007_020b6c54();
    func_ov007_020c3e3c(a->alloc);
    func_ov007_020c3e2c(a->free);
    func_ov007_020c3e1c(a->query);
    func_ov007_020c3bc8();
    func_ov007_020b4100(a->fileLoad, a->fileRelease);
    func_ov007_020bd960();
    func_ov007_020bd5a8();
    func_ov007_020bd500();
    func_ov007_020bd648(1);
    func_02045d9c();
    G2x::SetBlendBrightness((volatile unsigned short *)0x4000050, 0x3f, -0x10);
    G2x::SetBlendBrightness((volatile unsigned short *)0x4001050, 0x3f, -0x10);
    *(unsigned int *)0x4000000 &= ~0x1f00;
    *(unsigned int *)0x4001000 &= ~0x1f00;
    *(unsigned short *)0x4000304 &= ~0x8000;
    func_ov007_020b68e8();
    WORK->settings->bootArgs = b;
    WORK->callback = a->callback;
    func_ov007_020b30b0();
    func_ov007_020be00c();
    p = WORK;
    func_ov007_020b8714((int)p->cameras[1], (int)p->textures[10], p->settings->language);
    func_ov007_020b782c();
    func_ov007_020bfcec();
    func_ov007_020be50c();
    func_ov007_020b6544();
    flags = *b;
    WORK->nextFigureIndex = flags & 1;
    WORK->strokeStage[0] = (flags & 6) >> 1;
    WORK->strokeStage[1] = (flags & 0x18) >> 3;
    data_ov007_02103430 = 1;
    return 1;
}

// @symbol func_ov007_020b7090
int func_ov007_020b7090(u16 a0, u16 a1, u16 a2, u16 a3, int arg4) {
    u16 buf[4];
    int ret = 0;

    data_ov007_02103430 = 0;
    func_ov007_020c20b8((int)WORK->pad, &arg4);
    buf[0] = a0;
    buf[1] = a1;
    buf[2] = a2;
    buf[3] = a3;
    func_ov007_020b2e64(buf);
    func_ov007_020c1db0(WORK->touch, buf);
    func_ov007_020aed98(func_ov007_020bdebc());
    {
        Ov007Work *ip = WORK;
        if (ip->choiceReady != 0 || ip->pendingChoice == 1)
            ret = ip->pendingChoice;
    }
    return ret;
}

// @symbol func_ov007_020b7040
int func_ov007_020b7040(void) {
    func_ov007_020bcf90();
    func_ov007_020bc9c8((int)WORK->display);
    WORK->frameCount++;
    data_ov007_02103430 = 1;
    return 1;
}

// @symbol func_ov007_020b6f4c
int func_ov007_020b6f4c(void) {
    Ov007Work *p = WORK;
    int r;
    r = p->nextFigureIndex;
    {
        unsigned int t = (p->strokeStage[0] << 1) | (p->strokeStage[1] << 3);
        data_ov007_02103430 = 0;
        r |= t;
    }
    if (p->pendingChoice != 0xa && p->pendingChoice != 0xb && p->choiceReady != 0) {
        G2x::SetBlendBrightness((volatile unsigned short *)0x4000050, 0x3f, 0x10);
        G2x::SetBlendBrightness((volatile unsigned short *)0x4001050, 0x3f, 0x10);
        *(unsigned int *)0x4000000 &= ~0x1f00;
        *(unsigned int *)0x4001000 &= ~0x1f00;
    } else {
        G2x::SetBlendBrightness((volatile unsigned short *)0x4000050, 0x20, -0x10);
        G2x::SetBlendBrightness((volatile unsigned short *)0x4001050, 0x20, -0x10);
    }
    func_ov007_020b65e0();
    func_ov007_020b3050();
    func_ov007_020b7764();
    func_ov007_020bdf60();
    func_ov007_020b8690();
    func_ov007_020bfc54();
    func_ov007_020be47c();
    func_ov007_020b40c0();
    return r;
}

// @symbol func_ov007_020b6eb4
int func_ov007_020b6eb4(void) {
    ARMMathState s;
    if (data_ov007_02103430 != 0) {
        ARMMathSaveState(&s);
        func_ov007_020bf690();
        if (WORK->pendingCapture != 0) {
            func_ov007_020bd4e8();
            WORK->pendingCapture = 0;
        }
        if (WORK->pendingBgMode != 0) {
            func_ov007_020bd648(WORK->pendingBgMode);
            WORK->pendingBgMode = 0;
        }
        func_ov007_020b72a0();
        ARMMathLoadState(&s);
    }
    return 1;
}
}
