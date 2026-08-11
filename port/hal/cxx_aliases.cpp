// Gate-10 linkage aliases: C++-mangled references -> C-named definitions.
//
// Slice .cpp TUs that declare externs without extern "C" emit MSVC
// manglings for what are C-named symbols everywhere else. The linker
// alias closes the gap without touching src/. Decorated names are
// lifted verbatim from the link log; each maps to its cdecl C name.
#include <string.h>

#pragma comment(linker, "/alternatename:?FUN_02029a68@@YAXXZ=_FUN_02029a68")
#pragma comment(linker, "/alternatename:?_ZN6Player11ChangeStateERNS_5StateE@@YAXPAUPlayer@@PAUState@@@Z=__ZN6Player11ChangeStateERNS_5StateE")
#pragma comment(linker, "/alternatename:?_ZN6Player11ChangeStateERNS_5StateE@@YAXPAXPAUState@@@Z=__ZN6Player11ChangeStateERNS_5StateE")
#pragma comment(linker, "/alternatename:?_ZN6Player4HealEi@@YAXPAUPlayer@@H@Z=__ZN6Player4HealEi")
#pragma comment(linker, "/alternatename:?_ZN6Player7IsStateERNS_5StateE@@YAHPAXPAUState@@@Z=__ZN6Player7IsStateERNS_5StateE")
#pragma comment(linker, "/alternatename:?_ZN9Animation8GetFlagsEv@@YAHPAX@Z=__ZN9Animation8GetFlagsEv")
#pragma comment(linker, "/alternatename:?_ZNK9Animation12WillHitFrameEi@@YAHPAXH@Z=__ZNK9Animation12WillHitFrameEi")
#pragma comment(linker, "/alternatename:?data_0208e6ec@@3PAHA=_data_0208e6ec")
#pragma comment(linker, "/alternatename:?data_02092144@@3PAFA=_data_02092144")
#pragma comment(linker, "/alternatename:?data_020992a4@@3PAXA=_data_020992a4")
#pragma comment(linker, "/alternatename:?data_020992b4@@3PAXA=_data_020992b4")
#pragma comment(linker, "/alternatename:?data_0209b0c8@@3HA=_data_0209b0c8")
#pragma comment(linker, "/alternatename:?data_0209f49c@@3PADA=_data_0209f49c")
#pragma comment(linker, "/alternatename:?data_0209f49e@@3PAEA=_data_0209f49e")
#pragma comment(linker, "/alternatename:?data_0209f4ab@@3PAEA=_data_0209f4ab")
#pragma comment(linker, "/alternatename:?data_ov002_0211073c@@3PAHA=_data_ov002_0211073c")
#pragma comment(linker, "/alternatename:?data_ov002_021103f4@@3DA=_data_ov002_021103f4")
#pragma comment(linker, "/alternatename:?data_ov002_021101fc@@3PAHA=_data_ov002_021101fc")
#pragma comment(linker, "/alternatename:?func_ov002_020dba0c@@YAXPAX@Z=_func_ov002_020dba0c")
#pragma comment(linker, "/alternatename:?data_020a0e40@@3EA=_data_020a0e40")
#pragma comment(linker, "/alternatename:?data_020a0e40@@3PAEA=_data_020a0e40")
#pragma comment(linker, "/alternatename:?data_ov002_020ff1b0@@3PAHA=_data_ov002_020ff1b0")
#pragma comment(linker, "/alternatename:?data_ov002_020ff480@@3PAHA=_data_ov002_020ff480")
#pragma comment(linker, "/alternatename:?data_ov002_02110034@@3UState@@A=_data_ov002_02110034")
#pragma comment(linker, "/alternatename:?data_ov002_0211010c@@3UState@@A=_data_ov002_0211010c")
#pragma comment(linker, "/alternatename:?data_ov002_02110124@@3UState@@A=_data_ov002_02110124")
#pragma comment(linker, "/alternatename:?data_ov002_0211019c@@3PAHA=_data_ov002_0211019c")
#pragma comment(linker, "/alternatename:?data_ov002_0211019c@@3UState@@A=_data_ov002_0211019c")
#pragma comment(linker, "/alternatename:?data_ov002_021101e4@@3UState@@A=_data_ov002_021101e4")
#pragma comment(linker, "/alternatename:?data_ov002_0211022c@@3UState@@A=_data_ov002_0211022c")
#pragma comment(linker, "/alternatename:?data_ov002_0211049c@@3PAHA=_data_ov002_0211049c")
#pragma comment(linker, "/alternatename:?data_ov002_0211052c@@3UState@@A=_data_ov002_0211052c")
#pragma comment(linker, "/alternatename:?data_ov002_0211055c@@3UState@@A=_data_ov002_0211055c")
#pragma comment(linker, "/alternatename:?data_ov002_02110574@@3UState@@A=_data_ov002_02110574")
#pragma comment(linker, "/alternatename:?data_ov002_0211058c@@3HA=_data_ov002_0211058c")
#pragma comment(linker, "/alternatename:?data_ov002_021105bc@@3UState@@A=_data_ov002_021105bc")
#pragma comment(linker, "/alternatename:?data_ov002_021105d4@@3UState@@A=_data_ov002_021105d4")
#pragma comment(linker, "/alternatename:?func_020089f8@@YAXPAX@Z=_func_020089f8")
#pragma comment(linker, "/alternatename:?func_0200cae4@@YAHPAX@Z=_func_0200cae4")
#pragma comment(linker, "/alternatename:?func_ov002_020bcdf0@@YAXPAX@Z=_func_ov002_020bcdf0")
#pragma comment(linker, "/alternatename:?func_ov002_020c6adc@@YAHPAX@Z=_func_ov002_020c6adc")
#pragma comment(linker, "/alternatename:?func_ov002_020d2da0@@YAHPAX@Z=_func_ov002_020d2da0")
#pragma comment(linker, "/alternatename:?func_ov002_020d2e74@@YAXPAX@Z=_func_ov002_020d2e74")
#pragma comment(linker, "/alternatename:?func_ov002_020d2f24@@YAXPAX@Z=_func_ov002_020d2f24")
#pragma comment(linker, "/alternatename:?func_ov002_020d2fdc@@YAHPAX@Z=_func_ov002_020d2fdc")
#pragma comment(linker, "/alternatename:?func_ov002_020d3b9c@@YAHPAX@Z=_func_ov002_020d3b9c")
#pragma comment(linker, "/alternatename:?func_ov002_020d413c@@YAXPAXF@Z=_func_ov002_020d413c")
#pragma comment(linker, "/alternatename:?func_ov002_020d45c0@@YAXPAX@Z=_func_ov002_020d45c0")
#pragma comment(linker, "/alternatename:?func_ov002_020d4748@@YAXPAX@Z=_func_ov002_020d4748")
#pragma comment(linker, "/alternatename:?func_ov002_020e6780@@YAXPAD@Z=_func_ov002_020e6780")
#pragma comment(linker, "/alternatename:?Disable@IRQ@@SAIXZ=__ZN3IRQ7DisableEv")
#pragma comment(linker, "/alternatename:?Restore@IRQ@@SAXI@Z=__ZN3IRQ7RestoreEj")

extern "C" {
/* BSS the aliased data references land on (non-ov002 ring) */
int data_0208e6ec[4]; short data_02092144[4];
void *data_020992a4[4], *data_020992b4[4];
/* data_0209b0c8 moved to hal/camera_states.cpp (camera State object 12) */

}

namespace cstd { int strcmp(const char *a, const char *b);
                char *strchr(const char *s, char ch); }
extern "C" {
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN4cstd6strcmpEPKcS1_(const char *a, const char *b)
{ return cstd::strcmp(a, b); }
#else
int _ZN4cstd6strcmpEPKcS1_(const char *a, const char *b);  /* Linux: real symbol from src/_ZN4cstd6strcmpEPKcS1_ */
#endif /* _WIN32 */
char *_ZN4cstd6strchrEPKcc(const char *s, char ch)
{ return cstd::strchr(s, ch); }

/* PORT_HOST_ABI: ARM asm primitive (halfword copy loop), MSVC cannot assemble.
   MultiCopyHalf: halfword copy loop, (src, dst, byteCount) in r0-r2 */
void MultiCopyHalf(unsigned short *src, unsigned short *dst, unsigned n)
{
    for (unsigned i = 0; i < n; i += 2)
        *(unsigned short *)((char *)dst + i) = *(unsigned short *)((char *)src + i);
}

/* PORT_HOST_ABI: ARM/Thumb asm primitives (matrix builders), MSVC cannot assemble.
   Thumb matrix builders (asm primitives; semantics from their headers):
   4x3 fx32 rotation matrices from (sin, cos), 4096 = 1.0 */
void func_02052800(int *m, int s, int c)   /* X rotation */
{
    m[0] = 0x1000; m[1] = 0; m[2] = 0;
    m[3] = 0; m[4] = c; m[5] = s;
    m[6] = 0; m[7] = -s; m[8] = c;
    m[9] = 0; m[10] = 0; m[11] = 0;
}
/* PORT_HOST_ABI: ARM/Thumb asm primitive (Z-rotation matrix), MSVC cannot assemble. */
void func_0205283c(int *m, int s, int c)   /* Z rotation */
{
    m[0] = c; m[1] = s; m[2] = 0;
    m[3] = -s; m[4] = c; m[5] = 0;
    m[6] = 0; m[7] = 0; m[8] = 0x1000;
    m[9] = 0; m[10] = 0; m[11] = 0;
}

/* PORT_HOST_ABI: ARM asm veneer (ldr/bx tail-call), MSVC cannot assemble.
   asm veneer func_02059824 just tail-calls its C body */
void func_02059834(void);
void func_02059824(void) { func_02059834(); }

/* ITCM soft-float library, explicit call sites only (implicit double ops
   compile to native FP on host). Semantics fixed by the callers:
   the double formatter zero-tests with 9d40, negates via 8e10(0.0, v),
   and extracts digits through 859c. */
int func_01ff9d40(double x, double y) { return x == y; }
double func_01ff8708(double x, double y) { return x * y; }  /* dmul (frexp) */
/* single-precision ITCM pair cstd::atan2 leans on: i2f then float-compare */
int func_01ffa4bc(int a) { float f = (float)a; int b; memcpy(&b, &f, 4); return b; }
int func_01ff98f4(int a, int b)
{ float x, y; memcpy(&x, &a, 4); memcpy(&y, &b, 4); return x < y; }
/* float greater-than: the slide friction gate (i2f(speed) > 48.0f) */
int func_01ff99a4(int a, int b)
{ float x, y; memcpy(&x, &a, 4); memcpy(&y, &b, 4); return x > y; }
/* f2i truncation; one caller's two-arg decl is an r1 ride-through */
int func_01ffa344(int a, int b)
{ float x; memcpy(&x, &a, 4); (void)b; return (int)x; }
/* ITCM signed divide (walk-speed scaling) */
int func_01ffabe4(int a, int b) { return b ? a / b : 0; }
/* data_020994e0, cstd::atan2's own table, IS NOT HERE ANY MORE. It was
   storage with a guess for a comment ("the boot builds it at runtime") and
   nothing ever filled it, so atan2 returned 0 for every direction that was
   not exactly on an axis -- and every heading in the game goes through
   atan2. The camera could not hold a rotation, Vec3_HorzAngle answered 0
   for Mario, and slopes read as flat. The table is plain arm9 .data at
   0x020994e0 (0x804 bytes, atan(i/1024) in binangs for i = 0..0x400), well
   below bss_start 0x0209b000, so it comes out of the ROM image with the
   rest: port/tools/romdata.py NAMED. */
double func_01ff8e10(double x, double y) { return x - y; }
unsigned long long func_01ff859c(double x) { return (unsigned long long)x; }

/* ITCM soft-float compare: double(a:b) < double(c:d), EABI r0=low word */
int func_01ff9e2c(unsigned a, unsigned b, unsigned c, unsigned d)
{
    double x, y;
    unsigned long long xb = ((unsigned long long)b << 32) | a;
    unsigned long long yb = ((unsigned long long)d << 32) | c;
    memcpy(&x, &xb, 8);
    memcpy(&y, &yb, 8);
    return x < y;
}

/* PORT_HOST_ABI: reads the ARM CPSR mode bits, no host equivalent.
   ARMProcessorMode reads CPSR & 0x1f; host always reports system mode */
int ARMProcessorMode(void) { return 0x1f; }

/* DS thread scheduler context ops. The port runs the game on ONE fiber (the
   ntr rt loop owns real scheduling), so a save reports "already resumed"
   (setjmp-nonzero) and the reschedule path backs out without switching.
   A restore reaching the host would mean a second DS thread went live.
   PORT_HOST_ABI: ARM register context save/restore, no host equivalent. */
int ARMSaveContext(void *ctx) { (void)ctx; return 1; }
/* PORT_HOST_ABI: ARM register context restore (second DS thread), no host equivalent. */
void ARMRestoreContext(void *ctx) { (void)ctx; __debugbreak(); }

/* PORT_HOST_ABI: ARM asm primitive (hand-asm digit-carry), MSVC cannot assemble.
   func_02071644 (hand-asm): backward digit-carry increment over the decimal
   buffer at obj+5; overflow at the first digit writes 1 and bumps the s16
   exponent at obj+2. */
void func_02071644(unsigned char *obj, int len)
{
    unsigned char *first = obj + 5;
    unsigned char *p = first + len - 1;
    for (;;) {
        if (*p < 9) { *p += 1; return; }
        if (p != first) { *p = 0; --p; continue; }
        *p = 1;
        *(short *)(obj + 2) += 1;
        return;
    }
}

/* C-linkage face of Animation::WillHitFrame (C++ face lives in
   player_bridges; one TU cannot name both linkages) */
int hal_anim_willhit(void *self, int f);
#ifdef _WIN32
int _ZNK9Animation12WillHitFrameEi(void *self, int f)
{ return hal_anim_willhit(self, f); }
#endif /* _WIN32: this face + player_bridges' both define _ZNK9Animation12WillHitFrameEi,
   and on GCC hal_anim_willhit's C++-method call resolves to that same symbol, so the
   two mutually recurse. The real impl is src/_ZNK9Animation12WillHitFrameEi.cpp (gate10);
   Linux binds callers straight to it. */

/* SharedFilePtr construct veneers: on the DS these pass fileID in r1
   through a tail call the C decl never names (the ride-through catalog).
   Host spells out both args and routes to the HAL Construct. */
void *_ZN13SharedFilePtr9ConstructEj(void *self, unsigned id);
/* PORT_HOST_ABI: ARM r1 fileID ride-through into SharedFilePtr::Construct. */
int func_02017acc(void *self, unsigned id)
{ _ZN13SharedFilePtr9ConstructEj(self, id); return (int)self; }
/* PORT_HOST_ABI: ARM r1 fileID ride-through into SharedFilePtr::Construct. */
int SharedFilePtr_Construct_TexSeq(void *self, unsigned id)
{ _ZN13SharedFilePtr9ConstructEj(self, id); return (int)self; }
/* PORT_HOST_ABI: fileptr dtor veneer; host card seam does not refcount. */
int func_02017ab4(int x) { return x; }   /* static-dtor veneer: no-op */
/* PORT_HOST_ABI: ARM r1 fileID ride-through into SharedFilePtr::Construct. */
int func_02017b4c(void *self, unsigned id)
{ _ZN13SharedFilePtr9ConstructEj(self, id); return (int)self; }
/* PORT_HOST_ABI: ARM r1 fileID ride-through, gate 50 (ov080's PAINTING). The
   third ov080 sinit constructs its SharedFilePtrs through func_020178cc, the
   same one-arg veneer chain as func_02017acc (both end at func_02017e0c), so
   the same host spell-out serves it. func_020178b4 is the matching dtor-chain
   callback the sinit registers by address -- a host no-op like func_02017ab4,
   because the card seam does not refcount. */
int func_020178cc(void *self, unsigned id)
{ _ZN13SharedFilePtr9ConstructEj(self, id); return (int)self; }
/* PORT_HOST_ABI: fileptr dtor callback the third ov080 sinit registers by
   address; host card seam does not refcount, so the body is a no-op. */
int func_020178b4(int x) { return x; }   /* fileptr dtor callback: host no-op */
/* PORT_HOST_ABI: fileptr dtor body; host card seam does not refcount. */
int func_02017e34(int x) { return x; }   /* fileptr dtor body: host no-op */
/* PORT_HOST_ABI: fileptr dtor veneer; host card seam does not refcount. */
void SharedFilePtr_Destruct_TexSeq(void) {}
/* PORT_HOST_ABI: fileptr dtor veneer; host card seam does not refcount. */
void SharedFilePtr_Destruct_Anim(void) {}
void *data_020aa3f0;                     /* MSL global-dtor chain head */

/* PORT_HOST_ABI: the OBJECT-message box-open ride-through.
 *
 * St_Talk_Main opens a sign/NPC box (mStateWork==0) by calling
 * func_0201fc88(mAttachOffsetY) -- the raw object-message id. On the DS that
 * 0x24-byte function leaves the id in r0 across a call to
 * ObjectMessageIDToActualMessageID, which reads r0 and remaps the object id to
 * a real text id, then tail-calls func_0201f32c(text id) to raise the box. The
 * matched src func_0201fc88.c spells BOTH callees argumentless to mirror that
 * ride-through, so under MSVC the id is dropped: ObjectMessageIDToActualMessageID
 * reads a stale register, remaps garbage, and the box opens on an invalid id
 * and never activates (data_0209d660 stays 0). The plain-text path
 * (func_0201f32c, mStateWork==1) that the message probe uses is unaffected,
 * which is why the probe's box shows and a real sign's never did. This host
 * veneer spells the id through both calls; src/func_0201fc88.c is dropped from
 * slice_gate10 so this definition of _func_0201fc88 is the one that links, and
 * the existing /alternatename maps St_Talk_Main's mangled call onto it. */
extern "C" int ObjectMessageIDToActualMessageID(int id);
extern "C" void func_0201f32c(int msgID);
extern "C" void func_0201fc88(short id)
{
    func_0201f32c((short)ObjectMessageIDToActualMessageID(id));
}

/* PORT_HOST_ABI: SDK memset asm primitive (func_0205a588) -- the edge-preserving
   RMW byte-fill the FS/decompress path uses. No C to compile under MSVC, so the
   port spells it as memset, which is what its asm computes. gate 51 (the iron
   ball's shatter path reaches it). */
extern "C" void func_0205a588(void *p, int v, int n) { memset(p, v, n); }

/* gate 51: the ROLLING_IRON_BALL's state TUs call func_020ad660 by its
   un-prefixed name; the definition is the ov002-prefixed func_ov002_020ad660.
   Same cdecl symbol, so a plain alias. */
#pragma comment(linker, "/alternatename:_func_020ad660=_func_ov002_020ad660")

/* PORT_HOST_ABI: soft reset -- ARM7 IPC, card reload, unmapped 0x27ffc40 read.
   crash-screen-only ITCM entry; trap keeps it honest if ever reached */
void func_01ffdd98(int a) { (void)a; __debugbreak(); }

int data_0209cdcc, data_0209cde4[4], data_0209cde8[4];
int data_020a4b4c, data_020a4b50;
int data_020a7fc0[8];
/* GX bank-state u16 band 0x020a608a..0x020a60a0. On the DS these alias one
   register-cache block (data_020a6088's GXState overlaps 608a/608c); host
   keeps them as separate objects -- fine while nothing in the slice mixes
   the struct view with the field view on the same bit of state. */
unsigned short data_020a608a, data_020a608c, data_020a608e, data_020a6090;
unsigned short data_020a6092, data_020a6094, data_020a6096, data_020a6098;
unsigned short data_020a609a, data_020a609c, data_020a609e, data_020a60a0;
int data_0209a5e4, data_0209a5e8, data_020a612c[4], data_0209a438[8];
/* OS scheduler/thread BSS band 0x0209a628..0x0209a6f8 */
int data_0209a628[12], data_0209a658[10], data_0209a680[6], data_0209a698[4];
int data_0209a6a8[2], data_0209a6b0[2], data_0209a6b8[2], data_0209a6c0[2];
int data_0209a6c8, data_0209a6cc, data_0209a6d0, data_0209a6d4, data_0209a6d8;
int data_0209a6dc, data_0209a6e0, data_0209a6e4, data_0209a6e8, data_0209a6ec;
int data_0209a6f0, data_0209a6f4, data_0209a6f8;
unsigned char data_020a0e98;
/* The 32 sound-player records, stride 0x1c (func_0204f63c, func_0204f958,
   func_0204f9c4 and func_0204f504 all index it that way, and the ROM runs
   0x020a4d6c..0x020a50ec = 32 * 0x1c exactly). It was one int while sound was
   stubbed. data_020a4d84 is the SAME array seen from field +0x18 -- the
   playable-sequence limit that Stage::InitResources sets and func_0204f63c
   tests before it evicts a voice -- so hal/sdat/sound_abi.cpp hosts
   Sound::Player::SetPlayableSeqCount to write through THIS object and keep
   the two views aliased the way DS memory does. */
unsigned char data_020a4d6c[32 * 0x1c];
/* ov006 (cutscene overlay) fileptrs St_LevelEnter_Main releases; zeroed
   stand-ins -- Release guards on numRefs 0 */
unsigned char data_ov006_02140330[8], data_ov006_02140338[8];
/* data_ov089_02132894 / _021328b4 used to be sixteen zero bytes each
   here. They are ov089's own mount now (gate 22): the DOOR indexes
   element 5 of both and walked off the end of the fiction. */
int data_0209b48c, data_0209b4a0[4], data_0209b4ac;
/* The music fade engine's per-channel records: func_020490b0 runs i = 0..1
   over ALL FOUR of these (c48 the u16 channel ids, c4c the ramp rate, c54
   the target, c5c the current volume), and func_02049764 seeds [0] and [1]
   of c4c/c54/c5c on every course boot. Hosted as bare ints, c4c[1] zeroed
   data_020a4c54[0] behind it and c5c[1] wrote 0x7f000 over data_0209ee74,
   the particle tracker one word later, which is what killed the direct
   Bob-omb Battlefield boot the moment the level had a layer-1 track. The
   fade loop then read what survived at that word back as a volume every
   frame, which is the audio-corruption side of the same line. Full DS
   extent for all of them; c48 is u16[2] on the DS, one int here. */
int data_020a4c48, data_020a4c4c[2], data_020a4c54[2], data_020a4c5c[2];
/* data_0209ee74 IS A SysTracker*, not storage: every src reference spells it
   `extern char *data_0209ee74` and then indexes off its VALUE
   (func_02022a4c reads +0x774, func_02022774 writes +0x7d0..+0x7dc,
   func_02022d44 passes +0x7f0).

   THE 0x1000-BYTE STAND-IN THAT USED TO BACK IT IS GONE (gate 29). It was a
   zeroed block standing in for a tracker that did not exist, with this
   pointer initialised to it so the first effect to fire would find something
   rather than take the process with it. The tracker is real now: it is the
   Stage's own sub-object at Stage+0x50, and Particle::SysTracker::SysTracker
   publishes it here itself -- `data_0209ee74 = this` is the second-to-last
   line of src/_ZN8Particle10SysTrackerC1Ev.c. Nothing needs to seed it.

   PARTICLE_SYS_TRACKER is the SAME STORAGE, not a second object. decl_common.h
   declares it `extern char *PARTICLE_SYS_TRACKER` and
   Particle::RunningSlidingDustAt -- the Player's skid and slide dust -- reads
   its handle out of `PARTICLE_SYS_TRACKER + 0x750`; the ROM's own
   RunningSlidingDustAt loads from 0x0209ee74 for exactly that (relocs.txt
   from:0x02022bac to:0x0209ee74), so the two names are one global with two
   spellings. Aliased rather than duplicated: as a separate array it read as a
   null pointer and the first skid faulted at +0x750. */
void *data_0209ee74[4];
#pragma comment(linker, "/alternatename:_PARTICLE_SYS_TRACKER=_data_0209ee74")
int data_0209f32c[4], data_0209b4a4[4];
/* camera + player-list globals (gate-9 scoping notes) */
int data_0209d4b0[8];
int data_0209f274[8];
/* data_0209f324 (WIPES) moved to hal/fader_wipes.cpp: it is a POINTER to a
   staged array now, not blank storage, or the death path faults on it. */
int data_0209a5ec, data_0209a5f4[2], data_0209a5fc, data_0209a600;
int data_0209a604, data_0209a60c[2], data_0209a614, data_0209a618;
int data_0209a61c[4], data_020a6128, data_020a6134[4];
}

#pragma comment(linker, "/alternatename:__ZN2GX12SetBankForBGEt=?SetBankForBG@GX@@YAXG@Z")

/* Scene::ResetHardwareRegisters is defined against this exact local shadow
   in its own TU; mirror it so the manglings agree. */
struct Scene { void ResetHardwareRegisters(); };
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
extern "C" void _ZN5Scene22ResetHardwareRegistersEv(void *s)
{ ((Scene *)s)->Scene::ResetHardwareRegisters(); }
#else
extern "C" void _ZN5Scene22ResetHardwareRegistersEv(void *s);  /* Linux: real symbol from src/_ZN5Scene22ResetHardwareRegistersEv */
#endif /* _WIN32 */

#pragma comment(linker, "/alternatename:?data_020a0e98@@3EA=_data_020a0e98")
#pragma comment(linker, "/alternatename:?data_020a4d6c@@3PAEA=_data_020a4d6c")

/* A slice TU sees NestedHeapIterator::Next through a local shadow returning
   unsigned char*; the real definition returns int against the shared header.
   Mirror the shadow and hop through the C-named helper in player_bridges.
   LINUX CRITICAL: this out-of-line `NestedHeapIterator::Next` mangles to the
   SAME Itanium symbol as the real src/ method (slice_gate2) AND as
   player_bridges' hal_nhi_next target -- and hal_nhi_next calls
   `->NestedHeapIterator::Next(...)`, so on GCC this definition + hal_nhi_next
   form an infinite recursion (a stack-overflow SIGSEGV in Heap::SetupRootHeap
   during boot). On MSVC the C++ method and the extern-C forwarder mangle
   differently, so the hop is a real convention converter. On Linux the real
   src/ method already provides the symbol with the correct ABI, so omit this
   host copy and let hal_nhi_next bind straight to it. */
#ifdef _WIN32
struct HeapAllocator;
struct NestedHeapIterator { unsigned char *Next(HeapAllocator *h); };
extern "C" int hal_nhi_next(void *self, void *h);
unsigned char *NestedHeapIterator::Next(HeapAllocator *h)
{ return (unsigned char *)(size_t)hal_nhi_next(this, h); }
#endif /* _WIN32 -- Linux binds hal_nhi_next to the real src/ method */
#pragma comment(linker, "/alternatename:?GiveHealth@@YAHHH@Z=_GiveHealth")
#pragma comment(linker, "/alternatename:?data_0209caa0@@3PAHA=_data_0209caa0")
#pragma comment(linker, "/alternatename:?data_0209f2d8@@3EA=_data_0209f2d8")
#pragma comment(linker, "/alternatename:?data_0209f49c@@3GA=_data_0209f49c")
#pragma comment(linker, "/alternatename:?data_0209f4a0@@3FA=_data_0209f4a0")
#pragma comment(linker, "/alternatename:?data_0209f4ac@@3EA=_data_0209f4ac")
#pragma comment(linker, "/alternatename:?data_0209f4ae@@3EA=_data_0209f4ae")
#pragma comment(linker, "/alternatename:?data_ov002_0211010c@@3PAHA=_data_ov002_0211010c")
#pragma comment(linker, "/alternatename:?data_ov002_02110124@@3PAHA=_data_ov002_02110124")
#pragma comment(linker, "/alternatename:?data_0208e430@@3HA=_data_0208e430")
#pragma comment(linker, "/alternatename:?data_0209f32c@@3HA=_data_0209f32c")
#pragma comment(linker, "/alternatename:?func_02022d00@@YAIIIHHHPAX@Z=_func_02022d00")
#pragma comment(linker, "/alternatename:?NewSimple@System@Particle@@SAXIHHH@Z=__ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_")
/* ---- gate 29, the particle subsystem --------------------------------------
   All of these are STATIC members or namespace-scope data, so both sides are
   already cdecl and a plain alias is honest -- the four particle callbacks
   that are __thiscall METHODS are not here, they get real forwarding faces in
   hal/particle_bridges.cpp instead.

   The six Particle::*::Func are the effect VM's whole dispatch surface:
   func_0204a17c takes their addresses when it builds each definition's
   Behavior[] array, and it is a .cpp that spells them as C++ statics while
   the six defining TUs emit C names. */
#pragma comment(linker, "/alternatename:?Func@Acceleration@Particle@@SAXAAUEffectData@2@PADAAUVector3@@@Z=__ZN8Particle12Acceleration4FuncERNS_10EffectDataEPcR7Vector3")
#pragma comment(linker, "/alternatename:?Func@Jitter@Particle@@SAXAAUEffectData@2@PADAAUVector3@@@Z=__ZN8Particle6Jitter4FuncERNS_10EffectDataEPcR7Vector3")
#pragma comment(linker, "/alternatename:?Func@Converge@Particle@@SAXAAUEffectData@2@PADAAUVector3@@@Z=__ZN8Particle8Converge4FuncERNS_10EffectDataEPcR7Vector3")
#pragma comment(linker, "/alternatename:?Func@Turn@Particle@@SAXAAUEffectData@2@PADAAUVector3@@@Z=__ZN8Particle4Turn4FuncERNS_10EffectDataEPcR7Vector3")
#pragma comment(linker, "/alternatename:?Func@LimitPlane@Particle@@SAXAAUEffectData@2@PADAAUVector3@@@Z=__ZN8Particle10LimitPlane4FuncERNS_10EffectDataEPcR7Vector3")
#pragma comment(linker, "/alternatename:?Func@RadiusConverge@Particle@@SAXAAUEffectData@2@PADAAUVector3@@@Z=__ZN8Particle14RadiusConverge4FuncERNS_10EffectDataEPcR7Vector3")
/* the C++ face of the spawn entry, reached from ov002 actor code */
#pragma comment(linker, "/alternatename:?New@System@Particle@@SAXIIHHHPBUVector3_16@@PAUCallback@2@@Z=__ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE")
/* the engine's globals, spelled without extern "C" by the .cpp TUs that
   touch them: the tracker pointer, the two VRAM cursors, the default heap */
#pragma comment(linker, "/alternatename:?data_0209ee74@@3PAUSomeGlobal@@A=_data_0209ee74")
#pragma comment(linker, "/alternatename:?data_0209ee74@@3PAXA=_data_0209ee74")
#pragma comment(linker, "/alternatename:?data_0209ee84@@3HA=_data_0209ee84")
#pragma comment(linker, "/alternatename:?data_0209ee8c@@3HA=_data_0209ee8c")
#pragma comment(linker, "/alternatename:?data_020a0ea0@@3PAXA=_data_020a0ea0")
#pragma comment(linker, "/alternatename:?PlayBank0@Sound@@SAXIABUVector3@@@Z=__ZN5Sound9PlayBank0EjRK7Vector3")
#pragma comment(linker, "/alternatename:?data_02082214@@3PAFA=_data_02082214")
#pragma comment(linker, "/alternatename:?data_0209f264@@3EA=_data_0209f264")
#pragma comment(linker, "/alternatename:?data_0209f2f8@@3CA=_data_0209f2f8")
#pragma comment(linker, "/alternatename:?data_0209f2fc@@3EA=_data_0209f2fc")
#pragma comment(linker, "/alternatename:?data_ov002_0210a7e8@@3PAIA=_data_ov002_0210a7e8")
#pragma comment(linker, "/alternatename:?func_ov002_020bdd2c@@YAXPAX@Z=_func_ov002_020bdd2c")

/* Sound sequence-info lookup. This returned 0 the whole time the SDAT root
   was null, which is what made func_02051fb4 give up before every music
   start. The root is real now (hal/sdat/sdat.cpp), so the MSVC-mangled face
   forwards to the matched walker in src/, which reads root+0x84 and the SEQ
   record at sub+0x08 for real. */
struct SeqEntry;
extern "C" SeqEntry *_ZN5Sound17InfoSequenceEntry9GetWithIDEj(unsigned id);
struct Sound {
    struct InfoSequenceEntry { static SeqEntry *GetWithID(unsigned id); };
};
#ifdef _WIN32 /* LINUX: this method's own Itanium mangling IS the extern-C name it forwards to -> infinite self-recurse (SIGSEGV) at level-load BGM lookup. On GCC the real src/ body owns the symbol; do not define the face. */
SeqEntry *Sound::InfoSequenceEntry::GetWithID(unsigned id)
{ return _ZN5Sound17InfoSequenceEntry9GetWithIDEj(id); }
#endif

/* Heap::_Deallocate is a DS tail-call veneer to Deallocate; operator delete
   dispatches it as a method. Same-shadow definition forwarding to the HAL
   dealloc keeps the mangling the reference expects. */
struct Heap { void _Deallocate(void *ptr); };
extern "C" void _ZN4Heap10DeallocateEPv(void *self, void *ptr);
void Heap::_Deallocate(void *ptr) { _ZN4Heap10DeallocateEPv(this, ptr); }

/* RaycastGround::DetectClsn is defined against a local shadow in its own
   TU; mirror the shadow (no real header here) so the manglings agree. */
class RaycastGround { public: int DetectClsn(); };
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
extern "C" int _ZN13RaycastGround10DetectClsnEv(void *self)
{ return ((RaycastGround *)self)->DetectClsn(); }
#else
extern "C" int _ZN13RaycastGround10DetectClsnEv(void *self);  /* Linux: real symbol from src/_ZN13RaycastGround10DetectClsnEv */
#endif /* _WIN32 */
#pragma comment(linker, "/alternatename:?data_0209f254@@3EA=_data_0209f254")
#pragma comment(linker, "/alternatename:?data_0209f4a6@@3FA=_data_0209f4a6")
#pragma comment(linker, "/alternatename:?func_ov002_020bdd9c@@YAXPAX@Z=_func_ov002_020bdd9c")
#pragma comment(linker, "/alternatename:?func_ov002_020bdef0@@YAXPAX@Z=_func_ov002_020bdef0")
#pragma comment(linker, "/alternatename:?func_ov002_020bf13c@@YAXPAX@Z=_func_ov002_020bf13c")
#pragma comment(linker, "/alternatename:?func_ov002_020bf36c@@YAXPAX0@Z=_func_ov002_020bf36c")
#pragma comment(linker, "/alternatename:?func_ov002_020c2db8@@YAXPAX@Z=_func_ov002_020c2db8")
#pragma comment(linker, "/alternatename:?func_ov002_020c2e78@@YAXPAX@Z=_func_ov002_020c2e78")
#pragma comment(linker, "/alternatename:?func_ov002_020c4188@@YAHPAX@Z=_func_ov002_020c4188")
#pragma comment(linker, "/alternatename:?func_ov002_020ca940@@YAXPAX@Z=_func_ov002_020ca940")
#pragma comment(linker, "/alternatename:?func_ov002_020d8158@@YAXPAX@Z=_func_ov002_020d8158")
#pragma comment(linker, "/alternatename:?func_ov002_020d869c@@YAXPAX@Z=_func_ov002_020d869c")
#pragma comment(linker, "/alternatename:?func_ov002_020db704@@YAXPAX@Z=_func_ov002_020db704")
#pragma comment(linker, "/alternatename:?func_ov002_020e032c@@YAXPAX@Z=_func_ov002_020e032c")
#pragma comment(linker, "/alternatename:?func_ov002_020e4bb8@@YAXPAX@Z=_func_ov002_020e4bb8")
#pragma comment(linker, "/alternatename:?data_0209cee8@@3PAXA=_data_0209cee8")
#pragma comment(linker, "/alternatename:?data_0209b49c@@3HA=_data_0209b49c")
#pragma comment(linker, "/alternatename:?data_ov002_02110094@@3DA=_data_ov002_02110094")
#pragma comment(linker, "/alternatename:?data_ov002_0211013c@@3UState@@A=_data_ov002_0211013c")
#pragma comment(linker, "/alternatename:?data_ov002_021101b4@@3UState@@A=_data_ov002_021101b4")
#pragma comment(linker, "/alternatename:?data_ov002_0211034c@@3DA=_data_ov002_0211034c")
#pragma comment(linker, "/alternatename:?AngleDiff@@YAHHH@Z=_AngleDiff")
#pragma comment(linker, "/alternatename:?IsButtonInputValid@@YAHXZ=_IsButtonInputValid")
#pragma comment(linker, "/alternatename:?_ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii@@YAPAXHPAXHHHHHHHH@Z=__ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii")
#pragma comment(linker, "/alternatename:?_ZN5Model14SetPolygonModeEi@@YAHPAXH@Z=__ZN5Model14SetPolygonModeEi")
#pragma comment(linker, "/alternatename:?_ZNK6Player14GetBodyModelIDEjb@@YAHPAXIH@Z=__ZNK6Player14GetBodyModelIDEjb")
#pragma comment(linker, "/alternatename:?data_0208ee44@@3HA=_data_0208ee44")
#pragma comment(linker, "/alternatename:?data_0209d650@@3EA=_data_0209d650")
#pragma comment(linker, "/alternatename:?data_0209d65c@@3CA=_data_0209d65c")
#pragma comment(linker, "/alternatename:?data_0209d66c@@3EA=_data_0209d66c")
#pragma comment(linker, "/alternatename:?data_0209d670@@3EA=_data_0209d670")
#pragma comment(linker, "/alternatename:?data_0209d684@@3EA=_data_0209d684")
#pragma comment(linker, "/alternatename:?data_0209d688@@3EA=_data_0209d688")
#pragma comment(linker, "/alternatename:?data_0209d68c@@3EA=_data_0209d68c")
#pragma comment(linker, "/alternatename:?data_0209d69c@@3EA=_data_0209d69c")
#pragma comment(linker, "/alternatename:?data_0209d6a0@@3EA=_data_0209d6a0")
#pragma comment(linker, "/alternatename:?data_0209d6a8@@3EA=_data_0209d6a8")
#pragma comment(linker, "/alternatename:?data_0209d6b0@@3EA=_data_0209d6b0")
#pragma comment(linker, "/alternatename:?data_0209d6b4@@3EA=_data_0209d6b4")
#pragma comment(linker, "/alternatename:?data_0209d6bc@@3EA=_data_0209d6bc")
#pragma comment(linker, "/alternatename:?data_0209d6c4@@3EA=_data_0209d6c4")
#pragma comment(linker, "/alternatename:?data_0209d6c8@@3EA=_data_0209d6c8")
#pragma comment(linker, "/alternatename:?data_0209d6cc@@3EA=_data_0209d6cc")
#pragma comment(linker, "/alternatename:?data_0209d6d0@@3EA=_data_0209d6d0")
#pragma comment(linker, "/alternatename:?data_0209d6d8@@3FA=_data_0209d6d8")
#pragma comment(linker, "/alternatename:?data_0209d6dc@@3FA=_data_0209d6dc")
#pragma comment(linker, "/alternatename:?data_0209d6e4@@3PAEA=_data_0209d6e4")
#pragma comment(linker, "/alternatename:?data_0209d6f0@@3PAUStruct6f0@@A=_data_0209d6f0")
#pragma comment(linker, "/alternatename:?data_0209d6f4@@3HA=_data_0209d6f4")
#pragma comment(linker, "/alternatename:?data_0209f4a2@@3EA=_data_0209f4a2")
#pragma comment(linker, "/alternatename:?data_0209f4a4@@3EA=_data_0209f4a4")
#pragma comment(linker, "/alternatename:?data_020a0c80@@3PAPAXA=_data_020a0c80")
#pragma comment(linker, "/alternatename:?data_020a0db0@@3HA=_data_020a0db0")
#pragma comment(linker, "/alternatename:?data_020a0de8@@3PAEA=_data_020a0de8")
#pragma comment(linker, "/alternatename:?data_020a0de9@@3PAEA=_data_020a0de9")
#pragma comment(linker, "/alternatename:?data_020a0deb@@3PAEA=_data_020a0deb")
#pragma comment(linker, "/alternatename:?data_020a0e5a@@3EA=_data_020a0e5a")
#pragma comment(linker, "/alternatename:?data_ov002_020ff128@@3PAGA=_data_ov002_020ff128")
#pragma comment(linker, "/alternatename:?data_ov002_0210c390@@3EA=_data_ov002_0210c390")
#pragma comment(linker, "/alternatename:?data_ov002_0210c398@@3EA=_data_ov002_0210c398")
#pragma comment(linker, "/alternatename:?data_ov002_0210c3a0@@3EA=_data_ov002_0210c3a0")
#pragma comment(linker, "/alternatename:?data_ov002_0210c3a8@@3EA=_data_ov002_0210c3a8")
#pragma comment(linker, "/alternatename:?data_ov002_0210ffec@@3UState@@A=_data_ov002_0210ffec")
#pragma comment(linker, "/alternatename:?data_ov002_0211001c@@3UState@@A=_data_ov002_0211001c")
#pragma comment(linker, "/alternatename:?data_ov002_021100ac@@3HA=_data_ov002_021100ac")
#pragma comment(linker, "/alternatename:?data_ov002_02110574@@3DA=_data_ov002_02110574")
#pragma comment(linker, "/alternatename:?data_ov002_021105a4@@3UState@@A=_data_ov002_021105a4")
#pragma comment(linker, "/alternatename:?data_ov002_021105bc@@3DA=_data_ov002_021105bc")
#pragma comment(linker, "/alternatename:?data_ov002_0211067c@@3DA=_data_ov002_0211067c")
#pragma comment(linker, "/alternatename:?func_02012790@@YAXH@Z=_func_02012790")
#pragma comment(linker, "/alternatename:?func_02014fa4@@YAXPAD@Z=_func_02014fa4")
#pragma comment(linker, "/alternatename:?func_0201adfc@@YAXXZ=_func_0201adfc")
#pragma comment(linker, "/alternatename:?func_0201b388@@YAXH@Z=_func_0201b388")
#pragma comment(linker, "/alternatename:?func_0201b6f8@@YAXH@Z=_func_0201b6f8")
#pragma comment(linker, "/alternatename:?func_0201b7cc@@YAPAXXZ=_func_0201b7cc")
#pragma comment(linker, "/alternatename:?func_02059650@@YA_JXZ=_func_02059650")
#pragma comment(linker, "/alternatename:?func_ov002_020d82f0@@YAHPAX@Z=_func_ov002_020d82f0")
#pragma comment(linker, "/alternatename:?func_ov002_020d91b8@@YAXPADH@Z=_func_ov002_020d91b8")
#pragma comment(linker, "/alternatename:?func_ov002_020e6b74@@YAHPAXH@Z=_func_ov002_020e6b74")
#pragma comment(linker, "/alternatename:?data_ov002_02110064@@3PAHA=_data_ov002_02110064")
#pragma comment(linker, "/alternatename:?data_ov002_02110094@@3UState@Player@@A=_data_ov002_02110094")
#pragma comment(linker, "/alternatename:?data_ov002_021101cc@@3UState@@A=_data_ov002_021101cc")
#pragma comment(linker, "/alternatename:?data_ov002_02110604@@3UState@@A=_data_ov002_02110604")
#pragma comment(linker, "/alternatename:?GetSoundMode@@YAHXZ=_GetSoundMode")
#pragma comment(linker, "/alternatename:?SetSoundMode@@YAXH@Z=_SetSoundMode")
#pragma comment(linker, "/alternatename:?data_02088fb8@@3HA=_data_02088fb8")
#pragma comment(linker, "/alternatename:?data_020890a0@@3HA=_data_020890a0")
#pragma comment(linker, "/alternatename:?data_0209b270@@3EA=_data_0209b270")
#pragma comment(linker, "/alternatename:?data_0209b284@@3PAIA=_data_0209b284")
#pragma comment(linker, "/alternatename:?data_0209b2a4@@3PAIA=_data_0209b2a4")
#pragma comment(linker, "/alternatename:?data_0209b454@@3HA=_data_0209b454")
#pragma comment(linker, "/alternatename:?data_0209f21c@@3EA=_data_0209f21c")
#pragma comment(linker, "/alternatename:?data_0209fc48@@3HA=_data_0209fc48")
#pragma comment(linker, "/alternatename:?data_0209fc4c@@3HA=_data_0209fc4c")
#pragma comment(linker, "/alternatename:?func_02011c8c@@YAXXZ=_func_02011c8c")
#pragma comment(linker, "/alternatename:?data_ov002_0210a054@@3P8C@@AEXPAEHH@ZQ1@=_data_ov002_0210a054")
#pragma comment(linker, "/alternatename:?data_ov002_0210a064@@3P8C@@AEXPAEHH@ZQ1@=_data_ov002_0210a064")
#pragma comment(linker, "/alternatename:?data_ov002_0210a094@@3P8C@@AEXPAEHH@ZQ1@=_data_ov002_0210a094")
#pragma comment(linker, "/alternatename:?data_ov002_0210a0b4@@3P8C@@AEXPAEHH@ZQ1@=_data_ov002_0210a0b4")
#pragma comment(linker, "/alternatename:?data_ov002_0210a0dc@@3P8C@@AEXPAEHH@ZQ1@=_data_ov002_0210a0dc")
#pragma comment(linker, "/alternatename:?data_ov002_0210a124@@3P8C@@AEXPAEHH@ZQ1@=_data_ov002_0210a124")
#pragma comment(linker, "/alternatename:?data_ov002_0210a14c@@3P8C@@AEXPAEHH@ZQ1@=_data_ov002_0210a14c")
#pragma comment(linker, "/alternatename:?data_ov002_0210a36c@@3P8C@@AEXPAEHH@ZQ1@=_data_ov002_0210a36c")
#pragma comment(linker, "/alternatename:?data_ov002_0210a3c4@@3P8C@@AEXPAEHH@ZQ1@=_data_ov002_0210a3c4")
#pragma comment(linker, "/alternatename:?data_ov002_0210a3fc@@3P8C@@AEXPAEHH@ZQ1@=_data_ov002_0210a3fc")
#pragma comment(linker, "/alternatename:?data_ov002_0210a40c@@3P8C@@AEXPAEHH@ZQ1@=_data_ov002_0210a40c")
#pragma comment(linker, "/alternatename:?data_ov002_0210a44c@@3P8C@@AEXPAEHH@ZQ1@=_data_ov002_0210a44c")
#pragma comment(linker, "/alternatename:?data_ov002_0210a474@@3P8C@@AEXPAEHH@ZQ1@=_data_ov002_0210a474")
#pragma comment(linker, "/alternatename:?data_ov002_0210a534@@3P8C@@AEXPAEHH@ZQ1@=_data_ov002_0210a534")
#pragma comment(linker, "/alternatename:?Player_AdvanceAnims@@YAHPAX@Z=_Player_AdvanceAnims")
#pragma comment(linker, "/alternatename:?Player_ScaleByCharFactor@@YAHPAXH@Z=_Player_ScaleByCharFactor")
#pragma comment(linker, "/alternatename:?_ZN12MeshCollider7SetFileEP8KCL_FileR10CLPS_Block@@YAXPAX00@Z=__ZN12MeshCollider7SetFileEP8KCL_FileR10CLPS_Block")
#pragma comment(linker, "/alternatename:?_ZN12MeshCollider8LoadFileER13SharedFilePtr@@YAPAXPAX@Z=__ZN12MeshCollider8LoadFileER13SharedFilePtr")
#pragma comment(linker, "/alternatename:?_ZN12MeshColliderC1Ev@@YAXPAX@Z=__ZN12MeshColliderC1Ev")
#pragma comment(linker, "/alternatename:?_ZN13SharedFilePtr9ConstructEj@@YAPAXPAXI@Z=__ZN13SharedFilePtr9ConstructEj")
#pragma comment(linker, "/alternatename:?_ZN16MeshColliderBase6EnableEP5Actor@@YAHPAX0@Z=__ZN16MeshColliderBase6EnableEP5Actor")
#pragma comment(linker, "/alternatename:?_ZN6Player11ChangeStateERNS_5StateE@@YAHPAX0@Z=__ZN6Player11ChangeStateERNS_5StateE")
#pragma comment(linker, "/alternatename:?_ZN6Player6IsAnimEj@@YAHPAXI@Z=__ZN6Player6IsAnimEj")
#pragma comment(linker, "/alternatename:?_ZN6Player7SetAnimEji5Fix12IiEj@@YAHPAXIHHI@Z=__ZN6Player7SetAnimEji5Fix12IiEj")
#pragma comment(linker, "/alternatename:?data_0209f4a0@@3PADA=_data_0209f4a0")
#pragma comment(linker, "/alternatename:?data_ov002_0211007c@@3HA=_data_ov002_0211007c")
#pragma comment(linker, "/alternatename:?data_ov002_0211019c@@3HA=_data_ov002_0211019c")
#pragma comment(linker, "/alternatename:?data_ov002_021101b4@@3HA=_data_ov002_021101b4")
#pragma comment(linker, "/alternatename:?data_ov002_021101e4@@3HA=_data_ov002_021101e4")
#pragma comment(linker, "/alternatename:?data_ov002_02110424@@3PADA=_data_ov002_02110424")
#pragma comment(linker, "/alternatename:?data_ov002_02110454@@3HA=_data_ov002_02110454")
#pragma comment(linker, "/alternatename:?data_ov002_0211052c@@3HA=_data_ov002_0211052c")
#pragma comment(linker, "/alternatename:?data_ov002_0211055c@@3HA=_data_ov002_0211055c")
#pragma comment(linker, "/alternatename:?data_ov002_0211055c@@3PAHA=_data_ov002_0211055c")
#pragma comment(linker, "/alternatename:?data_ov002_02110724@@3HA=_data_ov002_02110724")
#pragma comment(linker, "/alternatename:?func_ov002_020d5c6c@@YAHPAX@Z=_func_ov002_020d5c6c")
#pragma comment(linker, "/alternatename:?func_ov002_020dde74@@YAHPAX@Z=_func_ov002_020dde74")
#pragma comment(linker, "/alternatename:?func_ov002_020e04a4@@YAXPAX@Z=_func_ov002_020e04a4")
#pragma comment(linker, "/alternatename:?func_ov002_020e2664@@YAHPAX@Z=_func_ov002_020e2664")
#pragma comment(linker, "/alternatename:?func_ov002_020e28d4@@YAHPAXHH@Z=_func_ov002_020e28d4")
#pragma comment(linker, "/alternatename:?data_ov002_021101e4@@3UState@Player@@A=_data_ov002_021101e4")
#pragma comment(linker, "/alternatename:?data_ov002_0211040c@@3UState@Player@@A=_data_ov002_0211040c")
#pragma comment(linker, "/alternatename:?data_ov002_021105a4@@3UState@Player@@A=_data_ov002_021105a4")
#pragma comment(linker, "/alternatename:?data_ov002_021105bc@@3UState@Player@@A=_data_ov002_021105bc")

/* stale caller names -> renamed callees (the #973 class, host side) */
#pragma comment(linker, "/alternatename:_func_02037670=__ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor")
#pragma comment(linker, "/alternatename:_func_02037764=__ZN11RaycastLineD1Ev")
#pragma comment(linker, "/alternatename:_func_020377b0=__ZN11RaycastLineC1Ev")
#pragma comment(linker, "/alternatename:_func_02038638=__ZN11RaycastLine10DetectClsnEv")
#pragma comment(linker, "/alternatename:_func_0203b0e8=_AngleDiff")
#pragma comment(linker, "/alternatename:_func_0203b4dc=__ZN4cstd5atan2E5Fix12IiES1_")
#pragma comment(linker, "/alternatename:_func_0203cf78=_Vec3_HorzLen")

/* gate-13 state-family ring (sprint/crouch/punch/slope-jump wave):
   C++-mangled refs from the new St_ TUs -> C-named defs/storage */
#pragma comment(linker, "/alternatename:?data_ov002_020ff130@@3PAHA=_data_ov002_020ff130")
#pragma comment(linker, "/alternatename:?data_ov002_020ff164@@3PAHA=_data_ov002_020ff164")
#pragma comment(linker, "/alternatename:?data_ov002_021101e4@@3PAHA=_data_ov002_021101e4")
#pragma comment(linker, "/alternatename:?data_ov002_02110694@@3PAHA=_data_ov002_02110694")
#pragma comment(linker, "/alternatename:?data_ov002_021106dc@@3PAHA=_data_ov002_021106dc")
#pragma comment(linker, "/alternatename:?data_ov002_02110724@@3PAHA=_data_ov002_02110724")
#pragma comment(linker, "/alternatename:?data_ov002_0210e160@@3HA=_data_ov002_0210e160")
#pragma comment(linker, "/alternatename:?data_ov002_0211013c@@3HA=_data_ov002_0211013c")
#pragma comment(linker, "/alternatename:?data_0209f4a4@@3PAFA=_data_0209f4a4")
#pragma comment(linker, "/alternatename:?data_0209f318@@3PAUCamera@@A=_data_0209f318")
/* gate 18 (RABBIT_KEY): func_ov085_0212cd0c.cpp spells the live Camera as a
   C++ `int *` at file scope -- the fourth spelling of the same object. */
#pragma comment(linker, "/alternatename:?data_0209f318@@3PAHA=_data_0209f318")
#pragma comment(linker, "/alternatename:?func_ov002_020d1164@@YAHPAX@Z=_func_ov002_020d1164")
#pragma comment(linker, "/alternatename:?func_ov002_020d1204@@YAHPAX@Z=_func_ov002_020d1204")
#pragma comment(linker, "/alternatename:?func_ov002_020d12b0@@YAHPAX@Z=_func_ov002_020d12b0")
#pragma comment(linker, "/alternatename:?func_ov002_020dc020@@YAHPAX@Z=_func_ov002_020dc020")
#pragma comment(linker, "/alternatename:?func_ov002_020e25f0@@YAHPAXH@Z=_func_ov002_020e25f0")
#pragma comment(linker, "/alternatename:?func_ov002_020e2ad0@@YAHPAX@Z=_func_ov002_020e2ad0")
#pragma comment(linker, "/alternatename:?func_ov002_020e2b6c@@YAHPAX@Z=_func_ov002_020e2b6c")
#pragma comment(linker, "/alternatename:?func_ov002_020e2ba8@@YAHPAX@Z=_func_ov002_020e2ba8")
#pragma comment(linker, "/alternatename:?func_ov002_020e2be4@@YAHPAX@Z=_func_ov002_020e2be4")
#pragma comment(linker, "/alternatename:?func_ov002_020e2c84@@YAHPAD@Z=_func_ov002_020e2c84")
#pragma comment(linker, "/alternatename:?Player_ReleaseHeldActor@@YAHPAX@Z=_Player_ReleaseHeldActor")
#pragma comment(linker, "/alternatename:?_ZN5Sound9PlayBank0EjRK7Vector3@@YAHIPAX@Z=__ZN5Sound9PlayBank0EjRK7Vector3")
#pragma comment(linker, "/alternatename:?data_ov002_0211061c@@3UState@@A=_data_ov002_0211061c")
#pragma comment(linker, "/alternatename:?data_ov002_02110634@@3UState@@A=_data_ov002_02110634")
#pragma comment(linker, "/alternatename:?data_0209f318@@3PAXA=_data_0209f318")
#pragma comment(linker, "/alternatename:?func_0200d580@@YAXPAUCamera@@H@Z=_func_0200d580")
#pragma comment(linker, "/alternatename:?func_ov002_020cc05c@@YAXPAXG@Z=_func_ov002_020cc05c")
#pragma comment(linker, "/alternatename:?func_ov002_020dbaec@@YAXPAX@Z=_func_ov002_020dbaec")
#pragma comment(linker, "/alternatename:?func_ov002_020dd5ec@@YAXPAX@Z=_func_ov002_020dd5ec")
#pragma comment(linker, "/alternatename:?func_ov002_020eee3c@@YAXPAD0@Z=_func_ov002_020eee3c")

/* gate-10 tier-2 state wave: C++-mangled refs from the new St_ TUs
   resolved onto the C-named defs and storage they actually link to. */
#pragma comment(linker, "/alternatename:?FUN_02029934@@YAXXZ=_FUN_02029934")
#pragma comment(linker, "/alternatename:?FUN_02029980@@YAXXZ=_FUN_02029980")
#pragma comment(linker, "/alternatename:?Player_AdvanceAnims@@YAXPAD@Z=_Player_AdvanceAnims")
#pragma comment(linker, "/alternatename:?Player_AdvanceAnims@@YAXPAX@Z=_Player_AdvanceAnims")
#pragma comment(linker, "/alternatename:?Player_DisableInteraction@@YAHPAX@Z=_Player_DisableInteraction")
#pragma comment(linker, "/alternatename:?Player_DisableInteraction@@YAXPAX@Z=_Player_DisableInteraction")
#pragma comment(linker, "/alternatename:?Player_ReleaseHeldActor@@YAXPAX@Z=_Player_ReleaseHeldActor")
#pragma comment(linker, "/alternatename:?Player_ScaleByCharFactor@@YAHPADH@Z=_Player_ScaleByCharFactor")
#pragma comment(linker, "/alternatename:?_Z14ApproachLinearRiii@@YAXPAHHH@Z=__Z14ApproachLinearRiii")
#pragma comment(linker, "/alternatename:?_ZN4cstd5atan2E5Fix12IiES1_@@YAHHH@Z=__ZN4cstd5atan2E5Fix12IiES1_")
#pragma comment(linker, "/alternatename:?_ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs@@YAXPAXABUVector3@@IHF@Z=__ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs")
#pragma comment(linker, "/alternatename:?_ZN5Sound13PlayCharVoiceEjjRK7Vector3@@YAXIIPAX@Z=__ZN5Sound13PlayCharVoiceEjjRK7Vector3")
#pragma comment(linker, "/alternatename:?_ZN6Player11ChangeStateERNS_5StateE@@YAXPAX0@Z=__ZN6Player11ChangeStateERNS_5StateE")
#pragma comment(linker, "/alternatename:?_ZN6Player12FinishedAnimEv@@YAHPAX@Z=__ZN6Player12FinishedAnimEv")
#pragma comment(linker, "/alternatename:?_ZN6Player7SetAnimEji5Fix12IiEj@@YAXPADIHHI@Z=__ZN6Player7SetAnimEji5Fix12IiEj")
#pragma comment(linker, "/alternatename:?_ZN6Player7SetAnimEji5Fix12IiEj@@YAXPAXHHHI@Z=__ZN6Player7SetAnimEji5Fix12IiEj")
#pragma comment(linker, "/alternatename:?_ZN6Player7SetAnimEji5Fix12IiEj@@YAXPAXIHHI@Z=__ZN6Player7SetAnimEji5Fix12IiEj")
#pragma comment(linker, "/alternatename:?_ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_@@YAXHHH@Z=__ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_")
#pragma comment(linker, "/alternatename:?_ZN9ActorBase18MarkForDestructionEv@@YAXPAX@Z=__ZN9ActorBase18MarkForDestructionEv")
#pragma comment(linker, "/alternatename:?_ZNK10ClsnResult9GetClsnIDEv@@YAHPAX@Z=__ZNK10ClsnResult9GetClsnIDEv")
#pragma comment(linker, "/alternatename:?_ZNK12WithMeshClsn13GetWallResultEv@@YAPAXPAX@Z=__ZNK12WithMeshClsn13GetWallResultEv")
#pragma comment(linker, "/alternatename:?_ZNK6Player14GetBodyModelIDEjb@@YAHPADIH@Z=__ZNK6Player14GetBodyModelIDEjb")
#pragma comment(linker, "/alternatename:?data_02092110@@3CA=_data_02092110")
#pragma comment(linker, "/alternatename:?data_0209f250@@3EA=_data_0209f250")
#pragma comment(linker, "/alternatename:?data_0209f28c@@3EA=_data_0209f28c")
#pragma comment(linker, "/alternatename:?data_0209f318@@3PADA=_data_0209f318")
#pragma comment(linker, "/alternatename:?data_0209f49e@@3GA=_data_0209f49e")
#pragma comment(linker, "/alternatename:?data_0209f49e@@3PAGA=_data_0209f49e")
#pragma comment(linker, "/alternatename:?data_0209f4a0@@3PAFA=_data_0209f4a0")
#pragma comment(linker, "/alternatename:?data_020a0e5a@@3PADA=_data_020a0e5a")
#pragma comment(linker, "/alternatename:?data_ov002_020ff0ec@@3PAEA=_data_ov002_020ff0ec")
#pragma comment(linker, "/alternatename:?data_ov002_020ff1c0@@3PAHA=_data_ov002_020ff1c0")
#pragma comment(linker, "/alternatename:?data_ov002_020ff1d0@@3PAHA=_data_ov002_020ff1d0")
#pragma comment(linker, "/alternatename:?data_ov002_020ff254@@3PAHA=_data_ov002_020ff254")
#pragma comment(linker, "/alternatename:?data_ov002_02109fe4@@3PAHA=_data_ov002_02109fe4")
#pragma comment(linker, "/alternatename:?data_ov002_0210a560@@3PAHA=_data_ov002_0210a560")
#pragma comment(linker, "/alternatename:?data_ov002_0210a578@@3PAIA=_data_ov002_0210a578")
#pragma comment(linker, "/alternatename:?data_ov002_0210a584@@3PAIA=_data_ov002_0210a584")
#pragma comment(linker, "/alternatename:?data_ov002_0210a60c@@3PAIA=_data_ov002_0210a60c")
#pragma comment(linker, "/alternatename:?data_ov002_0210a6d4@@3PAIA=_data_ov002_0210a6d4")
#pragma comment(linker, "/alternatename:?data_ov002_0211004c@@3UState@@A=_data_ov002_0211004c")
#pragma comment(linker, "/alternatename:?data_ov002_0211007c@@3PAHA=_data_ov002_0211007c")
#pragma comment(linker, "/alternatename:?data_ov002_0211013c@@3DA=_data_ov002_0211013c")
#pragma comment(linker, "/alternatename:?data_ov002_0211013c@@3PADA=_data_ov002_0211013c")
#pragma comment(linker, "/alternatename:?data_ov002_021101b4@@3DA=_data_ov002_021101b4")
#pragma comment(linker, "/alternatename:?data_ov002_0211031c@@3DA=_data_ov002_0211031c")
#pragma comment(linker, "/alternatename:?data_ov002_021103dc@@3DA=_data_ov002_021103dc")
#pragma comment(linker, "/alternatename:?data_ov002_02110424@@3DA=_data_ov002_02110424")
#pragma comment(linker, "/alternatename:?data_ov002_021106c4@@3PAHA=_data_ov002_021106c4")
/* ST_CLIMB: St_Climb_Init and St_Climb_Cleanup declare their externs without
   extern "C", so MSVC C++-mangles every reference. 021106f4 (ST_HEADSTAND)
   needs two aliases because the two sources spell its type differently. */
#pragma comment(linker, "/alternatename:?data_ov002_021106f4@@3PADA=_data_ov002_021106f4")
#pragma comment(linker, "/alternatename:?data_ov002_021106f4@@3PAHA=_data_ov002_021106f4")
/* ST_LEDGE_HANG (data_ov002_0210ffec): the same case as ST_CLIMB above.
   All three St_LedgeHang halves pull their declarations from
   include/decl_common.h, which is not wrapped in extern "C", so every
   reference comes out C++-mangled against a C-named definition. Init's
   SetPosRelativeToActor is worse -- it is declared at file scope in the
   source itself, outside any extern "C" -- and 02110004 (ST_LEDGE_GRAB,
   the pull-up Main changes into) is the data half of the same problem.
   Seven references, seven aliases; the sources stay byte-verified. */
#pragma comment(linker, "/alternatename:?func_ov002_020cfaf0@@YAHPAX@Z=_func_ov002_020cfaf0")
#pragma comment(linker, "/alternatename:?func_ov002_020cfbdc@@YAHPAX@Z=_func_ov002_020cfbdc")
#pragma comment(linker, "/alternatename:?func_ov002_020cfea4@@YAHPAX@Z=_func_ov002_020cfea4")
#pragma comment(linker, "/alternatename:?func_ov002_020d0948@@YAXPAX@Z=_func_ov002_020d0948")
#pragma comment(linker, "/alternatename:?func_ov002_020e63a4@@YAXPAX@Z=_func_ov002_020e63a4")
#pragma comment(linker, "/alternatename:?data_ov002_02110004@@3PAHA=_data_ov002_02110004")
#pragma comment(linker, "/alternatename:?_ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3@@YAXPADPAUVec3@@@Z=__ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3")
#pragma comment(linker, "/alternatename:?Player_ReleaseHeldActor@@YAHPAD@Z=_Player_ReleaseHeldActor")
#pragma comment(linker, "/alternatename:?func_ov002_020e3078@@YAHPAX0@Z=_func_ov002_020e3078")
#pragma comment(linker, "/alternatename:?func_ov002_020caf68@@YAHPAX@Z=_func_ov002_020caf68")
#pragma comment(linker, "/alternatename:?_ZN5Sound13PlayCharVoiceEjjRK7Vector3@@YAXIIABUVector3@@@Z=__ZN5Sound13PlayCharVoiceEjjRK7Vector3")
#pragma comment(linker, "/alternatename:?func_0200d10c@@YAXPAXE@Z=_func_0200d10c")
#pragma comment(linker, "/alternatename:?func_0200d1e4@@YAXPAD@Z=_func_0200d1e4")
#pragma comment(linker, "/alternatename:?func_0200d63c@@YAXPAXE@Z=_func_0200d63c")
#pragma comment(linker, "/alternatename:?func_0200d6b4@@YAXPAXE@Z=_func_0200d6b4")
#pragma comment(linker, "/alternatename:?func_0200d768@@YAXPAXE@Z=_func_0200d768")
#pragma comment(linker, "/alternatename:?func_0200d7a4@@YAXPAXE@Z=_func_0200d7a4")
#pragma comment(linker, "/alternatename:?func_0200d89c@@YAXPAD@Z=_func_0200d89c")
#pragma comment(linker, "/alternatename:?func_0201226c@@YAHHHHHHF@Z=_func_0201226c")
#pragma comment(linker, "/alternatename:?func_0201fc88@@YAXF@Z=_func_0201fc88")
#pragma comment(linker, "/alternatename:?func_02020388@@YAXH@Z=_func_02020388")
#pragma comment(linker, "/alternatename:?func_02022b04@@YAXHHH@Z=_func_02022b04")
#pragma comment(linker, "/alternatename:?func_02035638@@YAHPAE@Z=_func_02035638")
#pragma comment(linker, "/alternatename:?func_0203564c@@YAHH@Z=_func_0203564c")
#pragma comment(linker, "/alternatename:?func_ov002_020bdb50@@YAXPADH@Z=_func_ov002_020bdb50")
#pragma comment(linker, "/alternatename:?func_ov002_020beb38@@YAHPAD@Z=_func_ov002_020beb38")
#pragma comment(linker, "/alternatename:?func_ov002_020bf56c@@YAHPAXH@Z=_func_ov002_020bf56c")
#pragma comment(linker, "/alternatename:?func_ov002_020bf5e0@@YAXPAX@Z=_func_ov002_020bf5e0")
#pragma comment(linker, "/alternatename:?func_ov002_020bf88c@@YAXPAX@Z=_func_ov002_020bf88c")
#pragma comment(linker, "/alternatename:?func_ov002_020c04e0@@YAHPAD@Z=_func_ov002_020c04e0")
#pragma comment(linker, "/alternatename:?func_ov002_020c1eb4@@YAXPAXH@Z=_func_ov002_020c1eb4")
#pragma comment(linker, "/alternatename:?func_ov002_020c2f64@@YAXPAX@Z=_func_ov002_020c2f64")
#pragma comment(linker, "/alternatename:?func_ov002_020c47f4@@YAHPAD@Z=_func_ov002_020c47f4")
#pragma comment(linker, "/alternatename:?func_ov002_020cc660@@YAXPADH@Z=_func_ov002_020cc660")
#pragma comment(linker, "/alternatename:?func_ov002_020cd190@@YAXPAX@Z=_func_ov002_020cd190")
#pragma comment(linker, "/alternatename:?func_ov002_020cf20c@@YAHPAD@Z=_func_ov002_020cf20c")
#pragma comment(linker, "/alternatename:?func_ov002_020cf2f8@@YAXPAD@Z=_func_ov002_020cf2f8")
#pragma comment(linker, "/alternatename:?func_ov002_020cf384@@YAXPAD@Z=_func_ov002_020cf384")
#pragma comment(linker, "/alternatename:?func_ov002_020d1f78@@YAXPAXI@Z=_func_ov002_020d1f78")
#pragma comment(linker, "/alternatename:?func_ov002_020d3498@@YAXPAX@Z=_func_ov002_020d3498")
#pragma comment(linker, "/alternatename:?func_ov002_020d5ab4@@YAHPAX@Z=_func_ov002_020d5ab4")
#pragma comment(linker, "/alternatename:?func_ov002_020d6368@@YAXPAD@Z=_func_ov002_020d6368")
#pragma comment(linker, "/alternatename:?func_ov002_020d674c@@YAHPAD@Z=_func_ov002_020d674c")
#pragma comment(linker, "/alternatename:?func_ov002_020d718c@@YAXPAX@Z=_func_ov002_020d718c")
#pragma comment(linker, "/alternatename:?func_ov002_020d71a0@@YAXPAD@Z=_func_ov002_020d71a0")
#pragma comment(linker, "/alternatename:?func_ov002_020d93ac@@YAXPAD@Z=_func_ov002_020d93ac")
#pragma comment(linker, "/alternatename:?func_ov002_020d9dcc@@YAHPAX@Z=_func_ov002_020d9dcc")
#pragma comment(linker, "/alternatename:?func_ov002_020daa74@@YAXPAX@Z=_func_ov002_020daa74")
#pragma comment(linker, "/alternatename:?func_ov002_020db54c@@YAXPADHHH@Z=_func_ov002_020db54c")
#pragma comment(linker, "/alternatename:?func_ov002_020dcafc@@YAXPAD@Z=_func_ov002_020dcafc")
#pragma comment(linker, "/alternatename:?func_ov002_020de968@@YAXPAX@Z=_func_ov002_020de968")
#pragma comment(linker, "/alternatename:?func_ov002_020e0f38@@YAXPAXE@Z=_func_ov002_020e0f38")
#pragma comment(linker, "/alternatename:?func_ov002_020e0f38@@YAXPAXH@Z=_func_ov002_020e0f38")
#pragma comment(linker, "/alternatename:?func_ov002_020e28d4@@YAXPAXHH@Z=_func_ov002_020e28d4")
#pragma comment(linker, "/alternatename:?func_ov100_02144fcc@@YAHXZ=_func_ov100_02144fcc")

/* Player::ST_WAIT is an ov006 FUNCTION name; at that address ov002 holds
   the Wait State object, which is what St_WaitQuicksand_Main wants. */
#pragma comment(linker, "/alternatename:__ZN6Player7ST_WAITE=_data_ov002_02110154")

/* Return-type-only variants of methods the port already faces. __thiscall,
   same argument list, result in EAX -- the existing face is ABI-identical
   and every call site here discards or byte-truncates the result. */
#pragma comment(linker, "/alternatename:?ChangeState@Player@@QAEXAAUState@@@Z=?ChangeState@Player@@QAEHAAUState@@@Z")
#pragma comment(linker, "/alternatename:?SetAnim@Player@@QAEHIHHI@Z=?SetAnim@Player@@QAEXIHHI@Z")
#pragma comment(linker, "/alternatename:?SetAnim@Player@@QAEIIHHI@Z=?SetAnim@Player@@QAEXIHHI@Z")
#pragma comment(linker, "/alternatename:?GetBodyModelID@Player@@QBEEI_N@Z=?GetBodyModelID@Player@@QBEII_N@Z")

/* tier-2 round 2: the rest of the ring the new state TUs reach. */
#pragma comment(linker, "/alternatename:?_ZN5Actor13SpawnSoundObjEj@@YAPADPADI@Z=__ZN5Actor13SpawnSoundObjEj")
#pragma comment(linker, "/alternatename:?_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii@@YAPADIIABUVector3@@PBXHH@Z=__ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii")
#pragma comment(linker, "/alternatename:?_ZN5Sound7PlaySubEjjj5Fix12IiEb@@YAHIIIHH@Z=__ZN5Sound7PlaySubEjjj5Fix12IiEb")
#pragma comment(linker, "/alternatename:?_ZN6Player18SetNewHatCharacterEjjb@@YAXPADII_N@Z=__ZN6Player18SetNewHatCharacterEjjb")
#pragma comment(linker, "/alternatename:?_ZN6Player8HasNoCapEv@@YA_NPAD@Z=__ZN6Player8HasNoCapEv")
#pragma comment(linker, "/alternatename:?data_0209212c@@3HA=_data_0209212c")
#pragma comment(linker, "/alternatename:?data_0209f310@@3CA=_data_0209f310")
#pragma comment(linker, "/alternatename:?data_ov002_02110364@@3UState@@A=_data_ov002_02110364")
#pragma comment(linker, "/alternatename:?data_ov002_02110394@@3UState@@A=_data_ov002_02110394")
#pragma comment(linker, "/alternatename:?data_ov002_02110a5c@@3PAUEntry@@A=_data_ov002_02110a5c")
#pragma comment(linker, "/alternatename:?func_ov100_02145014@@YAHXZ=_func_ov100_02145014")

/* Same ADDRESS, different overlay label: 0x020e3078 and 0x020c5dec carry
   an ov002 function too, and ov002 is the overlay the port runs. The src
   files naming them ov006/ov007 describe a different overlay's bytes. */
#pragma comment(linker, "/alternatename:_func_ov006_020e3078=_func_ov002_020e3078")
#pragma comment(linker, "/alternatename:_func_ov007_020c5dec=_func_ov002_020c5dec")

/* gate 42: the same shared-base illusion for PEACH_PAINTING. ov010, ov052 and
   ov021 all link at 0x021111a0, so dsd named two of PeachPainting's OWN
   internal references (an arm_call and a load, both module:overlay(10) in the
   relocs) after ov052/ov021 symbols at the shared address:
     * data_ov052_02111e84 (ov052's CLPS collision table) is really
       func_ov010_02111e84, the InitResources matrix/scale helper.
     * RollingRock_Spawn (ov021 0x02112d64) is really data_ov010_02112d64, the
       8-byte BSS SharedFilePtr the ov010 sinits build; CleanupResources names
       it directly. Its address is used as a SharedFilePtr, so the data symbol
       is the right target. Both decls are C-linkage in decl_common's extern "C"
       block, so the references are plain cdecl.
   PORT_HOST_ABI: dsd shared-base misname -- the ov021 name at 0x02112d64 is
   ov010 BSS data in the running overlay set, aliased by address. */
#pragma comment(linker, "/alternatename:_data_ov052_02111e84=_func_ov010_02111e84")
#pragma comment(linker, "/alternatename:_RollingRock_Spawn=_data_ov010_02112d64")
#pragma comment(linker, "/alternatename:?data_0208e42c@@3CA=_data_0208e42c")
#pragma comment(linker, "/alternatename:?data_0209b470@@3CA=_data_0209b470")
#pragma comment(linker, "/alternatename:?data_0209b490@@3HA=_data_0209b490")
#pragma comment(linker, "/alternatename:?data_0209b494@@3HA=_data_0209b494")
#pragma comment(linker, "/alternatename:?data_0209b4b0@@3PAIA=_data_0209b4b0")
#pragma comment(linker, "/alternatename:?data_0209f4a4@@3FA=_data_0209f4a4")
#pragma comment(linker, "/alternatename:?func_ov002_020d5f34@@YAHPADPAX@Z=_func_ov002_020d5f34")
#pragma comment(linker, "/alternatename:?func_ov002_020d708c@@YAXPAD@Z=_func_ov002_020d708c")
#pragma comment(linker, "/alternatename:?func_ov002_020d708c@@YAXPAD@Z=_func_ov002_020d708c")

/* shared-body state Inits (Dive, BackFlip, HeadstandJump, SlideKickRecover,
   WaterJump): the C++-mangled refs their TUs emit -> the C-named storage */
#pragma comment(linker, "/alternatename:?data_ov002_020ff100@@3PAHA=_data_ov002_020ff100")

/* death-state ring (DeadHit, DeadPit, Squish, BurnLava + the KillPlayer
   chain): C++-mangled refs -> the C-named defs and ov002 storage */
#pragma comment(linker, "/alternatename:?KillPlayer@@YAXXZ=_KillPlayer")
#pragma comment(linker, "/alternatename:?func_ov002_020c0108@@YAXPAXH@Z=_func_ov002_020c0108")
#pragma comment(linker, "/alternatename:?func_ov002_020c647c@@YAHPADH@Z=_func_ov002_020c647c")
#pragma comment(linker, "/alternatename:?func_ov002_020c6538@@YAHPAD@Z=_func_ov002_020c6538")
#pragma comment(linker, "/alternatename:?func_ov002_020c6908@@YAHPAD@Z=_func_ov002_020c6908")
#pragma comment(linker, "/alternatename:?func_ov006_020e3078@@YAHPAXPAH@Z=_func_ov002_020e3078")
#pragma comment(linker, "/alternatename:?data_ov002_02109db8@@3PAEA=_data_ov002_02109db8")
#pragma comment(linker, "/alternatename:?data_ov002_0210a07c@@3PAIA=_data_ov002_0210a07c")
#pragma comment(linker, "/alternatename:?data_ov002_0210a424@@3PAHA=_data_ov002_0210a424")
#pragma comment(linker, "/alternatename:?data_ov002_021100f4@@3PAHA=_data_ov002_021100f4")
#pragma comment(linker, "/alternatename:?data_ov002_0211117c@@3EA=_data_ov002_0211117c")

/* NoControl ring (the cutscene/door/pipe state and its 19 per-step
   helpers): C++-mangled refs -> the C-named defs and ov002 storage */
#pragma comment(linker, "/alternatename:?func_ov002_020c84b0@@YAHPAD@Z=_func_ov002_020c84b0")
#pragma comment(linker, "/alternatename:?func_ov002_020c8540@@YAHPAD@Z=_func_ov002_020c8540")
#pragma comment(linker, "/alternatename:?func_ov002_020c8714@@YAXPAD@Z=_func_ov002_020c8714")
#pragma comment(linker, "/alternatename:?func_ov002_020c897c@@YAXPAD@Z=_func_ov002_020c897c")
#pragma comment(linker, "/alternatename:?func_ov002_020c8b54@@YAXPAD@Z=_func_ov002_020c8b54")
#pragma comment(linker, "/alternatename:?func_ov002_020c8b78@@YAXPAD@Z=_func_ov002_020c8b78")
#pragma comment(linker, "/alternatename:?func_ov002_020c8cb0@@YAXPAD@Z=_func_ov002_020c8cb0")
#pragma comment(linker, "/alternatename:?func_ov002_020c8d14@@YAXPAD@Z=_func_ov002_020c8d14")
#pragma comment(linker, "/alternatename:?func_ov002_020c8f0c@@YAXPAD@Z=_func_ov002_020c8f0c")
#pragma comment(linker, "/alternatename:?func_ov002_020c8f80@@YAXPAD@Z=_func_ov002_020c8f80")
#pragma comment(linker, "/alternatename:?func_ov002_020c904c@@YAXPAD@Z=_func_ov002_020c904c")
#pragma comment(linker, "/alternatename:?func_ov002_020c9128@@YAHPAD@Z=_func_ov002_020c9128")
#pragma comment(linker, "/alternatename:?func_ov002_020c91bc@@YAHPAD@Z=_func_ov002_020c91bc")
#pragma comment(linker, "/alternatename:?func_ov002_020c924c@@YAXPAD@Z=_func_ov002_020c924c")
#pragma comment(linker, "/alternatename:?func_ov002_020c9288@@YAXPAD@Z=_func_ov002_020c9288")
#pragma comment(linker, "/alternatename:?func_ov002_020c92fc@@YAXPAD@Z=_func_ov002_020c92fc")
#pragma comment(linker, "/alternatename:?func_ov002_020c94a4@@YAHPAD@Z=_func_ov002_020c94a4")
#pragma comment(linker, "/alternatename:?func_ov002_020c965c@@YAXPAD@Z=_func_ov002_020c965c")
#pragma comment(linker, "/alternatename:?func_ov002_020ca108@@YAXPAD@Z=_func_ov002_020ca108")
#pragma comment(linker, "/alternatename:?func_0200ee68@@YAHXZ=_func_0200ee68")
#pragma comment(linker, "/alternatename:?func_020072c0@@YAXXZ=_func_020072c0")
#pragma comment(linker, "/alternatename:?func_02053274@@YAHPBUVector3@@0@Z=_func_02053274")
#pragma comment(linker, "/alternatename:?Vec3_RotateYAndTranslate@@YAXPAUVector3@@PAXH1@Z=_Vec3_RotateYAndTranslate")
#pragma comment(linker, "/alternatename:?PlayBank0@Sound@@YAXIABUVector3@@@Z=__ZN5Sound9PlayBank0EjRK7Vector3")
#pragma comment(linker, "/alternatename:?_ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj@@YAXPAUPlayer@@AAUActorBase@@IPBUVector3@@II@Z=__ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj")
#pragma comment(linker, "/alternatename:?_ZN6Player12FinishedAnimEv@@YAHPAUPlayer@@@Z=__ZN6Player12FinishedAnimEv")
#pragma comment(linker, "/alternatename:?_ZNK6Player14GetBodyModelIDEjb@@YAHPBUPlayer@@I_N@Z=__ZNK6Player14GetBodyModelIDEjb")
#pragma comment(linker, "/alternatename:?_ZNK9Animation12WillHitFrameEi@@YAHPBUAnimation@@H@Z=__ZNK9Animation12WillHitFrameEi")
#pragma comment(linker, "/alternatename:?data_ov002_0210e150@@3HA=_data_ov002_0210e150")
#pragma comment(linker, "/alternatename:?data_ov002_0210f89c@@3HA=_data_ov002_0210f89c")
#pragma comment(linker, "/alternatename:?data_ov002_0210f8cc@@3PAHA=_data_ov002_0210f8cc")
/* return-type-only variant of the Animation::WillHitFrame face the port
   already has (bool vs int; same __thiscall shape, result in EAX) */
#pragma comment(linker, "/alternatename:?WillHitFrame@Animation@@QBEHH@Z=?WillHitFrame@Animation@@QBE_NH@Z")

/* gate 13, the real Camera: the C++-mangled references its TUs emit (they
   declare their externs outside extern "C") -> the C-named definitions, plus
   the two community names that sit on top of matched symbols. */
#pragma comment(linker, "/alternatename:?CAM_SPACE_CAM_POS_ASR_3@@3DA=_CAM_SPACE_CAM_POS_ASR_3")
#pragma comment(linker, "/alternatename:?data_0209b008@@3DA=_data_0209b008")
#pragma comment(linker, "/alternatename:?data_0209b008@@3PAUCamera_State@@A=_data_0209b008")
#pragma comment(linker, "/alternatename:?data_0209b41c@@3DA=_data_0209b41c")
#pragma comment(linker, "/alternatename:?data_0208733c@@3IA=_data_0208733c")
#pragma comment(linker, "/alternatename:?data_0209f20c@@3EA=_data_0209f20c")
#pragma comment(linker, "/alternatename:?data_0209f294@@3EA=_data_0209f294")
#pragma comment(linker, "/alternatename:?data_0209f2c4@@3EA=_data_0209f2c4")
#pragma comment(linker, "/alternatename:?_ZN6Camera11ChangeStateEPNS_5StateE@@YAHPAUCamera@@PAUCamera_State@@@Z=__ZN6Camera11ChangeStateEPNS_5StateE")
#pragma comment(linker, "/alternatename:?_ZNK6Camera12IsUnderwaterEv@@YAHPAX@Z=__ZNK6Camera12IsUnderwaterEv")
#pragma comment(linker, "/alternatename:?func_0200ca50@@YAHPAX@Z=_func_0200ca50")
#pragma comment(linker, "/alternatename:?func_0203dafc@@YAXH@Z=_func_0203dafc")
#pragma comment(linker, "/alternatename:?Math_Function_0203b0fc@@YAXPAHHHH@Z=_Math_Function_0203b0fc")
#pragma comment(linker, "/alternatename:?_ZN8Particle6System10NewWeatherEjj5Fix12IiES2_S2_PK11Vector3_16fj@@YAIIIHHHPBXE@Z=__ZN8Particle6System10NewWeatherEjj5Fix12IiES2_S2_PK11Vector3_16fj")
/* the G3i pair are host copies (port/unmatched/); their TU declares them as
   C names, Camera::Render as class statics */
#pragma comment(linker, "/alternatename:?PerspectiveW_@G3i@@SAXHHHHHH_NPAUMatrix4x3@@@Z=__ZN3G3i13PerspectiveW_E5Fix12IiES1_S1_S1_S1_S1_bP9Matrix4x3")
#pragma comment(linker, "/alternatename:?LookAt_@G3i@@SAXPBUVector3@@00_NPAUMatrix4x3@@@Z=__ZN3G3i7LookAt_EPK7Vector3S2_S2_bP9Matrix4x3")
#pragma comment(linker, "/alternatename:?SetBlendAlpha@G2x@@SAXPCGGGGG@Z=__ZN3G2x13SetBlendAlphaEPVttttt")
#pragma comment(linker, "/alternatename:?Render@OAM@@SAX_NPAUOamAttr@@HHHHHHHH@Z=__ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii")
/* community names for matched symbols: Vec3_DistSq IS func_0203cf94,
   STAR_MARKERS is the bss array at 0x0209f40c, and func_0200cc5c is
   Camera::SaveCameraStateBeforeTalk (both callers spell it argless) */
#pragma comment(linker, "/alternatename:_Vec3_DistSq=_func_0203cf94")
#pragma comment(linker, "/alternatename:_STAR_MARKERS=_data_0209f40c")
#pragma comment(linker, "/alternatename:_func_0200cc5c=__ZN6Camera25SaveCameraStateBeforeTalkEv")
/* ActorBase's own vtable symbol: the transient install the ctor/dtor chain
   writes, already storage in hal/actor_vtables.cpp under its DS name */
#pragma comment(linker, "/alternatename:__ZTV9ActorBase=_data_02099edc")
/* Camera::~Camera (D0) frees by C name; Memory::Deallocate's own TU declares
   Heap as a `class` (PAV) while every header spells it `struct` (PAU), so the
   alias carries the decorated name from the link log rather than a face */
#pragma comment(linker, "/alternatename:__ZN6Memory10DeallocateEPvP4Heap=?Deallocate@Memory@@YAXPAXPAVHeap@@@Z")

/* ---- gate 14, the level boot -------------------------------------------
   Stage::LoadClsnAndObjects and the sub-loaders pick their externs out of
   include/decl_common.h, which is generated without extern "C", so a .cpp
   TU emits MSVC manglings for symbols the .c definitions publish as plain C
   names. Same closure as the state waves; nothing in src/ changes. */
#pragma comment(linker, "/alternatename:?ContinueKuppaScriptIfNecessary@@YAHXZ=_ContinueKuppaScriptIfNecessary")
#pragma comment(linker, "/alternatename:?StartIntroCutscene@@YAXXZ=_StartIntroCutscene")
#pragma comment(linker, "/alternatename:?func_0202a850@@YAXHH@Z=_func_0202a850")
#pragma comment(linker, "/alternatename:?func_0203aca0@@YAXHH@Z=_func_0203aca0")
#pragma comment(linker, "/alternatename:?func_0203accc@@YAXH@Z=_func_0203accc")
#pragma comment(linker, "/alternatename:?data_02092134@@3HA=_data_02092134")
#pragma comment(linker, "/alternatename:?data_ov002_0210cb70@@3PAEA=_data_ov002_0210cb70")
#pragma comment(linker, "/alternatename:?data_ov002_0210cb88@@3PAGA=_data_ov002_0210cb88")
#pragma comment(linker, "/alternatename:?data_ov002_0210cbf4@@3PAGA=_data_ov002_0210cbf4")
#pragma comment(linker, "/alternatename:?data_ov002_0211118c@@3FA=_data_ov002_0211118c")
/* Sound::LoadInitialGroup is a class static in its TU and a C name to the
   kuppa tail; LoadGroupAndSetBank is the mirror case one call deeper. */
#pragma comment(linker, "/alternatename:__ZN5Sound16LoadInitialGroupEi=?LoadInitialGroup@Sound@@SAXH@Z")
#pragma comment(linker, "/alternatename:?LoadGroupAndSetBank@Sound@@SAXHH@Z=__ZN5Sound19LoadGroupAndSetBankEii")
/* gate 14, stage A2: the entrance step handlers. 020c71e0's own TU spells it
   as a C name while 020c72a4's declares it without extern "C". */
#pragma comment(linker, "/alternatename:?func_ov002_020c71e0@@YAXPAX@Z=_func_ov002_020c71e0")
/* NOT aliases: Actor::GetBitInDeathTable and Actor::AfterInitResources are
   real MSVC methods, so their C-named callers would enter a __thiscall body
   through a cdecl call and read `this` out of ecx garbage. Both get faces in
   hal/level_boot.cpp instead. */

/* ---- gate 16, the actor classes ----------------------------------------
   The class TUs declare their overlay data at C++ linkage (outside the
   extern "C" block the functions sit in), so each reference carries the
   MSVC mangling of whatever TYPE that TU happened to spell -- and different
   TUs of the same class spell the same array differently. ovdata.py
   publishes one plain C symbol; these are the per-mangling faces onto it.

   Tree: InitResources reads data_ov002_02110a48 as int[] and the model-id
   table as unsigned short[]; CleanupResources reads the same list heads as
   char*[]. Two manglings, one array. */
#pragma comment(linker, "/alternatename:?data_ov002_02110a48@@3PAHA=_data_ov002_02110a48")
#pragma comment(linker, "/alternatename:?data_ov002_02110a48@@3PAPADA=_data_ov002_02110a48")
#pragma comment(linker, "/alternatename:?data_ov002_0210abb8@@3PAGA=_data_ov002_0210abb8")
/* Actor's D2 picks func_0203b27c and func_02044104 out of decl_common.h,
   which is generated without extern "C", so the .cpp emits MSVC manglings
   for two plain C definitions. */
#pragma comment(linker, "/alternatename:?func_0203b27c@@YAXHH@Z=_func_0203b27c")
#pragma comment(linker, "/alternatename:?func_02044104@@YAXH@Z=_func_02044104")
/* _ZTV5Actor and _ZTV12ActorDerived are 0x0208e3a4 / 0x0208e4b8, the two
   base tables the constructor and destructor chains install transiently and
   never dispatch through. hal/actor_vtables.cpp already carries the storage
   under the data_ names; these are the class-name faces onto the same bytes,
   the _ZTV9ActorBase pattern one line up. */
#pragma comment(linker, "/alternatename:__ZTV5Actor=_data_0208e3a4")
#pragma comment(linker, "/alternatename:__ZTV12ActorDerived=_data_0208e4b8")
/* AMBIENT_SOUND_EFFECTS: its two TUs declare their overlay and engine data at
   C++ linkage, with the type each one happened to spell. */
#pragma comment(linker, "/alternatename:?data_ov002_02110aec@@3HA=_data_ov002_02110aec")
#pragma comment(linker, "/alternatename:?data_ov002_0210b498@@3PAHA=_data_ov002_0210b498")
#pragma comment(linker, "/alternatename:?data_0209b4ac@@3HA=_data_0209b4ac")
#pragma comment(linker, "/alternatename:?data_0209f220@@3EA=_data_0209f220")
#pragma comment(linker, "/alternatename:?IsStarCollectedInLevel@@YAHCH@Z=_IsStarCollectedInLevel")
/* Sound::PlayLong is already in the slice under its C name; the ambient
   actor references it at C++ linkage. */
#pragma comment(linker, "/alternatename:?_ZN5Sound8PlayLongEjjjRK7Vector3j@@YAIIIIPAXI@Z=__ZN5Sound8PlayLongEjjjRK7Vector3j")

/* ---- gate 16, the ov002 tier -------------------------------------------
   BLACK_BRICK_BLOCK / SIGN_POST / ONE_UP_MUSHROOM. Same shape as the TREE
   block above: overlay data declared at C++ linkage in the class TUs, with
   whatever type each one happened to spell, onto the one plain C symbol
   ovdata.py publishes. */
#pragma comment(linker, "/alternatename:?data_ov002_02108ab0@@3PADA=_data_ov002_02108ab0")
#pragma comment(linker, "/alternatename:?data_ov002_02108ab4@@3PADA=_data_ov002_02108ab4")
#pragma comment(linker, "/alternatename:?data_ov002_021089e0@@3UV3@@A=_data_ov002_021089e0")
#pragma comment(linker, "/alternatename:?data_ov002_0210e05c@@3DA=_data_ov002_0210e05c")
#pragma comment(linker, "/alternatename:?data_ov002_0210e064@@3DA=_data_ov002_0210e064")
#pragma comment(linker, "/alternatename:?data_ov002_0210d9b8@@3UModelCache@@A=_data_ov002_0210d9b8")
#pragma comment(linker, "/alternatename:?data_ov002_0210d9d8@@3USharedFilePtr@@A=_data_ov002_0210d9d8")
#pragma comment(linker, "/alternatename:?data_ov002_0210d9d8@@3PAXA=_data_ov002_0210d9d8")
#pragma comment(linker, "/alternatename:?data_ov002_0210da30@@3USharedFilePtr@@A=_data_ov002_0210da30")
#pragma comment(linker, "/alternatename:?data_ov002_0210da30@@3PAXA=_data_ov002_0210da30")
#pragma comment(linker, "/alternatename:?data_ov002_020ff040@@3PAEA=_data_ov002_020ff040")
#pragma comment(linker, "/alternatename:?data_ov002_020ff050@@3PAEA=_data_ov002_020ff050")
/* Plain cdecl on both sides -- the C definition is already in the slice, the
   reference just carries a C++ mangling from a TU that never wrapped it. */
#pragma comment(linker, "/alternatename:?DecIfAbove0_Byte@@YAEPAE@Z=_DecIfAbove0_Byte")
#pragma comment(linker, "/alternatename:?Matrix4x3_FromRotationY@@YAXPAUMatrix4x3@@H@Z=_Matrix4x3_FromRotationY")
#pragma comment(linker, "/alternatename:?MulVec3Mat4x3@@YAXPAUVector3@@PAUMatrix4x3@@0@Z=_MulVec3Mat4x3")
#pragma comment(linker, "/alternatename:?Vec3_Add@@YAXPAUVector3@@00@Z=_Vec3_Add")
#pragma comment(linker, "/alternatename:?func_ov002_020baf80@@YAXPAD@Z=_func_ov002_020baf80")
#pragma comment(linker, "/alternatename:?func_ov002_020bb060@@YAHPAX@Z=_func_ov002_020bb060")
#pragma comment(linker, "/alternatename:?func_ov002_020ee5d0@@YAXPAEH@Z=_func_ov002_020ee5d0")
#pragma comment(linker, "/alternatename:?_ZN12WithMeshClsn13SetLimMovFlagEv@@YAXPAX@Z=__ZN12WithMeshClsn13SetLimMovFlagEv")
/* The shared-header placeholders. G0/G1 are whatever the TU's own literal
   pool held, so each one is settled from the ROM's relocation table rather
   than by name:
     _ZN8SignPost16CleanupResourcesEv  0x020bbe28/2c -> 0x0210e064, 0x0210e05c
       (the model SharedFilePtr, then the KCL one -- Release in load order)
     the Platform D0 pair                            -> 0x020a0eac, the heap
       Memory::Deallocate takes; every other D0 in the family spells the same
       word data_020a0eac. */
#pragma comment(linker, "/alternatename:?G0@@3PAHA=_data_ov002_0210e064")
#pragma comment(linker, "/alternatename:?G1@@3PAHA=_data_ov002_0210e05c")
#pragma comment(linker, "/alternatename:_G0=_data_020a0eac")
/* SignPost::CleanupResources carried from main names its two SharedFilePtrs
   by role instead of G0/G1: SignPost_ModelFile = 0x0210e064 (released first,
   ROM order) and SignPost_ClsnFile = 0x0210e05c (main's ov002 symbols.txt
   rows 2795/2797 pin both addresses). The declarations sit outside the
   file's extern "C" reach, so MSVC mangles them; same targets as the
   G0/G1 rows above, which stay for any TU still spelling the placeholders. */
#pragma comment(linker, "/alternatename:?SignPost_ModelFile@@3PAHA=_data_ov002_0210e064")
#pragma comment(linker, "/alternatename:?SignPost_ClsnFile@@3PAHA=_data_ov002_0210e05c")
/* src/_ZN18MovingCylinderClsnD1Ev.c spells its two constants by role: the
   base destructor at 0x02015058 is CylinderClsn::~CylinderClsn (D2) and the
   vtable it installs first is _ZTV18MovingCylinderClsn (0x0208e6d4). */
#pragma comment(linker, "/alternatename:_base_dtor_MovingCylinderClsn=__ZN12CylinderClsnD2Ev")
#pragma comment(linker, "/alternatename:_vtbl_MovingCylinderClsn=__ZTV18MovingCylinderClsn")
/* ...and the WithPos derivative one level up, which the butterfly's hosted D1
   tears down (gate 21 linkloop round 2): its base destructor at 0x02014954 is
   MovingCylinderClsn's D2 and the vtable is the host storage in
   hal/actor_vtables.cpp. */
#pragma comment(linker, "/alternatename:_base_dtor_MovingCylinderClsnWithPos=__ZN18MovingCylinderClsnD2Ev")
#pragma comment(linker, "/alternatename:_vtbl_MovingCylinderClsnWithPos=__ZTV25MovingCylinderClsnWithPos")
/* ov002's Enemy constructor is func_ov002_020aed98 -- see the header of that
   entry in slice_gate16.txt for why the file named _ZN5EnemyC2Ev is ov007's. */
#pragma comment(linker, "/alternatename:__ZN5EnemyC2Ev=_func_ov002_020aed98")
#pragma comment(linker, "/alternatename:?data_ov002_0211025c@@3PAHA=_data_ov002_0211025c")
/* the same per-mangling faces, one round further into the 1-up's chain */
#pragma comment(linker, "/alternatename:?data_0209f40c@@3PAHA=_data_0209f40c")
#pragma comment(linker, "/alternatename:?data_0209f208@@3EA=_data_0209f208")
#pragma comment(linker, "/alternatename:?IsStarCollectedInCurLevel@@YAHH@Z=_IsStarCollectedInCurLevel")
#pragma comment(linker, "/alternatename:?_ZN10SphereClsn10DetectClsnEv@@YAHPAX@Z=__ZN10SphereClsn10DetectClsnEv")
#pragma comment(linker, "/alternatename:?_ZNK12WithMeshClsn15ShouldUpdatePosEv@@YAHPAX@Z=__ZNK12WithMeshClsn15ShouldUpdatePosEv")
#pragma comment(linker, "/alternatename:?_ZNK12WithMeshClsn16ShouldUpdatePosYEv@@YAHPAX@Z=__ZNK12WithMeshClsn16ShouldUpdatePosYEv")
#pragma comment(linker, "/alternatename:?_ZN12WithMeshClsn19ClearAllGroundFlagsEv@@YAXPAX@Z=__ZN12WithMeshClsn19ClearAllGroundFlagsEv")
#pragma comment(linker, "/alternatename:?func_020355a0@@YAHPAX@Z=_func_020355a0")
#pragma comment(linker, "/alternatename:?func_02038a38@@YAHPAX@Z=_func_02038a38")
#pragma comment(linker, "/alternatename:?func_020371b0@@YAXPAXH@Z=_func_020371b0")
#pragma comment(linker, "/alternatename:?FUN_0202a130@@YAXXZ=_FUN_0202a130")
#pragma comment(linker, "/alternatename:?SetStarMarker@@YAXHHH@Z=_SetStarMarker")

/* ---- gate 17, the level overlay's own classes ---------------------------
   Same shape again: overlay data declared at C++ linkage with whatever type
   the TU spelled, and plain cdecl helpers referenced through a C++ mangling
   because the declaring TU never wrapped them. */
#pragma comment(linker, "/alternatename:?data_ov009_02113c20@@3PADA=_data_ov009_02113c20")
#pragma comment(linker, "/alternatename:?data_ov009_02113c28@@3PADA=_data_ov009_02113c28")
#pragma comment(linker, "/alternatename:?data_ov009_02113eb0@@3PAHA=_data_ov009_02113eb0")
#pragma comment(linker, "/alternatename:?data_ov009_02113eb8@@3PAHA=_data_ov009_02113eb8")
#pragma comment(linker, "/alternatename:?data_ov009_02113d8c@@3PAUVector3@@A=_data_ov009_02113d8c")
#pragma comment(linker, "/alternatename:?data_ov009_02113de0@@3PAUVector3@@A=_data_ov009_02113de0")
#pragma comment(linker, "/alternatename:?data_ov009_02113e34@@3PAUVector3@@A=_data_ov009_02113e34")
#pragma comment(linker, "/alternatename:?data_0209caa0@@3UD0209caa0@@A=_data_0209caa0")
#pragma comment(linker, "/alternatename:?data_0209f2d8@@3PAEA=_data_0209f2d8")
#pragma comment(linker, "/alternatename:?_ZN5Model8LoadFileER13SharedFilePtr@@YAHPAX@Z=__ZN5Model8LoadFileER13SharedFilePtr")
#pragma comment(linker, "/alternatename:?_ZN9Animation8LoadFileER13SharedFilePtr@@YAHPAX@Z=__ZN9Animation8LoadFileER13SharedFilePtr")
#pragma comment(linker, "/alternatename:?_ZN9ModelBase7SetFileEP8BMD_Fileii@@YAHPAXHHH@Z=__ZN9ModelBase7SetFileEP8BMD_Fileii")
#pragma comment(linker, "/alternatename:?_ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj@@YAHPAXHHHI@Z=__ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj")
#pragma comment(linker, "/alternatename:?_ZN8Platform13IsClsnInRangeE5Fix12IiES1_@@YAHPAXHH@Z=__ZN8Platform13IsClsnInRangeE5Fix12IiES1_")
#pragma comment(linker, "/alternatename:?_ZN8Platform19UpdateClsnPosAndRotEv@@YAHPAX@Z=__ZN8Platform19UpdateClsnPosAndRotEv")
#pragma comment(linker, "/alternatename:?_ZN8Platform21UpdateModelPosAndRotYEv@@YAHPAX@Z=__ZN8Platform21UpdateModelPosAndRotYEv")

/* ---- gate 18: ov085's two classes -------------------------------------
   The same shape as every wave before it. A slice .cpp that declares its
   externs without extern "C" emits an MSVC mangling for what is a C name
   everywhere else in the build; the alias closes the gap without touching
   src/. Decorated names are lifted verbatim from the link log. */

/* gate 18 link ring */
#pragma comment(linker, "/alternatename:?NumStars@@YAEXZ=_NumStars")
#pragma comment(linker, "/alternatename:?RandomIntInternal@@YAIPAH@Z=_RandomIntInternal")
#pragma comment(linker, "/alternatename:?_ZN11ShadowModel12InitCylinderEv@@YAHPAX@Z=__ZN11ShadowModel12InitCylinderEv")
#pragma comment(linker, "/alternatename:?_ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_@@YAXPAX0HH00@Z=__ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_")
#pragma comment(linker, "/alternatename:?_ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj@@YAXPAX0HHII@Z=__ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj")
#pragma comment(linker, "/alternatename:?_ZN5Actor13ClosestPlayerEv@@YAPAXPAX@Z=__ZN5Actor13ClosestPlayerEv")
#pragma comment(linker, "/alternatename:?_ZN5Model8LoadFileER13SharedFilePtr@@YAPAXPAX@Z=__ZN5Model8LoadFileER13SharedFilePtr")
#pragma comment(linker, "/alternatename:?_ZN7PathPtr6FromIDEj@@YAXPAXI@Z=__ZN7PathPtr6FromIDEj")
#pragma comment(linker, "/alternatename:?_ZN7PathPtrC1Ev@@YAXPAX@Z=__ZN7PathPtrC1Ev")
#pragma comment(linker, "/alternatename:?_ZN9Animation8LoadFileER13SharedFilePtr@@YAXPAX@Z=__ZN9Animation8LoadFileER13SharedFilePtr")
#pragma comment(linker, "/alternatename:?_ZN9ModelBase7SetFileEP8BMD_Fileii@@YAHPAX0HH@Z=__ZN9ModelBase7SetFileEP8BMD_Fileii")
#pragma comment(linker, "/alternatename:?_ZNK7PathPtr7GetNodeER7Vector3j@@YAXPAX0I@Z=__ZNK7PathPtr7GetNodeER7Vector3j")
#pragma comment(linker, "/alternatename:?_ZNK7PathPtr8NumNodesEv@@YAHPAX@Z=__ZNK7PathPtr8NumNodesEv")
#pragma comment(linker, "/alternatename:?data_0209caa0@@3PADA=_data_0209caa0")
#pragma comment(linker, "/alternatename:?data_0209e650@@3HA=_data_0209e650")
#pragma comment(linker, "/alternatename:?data_ov085_021305ac@@3HA=_data_ov085_021305ac")
#pragma comment(linker, "/alternatename:?data_ov085_021305b0@@3DA=_data_ov085_021305b0")
#pragma comment(linker, "/alternatename:?data_ov085_021305b8@@3DA=_data_ov085_021305b8")
#pragma comment(linker, "/alternatename:?data_ov085_021305c0@@3DA=_data_ov085_021305c0")
#pragma comment(linker, "/alternatename:?data_ov085_021305c8@@3DA=_data_ov085_021305c8")
#pragma comment(linker, "/alternatename:?data_ov085_021305d0@@3DA=_data_ov085_021305d0")
#pragma comment(linker, "/alternatename:?data_ov085_021305d8@@3DA=_data_ov085_021305d8")
#pragma comment(linker, "/alternatename:?data_ov085_021305e0@@3DA=_data_ov085_021305e0")
#pragma comment(linker, "/alternatename:?data_ov085_021306cc@@3DA=_data_ov085_021306cc")
#pragma comment(linker, "/alternatename:?data_ov085_021306ec@@3PADA=_data_ov085_021306ec")
#pragma comment(linker, "/alternatename:?data_ov085_0213073c@@3USharedFilePtr@@A=_data_ov085_0213073c")
#pragma comment(linker, "/alternatename:?data_ov085_02130744@@3USharedFilePtr@@A=_data_ov085_02130744")
#pragma comment(linker, "/alternatename:?data_ov085_0213074c@@3USharedFilePtr@@A=_data_ov085_0213074c")
#pragma comment(linker, "/alternatename:?data_ov085_02130790@@3HA=_data_ov085_02130790")
#pragma comment(linker, "/alternatename:?data_ov085_021307b0@@3PAHA=_data_ov085_021307b0")
#pragma comment(linker, "/alternatename:?data_ov085_021307d0@@3HA=_data_ov085_021307d0")
#pragma comment(linker, "/alternatename:?data_ov085_021307e0@@3HA=_data_ov085_021307e0")
#pragma comment(linker, "/alternatename:?data_ov085_02130810@@3PADA=_data_ov085_02130810")
#pragma comment(linker, "/alternatename:?func_02013890@@YAHHH@Z=_func_02013890")
#pragma comment(linker, "/alternatename:?func_ov002_020d228c@@YAXPAX@Z=_func_ov002_020d228c")
#pragma comment(linker, "/alternatename:?func_ov085_0212bc78@@YAXPAX0@Z=_func_ov085_0212bc78")
#pragma comment(linker, "/alternatename:?func_ov085_0212bcc8@@YAXPAD@Z=_func_ov085_0212bcc8")
#pragma comment(linker, "/alternatename:?func_ov085_0212c150@@YAXPAX@Z=_func_ov085_0212c150")
#pragma comment(linker, "/alternatename:?func_ov085_0212e728@@YAHPAX0@Z=_func_ov085_0212e728")

/* ---- gate 19: ov098's CANNON ------------------------------------------- */

/* gate 19 link ring */
#pragma comment(linker, "/alternatename:?data_ov098_0213c8e8@@3DA=_data_ov098_0213c8e8")
#pragma comment(linker, "/alternatename:?data_ov098_0213c8fc@@3PAUEntry@@A=_data_ov098_0213c8fc")
#pragma comment(linker, "/alternatename:?func_ov098_0213b15c@@YAXPAX@Z=_func_ov098_0213b15c")
#pragma comment(linker, "/alternatename:?data_ov002_021102d4@@3UState@@A=_data_ov002_021102d4")

/* ---- gate 20: ov002's EXIT and WATERFALL_MIST -------------------------- */

/* The exit's camera move is a real C++ method over a LOCAL SHADOW Camera
   (src/_ZN6Camera10LookAtExitER5Actor.cpp declares its own `struct Camera`
   with just the two methods it needs), so MSVC emits it under the shadow's
   mangling while its caller -- the exit's own Behavior -- spells it the
   Itanium way. Both ends are real code; only the spelling differs.
   The shadow's ChangeState and the State object it enters resolve the same
   way, onto the camera the port already carries: the dispatcher in
   hal/camera_bridges.cpp and state 15 of the nineteen in
   hal/camera_states.cpp (0x020095c4 / 0x02009540 -- the exit look). */
/* LookAtExit and Camera::ChangeState were the same broken thiscall aliases
   as the gate-22 door ring (one in each direction); both are real faces in
   hal/door_ring_faces.cpp now. */
#pragma comment(linker, "/alternatename:?data_0209b0f8@@3UState@Camera@@A=_data_0209b0f8")

/* ---- gate 21: ov100's BUTTERFLY and FISH ------------------------------- */

/* gate 21 link ring. Same two shapes as every gate before it: a C++ TU that
   declares a mounted datum with a type of its own gets MSVC mangling for a
   symbol the mount emits as plain C, and a TU that declares an Itanium-named
   function without extern "C" gets it mangled as well. The bytes and the
   bodies are the same either way. */
#pragma comment(linker, "/alternatename:?data_ov100_021473a4@@3PAPAHA=_data_ov100_021473a4")
#pragma comment(linker, "/alternatename:?data_ov100_021473b0@@3PAPAHA=_data_ov100_021473b0")
#pragma comment(linker, "/alternatename:?data_ov100_021489cc@@3PAHA=_data_ov100_021489cc")
#pragma comment(linker, "/alternatename:?data_ov100_021473a4@@3PAPAUSharedFilePtr@@A=_data_ov100_021473a4")
#pragma comment(linker, "/alternatename:?data_ov100_021473b0@@3PAPAUSharedFilePtr@@A=_data_ov100_021473b0")
#pragma comment(linker, "/alternatename:?data_ov100_021489cc@@3USharedFilePtr@@A=_data_ov100_021489cc")
#pragma comment(linker, "/alternatename:?data_ov002_0210d9d8@@3USFP@@A=_data_ov002_0210d9d8")
#pragma comment(linker, "/alternatename:?data_ov100_02148600@@3USFP@@A=_data_ov100_02148600")
#pragma comment(linker, "/alternatename:?data_ov100_02148608@@3USFP@@A=_data_ov100_02148608")
#pragma comment(linker, "/alternatename:?data_ov100_02148668@@3USFP@@A=_data_ov100_02148668")
#pragma comment(linker, "/alternatename:?data_020a0e68@@3UMtx@@A=_data_020a0e68")
#pragma comment(linker, "/alternatename:?data_ov100_02148628@@3PAUPMF@@A=_data_ov100_02148628")
#pragma comment(linker, "/alternatename:?_ZN5Actor10FindWithIDEj@@YAPAXI@Z=__ZN5Actor10FindWithIDEj")
#pragma comment(linker, "/alternatename:?_ZN13SharedFilePtr7ReleaseEv@@YAXPAUSharedFilePtr@@@Z=__ZN13SharedFilePtr7ReleaseEv")
#pragma comment(linker, "/alternatename:?_ZN5Actor15IsPlayerInRangeEi@@YAHPAXH@Z=__ZN5Actor15IsPlayerInRangeEi")
#pragma comment(linker, "/alternatename:?func_ov100_02146280@@YAXXZ=_func_ov100_02146280")
/* One TU spells ActorBase::MarkForDestruction by an ad-hoc C name rather than
   the Itanium one -- the same body either way. */
#pragma comment(linker, "/alternatename:?ActorBase_MarkForDestruction@@YAXPAX@Z=__ZN9ActorBase18MarkForDestructionEv")

/* ---- gate 22: ov100's DOOR --------------------------------------------- */

/* gate 22 link ring: the same two shapes again. The one that is not just a
   spelling is `func_02132894` -- the door's InitResources TU declares ov089's
   key-model SharedFilePtr table under a func_ name and without the overlay
   prefix, and LoadKeyModels spells the same bytes data_ov089_02132894. One
   array either way. */
#pragma comment(linker, "/alternatename:_func_02132894=_data_ov089_02132894")
#pragma comment(linker, "/alternatename:?data_02092110@@3DA=_data_02092110")
#pragma comment(linker, "/alternatename:?data_0209caa0@@3DA=_data_0209caa0")
#pragma comment(linker, "/alternatename:?data_0209f318@@3DA=_data_0209f318")
#pragma comment(linker, "/alternatename:?data_ov100_021488a4@@3DA=_data_ov100_021488a4")
#pragma comment(linker, "/alternatename:?data_ov100_021488b4@@3DA=_data_ov100_021488b4")
#pragma comment(linker, "/alternatename:?data_ov100_021488b4@@3PADA=_data_ov100_021488b4")
#pragma comment(linker, "/alternatename:?data_ov100_021488f4@@3PADA=_data_ov100_021488f4")
#pragma comment(linker, "/alternatename:?data_ov100_02148704@@3EA=_data_ov100_02148704")
#pragma comment(linker, "/alternatename:?data_ov100_02148204@@3PADA=_data_ov100_02148204")
#pragma comment(linker, "/alternatename:?data_ov100_02148204@@3PAUElem@@A=_data_ov100_02148204")
#pragma comment(linker, "/alternatename:?UnloadKeyModels@@YAXH@Z=_UnloadKeyModels")

/* gate 22 round 1: the Player and Camera door entry points. Each is a real
   C++ method in its own TU and each caller spells it the Itanium way. These
   were link aliases here ("the two names are the same body and neither side
   needs a thunk") -- WRONG on 32-bit MSVC: the method is __thiscall (this in
   ecx, callee pops stack args) while the Itanium-name callers are cdecl
   (self on the stack, caller pops). Every call through an alias ran with
   garbage `this`, and the one-stack-arg methods also unbalanced ESP by 4:
   the 2026-08-07 door-open crash was func_ov100_02144730's RET landing on
   its own door argument after GoBehindPlayer's `ret 4`. The whole ring is
   real cdecl faces in hal/method_faces.cpp now, LookAtExit and
   TryExitWhiteDoorWithStar included. */
#pragma comment(linker, "/alternatename:_func_020ca78c=_func_ov002_020ca78c")
/* ...and the rest of the ring those two pulled in. */
#pragma comment(linker, "/alternatename:?data_020873dc@@3HA=_data_020873dc")
#pragma comment(linker, "/alternatename:?data_0208742c@@3HA=_data_0208742c")
#pragma comment(linker, "/alternatename:?data_0209b0e8@@3HA=_data_0209b0e8")
#pragma comment(linker, "/alternatename:?func_0200c66c@@YAXPAX0PAH11@Z=_func_0200c66c")
#pragma comment(linker, "/alternatename:?func_0200cb58@@YAXPAXH@Z=_func_0200cb58")
#pragma comment(linker, "/alternatename:?data_ov002_020ff480@@3PAPAUSharedFilePtr@@A=_data_ov002_020ff480")
#pragma comment(linker, "/alternatename:?data_02092128@@3PAEA=_data_02092128")
#pragma comment(linker, "/alternatename:?data_0209caa0@@3PAEA=_data_0209caa0")
#pragma comment(linker, "/alternatename:?_ZN10ModelAnim213Func_020162C4Eji5Fix12IiEt@@YAXPAXIHHG@Z=__ZN10ModelAnim213Func_020162C4Eji5Fix12IiEt")
#pragma comment(linker, "/alternatename:?_ZN10ModelAnim24CopyERKS_Pcj@@YAXPAX0PADI@Z=__ZN10ModelAnim24CopyERKS_Pcj")
#pragma comment(linker, "/alternatename:?func_ov002_020e6330@@YAXPAX@Z=_func_ov002_020e6330")
#pragma comment(linker, "/alternatename:?_ZN6Player12GetTalkStateEv@@YAHPAX@Z=__ZN6Player12GetTalkStateEv")
#pragma comment(linker, "/alternatename:?_ZN6Player12Unk_020c9e5cEh@@YAHPAXE@Z=__ZN6Player12Unk_020c9e5cEh")

/* gate 22 round 2: the last of the door ring (TryExitWhiteDoorWithStar is a
   face in hal/method_faces.cpp, see the round-1 note). */
#pragma comment(linker, "/alternatename:?_ZN6Player17SetNoControlStateEhih@@YAHPAXEHE@Z=__ZN6Player17SetNoControlStateEhih")

/* ---- gate 23: ov102's QUESTION_BLOCK ----------------------------------- */

/* gate 23 link ring, one shape only: C++ TUs that declare a mounted datum or
   an Itanium-named function with a type of their own get MSVC mangling for
   symbols the mount and the other TUs emit as plain C. */
#pragma comment(linker, "/alternatename:?data_ov002_0210d9a0@@3PADA=_data_ov002_0210d9a0")
#pragma comment(linker, "/alternatename:?data_ov002_0210d9b0@@3PADA=_data_ov002_0210d9b0")
#pragma comment(linker, "/alternatename:?data_ov002_0210d9c0@@3PADA=_data_ov002_0210d9c0")
#pragma comment(linker, "/alternatename:?data_ov002_0210d9d0@@3PADA=_data_ov002_0210d9d0")
#pragma comment(linker, "/alternatename:?data_ov002_0210d9d8@@3PADA=_data_ov002_0210d9d8")
#pragma comment(linker, "/alternatename:?data_ov002_0210d9e0@@3PADA=_data_ov002_0210d9e0")
#pragma comment(linker, "/alternatename:?data_ov002_0210da18@@3PADA=_data_ov002_0210da18")
#pragma comment(linker, "/alternatename:?data_ov002_0210da30@@3PADA=_data_ov002_0210da30")
#pragma comment(linker, "/alternatename:?data_ov002_0210da40@@3PADA=_data_ov002_0210da40")
#pragma comment(linker, "/alternatename:?data_ov002_0210da58@@3PADA=_data_ov002_0210da58")
#pragma comment(linker, "/alternatename:?data_ov102_0214e7d0@@3PADA=_data_ov102_0214e7d0")
#pragma comment(linker, "/alternatename:?data_ov102_0214e7d8@@3PADA=_data_ov102_0214e7d8")
#pragma comment(linker, "/alternatename:?data_ov102_0214e7e0@@3PADA=_data_ov102_0214e7e0")
#pragma comment(linker, "/alternatename:?data_ov102_0214e7e8@@3PADA=_data_ov102_0214e7e8")
#pragma comment(linker, "/alternatename:?data_ov102_0214e7f0@@3PADA=_data_ov102_0214e7f0")
#pragma comment(linker, "/alternatename:?data_ov102_0214e7f8@@3PADA=_data_ov102_0214e7f8")
#pragma comment(linker, "/alternatename:?data_ov102_0214e800@@3PADA=_data_ov102_0214e800")
#pragma comment(linker, "/alternatename:?data_ov102_0214e808@@3PADA=_data_ov102_0214e808")
#pragma comment(linker, "/alternatename:?_ZN16MeshColliderBase6EnableEP5Actor@@YAXPAX0@Z=__ZN16MeshColliderBase6EnableEP5Actor")
#pragma comment(linker, "/alternatename:?_ZN5Actor9UpdatePosEP12CylinderClsn@@YAXPAX0@Z=__ZN5Actor9UpdatePosEP12CylinderClsn")
#pragma comment(linker, "/alternatename:?_ZN9Animation7AdvanceEv@@YAXPAX@Z=__ZN9Animation7AdvanceEv")
#pragma comment(linker, "/alternatename:?func_02039394@@YAXPAHH@Z=_func_02039394")
#pragma comment(linker, "/alternatename:?func_020393a4@@YAXPAHH@Z=_func_020393a4")
#pragma comment(linker, "/alternatename:?func_ov102_02149df0@@YAXPAX@Z=_func_ov102_02149df0")
#pragma comment(linker, "/alternatename:?func_ov102_02149e38@@YAXPAD@Z=_func_ov102_02149e38")
#pragma comment(linker, "/alternatename:?func_ov102_02149ea4@@YAXPAX@Z=_func_ov102_02149ea4")
#pragma comment(linker, "/alternatename:?func_ov102_02149ff0@@YAXPAD@Z=_func_ov102_02149ff0")
/* One TU declares RaycastGround::SetObjAndPos's second parameter as void*
   where every other one spells it Actor*; the same body, one mangling apart. */
#pragma comment(linker, "/alternatename:?SetObjAndPos@RaycastGround@@QAEXABUVector3@@PAX@Z=?SetObjAndPos@RaycastGround@@QAEXABUVector3@@PAUActor@@@Z")

// gate 25, the bottom screen: LoadFont.cpp and Stage::CheckCameraInput both
// declare their globals outside extern "C", so MSVC emits C++ manglings for
// what are C-named symbols everywhere else in the port.
#pragma comment(linker, "/alternatename:?data_0209d698@@3EA=_data_0209d698")
#pragma comment(linker, "/alternatename:?data_0209d6f8@@3HA=_data_0209d6f8")
#pragma comment(linker, "/alternatename:?data_0209d660@@3EA=_data_0209d660")
#pragma comment(linker, "/alternatename:?data_0209d6d4@@3FA=_data_0209d6d4")
#pragma comment(linker, "/alternatename:?data_0209f350@@3PAEA=_data_0209f350")
#pragma comment(linker, "/alternatename:?data_0209f368@@3PAGA=_data_0209f368")
#pragma comment(linker, "/alternatename:?data_020a0de8@@3PAUTouchInfo@@A=_data_020a0de8")
#pragma comment(linker, "/alternatename:?data_0209f498@@3PAUCamInput@@A=_data_0209f498")
#pragma comment(linker, "/alternatename:?data_0209f318@@3PAEA=_data_0209f318")
#pragma comment(linker, "/alternatename:?data_ov002_02111180@@3EA=_data_ov002_02111180")
// Stage::CheckCameraInput is a real MSVC static member; walk_window reaches
// it by the Itanium name every other caller in the port uses. Same signature,
// same cdecl, no arguments -- the alias is exact.
#pragma comment(linker, "/alternatename:__ZN5Stage16CheckCameraInputEv=?CheckCameraInput@Stage@@SAXXZ")
// LoadFont declares GX/GXS::LoadBGPltt as MSVC static members; their TUs
// define the Itanium C name. Same cdecl, same three arguments.
#pragma comment(linker, "/alternatename:?LoadBGPltt@GX@@SAXPBXII@Z=__ZN2GX10LoadBGPlttEPKvjj")
#pragma comment(linker, "/alternatename:?LoadBGPltt@GXS@@SAXPBXII@Z=__ZN3GXS10LoadBGPlttEPKvjj")
/* ---- gate 26: the boot spine ----------------------------------------------
   Stage::RenderModel declares its two engine globals OUTSIDE extern "C", so
   the TU emits C++-decorated references while the definitions are C-linkage:
   data_020755d4 is the {125,125,125} render scale romdata.py emits, and
   data_0209f340 is the current LVL_Overlay pointer in hal/actor_vtables.cpp.
   Both are plain storage with no ABI to get wrong, so an alias is right here
   (unlike Model::LoadAndSetFile, which is a __thiscall member and needs a
   real face -- see hal/method_faces.cpp). */
#pragma comment(linker, "/alternatename:?data_020755d4@@3DA=_data_020755d4")
#pragma comment(linker, "/alternatename:?data_0209f340@@3PAUInfo@@A=_data_0209f340")

/* ---- the tree-grab chain (slice_gate10 tail) -------------------------------
   func_02014f5c calls the grab test by its arm9-side name; the definition is
   the ov002 TU. Plain C name to C name, cdecl both sides. */
#pragma comment(linker, "/alternatename:_func_020caf98=_func_ov002_020caf98")
/* func_ov002_020caf98.cpp declares its Player externs without extern "C" and
   its State nested in Player, so every reference lands on the nested-State
   spelling. The four records are the same C-named storage the rest of the
   state family already aliases (the line-460 precedent). */
#pragma comment(linker, "/alternatename:?data_ov002_0211013c@@3UState@Player@@A=_data_ov002_0211013c")
#pragma comment(linker, "/alternatename:?data_ov002_0211031c@@3UState@Player@@A=_data_ov002_0211031c")
#pragma comment(linker, "/alternatename:?data_ov002_021101b4@@3UState@Player@@A=_data_ov002_021101b4")
#pragma comment(linker, "/alternatename:?data_ov002_021106dc@@3UState@Player@@A=_data_ov002_021106dc")
/* Return-type-only variant of the nested-State ChangeState face in
   hal/reverse_bridges.cpp -- __thiscall, same argument, result in EAX, and
   the one call site discards it (the line-616 precedent). */
#pragma comment(linker, "/alternatename:?ChangeState@Player@@QAEHAAUState@1@@Z=?ChangeState@Player@@QAEXAAUState@1@@Z")

/* ---- after the 2026-08-03 main merge --------------------------------------
   Main's class-rename and mangled-declaration waves changed which spelling a
   slice TU reaches a symbol by. Three of those are new C++ manglings over
   storage the port already hosts under the plain name -- the usual shape, one
   line each. St_LevelEnter_Main gained a member-function-pointer array type
   for the state table and an `Obj *` for the camera; the fader wipe starters
   that main now compiles (4eae13f3b) spell the wipe array as `FaderWipe *`. */
#pragma comment(linker, "/alternatename:?data_ov002_0211075c@@3PAP8C@@AEXXZA=_data_ov002_0211075c")
#pragma comment(linker, "/alternatename:?data_0209f318@@3PAUObj@@A=_data_0209f318")
#pragma comment(linker, "/alternatename:?data_0209f324@@3PAUFaderWipe@@A=_data_0209f324")

/* ActorBase::MarkForDestruction under its ad-hoc C name. The C++-mangled
   spelling was already aliased above (line ~987); the sweep moved
   func_ov100_0214109c onto the plain one, so it needs the same target. */
#pragma comment(linker, "/alternatename:_ActorBase_MarkForDestruction=__ZN9ActorBase18MarkForDestructionEv")

/* G1, the shared-header placeholder, is now inside decl_common.h's extern "C"
   block, so it arrives as a plain C name rather than ?G1@@3PAHA. Same target
   as the mangled alias above: SignPost::InitResources loads its KCL through
   data_ov002_0210e05c and CleanupResources releases G1, so the pair has to
   land on one object.

   G0/G1 are single global names, so every TU that uses them collapses onto
   one target, and this alias binds that one target to SignPost's KCL. Only
   SignPost may spell its files G0/G1 now: MetalNet::CleanupResources was the
   other user in the port's slices and, before the src-side fix, its G0/G1
   collapsed here too, so it released SignPost's two file pointers instead of
   its own (the CannonHatch disease). That is FIXED at the source now --
   src/_ZN8MetalNet16CleanupResourcesEv.cpp releases its own
   data_ov009_02113e90 / data_ov009_02113e88 by name, so it no longer reaches
   G0/G1 and this alias serves SignPost alone. The alias stays because SignPost
   still needs it. */
#pragma comment(linker, "/alternatename:_G1=_data_ov002_0210e05c")

/* ---- gate 40: STAR_DOOR's InitResources data references --------------------
   _ZN4Door13InitResourcesEv.cpp declares its four data symbols with plain
   `extern` (C++ linkage) and as arrays, so MSVC mangles them to names the C
   mount and host globals do not carry. Each lands on the same C object every
   other reader uses: data_ov100_02148934 / data_ov100_02148974 are the star
   door's own ov100 records (mount), data_0209f250 the local-player index
   (level_boot), data_0209f394 the local-player array (cxxname_bridge). Same
   shape as the ov100 aliases at ~line 1048 above. */
#pragma comment(linker, "/alternatename:?data_ov100_02148934@@3USharedFilePtr@@A=_data_ov100_02148934")
#pragma comment(linker, "/alternatename:?data_ov100_02148974@@3PAHA=_data_ov100_02148974")
#pragma comment(linker, "/alternatename:?data_0209f250@@3PAEA=_data_0209f250")
#pragma comment(linker, "/alternatename:?data_0209f394@@3PAHA=_data_0209f394")
/* the star door's eight callbacks reach two more, one per spelling: the
   camera-relative geometry base (func_ov100_02145988) and the door's own
   0x50-tagged CLPS row test (func_ov100_02145b10). data_ov100_02148390 is
   mounted; data_020a0ebc is auto_bss's. */
#pragma comment(linker, "/alternatename:?data_020a0ebc@@3DA=_data_020a0ebc")
#pragma comment(linker, "/alternatename:?data_ov100_02148390@@3PAUE6@@A=_data_ov100_02148390")
/* func_ov100_02145988 spells data_ov100_02148974 as a plain char, a third
   mangling of the callback table the InitResources block spells as int*. */
#pragma comment(linker, "/alternatename:?data_ov100_02148974@@3DA=_data_ov100_02148974")

/* ---- gate 60-61: ov015 platform data references (Whomp's Fortress) ---------
   MovingBarSmall::InitResources declares three of its ov015 tuning words with
   plain `extern void*` (C++ linkage: ?..@@3PAXA), so MSVC mangles them to
   names the ov015 per-symbol mount does not carry. Each lands on the same C
   object the mount publishes. */
#pragma comment(linker, "/alternatename:?data_ov015_02114a64@@3PAXA=_data_ov015_02114a64")
#pragma comment(linker, "/alternatename:?data_ov015_02114a5c@@3PAXA=_data_ov015_02114a5c")
#pragma comment(linker, "/alternatename:?data_ov015_02113594@@3PAXA=_data_ov015_02113594")
/* The two platforms' InitResources thread their MovingMeshCollider through
   MeshColliderBase's two update statics, which src spells at C linkage
   (_ZN16MeshColliderBase..) but the .cpp defines as real C++ methods (MSVC
   ?..@@SAX..). Alias the C name onto the method the caller means -- the same
   shape as the STAR_DOOR method aliases above. */
#pragma comment(linker, "/alternatename:__ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_=?UpdatePosAndAngs@MeshColliderBase@@SAXAAU1@PAUActor@@AAUClsnResult@@AAUVector3@@PAUVector3_16@@4@Z")
#pragma comment(linker, "/alternatename:__ZN16MeshColliderBase21UpdatePosWithVelocityERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_=?UpdatePosWithVelocity@MeshColliderBase@@SAXAAU1@PAUActor@@AAUClsnResult@@AAUVector3@@PAUVector3_16@@4@Z")
/* gate 63: func_ov015_02112c84 (RotatingPlatformWf's CleanupResources) calls
   func_ov002_020b66a8 by a name the decompiler emitted without the ov002_
   prefix; the real symbol is the ov002 one, in the slice above. */
#pragma comment(linker, "/alternatename:_func_020b66a8=_func_ov002_020b66a8")
/* func_ov002_020b676c (RotatingPlatformWf's shared Behavior) spells
   MeshColliderBase::UpdatePosAndAngs as an `extern int` -- a DATA mangling
   (?..@@3HA) of the same function the MovingBar path spells as a call. Land it
   on the same MSVC method. */
#pragma comment(linker, "/alternatename:?_ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_@@3HA=?UpdatePosAndAngs@MeshColliderBase@@SAXAAU1@PAUActor@@AAUClsnResult@@AAUVector3@@PAUVector3_16@@4@Z")
/* gate 72: func_ov091_02133254 (the THWOMP's InitResources helper) spells the
   same static as an `extern void *` -- the ?..@@3PAXA data mangling of the
   function whose address it stores through func_020393d4. Land it on the same
   MSVC method as the int form above. */
#pragma comment(linker, "/alternatename:?_ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_@@3PAXA=?UpdatePosAndAngs@MeshColliderBase@@SAXAAU1@PAUActor@@AAUClsnResult@@AAUVector3@@PAUVector3_16@@4@Z")
/* gate 74: SlidingPlatformWf::InitResources threads its collider through
   MeshColliderBase::UpdatePosWithTransform, the third of the three update
   statics; the src spells the C name and the .cpp defines the MSVC method
   (slice_gate59), the same shape as UpdatePosAndAngs above. */
#pragma comment(linker, "/alternatename:__ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_=?UpdatePosWithTransform@MeshColliderBase@@SAXAAU1@PAUActor@@AAUClsnResult@@AAUVector3@@PAUVector3_16@@4@Z")
/* gate 74: SlidingPlatformWf::InitResources declares five of its ov091
   construction-data tables with C++ types (SFP*, char*, u16*), so MSVC mangles
   the references to names the ov091 per-symbol mount does not carry. Each lands
   on the same C object the mount publishes -- the ov015 platform-data case. */
#pragma comment(linker, "/alternatename:?data_ov091_02135024@@3PAUSFP@@A=_data_ov091_02135024")
#pragma comment(linker, "/alternatename:?data_ov091_02135028@@3PADA=_data_ov091_02135028")
#pragma comment(linker, "/alternatename:?data_ov091_0213502c@@3PADA=_data_ov091_0213502c")
#pragma comment(linker, "/alternatename:?data_ov091_02134514@@3PAGA=_data_ov091_02134514")
#pragma comment(linker, "/alternatename:?data_ov091_02134504@@3PAGA=_data_ov091_02134504")
/* gate 73: RotatingUpDownPlatformUtm's Init and Clean declare their two SFP
   tables with C++ types -- void** in Init, SFP* in Clean, two manglings of the
   same mounted object -- and Behavior spells the arm9 Matrix4x3 scratch
   data_020a0e68 as char* (a third mangling of the same host array). */
#pragma comment(linker, "/alternatename:?data_ov091_02134c30@@3PAPAXA=_data_ov091_02134c30")
#pragma comment(linker, "/alternatename:?data_ov091_02134c30@@3PAUSFP@@A=_data_ov091_02134c30")
#pragma comment(linker, "/alternatename:?data_ov091_02134c34@@3PAPAXA=_data_ov091_02134c34")
#pragma comment(linker, "/alternatename:?data_ov091_02134c34@@3PAUSFP@@A=_data_ov091_02134c34")
#pragma comment(linker, "/alternatename:?data_020a0e68@@3PADA=_data_020a0e68")
/* gates 70-71: the two piranhas' Init/spit closures declare four ov084 statics
   with C++ types (SharedFilePtr tables as void**, a byte table, the arm9
   Matrix4x3 scratch as int*), so MSVC mangles the references to names the ov084
   mount does not carry. Each lands on the same C object the mount publishes. */
#pragma comment(linker, "/alternatename:?data_ov084_02130e14@@3PAPAXA=_data_ov084_02130e14")
#pragma comment(linker, "/alternatename:?data_ov084_02130e04@@3PAPAXA=_data_ov084_02130e04")
#pragma comment(linker, "/alternatename:?data_ov084_02130e0c@@3PAPAXA=_data_ov084_02130e0c")
#pragma comment(linker, "/alternatename:?data_ov084_02130294@@3PAEA=_data_ov084_02130294")
#pragma comment(linker, "/alternatename:?data_020a0e68@@3PAHA=_data_020a0e68")
/* gate 70: func_ov084_0212fc10 (a piranha state) calls ModelAnim::SetAnim by
   its unprefixed C name func_02016748; the real symbol is the arm9 method, in
   the build since gate 7. */
#pragma comment(linker, "/alternatename:_func_02016748=__ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj")
/* gate 191: func_ov018_0211123c (SkiLift's own OnHitByMegaChar) declares a
   LOCAL `struct Platform { void KillByMegaChar(Player &); };` and calls it
   through a qualified, non-virtual dispatch. Platform::KillByMegaChar is a
   real method per include/Platform.h, but its matched body
   (src/_ZN8Platform14KillByMegaCharER6Player.c, already in the build since
   gate 64) is plain C linkage -- the flat ROM name, not MSVC's re-mangling
   of the local declaration. Alias the mangled name the local struct produces
   onto the real C body. */
#pragma comment(linker, "/alternatename:?KillByMegaChar@Platform@@QAEXAAUPlayer@@@Z=__ZN8Platform14KillByMegaCharER6Player")
/* gate 191: src/_ZN7SkiLift6RenderEv.cpp (MotherPenguin's own Render, under
   the SkiLift misnomer) declares a LOCAL `struct Model { void Render(Vector3
   const *); };` -- non-virtual -- and calls it through a qualified dispatch.
   The real Model::Render (include/Model.h) IS virtual, so its matched body
   (src/_ZN5Model6RenderEPK7Vector3.cpp, already in the build since gate 4b/33)
   mangles as the VIRTUAL-qualifier form (?Render@Model@@UAEX...), while the
   local non-virtual declaration's call site wants the NON-VIRTUAL-qualifier
   form (?Render@Model@@QAEX...) -- same method, same body, only the
   virtual/non-virtual qualifier letter (U vs Q) differs in the mangling.
   Alias the call site's spelling onto the real one. */
#pragma comment(linker, "/alternatename:?Render@Model@@QAEXPBUVector3@@@Z=?Render@Model@@UAEXPBUVector3@@@Z")
/* gate 192: MrBlizzard's InitResources/Behavior (main's carried copies)
   declare Actor::Spawn under the mangled name
   _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as (a signed-char/short tail
   param spelling), extern "C" -- a different FLAT symbol than the one the
   port already hosts, _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii (in the
   build since gate 10). Actor::Spawn is cdecl-static with no `this` to
   lose, so this is the same real ROM function under a second C-linkage
   spelling -- the Enemy::SpawnCoin/PowerStarCreate precedent applied to a
   second parameter-type spelling instead of a second declaring TU. */
#pragma comment(linker, "/alternatename:__ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as=__ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii")
/* gate 192: several ov081 files declare data_ov081_* externs at file scope
   OUTSIDE any extern "C" block (MrBlizzard's InitResources/Behavior, and
   three helper functions), so MSVC C++-mangles the references -- the
   ov084/piranha treatment. Each mounted symbol (port/ov081_syms.txt, plain
   C-linkage unsigned char arrays) is aliased under every C++-mangled
   spelling a caller's local type produces for it. Two different callers
   spell data_ov081_02128998's tag differently (Vec3 vs Vector3) and
   data_ov081_02128d98's differently (S2 vs void*) -- both spellings alias
   onto the one real mount. */
#pragma comment(linker, "/alternatename:?data_ov081_02128d90@@3PAXA=_data_ov081_02128d90")
#pragma comment(linker, "/alternatename:?data_ov081_02128d98@@3PAXA=_data_ov081_02128d98")
#pragma comment(linker, "/alternatename:?data_ov081_02128d98@@3US2@@A=_data_ov081_02128d98")
#pragma comment(linker, "/alternatename:?data_ov081_02128d88@@3PAXA=_data_ov081_02128d88")
#pragma comment(linker, "/alternatename:?data_ov081_02128da0@@3PAXA=_data_ov081_02128da0")
#pragma comment(linker, "/alternatename:?data_ov081_02128998@@3UVec3@@A=_data_ov081_02128998")
#pragma comment(linker, "/alternatename:?data_ov081_02128998@@3UVector3@@A=_data_ov081_02128998")
#pragma comment(linker, "/alternatename:?data_ov081_02128e54@@3PAXA=_data_ov081_02128e54")
#pragma comment(linker, "/alternatename:?data_ov081_02128e84@@3PAXA=_data_ov081_02128e84")
#pragma comment(linker, "/alternatename:?data_ov081_02128e24@@3PAXA=_data_ov081_02128e24")
#pragma comment(linker, "/alternatename:?data_ov081_02128db8@@3HA=_data_ov081_02128db8")
#pragma comment(linker, "/alternatename:?data_ov081_02128e44@@3PADA=_data_ov081_02128e44")
#pragma comment(linker, "/alternatename:?data_ov081_02128e64@@3PADA=_data_ov081_02128e64")
/* gate 192: MrBlizzard's InitResources declares
   _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj
   OUTSIDE any extern "C" block (right after the block that closes above
   its own declaration), so MSVC mangles the call site's reference. The
   real body (src/..., a plain .c file, C linkage by default, in the
   build since gate 10) is the flat name. */
#pragma comment(linker, "/alternatename:?_ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj@@YAXPAX0PBXHHII@Z=__ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj")
/* gate 193: func_ov072_02120a08.cpp (daBgSnwmn_c::Behavior, matched src)
   declares data_ov072_02122c70 as `extern const Vector3`, which MSVC
   mangles WITH the const qualifier; ovdata.py's generated host array is a
   flat C u8[12] under the plain name. */
#pragma comment(linker, "/alternatename:?data_ov072_02122c70@@3UVector3@@B=_data_ov072_02122c70")
/* gate 193: src/_ZN11BabyPenguinD0Ev.c spells the class's own table as
   _ZTV9daPgBby_c (a plain C-linkage name, dsd's RTTI-derived spelling),
   the same address as hal/actor_classes_ov072.cpp's own host array
   _ZTV11BabyPenguin. */
#pragma comment(linker, "/alternatename:__ZTV9daPgBby_c=__ZTV11BabyPenguin")
/* gate 193: func_ov072_02120e50.cpp (daBgSnwmn_c's own Behavior-adjacent
   penguin-catch helper, matched src) calls Player::TryGrab through a
   LOCAL shadow class declaring it `bool TryGrab(Actor&)` -- MSVC mangles
   the RETURN TYPE in, producing ?TryGrab@Player@@QAE_NAAUActor@@@Z (_N =
   bool). The real body (src/_ZN6Player7TryGrabER5Actor.cpp, gate 10,
   already in the build) is extern "C", flat, returns plain int -- same
   ABI-level truthy return, different label only. */
#pragma comment(linker, "/alternatename:?TryGrab@Player@@QAE_NAAUActor@@@Z=__ZN6Player7TryGrabER5Actor")
/* gate 193: func_ov072_021218dc.cpp (BabyPenguin's own state-machine
   body, matched src) declares data_ov072_02122cac/02122ca4 as `extern
   void*[]` OUTSIDE any extern "C" block, so MSVC mangles them; ovdata.py's
   generated host arrays are flat C u8[] under the plain names. */
#pragma comment(linker, "/alternatename:?data_ov072_02122cac@@3PAPAXA=_data_ov072_02122cac")
#pragma comment(linker, "/alternatename:?data_ov072_02122ca4@@3PAPAXA=_data_ov072_02122ca4")
/* fix round (SIG-RP order): src/func_ov016_02112ae4.c carried wholesale
   from main (hash-verified), which spells RockPillar's two SharedFilePtrs
   by their friendly main-side names; this worktree's ov016 mount predates
   the rename and mounts the raw address names. Mapping verified against
   main's own config/arm9/overlays/ov016/symbols.txt (ClsnFile=0x02114e1c,
   ModelFile=0x02114e24). */
#pragma comment(linker, "/alternatename:_RockPillar_ClsnFile=_data_ov016_02114e1c")
#pragma comment(linker, "/alternatename:_RockPillar_ModelFile=_data_ov016_02114e24")
/* gate 194: the ov072_02122cac shape a third time. src/_ZN10HootTheOwl13
   InitResourcesEv.cpp declares data_ov094_02136b40 as `extern void*`
   OUTSIDE any extern "C" block; src/func_ov094_021359d8.cpp (a state
   handler, plain matched src) declares data_ov094_02136ae8/02136af8 as
   `extern void**` and data_ov094_02136b30 as `extern void*`, also outside
   extern "C". All four are state cells the ov094 per-symbol mount already
   publishes as flat C symbols under the plain names. */
#pragma comment(linker, "/alternatename:?data_ov094_02136b40@@3PAXA=_data_ov094_02136b40")
#pragma comment(linker, "/alternatename:?data_ov094_02136ae8@@3PAPAXA=_data_ov094_02136ae8")
#pragma comment(linker, "/alternatename:?data_ov094_02136af8@@3PAPAXA=_data_ov094_02136af8")
#pragma comment(linker, "/alternatename:?data_ov094_02136b30@@3PAXA=_data_ov094_02136b30")

/* gate 200 (PUSH_BLOCK 306 + MUGEN_BGM 351, ov002): the same shape.
   PushBlock::InitResources declares data_ov002_0210d9d0/0210d9b0 as
   `extern void**` outside extern "C"; MugenBgm's two real methods declare
   _Znwj, Fog::Init and the arm9 cells data_0209f394/data_020a0ebc the same
   way. The C-named definitions all exist (the ov002 mount, the hosted arm9
   data, the operator-new seam, the sliced Fog::Init). */
#pragma comment(linker, "/alternatename:?data_ov002_0210d9d0@@3PAPAXA=_data_ov002_0210d9d0")
#pragma comment(linker, "/alternatename:?data_ov002_0210d9b0@@3PAPAXA=_data_ov002_0210d9b0")
#pragma comment(linker, "/alternatename:?_Znwj@@YAPAXI@Z=__Znwj")
#pragma comment(linker, "/alternatename:?_ZN3Fog4InitEt5Fix12IiES1_@@YAXPAXGHH@Z=__ZN3Fog4InitEt5Fix12IiES1_")
#pragma comment(linker, "/alternatename:?data_0209f394@@3PAPADA=_data_0209f394")
#pragma comment(linker, "/alternatename:?data_020a0ebc@@3PAHA=_data_020a0ebc")
#pragma comment(linker, "/alternatename:?data_ov002_0211028c@@3UState@@A=_data_ov002_0211028c")
#pragma comment(linker, "/alternatename:?data_ov002_0211004c@@3HA=_data_ov002_0211004c")
