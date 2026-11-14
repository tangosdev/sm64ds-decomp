//cpp
#include "types.h"
#include "Heap.h"
#include "nitro/os/system.h"
#include "nitro/os/interrupt.h"
#include "nitro/fx/fx.h"
#include "nitro/gx/gx.h"
#include "dScene_c.h"
#include "private/watchdog_alarm.h"
#include "nitro/gx/g3imm_light.h"
#include "nitro/hw/registers.h"
#include "nitro/mb/mb.h"
#include "nitro/pxi/init.h"

/* NitroSDK's inline IME helper (os/common/interrupt.h); returns the previous
 * master enable. */
static inline int OS_EnableIrq(void) {
    u16 prep = REG_IME;
    REG_IME = 1;
    return prep;
}

extern void func_020427f8(void);
extern u64 func_02042784(void);
extern "C" {

extern void func_0203d740(void);
}

extern void ClearSaveBlock(void);
extern void func_0201a4e4(void);
extern "C" {

extern void func_0203bbc0(void);
}

extern void func_0203bb5c(void);
extern void func_0201fec8(void);
extern void func_02042f68(int backupType, unsigned char *magic);
extern "C" {

extern void func_02018aa4(void);
}

extern void func_0203ad84(void);
extern void func_02019440(void);
extern void func_020134c8(void);
extern "C" {

extern void func_0203b684(void);
extern void func_020233f0(void);
extern void func_020196cc(void);
extern void func_0201a5cc(void);
}

extern char data_0208ee50[12];
/* save magic, zero padded */
extern char ACTOR_SPAWN_TABLE[];
extern void *data_020a4bb8;
struct S0209ee90 {
    char pad[0x44];
    u32 f44;
};
extern struct S0209ee90 data_0209ee90;

/* The game's start-up, run once from main before the frame loop
   (func_020197b8). On a download-play child (MB_IsMultiBootChild) it
   leaves out func_0201fec8 (which reads the owner's language and starts the
   link through func_0203db64), the file system (func_02018aa4) and
   func_020134c8; the boot job func_0201a2f8 runs those three later for a
   child. The /BUILDTIME read (func_0203ad84) is the one step a child never
   makes, and it masks the card IREQ interrupt (bit 20). Every console
   calls the save setup (func_02042f68) and sets up the game heap, the
   display (func_02019440), the actor spawn table and the boot scene. */
void func_0201a054(void) {
    int isChild = MB_IsMultiBootChild();
    func_020427f8();
    func_02042784();
    FX_Init();
    GX_Init();
    PXI_Init();
    func_0203d740();
    ClearSaveBlock();
    func_0201a4e4();
    OS_EnableIrqMask(OS_IE_V_BLANK);
    OS_EnableIrq();
    OS_EnableInterrupts();
    GX_VBlankIntr(1);
    func_0203bbc0();
    func_0203bb5c();
    if (!isChild)
        func_0201fec8();
    func_02042f68(0xd01, (unsigned char *)data_0208ee50); /* 0xd01: a 64-Kbit EEPROM */
    if (!isChild)
        func_02018aa4();
    if (isChild)
        OS_DisableIrqMask(OS_IE_CARD_IREQ);
    if (!isChild)
        func_0203ad84();
    func_0201a9fc(&data_0209d574);
    mHeap::Heap_t::InitializeGameHeap(0x3b000, 0);
    func_0201a9fc(&data_0209d574);
    func_02019440();
    if (!isChild)
        func_020134c8();
    func_0203b684();
    data_020a4bb8 = ACTOR_SPAWN_TABLE;
    func_020233f0();
    dScene_c::PrepareToSpawnBoot();
    func_020196cc();
    data_0209ee90.f44 = 0x100;
    G3_LightVector(GX_LIGHTID_0, 0, -0xb50, -0xb50); /* (0, -1/sqrt2, -1/sqrt2) */
    G3_LightColor(GX_LIGHTID_0, GX_RGB(31, 31, 31));
    REG_DIF_AMB = GX_RGB(31, 31, 31);
    REG_SPE_EMI = 0;
    func_0201a5cc();
}
