/* SOUND_OBJECT's (daSoundObj_c, actor 359) seven-cell PMF dispatch seat and
 * its Behavior host copy, the Cap_StateDispatch treatment applied to the
 * positional sound emitter -- gate 190's fix round.
 *
 * NO HOST COPY OF THE BEHAVIOR IS LEFT -- run link100, lane PMFB3.
 *
 * WHAT THE OLD REASON SAID, and why exactly half of it was wrong. Gate 190
 * wrote: "MSVC's PMF for an incomplete class is the unknown-inheritance
 * representation (a different size entirely), so the matched TU cannot even
 * index the table correctly on the host". Lane PMFC2 measured that claim across
 * all 144 family-C rows and it does not survive /vmg /vmm, which this build has
 * had on every C++ TU since block R8: under those two options MSVC's pointer to
 * member is the 8-byte {function, delta} pair for a complete class and an
 * incomplete one alike. This TU's own /FAsc listing
 * (runs/link100/out/PMFB3/listings/SoundObject_Behavior_nozp.asm) emits
 * ?data_ov002_0211110c@@3PAP8C@@AEHPAX@ZA[eax*8] and [eax*8+4] against the
 * ROM's `add r3, r1, r0, lsl #3` at 0x020f9510 -- eight against eight.
 *
 * WHAT WAS ACTUALLY IN THE WAY, and it was two things, both fixed here:
 *   1. the code word held a DS address, which port_sound_object_states_seat
 *      below already replaced with a host address at boot -- but with a plain
 *      cdecl body, and the matched TU's dispatch is `call eax` with the
 *      receiver in ECX and the argument PUSHED under callee cleanup. So the
 *      seven installed words are __fastcall FACES now, not bare bodies.
 *   2. the port-only out-of-range refusal this file's Behavior carried. It has
 *      moved DOWN one level, into port_sound_object_behavior, the slot-6 face
 *      hal/actor_classes_ccm.cpp's vtable fill already went through, so the
 *      matched TU is what dispatches and an out-of-range param still cannot
 *      reach it.
 *
 * THE TABLE. data_ov002_0211110c is 7 bss cells x 8 bytes (0x0211110c..
 * 0x02111144, the next-symbol delta -- sel 0..6; InitResources accepts param
 * <= 7 but the ROM table has no entry 7, a latent off-table dispatch on the
 * DS too, so sel 7 is refused loudly instead of silently reading the
 * neighbouring cell). THE ROM'S GATE IS OFF BY ONE, and this lane read both
 * sides of it out of extracted/overlays/overlay_0002.bin
 * (runs/link100/out/PMFB3/rom_soundobj.txt): InitResources accepts param <= 7
 * and indexes a 0xc-stride tuning table at data_ov002_0210c080 with it, but
 * that table has SEVEN rows -- row 7 at 0x0210c0d4 reads 00000000 / 0210c048 /
 * 020f95e0, a pointer and a code address, which is the next thing in the image
 * and not a tuning row -- and the dispatch table has seven cells. A param-7
 * emitter runs off the end of both on the cartridge as well; the refusal is the
 * port declining to reproduce a latent ROM defect, and it is the only
 * port-only behaviour in this file. __sinit_ov002_02107f88 (linked at gate 10, runs with
 * ov002's boot sinits) copies the seven {function, 0} source records at
 * data_ov002_0210c010..0210c040 into the cells -- DS CODE ADDRESSES, the
 * ovdata contract. port_sound_object_states_seat rewrites each cell's fn with
 * its host body after checking it against the ROM address that body was
 * compiled from (the Cap seat shape: a mount pointing at the wrong bytes
 * aborts instead of calling into the overlay image). All seven are the
 * nonvirtual {function, 0} form; the raw ROM bytes at 0x0210c010..0x0210c048
 * confirm every delta word is 0.
 *
 * THE FIVE DISTINCT TARGETS, all matched src, all the same
 * `int fn(self, u16 *counter)` C shape:
 *   [0] 0x0200f8f8 Sound::PlaySecretSound       (arm9, slice_gate32)
 *   [1] 0x0200f874 Sound::PlaySmallSecretSound  (arm9, slice_gate41)
 *   [2] 0x0200f7f0 func_0200f7f0                (arm9, joins slice_gate190)
 *   [3] 0x020f9468 func_ov002_020f9468          (ov002, joins slice_gate190)
 *   [4] 0x020f93a8 func_ov002_020f93a8          (ov002, joins slice_gate190)
 *   [5] 0x020f9468 (shares [3])
 *   [6] 0x020f9468 (shares [3])   <- the Actor::SpawnSoundObj(6) path
 */
#include <cstdio>
#include <cstdlib>

extern "C" {
int _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned soundID, unsigned vol,
                                    unsigned pan, int dist, int loop);
void _ZN9ActorBase18MarkForDestructionEv(void *self);
int _ZN5Sound15PlaySecretSoundEP5ActorPt(void *self, unsigned short *counter);
int _ZN5Sound20PlaySmallSecretSoundEP5ActorPt(void *self,
                                              unsigned short *counter);
int func_0200f7f0(void *self, unsigned short *counter);
int func_ov002_020f9468(char *self, unsigned short *counter);
int func_ov002_020f93a8(char *self, unsigned short *counter);

extern unsigned char data_0208e430[4];   /* current music id, romdata */
extern int data_0209b49c[];              /* auto_bss */
extern int data_0209b490[];              /* auto_bss */

struct PortSoundObjPair { unsigned fn; int delta; };
extern PortSoundObjPair data_ov002_0211110c[];   /* the 7 dispatch cells */

/* the matched TU, taken off this file's host copy by lane PMFB3. Same C name,
   same one-argument cdecl signature the vtable face already called. */
int _ZN11SoundObject8BehaviorEv(char *cc);

/* the port's slot-6 body: the out-of-range refusal, then the matched TU. */
int port_sound_object_behavior(char *cc);
}

/* THE ONE ALIAS THE MATCHED TU ASKS FOR. src/_ZN11SoundObject8BehaviorEv.cpp
   declares `extern PMF data_ov002_0211110c[];` at namespace scope rather than
   inside its extern "C" block, so MSVC spells the reference with the C++
   decoration below -- read off that TU's own /FAsc listing, not guessed -- while
   the ov002 mount defines the same address and the same bytes under C linkage.
   src/ is byte-locked against mwccarm, so the source cannot be given the
   extern "C" instead. Safe under port/tools/alternatename_guard.py for
   hal/pmfc_aliases.cpp's reason: nothing in the tree DEFINES the LHS, so it
   cannot acquire a real definition and be silently defeated. */
#pragma comment(linker, "/alternatename:?data_ov002_0211110c@@3PAP8C@@AEHPAX@ZA=_data_ov002_0211110c")

enum { PORT_SOUND_OBJ_CELLS = 7 };

typedef int (*PortSoundObjFn)(char *, unsigned short *);
typedef int (__fastcall *PortSoundObjFace)(void *, void *, unsigned short *);

/* ---- THE FIVE FACES, run link100 lane PMFB3 -----------------------------
   WHY THE SEATED WORD CHANGED SHAPE. Until this lane the code word held one of
   the five plain cdecl bodies, because the dispatcher reading it was this
   file's own host copy and called it that way. The matched TU is what reads it
   now, and MSVC compiles its `(c->*data_ov002_0211110c[sel])(cc + 0xdc)` as

       lea   edi, [esi+220] / push edi                the one argument
       mov   ecx, ?data_ov002_0211110c@@...[eax*8+4]  the adjustment word
       mov   eax, ?data_ov002_0211110c@@...[eax*8]    the code word
       add   ecx, esi                                 this
       call  eax                                      and NO add esp,4 after it

   -- receiver in ecx, one argument on the stack, callee cleanup. __fastcall
   takes the first argument in ecx, ignores edx (which the adjustment word never
   moves, all seven being {code, 0} in the image), and cleans the same four
   bytes. Five faces cover the seven cells: the ROM parks 0x020f9468 in three of
   them. */
#define SOB_FACE(sym)                                                         \
    static int __fastcall sob_cell_##sym(void *self, void *dead_edx,          \
                                         unsigned short *counter)             \
    {                                                                         \
        (void)dead_edx;                                                       \
        return ((PortSoundObjFn)sym)((char *)self, counter);                  \
    }

SOB_FACE(_ZN5Sound15PlaySecretSoundEP5ActorPt)
SOB_FACE(_ZN5Sound20PlaySmallSecretSoundEP5ActorPt)
SOB_FACE(func_0200f7f0)
SOB_FACE(func_ov002_020f9468)
SOB_FACE(func_ov002_020f93a8)

#undef SOB_FACE

static const struct { unsigned rom; PortSoundObjFace face; }
g_sound_obj_states[PORT_SOUND_OBJ_CELLS] = {
    {0x0200f8f8, sob_cell__ZN5Sound15PlaySecretSoundEP5ActorPt},
    {0x0200f874, sob_cell__ZN5Sound20PlaySmallSecretSoundEP5ActorPt},
    {0x0200f7f0, sob_cell_func_0200f7f0},
    {0x020f9468, sob_cell_func_ov002_020f9468},
    {0x020f93a8, sob_cell_func_ov002_020f93a8},
    {0x020f9468, sob_cell_func_ov002_020f9468},
    {0x020f9468, sob_cell_func_ov002_020f9468},
};

extern "C" void port_sound_object_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    PortSoundObjPair *p = data_ov002_0211110c;
    for (int i = 0; i < PORT_SOUND_OBJ_CELLS; ++i) {
        if (p[i].fn != g_sound_obj_states[i].rom || p[i].delta != 0) {
            std::fprintf(stderr, "FATAL: SoundObject cell %d: the sinit left "
                         "%08x/%d, the ROM's own records say %08x/0 -- WRONG "
                         "BYTES\n", i, p[i].fn, p[i].delta,
                         g_sound_obj_states[i].rom);
            std::abort();
        }
        p[i].fn = (unsigned)(size_t)g_sound_obj_states[i].face;
    }
}

/* ---- THE PORT'S SLOT-6 BODY, run link100 lane PMFB3 ----------------------
   WHAT THIS IS AND IS NOT. It is NOT a host copy of Behavior: the body that
   runs is src/_ZN11SoundObject8BehaviorEv.cpp, the ROM's own, on
   port/slice_pmfb3.txt. This is the port-only out-of-range refusal that gate
   190 wrote INSIDE the host copy, moved to the one place it can live once the
   matched TU is what dispatches -- ahead of it, at the vtable edge that
   hal/actor_classes_ccm.cpp's fill already routed through a face (sob_init,
   sob_d1 and sob_d0 are the same shape and always were).

   WHY IT STILL HAS TO EXIST. The dispatch table is seven cells, 0x0211110c..
   0x02111143, and the ov002 mount publishes data_ov002_02111144 and
   data_ov002_02111148 as the next two symbols. A param-7 emitter makes the
   matched TU read eight bytes of somebody else's storage and call whatever word
   is in it. The refusal is the port declining to reproduce a ROM defect the
   cartridge shares (see the header: the tuning table InitResources indexes with
   the same param has seven rows too), and it is the whole of the port-only
   behaviour left in this class. */
extern "C" int port_sound_object_behavior(char *cc)
{
    int sel = *(int *)(cc + 8);
    if (sel < 0 || sel >= PORT_SOUND_OBJ_CELLS) {
        /* sel 7 passes InitResources' `> 7` gate but has no table entry --
           off-table on the DS as well (it would dispatch the neighbouring
           symbol's bytes). Loudly decline and self-destruct rather than mimic a
           garbage dispatch. */
        std::fprintf(stderr, "SOUND_OBJECT: param %d has no dispatch cell "
                     "(table is 0..6) -- destroying the emitter\n", sel);
        _ZN9ActorBase18MarkForDestructionEv(cc);
        return 1;
    }
    return _ZN11SoundObject8BehaviorEv(cc);
}
