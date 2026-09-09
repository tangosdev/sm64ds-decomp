/* ---------------------------------------------------------------------------
 * ctor_runner.cpp -- THE ROM'S OWN STATIC-INITIALISER RUNNER, AT ENTRY'S POINT.
 *
 * Run link100, lane CTOR, rung C1a. Lane CWSCOUT measured the root; this file
 * lands it.
 *
 * WHAT THE ROM DOES. Entry (arm9 0x02004800) ends with three instructions and
 * a tail jump, read out of extracted/arm9_dec.bin at base 0x02004000:
 *
 *     020048a0  bl 0x1ffafd4
 *     020048a4  bl 0x2019780        func_02019780()
 *     020048a8  bl 0x2072f94        func_02072f94()      <-- THIS ONE
 *     020048ac  ldr r1, [pc, #0x1c]     -> 0x02007000 = main
 *     020048b0  ldr lr, [pc, #0x1c]     -> 0xffff0000
 *     020048b4  bx  r1
 *
 * func_02072f94 is CodeWarrior's __init_cpp / static-initialiser runner and
 * src/func_02072f94.c is the whole of it:
 *
 *     fn_t *p = (fn_t *)0x2086b60;
 *     while (p != 0 && (f = *p) != 0) { f(); p++; }
 *
 * 0x02086b60 is the .ctor table: 23 __sinit_* addresses and a NULL terminator
 * at 0x02086bbc, confirmed both ways -- config/arm9/symbols.txt:3210-3232 names
 * the 23 words .p__sinit_<addr>, and the image itself reads:
 *
 *     02086b60: 02073a24   02086b7c: 02074e0c   02086b98: 02074fe4
 *     02086b64: 02073e6c   02086b80: 02074e44   02086b9c: 0207501c
 *     02086b68: 02074d90   02086b84: 02074e80   02086ba0: 02075054
 *     02086b6c: 02074da8   02086b88: 02074e84   02086ba4: 020750b4
 *     02086b70: 02074dbc   02086b8c: 02074edc   02086ba8: 020750b8
 *     02086b74: 02074dc0   02086b90: 02074f80   02086bac: 020750ec
 *     02086b78: 02074dc4   02086b94: 02074fb8   02086bb0: 0207511c
 *                                               02086bb4: 02075150
 *                                               02086bb8: 02075154
 *                                               02086bbc: 00000000
 *
 * WHERE THE TABLE LIVES IN THIS PORT, and why it is not just a host array.
 * The runner is a MATCHED TU and it reads a LITERAL DS ADDRESS. The port's
 * hosted DS globals are ordinary host symbols wherever the linker puts them,
 * so a host array called data_02086b60 would be invisible to that load. What
 * IS at 0x02086b60 in this process is DS main RAM: ntr/io.cpp reserves
 * 0x02000000..0x02400000 zero-filled from a TLS callback before the CRT runs,
 * which is the same mapping hal/level_boot.cpp leans on when it says a raw DS
 * pointer read "does not fault, because ntr/io.cpp reserves the DS address
 * space zero-filled". So the table is PUBLISHED there: kCtorTable below is the
 * ROM's 23 words plus the terminator with every word BOUND to the host body
 * that answers for that initialiser, and port_rom_entry_ctors() copies it to
 * 0x02086b60 one instruction before the ROM's own runner walks it.
 *
 * That is the port's romdata pointer-table contract, applied to a table the
 * port had never mounted: a relocated word carries a DS address, and a DS
 * address the port cannot execute must be bound to the host body before
 * anything dispatches through it. 0x02086b60 is arm9 .rodata (the ROM's own
 * .data does not start until 0x02086bc0), nothing else in this process writes
 * that page, and no overlay mounts anywhere near it -- the level and minigame
 * overlays start at 0x0210xxxx.
 *
 * MAIN RAM IS THE ONE RESERVED RANGE ntr/io.cpp CALLS SURVIVABLE TO LOSE, so
 * the publish is checked rather than assumed. If the page is not committed and
 * writable the runner is NOT called (a walk from an unmapped literal is an
 * access violation with no message), the table is run directly instead, and
 * the [ctor] line says so. That branch is a host safety net for a mapping the
 * port already documents as raceable; it is not the normal path and it says
 * when it is taken.
 *
 * WHERE IT IS CALLED FROM. hal/rom_main.cpp's port_rom_main_run(), at its
 * head, which tests/walk_window.cpp calls immediately after
 * port_boot_rom_pre_main() and Heap::InitializeRootHeap() -- func_02019780's
 * own body and its fourth call. So the port's order is now Entry's order:
 * func_02019780, then func_02072f94, then main. Nothing was moved to make room.
 *
 * WHAT RUNS AND WHAT DOES NOT. After rungs C1a and C1b, SIXTEEN of the 23 words
 * are bound to the ROM's own initialiser and SEVEN are bound to a FACE that
 * names the initialiser it stands for and returns. The faces do not abort: __sinit_02073a24 is word 0
 * and is the last rung of this campaign, so an aborting face there would kill
 * every boot before the first frame. They report once each, by name, and the
 * summary line counts them, which is what makes the walk complete AND honest:
 * the runner really visits all 23 words, and the run says exactly which ones
 * the port cannot answer for yet.
 *
 * THE TWO HAND CALLS ARE GONE IN THIS COMMIT. __sinit_02073e6c was called from
 * hal/level_boot.cpp's port_intro_seat_scripts() and __sinit_02074f80 from a
 * C++ static object in hal/fdr_arm9_fader_seat.cpp. Both are in the link
 * already (slice_intro.txt and slice_fdr.txt) and both are now words in this
 * table, so leaving the hand calls would have run them TWICE. Neither is
 * idempotent in the interesting direction -- __sinit_02074f80 pushes a
 * destructor node onto data_020aa3f0 through func_020731dc, so a second run
 * pushes a second node for the same object -- which is why the two words go
 * through counting shims and the [ctor] line prints both counters. One each is
 * the pass; anything else is a defect this file will say out loud.
 *
 * ORDER, stated because it is the whole risk of these rungs:
 *   * The eight initialisers rung C1a adds allocate nothing. Five have empty
 *     bodies; __sinit_02074d90 zeroes two words of data_0209b468;
 *     __sinit_02074da8 calls func_02011a5c, which zeroes data_0209b53c;
 *     __sinit_02074fb8 stores two pointers into data_020a0c68. No call to
 *     Heap::, Memory::, operator new or malloc appears anywhere in the eight
 *     or in func_02011a5c, so none of them can touch a game heap that Entry
 *     has not built yet. (The root heap DOES exist here -- walk_window calls
 *     Heap::InitializeRootHeap one line above -- and the game heap does not;
 *     neither is needed.)
 *   * All three globals are zeroed host BSS at this point in the boot and no
 *     host seat writes any of them before Entry: data_0209b468 and
 *     data_0209b53c are hal/actor_vtables.cpp storage that only actor and
 *     sound code fills, later; data_020a0c68 is hal/scene_link100_boot.cpp
 *     storage whose only ROM reader (func_0203506c) is not in this link.
 *   * The ov002 static initialisers tests/walk_window.cpp calls by hand keep
 *     their place AFTER this call, unchanged. That is the ROM's order too:
 *     ov002 is an overlay and is not resident at Entry, so its .ctor work
 *     cannot precede the arm9 table's. Nothing in the eight names an ov002
 *     symbol.
 *   * hal/level_boot.cpp still seats data_02089dcc's one code pointer at intro
 *     time. __sinit_02073e6c writes bytes 0x22..0x25, 0x3e..0x41 and
 *     0x30d..0x310 of that blob and the seat writes the word at 0x180, so the
 *     two are disjoint and the order between them cannot change a byte.
 *
 * RUNG C1b ADDS SIX MORE, and they allocate nothing either: five of the six are
 * a handful of field stores plus one func_020731dc push, and the sixth
 * (__sinit_02074dc4) additionally calls func_0203b9b4 on data_0209d4b8 and
 * Timer's constructor func_0201964c on data_0209d4c8, both of which are field
 * stores into storage hal/auto_bss.cpp already hosts. The same grep over the
 * six and over func_0201964c returns nothing for Heap, Memory, operator new or
 * malloc.
 *
 * TWO OF C1b's NINE ARE DELIBERATELY LEFT FACED and the reasons are in their
 * face messages:
 *   __sinit_02074e0c calls func_0201aa18, whose chain is
 *   func_0201aa18 -> func_0201aad4 -> func_0201aac8 -> func_02059ba0, and
 *   src/func_0201aac8.c is a tail-call veneer decompiled as `void(void)` that
 *   its caller invokes WITH an argument. On ARM the value rides r0 through the
 *   veneer; on the host it does not. Linking that at Entry would hand
 *   func_02059ba0 whatever the host left in a register. That is a PORT_HOST_ABI
 *   question, not a linkage one.
 *   __sinit_0207501c CONSTRUCTS a RaycastLine into data_020a0d0c, and
 *   hal/clsn_vtable.cpp hosts that as a 0x10 grouped-section block whose
 *   neighbours hal/mmc_vtable.cpp:148 asserts at ROM spacing. The constructor
 *   writes past +0x64. Whether that stays inside the band is a layout question
 *   owned by those two files.
 *
 * ONE ROM DATA OBJECT IS HOSTED HERE, because __sinit_02074fb8 names it and
 * nothing else in the port does: data_0208ee14, 16 bytes at arm9 .data, the
 * four-slot Scene graph-callback table. Its words are code addresses and
 * config/arm9/relocs.txt carries the relocations, so it gets the same bind
 * treatment as the .ctor table itself. Scene::GraphCallback0/1/3 are already
 * in the link; GraphCallback2 joins them because this table's third word names
 * it. Every one of the four is an eight-byte `return 1`.
 * ---------------------------------------------------------------------------
 */

#include <cstdio>
#include <cstring>

#if defined(_WIN32)
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#endif

#include "dsstate_seg.h"

extern "C" {

/* The runner itself: src/func_02072f94.c, arm9 0x02072f94. */
void func_02072f94(void);

/* The ten initialisers this build answers for. */
void __sinit_02073e6c(void);   /* was hand-called from hal/level_boot.cpp */
void __sinit_02074d90(void);
void __sinit_02074da8(void);
void __sinit_02074dbc(void);
void __sinit_02074dc0(void);
void __sinit_02074dc4(void);   /* C1b */
void __sinit_02074e44(void);   /* C1b */
void __sinit_02074e80(void);
void __sinit_02074fe4(void);   /* C1b */
void __sinit_02074f80(void);   /* was hand-called from hal/fdr_arm9_fader_seat.cpp */
void __sinit_02074fb8(void);
void __sinit_020750b4(void);
void __sinit_020750b8(void);   /* C1b */
void __sinit_020750ec(void);   /* C1b */
void __sinit_0207511c(void);   /* C1b */
void __sinit_02075150(void);

/* The MSL global-destructor chain head, hosted in hal/cxx_aliases.cpp. Every
   initialiser that constructs a static pushes a three-word node onto it
   through func_020731dc; nothing ever pops it. Its length is part of the
   [ctor] line because it is the one shape this rung changes that outlives the
   call. */
extern void *data_020aa3f0;

/* Scene's four graph callbacks, the words of data_0208ee14 below. */
int _ZN5Scene14GraphCallback0Ev(void);
int _ZN5Scene14GraphCallback1Ev(void);
int _ZN5Scene14GraphCallback2Ev(void);
int _ZN5Scene14GraphCallback3Ev(void);

}  /* extern "C" */

/* ---- data_0208ee14, the four-slot Scene graph-callback table --------------
 *
 * ROM span 0x0208ee14..0x0208ee24 (next symbol in config/arm9/symbols.txt is
 * data_0208ee24), four words, every one of them a relocation into arm9 code:
 *
 *     0208ee14  02018eb8  _ZN5Scene14GraphCallback0Ev
 *     0208ee18  02018eb0  _ZN5Scene14GraphCallback1Ev
 *     0208ee1c  02018ea8  _ZN5Scene14GraphCallback2Ev
 *     0208ee20  02018ea0  _ZN5Scene14GraphCallback3Ev
 *
 * It is hosted here and not in hal/arm9_tables_link100.cpp because the reason
 * it is needed is this rung's: src/__sinit_02074fb8.c names it, and that
 * initialiser is one of the eight. The store it appears in is dead in the ROM
 * itself (`data_020a0c68[0] = &data_0208ee14;` is overwritten by the next
 * statement with &data_02094390), so nothing in this build reads a word back
 * out of this table -- but a table of code addresses is bound to the bodies
 * whether or not this build dispatches it, because that is the contract and
 * because the next lane to link func_0203506c inherits it. */
DSSTATE_BEGIN
extern "C" void *data_0208ee14[4] = {
    (void *)&_ZN5Scene14GraphCallback0Ev,
    (void *)&_ZN5Scene14GraphCallback1Ev,
    (void *)&_ZN5Scene14GraphCallback2Ev,
    (void *)&_ZN5Scene14GraphCallback3Ev,
};
DSSTATE_END

/* ---- rung C1b: the seven destruct-node cells --------------------------
 *
 * Every one of them is the third argument of a func_020731dc call in the
 * initialiser that names it -- the three-word node src/func_020731dc.c pushes
 * onto data_020aa3f0 (node[0] = previous head, node[1] = the destructor,
 * node[2] = the object). Nothing in this port hosted any of them, and nothing
 * in this port reads one back: the ROM's only reader of the chain is
 * func_02072f3c, the exit-time walker behind MSL's exit(), which is not in the
 * link.
 *
 * SIZED BY ROM SPAN, not by the three words the writer uses, per the port's
 * undersized-globals rule: each is the delta to the next symbol in
 * config/arm9/symbols.txt.
 *
 *     data_0209d4bc  0x0209d4bc..0x0209d4c8   12   __sinit_02074dc4
 *     data_0209e654  0x0209e654..0x0209e660   12   __sinit_02074e44
 *     data_020a0ce0  0x020a0ce0..0x020a0cec   12   __sinit_02074fe4
 *     data_020a0eb0  0x020a0eb0..0x020a0ebc   12   __sinit_020750b8
 *     data_020a0ec8  0x020a0ec8..0x020a0ed0    8   __sinit_020750ec (the object)
 *     data_020a0ed0  0x020a0ed0..0x020a0edc   12   __sinit_020750ec (the node)
 *     data_020a0ee4  0x020a0ee4..0x020a0ef0   12   __sinit_0207511c
 *
 * FOUR OF THE SEVEN ARE NOT COVERED BY ANY OTHER HOST OBJECT AT ALL. The other
 * three (data_0209d4bc, data_0209e654, data_020a0ed0) fall inside the nominal
 * span of one of hal/auto_bss.cpp's generic `int[8]` neighbours -- that file's
 * generous default, which it itself narrows to the ROM span wherever a ROM
 * reader cross-reads a block (see its data_020a0dbc note). Nothing cross-reads
 * these: the writer is func_020731dc and the reader is not in the link, so the
 * generosity is slack rather than aliasing and a byte-exact host beside it is
 * the honest shape rather than a second name for live storage. */
DSSTATE_BEGIN
extern "C" {
unsigned char data_0209d4bc[12];
unsigned char data_0209e654[12];
unsigned char data_020a0ce0[12];
unsigned char data_020a0eb0[12];
unsigned char data_020a0ec8[8];
unsigned char data_020a0ed0[12];
unsigned char data_020a0ee4[12];
}
DSSTATE_END

namespace {

/* ---- the counters -------------------------------------------------------- */

int g_hits_02073e6c;
int g_hits_02074f80;
int g_faces_reported;
int g_rom_bodies_run;

/* ---- the two counting shims ---------------------------------------------- */

void ctor_02073e6c(void)
{
    ++g_hits_02073e6c;
    __sinit_02073e6c();
}

void ctor_02074f80(void)
{
    ++g_hits_02074f80;
    __sinit_02074f80();
}

/* ---- the thirteen faces --------------------------------------------------
 *
 * One function per initialiser so the name in the message is the name of the
 * word that reached it, not a shared string with an argument. Each reports
 * once. They RETURN: see the header for why an abort at word 0 would be a
 * boot that never starts rather than a fact anybody could act on. */

void ctor_face(const char *name, const char *why)
{
    ++g_faces_reported;
    std::fprintf(stderr, "  [ctor] NOT LINKED: %s -- %s\n", name, why);
}

#define CTOR_FACE(sym, why)                                                    \
    void ctor_face_##sym(void)                                                 \
    {                                                                          \
        static int said;                                                       \
        if (said) { ++g_faces_reported; return; }                              \
        said = 1;                                                              \
        ctor_face("__sinit_" #sym, why);                                       \
    }

CTOR_FACE(02073a24, "38 rodata constants (data_02086bc8..data_02086e40) are hosted nowhere; rung C1e")
CTOR_FACE(02074e0c, "func_0201aa18 -> func_0201aad4 -> func_0201aac8 is an argument-dropping tail-call veneer chain into func_02059ba0; a PORT_HOST_ABI question, not a linkage one")
CTOR_FACE(02074e84, "Clipper's ctor/dtor, data_0208ee14's MSVC name and data_0209f388 are hosted nowhere; rung C1d")
CTOR_FACE(02074edc, "data_0209f5c4 and data_0209f5dc are hosted nowhere; rung C1c, and hal/method_faces.cpp writes the same vptr")
CTOR_FACE(0207501c, "it constructs a RaycastLine into data_020a0d0c, whose host block is 0x10 in a grouped section hal/mmc_vtable.cpp:148 asserts at ROM spacing, and the ctor writes past +0x64")
CTOR_FACE(02075054, "data_020a0db8, data_020a0dc0 and data_020a0dcc are hosted nowhere; rung C1c, and hal/auto_bss.cpp hand-seeds the same block")
CTOR_FACE(02075154, "data_02099f48..data_02099f70 are hosted nowhere; rung C1c, and hal/actor_registry.cpp seats the same five callbacks")

#undef CTOR_FACE

/* ---- the table -----------------------------------------------------------
 *
 * The ROM's 23 words in the ROM's order, plus the ROM's NULL terminator. Word
 * N here IS the word at 0x02086b60 + 4*N, and the comment on each line is the
 * DS address that word holds in the cartridge. */

typedef void (*ctor_fn)(void);

struct CtorWord {
    unsigned    ds;       /* what the ROM's word holds */
    ctor_fn     host;     /* what this build binds it to */
    const char *name;     /* the initialiser's name, for the log */
    int         rom;      /* 1 = the ROM's own body runs, 0 = a face */
};

const CtorWord kCtorTable[] = {
    { 0x02073a24, ctor_face_02073a24, "__sinit_02073a24", 0 },
    { 0x02073e6c, ctor_02073e6c,      "__sinit_02073e6c", 1 },
    { 0x02074d90, __sinit_02074d90,   "__sinit_02074d90", 1 },
    { 0x02074da8, __sinit_02074da8,   "__sinit_02074da8", 1 },
    { 0x02074dbc, __sinit_02074dbc,   "__sinit_02074dbc", 1 },
    { 0x02074dc0, __sinit_02074dc0,   "__sinit_02074dc0", 1 },
    { 0x02074dc4, __sinit_02074dc4, "__sinit_02074dc4", 1 },
    { 0x02074e0c, ctor_face_02074e0c, "__sinit_02074e0c", 0 },
    { 0x02074e44, __sinit_02074e44, "__sinit_02074e44", 1 },
    { 0x02074e80, __sinit_02074e80,   "__sinit_02074e80", 1 },
    { 0x02074e84, ctor_face_02074e84, "__sinit_02074e84", 0 },
    { 0x02074edc, ctor_face_02074edc, "__sinit_02074edc", 0 },
    { 0x02074f80, ctor_02074f80,      "__sinit_02074f80", 1 },
    { 0x02074fb8, __sinit_02074fb8,   "__sinit_02074fb8", 1 },
    { 0x02074fe4, __sinit_02074fe4, "__sinit_02074fe4", 1 },
    { 0x0207501c, ctor_face_0207501c, "__sinit_0207501c", 0 },
    { 0x02075054, ctor_face_02075054, "__sinit_02075054", 0 },
    { 0x020750b4, __sinit_020750b4,   "__sinit_020750b4", 1 },
    { 0x020750b8, __sinit_020750b8, "__sinit_020750b8", 1 },
    { 0x020750ec, __sinit_020750ec, "__sinit_020750ec", 1 },
    { 0x0207511c, __sinit_0207511c, "__sinit_0207511c", 1 },
    { 0x02075150, __sinit_02075150,   "__sinit_02075150", 1 },
    { 0x02075154, ctor_face_02075154, "__sinit_02075154", 0 },
};
const unsigned kCtorWords = sizeof kCtorTable / sizeof kCtorTable[0];

/* The ROM's own address of the table and of its terminator word. */
const unsigned kCtorBase = 0x02086b60u;

/* How long the MSL global-destructor chain is. Nodes are three words and
   node[0] is the previous head (src/func_020731dc.c), so this is a walk, not a
   counter somebody has to keep in step. The cap is a cycle guard: a corrupt
   chain must not hang the boot. */
int dtor_chain_len(void)
{
    int n = 0;
    void **p = (void **)data_020aa3f0;
    while (p && n < 4096) { p = (void **)p[0]; ++n; }
    return n;
}

/* Is the ROM's table address a committed, writable page in this process? */
bool ctor_page_ok(void)
{
#if defined(_WIN32)
    MEMORY_BASIC_INFORMATION mbi;
    std::memset(&mbi, 0, sizeof mbi);
    if (!VirtualQuery((LPCVOID)(size_t)kCtorBase, &mbi, sizeof mbi))
        return false;
    if (mbi.State != MEM_COMMIT)
        return false;
    const DWORD w = PAGE_READWRITE | PAGE_WRITECOPY |
                    PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;
    if ((mbi.Protect & w) == 0)
        return false;
    /* The whole table plus the terminator has to be inside the same region. */
    const size_t need = (size_t)(kCtorWords + 1) * 4u;
    const size_t have = (size_t)mbi.RegionSize -
                        ((size_t)kCtorBase - (size_t)mbi.BaseAddress);
    return have >= need;
#else
    return false;
#endif
}

}  /* anonymous namespace */

extern "C" void port_rom_entry_ctors(void)
{
    static int done;
    if (done) {
        std::fprintf(stderr, "  [ctor] port_rom_entry_ctors called twice; the "
                             "second call did nothing\n");
        return;
    }
    done = 1;

    const int dtors_before = dtor_chain_len();
    const bool page = ctor_page_ok();

    if (page) {
        unsigned *w = (unsigned *)(size_t)kCtorBase;
        for (unsigned i = 0; i < kCtorWords; ++i)
            w[i] = (unsigned)(size_t)kCtorTable[i].host;
        w[kCtorWords] = 0;                 /* the ROM's own terminator */
        func_02072f94();                   /* Entry's own call, 0x020048a8 */
    } else {
        std::fprintf(stderr, "  [ctor] DS main RAM at %08x is not committed "
                             "and writable, so the ROM's runner cannot walk "
                             "its own table; running the 23 words directly. "
                             "See ntr/io.cpp: main RAM is the one reserved "
                             "range a process may lose.\n", kCtorBase);
        for (unsigned i = 0; i < kCtorWords; ++i)
            kCtorTable[i].host();
    }

    for (unsigned i = 0; i < kCtorWords; ++i)
        if (kCtorTable[i].rom) ++g_rom_bodies_run;

    const int dtors_after = dtor_chain_len();

    std::fprintf(stderr,
                 "[ctor] func_02072f94 walked %u words at %08x (%s): %d ran as "
                 "ROM bodies, %d as faces; __sinit_02073e6c hits %d, "
                 "__sinit_02074f80 hits %d; dtor chain on data_020aa3f0 "
                 "%d -> %d\n",
                 kCtorWords, kCtorBase,
                 page ? "the ROM's own runner" : "host fallback walk",
                 g_rom_bodies_run, (int)kCtorWords - g_rom_bodies_run,
                 g_hits_02073e6c, g_hits_02074f80,
                 dtors_before, dtors_after);

    if (g_hits_02073e6c != 1 || g_hits_02074f80 != 1)
        std::fprintf(stderr, "  [ctor] ONCE-ONLY CHECK FAILED: the two "
                             "initialisers that used to be hand-called ran "
                             "%d and %d times, not 1 and 1\n",
                     g_hits_02073e6c, g_hits_02074f80);
    else
        std::fprintf(stderr, "  [ctor] once-only check PASS: __sinit_02073e6c "
                             "1, __sinit_02074f80 1\n");
}
