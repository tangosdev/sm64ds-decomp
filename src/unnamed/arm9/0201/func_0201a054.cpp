//cpp
#include "types.h"
#include "Heap.h"
#include "IRQ.h"
#include "dScene_c.h"

extern "C" {

extern void func_020427f8(void);
extern u64 func_02042784(void);
extern void func_02053a8c(void);
extern void func_02053c40(void);
extern void PXI_Init(void);
extern void func_0203d740(void);
extern void func_02013e64(void);
extern void func_0201a4e4(void);
extern int func_02053be0(int enable);
extern void func_0203bbc0(void);
extern void func_0203bb5c(void);
extern void func_0201fec8(void);
extern void func_02042f68(int backupType, void *magic);
extern void func_02018aa4(void);
extern void func_0203ad84(void);
extern void func_0201a9fc(void *c);
extern void func_02019440(void);
extern void func_020134c8(void);
extern void func_0203b684(void);
extern void func_020233f0(void);
extern void func_020196cc(void);
extern void func_0201a5cc(void);

extern char data_0208ee50[]; /* "ds mario", the save magic */
extern char data_0209d574[];
extern char ACTOR_SPAWN_TABLE[];
extern void *data_020a4bb8;
struct S0209ee90 {
    char pad[0x44];
    u32 f44;
};
extern struct S0209ee90 data_0209ee90;

/* The game's start-up, run once from main before the frame loop
   (func_020197b8). On a download-play child (boot type 2 at 0x27ffc40) it
   leaves out func_0201fec8 (which reads the owner's language and starts the
   link through func_0203db64), the file system (func_02018aa4) and
   func_020134c8; the boot job func_0201a2f8 runs those three later for a
   child. The /BUILDTIME read (func_0203ad84) is the one step a child never
   makes, and it masks the card IREQ interrupt (bit 20). Every console
   calls the save setup (func_02042f68) and sets up the game heap, the
   display (func_02019440), the actor spawn table and the boot scene. */
void func_0201a054(void) {
    int isChild = (*(u16 *)0x27ffc40 == 2);
    func_020427f8();
    func_02042784();
    func_02053a8c();
    func_02053c40();
    PXI_Init();
    func_0203d740();
    func_02013e64();
    func_0201a4e4();
    IRQ::EnableIRQs(1);
    {
        /* REG_IME = 1, NitroSDK's OS_EnableIrq inlined */
        volatile u16 *p = (volatile u16 *)0x4000208;
        u16 old = *p;
        *p = 1;
    }
    IRQ::Enable();
    func_02053be0(1);
    func_0203bbc0();
    func_0203bb5c();
    if (!isChild)
        func_0201fec8();
    func_02042f68(0xd01, data_0208ee50); /* 0xd01: a 64-Kbit EEPROM */
    if (!isChild)
        func_02018aa4();
    if (isChild)
        IRQ::DisableIRQs(0x100000);
    if (!isChild)
        func_0203ad84();
    func_0201a9fc(data_0209d574);
    mHeap::Heap_t::InitializeGameHeap(0x3b000, 0);
    func_0201a9fc(data_0209d574);
    func_02019440();
    if (!isChild)
        func_020134c8();
    func_0203b684();
    data_020a4bb8 = ACTOR_SPAWN_TABLE;
    func_020233f0();
    dScene_c::PrepareToSpawnBoot();
    func_020196cc();
    data_0209ee90.f44 = 0x100;
    *(volatile u32 *)0x40004c8 = 0x296a5800;
    *(volatile u32 *)0x40004cc = 0x7fff;
    *(volatile u32 *)0x40004c0 = 0x7fff;
    *(volatile u32 *)0x40004c4 = 0;
    func_0201a5cc();
}
}
