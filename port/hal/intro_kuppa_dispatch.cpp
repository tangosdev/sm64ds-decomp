/* THE KUPPA SCRIPT'S TWO COMMAND DISPATCHERS.
 *
 * THE FIRST ONE IS THE ROM'S OWN CODE NOW (run link100, lane SMALLS, rung 2):
 * src/func_ov002_020bd664.cpp is on port/slice_smalls.txt and this file no
 * longer defines that symbol. What is left of it here is a SEAT --
 * port_kuppa_cmd_seat below -- and fourteen __fastcall faces. The banner that
 * follows is the host copy's, kept because it is the derivation the seat rests
 * on; the section headed RUNG 2 further down says what changed and why.
 *
 * ---- the host copy's banner, as it stood --------------------------------
 *
 * HOST COPY of src/func_ov002_020bd664.cpp -- the kuppa script's COMMAND
 * DISPATCHER, read as a plain function pointer instead of an mwcc
 * pointer-to-member.
 *
 * THE GATE-16 CASE AGAIN (LakituBro_Behavior.cpp, RabbitKey_StateSeat.cpp,
 * SignPost_StateDispatch.cpp are the precedents). The matched source is:
 *
 *     struct C;                                    // INCOMPLETE
 *     typedef void (C::*PMF)(unsigned char*, int, int);
 *     extern PMF data_ov002_0210a14c;  ... fourteen of them ...
 *     void func_ov002_020bd664(C* self, unsigned char* p, int a2, int a3) {
 *         static PMF table[14] = { data_ov002_0210a14c, ... };
 *         (self->*table[p[6]])(p + 7, a2, a3);
 *     }
 *
 * mwcc lays a pointer-to-member out as two words, {function, delta}. MSVC forms
 * a PMF over an INCOMPLETE class as the four-word GENERAL representation, so
 * each `table[i]` initialiser reads sixteen bytes out of an eight-byte ROM
 * record -- the slot and its neighbour -- and the call then uses neighbour data
 * as its `this` adjustment. p[6] is also indexed with no bound of any kind.
 *
 * WHAT THIS DOES INSTEAD. It reads the ROM's own two-word record, checks the
 * function word against the address its host body was compiled from, refuses on
 * a disagreement, and calls the host body. The DS addresses in ov002 .data are
 * never rewritten -- unlike the LakituBro seat, nothing else consumes these
 * fourteen slots, so resolving at dispatch is enough and leaves the mounted
 * bytes exactly as the ROM shipped them. That also means there is no seat
 * ordering to get wrong: the first command the script runs resolves the table.
 *
 * WHY EVERY HANDLER IS DECLARED WITH THE SAME FOUR PARAMETERS even though the
 * bodies take one or two: the ROM's typedef IS that four-word call, and on x86
 * __cdecl the CALLER cleans the stack, so handing a body more words than it
 * reads is safe and is exactly what the ROM does on ARM with its argument
 * registers. This is the safe direction of an arity difference -- the direction
 * port/tools/aritycheck_plainfunc_baseline.txt polices is a declaration taking
 * FEWER words than the definition, which is the one that reads a slot nobody
 * wrote.
 *
 * PORT_HOST_ABI: mwcc pointer-to-member dispatch; MSVC's PMF over an incomplete
 * class is the wider general representation. See the header.
 */
#include <cstdio>
#include <cstdlib>

/* The ROM's record: {function address, this-adjustment}. */
struct PortPmf { unsigned fn; int delta; };

extern "C" {

/* The fourteen command bodies, ov002, all matched and offered through
   port/slice_intro.txt. Declared with the ROM's own four-word call shape (see
   the header) rather than each body's narrower one. */
typedef void (*KuppaCmd)(void *self, unsigned char *p, int a2, int a3);

void func_ov002_020bccfc(void *, unsigned char *, int, int);
void func_ov002_020bcd18(void *, unsigned char *, int, int);
void func_ov002_020bcd38(void *, unsigned char *, int, int);
void func_ov002_020bd20c(void *, unsigned char *, int, int);
void func_ov002_020bd354(void *, unsigned char *, int, int);
void func_ov002_020bd3a0(void *, unsigned char *, int, int);
void func_ov002_020bd438(void *, unsigned char *, int, int);
void func_ov002_020bd45c(void *, unsigned char *, int, int);
void func_ov002_020bd4ac(void *, unsigned char *, int, int);
void func_ov002_020bd4c8(void *, unsigned char *, int, int);
void func_ov002_020bd4e0(void *, unsigned char *, int, int);
void func_ov002_020bd600(void *, unsigned char *, int, int);
/* HOST COPY, unmatched/Ov002_KuppaCmd_020bd480.cpp -- see the trap note
   below. Same four-word declaration as the other twelve. */
void func_ov002_020bd480(void *, unsigned char *, int, int);
void func_ov002_020bd250(void *, unsigned char *, int, int);

/* The fourteen {function, delta} statics, in the ROM's own table order (the
   order func_ov002_020bd664's initialiser lists them, which is the command
   index order -- NOT address order). Storage is the ov002 .data mount. */
extern PortPmf data_ov002_0210a14c[], data_ov002_0210a124[],
    data_ov002_0210a054[], data_ov002_0210a0dc[], data_ov002_0210a534[],
    data_ov002_0210a0b4[], data_ov002_0210a094[], data_ov002_0210a40c[],
    data_ov002_0210a474[], data_ov002_0210a44c[], data_ov002_0210a3fc[],
    data_ov002_0210a3c4[], data_ov002_0210a064[], data_ov002_0210a36c[];

}  /* extern "C" */

/* ALL FOURTEEN ARE HOSTED NOW. TWO WERE TRAPPED, AND THE TRAPS ARE WHAT GOT
   THEM HOSTED -- which is the whole point of having left them loud.

     0210a534 -> func_ov002_020bd480   was "needs Sound::PlayCharVoice"
     0210a44c -> func_ov002_020bd250   was "needs Player::Hurt"

   Both were declared unhosted while the intro bit was force-set in
   hal/level_boot.cpp and nothing in the port could reach either one. With the
   script chain running the opening reached both, each trap named its command
   and address exactly as its last line promised, and in BOTH cases the finding
   was the same: the premise had gone stale, not the command being exotic. Every
   callee either needs was already in this binary --

     Sound::PlayCharVoice            slice_gate10.txt:1225
     Player::Hurt                    slice_gate10.txt:610
     Vec3_RotateYAndTranslate        slice_gate10.txt:191
     Particle::System::NewSimple     slice_gate29.txt:83

   -- so hosting them drags in nothing. That is worth recording as a pattern
   rather than a coincidence: an "unhosted" note written when a path was
   unreachable is a claim with an expiry date, and nothing re-checks it.

   Both are HOST COPIES (unmatched/Ov002_KuppaCmd_020bd480.cpp and
   unmatched/Ov002_KuppaCmd_020bd250.cpp) rather than src lines, because each
   matched TU reaches its callees through a local shadow class or namespace
   whose MSVC mangling has no alias in hal/cxx_aliases.cpp -- and for Hurt the
   shadow makes it a THISCALL member, so `this` would have ridden in ecx.

   The trap machinery below is kept, unused. It costs nothing, and the next
   command this lane cannot host should say so the same way. */
static void kuppa_cmd_trap(unsigned idx, const char *slot, unsigned rom,
                           const char *needs)
{
    std::fprintf(stderr,
                 "UNHOSTED: kuppa command %u (slot %s -> %08x) is not hosted "
                 "-- it needs %s. The opening was not expected to reach it; "
                 "that it did is the finding.\n", idx, slot, rom, needs);
    std::abort();
}

/* command index -> {the ROM slot, the address the host body was compiled from,
   the host body}. The middle column is what makes a wrong mount loud. */
static const struct {
    PortPmf *slot;
    unsigned rom;
    KuppaCmd host;
    const char *name;
} g_cmds[14] = {
    { data_ov002_0210a14c, 0x020bd600, func_ov002_020bd600, "0210a14c" },
    { data_ov002_0210a124, 0x020bd4e0, func_ov002_020bd4e0, "0210a124" },
    { data_ov002_0210a054, 0x020bd4c8, func_ov002_020bd4c8, "0210a054" },
    { data_ov002_0210a0dc, 0x020bd4ac, func_ov002_020bd4ac, "0210a0dc" },
    { data_ov002_0210a534, 0x020bd480, func_ov002_020bd480, "0210a534" },
    { data_ov002_0210a0b4, 0x020bd45c, func_ov002_020bd45c, "0210a0b4" },
    { data_ov002_0210a094, 0x020bd438, func_ov002_020bd438, "0210a094" },
    { data_ov002_0210a40c, 0x020bd3a0, func_ov002_020bd3a0, "0210a40c" },
    { data_ov002_0210a474, 0x020bd354, func_ov002_020bd354, "0210a474" },
    { data_ov002_0210a44c, 0x020bd250, func_ov002_020bd250, "0210a44c" },
    { data_ov002_0210a3fc, 0x020bd20c, func_ov002_020bd20c, "0210a3fc" },
    { data_ov002_0210a3c4, 0x020bcd38, func_ov002_020bcd38, "0210a3c4" },
    { data_ov002_0210a064, 0x020bcd18, func_ov002_020bcd18, "0210a064" },
    { data_ov002_0210a36c, 0x020bccfc, func_ov002_020bccfc, "0210a36c" },
};

/* ---- RUNG 2: THE MATCHED TU RUNS, AND THIS IS WHAT LETS IT ----------------
 *
 * WHAT THE MATCHED TU EMITS. src/func_ov002_020bd664.cpp is
 *
 *     struct C;                                    // INCOMPLETE
 *     typedef void (C::*PMF)(unsigned char*, int, int);
 *     extern PMF data_ov002_0210a14c;  ... fourteen of them ...
 *     void func_ov002_020bd664(C* self, unsigned char* p, int a2, int a3) {
 *         static PMF table[14] = { data_ov002_0210a14c, ... };
 *         (self->*table[p[6]])(p + 7, a2, a3);
 *     }
 *
 * The banner above says MSVC forms that pointer-to-member as the four-word
 * GENERAL representation, so each initialiser would read sixteen bytes out of
 * an eight-byte ROM record. THAT PREMISE EXPIRED. port/CMakeLists.txt puts
 * /vmg AND /vmm on the whole target (block R8), and with both on sizeof(PMF)
 * is EIGHT -- the ROM's own {function, delta} pair. The emitted dispatch, read
 * out of the object (runs/link100/out/PMFB8/listings/
 * func_ov002_020bd664_nozp.asm), is
 *
 *     push a3 / push a2 / push p+7
 *     mov ecx,[table + edx*8 + 4] / add ecx,self      <- this, adjusted
 *     mov eax,[table + edx*8] / call eax
 *
 * -- stride eight, receiver in ecx, THREE stack words, and NO `add esp,12`
 * after the call: the callee cleans. That is exactly __fastcall's shape with a
 * dead second register argument, which is what the fourteen faces below are.
 * /Zp4 makes no difference to it (PMFB8 measured 0 diff lines).
 *
 * WHY A SEAT AND NOT A DISPATCH-SITE RESOLUTION. The host copy read the mounted
 * record on EVERY call and never wrote it, which is what bought it "no seat
 * ordering to get wrong". The matched TU cannot do that: MSVC copies the
 * fourteen pairs into a function-static table on the FIRST call, so by then the
 * records must already hold faces. port_kuppa_cmd_seat writes them, and the
 * harnesses call it in ov002's bring-up (tests/walk_window.cpp and
 * tests/smoke_player.cpp, beside port_cutscene_states_seat and the ov002
 * sinits) -- long before any script runs. It is idempotent and it refuses,
 * loudly, on any record that does not hold the ROM's own {address, 0} first,
 * so a mount that changes shape aborts instead of being reinterpreted.
 *
 * THE ORDERING IS SAFE HERE and it is worth saying why, because lane PMFB8
 * wrote that a static initialiser could not substitute "because the overlay
 * mount is filled at overlay-load time". These fourteen records are NOT filled
 * at load time and not filled by a sinit: ov002 is the engine overlay, its
 * .data is a generated byte array in the image (build/port/host-src/
 * ov002_data.c carries `u8 data_ov002_0210a14c[8] = { 0,214,11,2, 0,0,0,0 }`,
 * the DS address 0x020BD600 with delta 0), and port_ov002_patch's pointer pass
 * touches none of the fourteen. The seat is still called from the bring-up
 * rather than from a static initialiser, because a call in the bring-up is the
 * thing a later reader can find.
 *
 * WHAT THE UNIVERSE IS. Fourteen records, fourteen distinct code words, all
 * {code, 0}, and the ONLY references to any of them anywhere in ov002 are this
 * row's own fourteen literal-pool loads at 020bd7ec..020bd824 -- no sinit and
 * no second dispatcher can see them (runs/link100/out/PMFB8/sweep_g2_ov002.txt).
 * So the set is closed by the ROM, not by a sweep.
 *
 * ONE THING IS LOST WITH THE HOST COPY, recorded rather than papered over: its
 * `idx >= 14` refusal. The ROM indexes this table with no bound and the matched
 * TU does the same, so a script byte the tree has never seen now reads past the
 * static table instead of naming itself and aborting. The two mount properties
 * that refusal also checked are still checked -- once, at the seat, for all
 * fourteen records -- and the captured runs this rung ships are what say the
 * bytes the game actually plays stay in range.
 */

/* MSVC decorates every one of the fourteen `extern PMF` names in the matched
   TU (it is a C++ declaration of a pointer-to-member, not an extern "C" one),
   so the object asks for ?data_ov002_0210a14c@@3P8C@@AEXPAEHH@ZQ1@ while the
   ov002 mount defines the C name. Fourteen bridges, one per record; the LHS is
   defined nowhere else, so none of them can be defeated. */
#pragma comment(linker, "/alternatename:?data_ov002_0210a14c@@3P8C@@AEXPAEHH@ZQ1@=_data_ov002_0210a14c")
#pragma comment(linker, "/alternatename:?data_ov002_0210a124@@3P8C@@AEXPAEHH@ZQ1@=_data_ov002_0210a124")
#pragma comment(linker, "/alternatename:?data_ov002_0210a054@@3P8C@@AEXPAEHH@ZQ1@=_data_ov002_0210a054")
#pragma comment(linker, "/alternatename:?data_ov002_0210a0dc@@3P8C@@AEXPAEHH@ZQ1@=_data_ov002_0210a0dc")
#pragma comment(linker, "/alternatename:?data_ov002_0210a534@@3P8C@@AEXPAEHH@ZQ1@=_data_ov002_0210a534")
#pragma comment(linker, "/alternatename:?data_ov002_0210a0b4@@3P8C@@AEXPAEHH@ZQ1@=_data_ov002_0210a0b4")
#pragma comment(linker, "/alternatename:?data_ov002_0210a094@@3P8C@@AEXPAEHH@ZQ1@=_data_ov002_0210a094")
#pragma comment(linker, "/alternatename:?data_ov002_0210a40c@@3P8C@@AEXPAEHH@ZQ1@=_data_ov002_0210a40c")
#pragma comment(linker, "/alternatename:?data_ov002_0210a474@@3P8C@@AEXPAEHH@ZQ1@=_data_ov002_0210a474")
#pragma comment(linker, "/alternatename:?data_ov002_0210a44c@@3P8C@@AEXPAEHH@ZQ1@=_data_ov002_0210a44c")
#pragma comment(linker, "/alternatename:?data_ov002_0210a3fc@@3P8C@@AEXPAEHH@ZQ1@=_data_ov002_0210a3fc")
#pragma comment(linker, "/alternatename:?data_ov002_0210a3c4@@3P8C@@AEXPAEHH@ZQ1@=_data_ov002_0210a3c4")
#pragma comment(linker, "/alternatename:?data_ov002_0210a064@@3P8C@@AEXPAEHH@ZQ1@=_data_ov002_0210a064")
#pragma comment(linker, "/alternatename:?data_ov002_0210a36c@@3P8C@@AEXPAEHH@ZQ1@=_data_ov002_0210a36c")

/* ONE FACE PER CODE WORD. `this` arrives in ecx and the three script arguments
   on the stack, callee-cleaned -- __fastcall with a dead edx, the shape lane
   PMFB8 settled for the door and for Bubba. Each face forwards to the same
   __cdecl body the host dispatcher called, declared with the ROM's own
   four-word call shape for the reason the banner above gives. */
/* The census the faces keep. It is the only way to say from outside the
   process that the MATCHED dispatcher ran: a face is entered exactly when
   func_ov002_020bd664 has read a seated record and called through it. Written
   at exit, and only when SM64DS_KUPPA_LOG names a file -- the same shape lane
   MPG's SM64DS_MPG_LOG uses. */
static unsigned g_kuppa_hits[14];

#define KUPPA_FACE(ix, sym)                                                   \
    static void __fastcall kuppa_face_##sym(void *self, void *dead_edx,       \
                                            unsigned char *p, int a2, int a3) \
    {                                                                         \
        (void)dead_edx;                                                       \
        ++g_kuppa_hits[ix];                                                   \
        sym(self, p, a2, a3);                                                 \
    }

KUPPA_FACE(0,  func_ov002_020bd600)
KUPPA_FACE(1,  func_ov002_020bd4e0)
KUPPA_FACE(2,  func_ov002_020bd4c8)
KUPPA_FACE(3,  func_ov002_020bd4ac)
KUPPA_FACE(4,  func_ov002_020bd480)
KUPPA_FACE(5,  func_ov002_020bd45c)
KUPPA_FACE(6,  func_ov002_020bd438)
KUPPA_FACE(7,  func_ov002_020bd3a0)
KUPPA_FACE(8,  func_ov002_020bd354)
KUPPA_FACE(9,  func_ov002_020bd250)
KUPPA_FACE(10, func_ov002_020bd20c)
KUPPA_FACE(11, func_ov002_020bcd38)
KUPPA_FACE(12, func_ov002_020bcd18)
KUPPA_FACE(13, func_ov002_020bccfc)

static void kuppa_census_dump(void)
{
    const char *path = std::getenv("SM64DS_KUPPA_LOG");
    const char *lvl = std::getenv("SM64DS_LEVEL");
    const char *scn = std::getenv("SM64DS_SCENE");
    if (!path)
        return;
    std::FILE *f = std::fopen(path, "a");
    if (!f)
        return;
    unsigned total = 0;
    for (unsigned i = 0; i < 14; ++i)
        total += g_kuppa_hits[i];
    std::fprintf(f, "kuppa level=%s scene=%s total=%u", lvl ? lvl : "-",
                 scn ? scn : "-", total);
    for (unsigned i = 0; i < 14; ++i)
        std::fprintf(f, " %s=%u", g_cmds[i].name, g_kuppa_hits[i]);
    std::fputc('\n', f);
    std::fclose(f);
}

/* the faces in g_cmds order, which is the command index order */
static void *const g_cmd_faces[14] = {
    (void *)&kuppa_face_func_ov002_020bd600,
    (void *)&kuppa_face_func_ov002_020bd4e0,
    (void *)&kuppa_face_func_ov002_020bd4c8,
    (void *)&kuppa_face_func_ov002_020bd4ac,
    (void *)&kuppa_face_func_ov002_020bd480,
    (void *)&kuppa_face_func_ov002_020bd45c,
    (void *)&kuppa_face_func_ov002_020bd438,
    (void *)&kuppa_face_func_ov002_020bd3a0,
    (void *)&kuppa_face_func_ov002_020bd354,
    (void *)&kuppa_face_func_ov002_020bd250,
    (void *)&kuppa_face_func_ov002_020bd20c,
    (void *)&kuppa_face_func_ov002_020bcd38,
    (void *)&kuppa_face_func_ov002_020bcd18,
    (void *)&kuppa_face_func_ov002_020bccfc,
};

/* Called from ov002's bring-up in tests/walk_window.cpp and
   tests/smoke_player.cpp, beside port_cutscene_states_seat and the ov002
   sinits. Idempotent; refuses on any record the ROM did not leave as
   {its own address, 0}. */
extern "C" void port_kuppa_cmd_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < 14; ++i) {
        PortPmf *slot = g_cmds[i].slot;
        if (slot->fn != g_cmds[i].rom || slot->delta != 0) {
            std::fprintf(stderr, "FATAL: kuppa command %u (%s): the ov002 "
                         "mount holds %08x/%d, the ROM's own record says "
                         "%08x/0 -- WRONG BYTES\n",
                         i, g_cmds[i].name, slot->fn, slot->delta,
                         g_cmds[i].rom);
            std::abort();
        }
        slot->fn = (unsigned)(size_t)g_cmd_faces[i];
    }
    std::atexit(kuppa_census_dump);
    if (std::getenv("SM64DS_KUPPA_SEAT_REPORT"))
        std::printf("[kuppa] fourteen command records seated, "
                    "index 0 %08x .. index 13 %08x\n",
                    g_cmds[0].slot->fn, g_cmds[13].slot->fn);
}

/* ---- THE SECOND DISPATCHER: THE CUTSCENE OBJECTS' OWN COMMANDS ------------
 * src/func_ov002_020f7d74.cpp, reached by the other script entry point
 * (func_0200e494, where the first is reached through func_0200e5ac). Same
 * gate-16 shape, TWENTY-FOUR slots instead of fourteen, and this is the one
 * that drives the opening's cast -- with the first dispatcher seated the run
 * got straight to it and died on slot 22:
 *
 *   FAULT c0000005 accessing 020f26d4
 *     ... StartIntroCutscene -> RunKuppaScript -> ProcessKuppaScript+0x329
 *     -> func_0200e494+0x39 -> func_ov002_020f7d74+0x6a -> 0x020f26d4
 *
 * All twenty-four targets were matched and none was offered; they ride in
 * through port/slice_intro.txt. Its src line is commented out in
 * slice_gate10.txt:874, directly below the first dispatcher's.
 */
extern "C" {
void func_ov002_020f7bb8(void *, unsigned char *, int, int);
void func_ov002_020f79c0(void *, unsigned char *, int, int);
void func_ov002_020f7780(void *, unsigned char *, int, int);
void func_ov002_020f7538(void *, unsigned char *, int, int);
void func_ov002_020f7410(void *, unsigned char *, int, int);
void func_ov002_020f7384(void *, unsigned char *, int, int);
void func_ov002_020f72bc(void *, unsigned char *, int, int);
void func_ov002_020f71c4(void *, unsigned char *, int, int);
void func_ov002_020f7038(void *, unsigned char *, int, int);
void func_ov002_020f7020(void *, unsigned char *, int, int);
void func_ov002_020f6e48(void *, unsigned char *, int, int);
void func_ov002_020f6c60(void *, unsigned char *, int, int);
void func_ov002_020f6c34(void *, unsigned char *, int, int);
void func_ov002_020f6c24(void *, unsigned char *, int, int);
void func_ov002_020f6bc0(void *, unsigned char *, int, int);
void func_ov002_020f6b4c(void *, unsigned char *, int, int);
void func_ov002_020f6b28(void *, unsigned char *, int, int);
void func_ov002_020f6ae4(void *, unsigned char *, int, int);
void func_ov002_020f6ab8(void *, unsigned char *, int, int);
void func_ov002_020f6a9c(void *, unsigned char *, int, int);
void func_ov002_020f6448(void *, unsigned char *, int, int);
void func_ov002_020f63f8(void *, unsigned char *, int, int);
void func_ov002_020f26d4(void *, unsigned char *, int, int);
void func_ov002_020f26c4(void *, unsigned char *, int, int);

extern PortPmf data_ov002_0210b708[], data_ov002_0210b678[],
    data_ov002_0210b618[], data_ov002_0210b620[], data_ov002_0210b640[],
    data_ov002_0210b628[], data_ov002_0210b648[], data_ov002_0210b8c0[],
    data_ov002_0210b900[], data_ov002_0210b938[], data_ov002_0210b930[],
    data_ov002_0210b928[], data_ov002_0210b920[], data_ov002_0210b918[],
    data_ov002_0210b910[], data_ov002_0210b908[], data_ov002_0210b750[],
    data_ov002_0210b8f8[], data_ov002_0210b8f0[], data_ov002_0210b8e8[],
    data_ov002_0210b8e0[], data_ov002_0210b8d8[], data_ov002_0210b8d0[],
    data_ov002_0210b8c8[];
}

/* slot order is the ROM's own initialiser order = the command index order */
static const struct {
    PortPmf *slot;
    unsigned rom;
    KuppaCmd host;
    const char *name;
} g_objcmds[24] = {
    { data_ov002_0210b708, 0x020f7bb8, func_ov002_020f7bb8, "0210b708" },
    { data_ov002_0210b678, 0x020f79c0, func_ov002_020f79c0, "0210b678" },
    { data_ov002_0210b618, 0x020f7780, func_ov002_020f7780, "0210b618" },
    { data_ov002_0210b620, 0x020f7538, func_ov002_020f7538, "0210b620" },
    { data_ov002_0210b640, 0x020f7410, func_ov002_020f7410, "0210b640" },
    { data_ov002_0210b628, 0x020f7384, func_ov002_020f7384, "0210b628" },
    { data_ov002_0210b648, 0x020f72bc, func_ov002_020f72bc, "0210b648" },
    { data_ov002_0210b8c0, 0x020f71c4, func_ov002_020f71c4, "0210b8c0" },
    { data_ov002_0210b900, 0x020f7038, func_ov002_020f7038, "0210b900" },
    { data_ov002_0210b938, 0x020f7020, func_ov002_020f7020, "0210b938" },
    { data_ov002_0210b930, 0x020f6e48, func_ov002_020f6e48, "0210b930" },
    { data_ov002_0210b928, 0x020f6c60, func_ov002_020f6c60, "0210b928" },
    { data_ov002_0210b920, 0x020f6c34, func_ov002_020f6c34, "0210b920" },
    { data_ov002_0210b918, 0x020f6c24, func_ov002_020f6c24, "0210b918" },
    { data_ov002_0210b910, 0x020f6bc0, func_ov002_020f6bc0, "0210b910" },
    { data_ov002_0210b908, 0x020f6b4c, func_ov002_020f6b4c, "0210b908" },
    { data_ov002_0210b750, 0x020f6b28, func_ov002_020f6b28, "0210b750" },
    { data_ov002_0210b8f8, 0x020f6ae4, func_ov002_020f6ae4, "0210b8f8" },
    { data_ov002_0210b8f0, 0x020f6ab8, func_ov002_020f6ab8, "0210b8f0" },
    { data_ov002_0210b8e8, 0x020f6a9c, func_ov002_020f6a9c, "0210b8e8" },
    { data_ov002_0210b8e0, 0x020f6448, func_ov002_020f6448, "0210b8e0" },
    { data_ov002_0210b8d8, 0x020f63f8, func_ov002_020f63f8, "0210b8d8" },
    { data_ov002_0210b8d0, 0x020f26d4, func_ov002_020f26d4, "0210b8d0" },
    { data_ov002_0210b8c8, 0x020f26c4, func_ov002_020f26c4, "0210b8c8" },
};

// PORT_HOST_ABI: mwcc pointer-to-member dispatch; MSVC's PMF over an incomplete class is the wider general representation
extern "C" void func_ov002_020f7d74(void *self, unsigned char *p, int a2,
                                    int a3)
{
    const unsigned idx = p[6];
    if (idx >= 24) {
        std::fprintf(stderr, "FATAL: cutscene-object command index %u out of "
                     "range (script byte at %p)\n", idx, (void *)(p + 6));
        std::abort();
    }
    const PortPmf *slot = g_objcmds[idx].slot;
    if (slot->fn != g_objcmds[idx].rom) {
        std::fprintf(stderr, "FATAL: cutscene-object command %u (%s): the mount "
                     "holds %08x, the ROM's own table says %08x -- WRONG "
                     "BYTES\n", idx, g_objcmds[idx].name, slot->fn,
                     g_objcmds[idx].rom);
        std::abort();
    }
    if (slot->delta != 0) {
        std::fprintf(stderr, "FATAL: cutscene-object command %u (%s) is not a "
                     "plain pointer-to-member (delta %d)\n",
                     idx, g_objcmds[idx].name, slot->delta);
        std::abort();
    }
    g_objcmds[idx].host(self, p + 7, a2, a3);
}

/* ---- THREE MORE GATE-16 SITES, TRAPPED RATHER THAN MISDISPATCHED ----------
 * Seating CUTSCENE_OBJECT's closure pulled in three more pointer-to-member
 * dispatch sites, each the same shape as the command dispatcher above -- a PMF
 * over an INCOMPLETE class, indexed out of an ov002 array:
 *
 *   src/func_ov002_020f2dd4.cpp   (c->*data_ov002_02110ebc[c->idx].pmf)(0)
 *   src/func_ov002_020f37a0.cpp   (c->*data_ov002_02110e4c[c->idx].pmf)(0)
 *   src/func_ov002_020f5990.cpp   (((C*)c)->*data_ov002_02110e7c[s->f46])(i)
 *
 * All three arrays are ov002 BSS built by __sinit_ov002_02107370 out of
 * {function, delta} statics, exactly like LakituBro's eleven states. Compiled as
 * written on MSVC each index reads FOUR words out of a TWO-word record and then
 * calls through neighbour data.
 *
 * ---- link100 TAIL2 ----
 * THEY ARE HOSTED NOW, and this paragraph is what they were before. It read
 * "THEY ARE TRAPPED, NOT HOSTED, AND NOT LEFT TO MISDISPATCH", sized the work
 * at "three more seat tables plus the closure of every function those arrays
 * name -- a round of work in its own right", and noted that the arrays' target
 * bodies were not offered either.
 *
 * All three claims were right and all three are now spent. Run link100 lane
 * TAIL2 read __sinit_ov002_02107370's own assignments instead of the five
 * sites: there are TEN of these arrays, not five, they tile ov002 bss
 * contiguously at 0x02110e24..0x0211104c, and their 69 cells name 57 bodies of
 * which 54 were matched-and-unlinked. The five sites here are five of the ten
 * readers; the other five ARE state bodies that dispatch a further array.
 *
 * All ten readers, all ten tables and the 49 body TUs now live in
 * hal/cutscene_states_link100.cpp (generated from the ROM by
 * port/tools/tail2_gen_states.py, source list port/slice_gate219.txt), built
 * the same way as this file's own two dispatchers: read the ROM's two-word
 * record, check the function word, call the host body, never rewrite the
 * mount. The five definitions that stood here would collide with it, so they
 * come out; nothing else in this file changes. Their src lines stay commented
 * out in port/slice_intro.txt, because a matched TU whose body is a
 * pointer-to-member dispatch still cannot compile under MSVC.
 */
