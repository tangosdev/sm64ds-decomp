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
 * background mode) under a saved divider and square-root context; func_ov007_020b6f4c fades
 * out, tears every module down and returns the flags to hand back.
 * sFrameReady is cleared as the bring-up, the frame's logic and the
 * teardown start, and set once the bring-up or the frame's drawing is done;
 * func_ov007_020b6eb4 does nothing until it is set.
 *
 * Edges: the bss word 0x02103430 is referenced by these five functions and
 * nothing else in the overlay, and the range is the one build/tu_map.json
 * cuts between its unattributed ov007 units 0x020ad660..0x020b6eb4 and
 * 0x020b72a0..0x020cc028. The previous function, func_ov007_020b6d40, is one
 * of the init dispatcher's builders; the next, Display_Upload, is the
 * display hook func_ov007_020b6eb4 calls.
 *
 * Emission order: mwccarm 2004/b56 emits one .text section per function in
 * the reverse of source order, so the highest-address function is written
 * first here and the functions come out in ROM order.
 *
 * The five entry points keep their address names under C linkage: their
 * callers in dScDSMT_c and include/decl_common.h are held by other branches.
 * The callee names (Work_Create, MenuGfx_*, MenuSound_*, ScreenFade_*,
 * Display_*, Pad_Update) are coined from their bodies; ov007 records no
 * names.
 */
#include "types.h"
#include "nitro/gx/g2.h"
#include "Ov007Work.h"
#include "Ov007Display.h"
#include "Ov007Settings.h"
#include "Ov007Touch.h"
#include "nitro/cp/context.h"
#include "nitro/hw/registers.h"
#include "nitro/gx/gx.h"
#include "Ov007MenuMem.h"



/* func_ov007_020b7138's first argument: the hooks dScDSMT_c hands the menu
 * (data_ov007_02103290). */
typedef struct Ov007MenuHooks {
    int alloc;               /* 0x00: installed by MenuMem_SetAllocHook */
    int free;                /* 0x04: MenuMem_SetFreeHook */
    int query;               /* 0x08: MenuMem_SetMaxAllocSizeHook */
    int fileLoad;            /* 0x0c: func_ov007_020b4100 */
    int fileRelease;         /* 0x10: func_ov007_020b4100 */
    int (*callback)(int id); /* 0x14: becomes the work block's callback */
} Ov007MenuHooks;

/* set once the bring-up or a frame's drawing is done; cleared as the
 * bring-up, the frame's logic and the teardown start */
extern int sFrameReady;

void func_02045d9c(void);
extern void func_ov007_020aed98(void);
extern void func_ov007_020b2e64(TPData *sample);
extern void func_ov007_020b3050(void);
extern void func_ov007_020b30b0(void);
extern void func_ov007_020b40c0(void);
extern void func_ov007_020b4100(int a, int b);
extern "C" void func_ov007_020b6544(void);
extern "C" void func_ov007_020b65e0(void);
extern "C" void Work_Create(void);
extern void func_ov007_020b6c54(void);
extern "C" void Display_Upload(void);
extern void func_ov007_020b7764(void);
extern void func_ov007_020b782c(void);
extern void func_ov007_020b8690(void);
extern void func_ov007_020b8714(int a0, int a1, int a2);
extern void Display_DrawOam(Ov007Display *d);
extern void func_ov007_020bcf90(void);
extern void MenuGfx_StartCapture(void);
extern void MenuGfx_InitSubEngine(void);
extern void MenuGfx_InitMainEngine(void);
extern void MenuGfx_SetBgMode(int mode);
extern void MenuGfx_ResetVram(void);
extern int *MenuSound_UpdateLoops(void);
extern void MenuSound_Destroy(void);
extern void MenuSound_Create(void);
extern void func_ov007_020be47c(void);
extern void func_ov007_020be50c(void);
extern void ScreenFade_Update(void);
extern void ScreenFade_Destroy(void);
extern void ScreenFade_Create(void);
extern "C" void func_ov007_020c1db0(Ov007Touch *touch, TPData *sample);
extern "C" void Pad_Update(struct Ov007Pad *pad, void *src);
extern void func_ov007_020c3bc8(void);

int func_ov007_020b7138(Ov007MenuHooks *hooks, Ov007BootArgs *bootArgs) {
    int flags;

    sFrameReady = 0;
    func_ov007_020b6c54();
    MenuMem_SetAllocHook(hooks->alloc);
    MenuMem_SetFreeHook(hooks->free);
    MenuMem_SetMaxAllocSizeHook(hooks->query);
    func_ov007_020c3bc8();
    func_ov007_020b4100(hooks->fileLoad, hooks->fileRelease);
    MenuGfx_ResetVram();
    MenuGfx_InitMainEngine();
    MenuGfx_InitSubEngine();
    MenuGfx_SetBgMode(1);
    func_02045d9c();
    G2_SetBlendBrightness(0x3f, -0x10);
    G2S_SetBlendBrightness(0x3f, -0x10);
    GX_SetVisiblePlane(0);
    GXS_SetVisiblePlane(0);
    GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);
    Work_Create();
    WORK->settings->bootArgs = bootArgs;
    WORK->callback = hooks->callback;
    func_ov007_020b30b0();
    MenuSound_Create();
    func_ov007_020b8714((int)WORK->cameras[1], (int)WORK->textures[10], WORK->settings->language);
    func_ov007_020b782c();
    ScreenFade_Create();
    func_ov007_020be50c();
    func_ov007_020b6544();
    flags = bootArgs->bootFlags;
    WORK->nextFigureIndex = flags & 1;
    WORK->strokeStage[0] = (flags & 6) >> 1;
    WORK->strokeStage[1] = (flags & 0x18) >> 3;
    sFrameReady = 1;
    return 1;
}

int func_ov007_020b7090(u16 x, u16 y, u16 touch, u16 validity, int keys) {
    TPData sample;
    int choice = 0;

    sFrameReady = 0;
    Pad_Update(WORK->pad, &keys);
    sample.x = x;
    sample.y = y;
    sample.touch = touch;
    sample.validity = validity;
    func_ov007_020b2e64(&sample);
    func_ov007_020c1db0(WORK->touch, &sample);
    MenuSound_UpdateLoops();
    func_ov007_020aed98();
    if (WORK->choiceReady != 0 || WORK->pendingChoice == 1)
        choice = WORK->pendingChoice;
    return choice;
}

int func_ov007_020b7040(void) {
    func_ov007_020bcf90();
    Display_DrawOam(WORK->display);
    WORK->frameCount++;
    sFrameReady = 1;
    return 1;
}

int func_ov007_020b6f4c(void) {
    Ov007Work *work = WORK;
    int flags = work->nextFigureIndex;
    unsigned int strokeBits = (work->strokeStage[0] << 1) | (work->strokeStage[1] << 3);

    sFrameReady = 0;
    flags |= strokeBits;
    if (work->pendingChoice != 0xa && work->pendingChoice != 0xb && work->choiceReady != 0) {
        G2_SetBlendBrightness(0x3f, 0x10);
        G2S_SetBlendBrightness(0x3f, 0x10);
        GX_SetVisiblePlane(0);
        GXS_SetVisiblePlane(0);
    } else {
        G2_SetBlendBrightness(0x20, -0x10);
        G2S_SetBlendBrightness(0x20, -0x10);
    }
    func_ov007_020b65e0();
    func_ov007_020b3050();
    func_ov007_020b7764();
    MenuSound_Destroy();
    func_ov007_020b8690();
    ScreenFade_Destroy();
    func_ov007_020be47c();
    func_ov007_020b40c0();
    return flags;
}

int func_ov007_020b6eb4(void) {
    CPContext cpContext;
    if (sFrameReady != 0) {
        CP_SaveContext(&cpContext);
        ScreenFade_Update();
        if (WORK->pendingCapture != 0) {
            MenuGfx_StartCapture();
            WORK->pendingCapture = 0;
        }
        if (WORK->pendingBgMode != 0) {
            MenuGfx_SetBgMode(WORK->pendingBgMode);
            WORK->pendingBgMode = 0;
        }
        Display_Upload();
        CPi_RestoreContext(&cpContext);
    }
    return 1;
}

