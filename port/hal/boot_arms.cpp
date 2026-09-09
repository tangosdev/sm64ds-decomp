// ---------------------------------------------------------------------------
// boot_arms.cpp -- THE REFUSED ARMS OF func_0201a054, RUN FROM THE ROM'S OWN
// SEAM.
//
// Run link100, lane R2ABC, rungs R2a / R2b / R2c of lane BOOTSCOUT's staged
// boot plan. hal/rom_main.cpp made src/main.c real ROM code and left main's
// fifth call -- func_0201a054 -- as a host seam that only counted its hits.
// This file is what that seam calls: the arms of that body which
// hal/boot_os.cpp's transcription refused, in the ROM's own order.
//
// THE DERIVATION IS port/slice_r2abc.txt, not this header. Which refusal was
// stale and which still holds, the ROM's own bytes for each arm, the device
// table the identify walks, and the two blockers that keep R2b out are all
// measured there. What the comments below cover is what a reader of the CODE
// needs: the boot order, the storage checks, and the one edge that is still a
// host bridge.
//
// WHAT THE SEVEN NEWLY LINKED TUs ARE: LINKAGE COVERAGE. src/func_02013e64.c
// on R2a, and the six of R2c (func_02042f68, func_020603c8, func_020607e8,
// func_02060364, func_02060398, func_02060470), are the ROM's own code running
// on this path where host code used to stand in. That is what the count
// measures, and it is not seven removals of host behaviour. R2a's memset
// writes zeros over zeros here: all five sections are zero-initialised host
// storage and every host writer of the save block is below this seam. The
// identify reaches the same device row ntr/backup.cpp already parks statically,
// so func_0206045c returns the same 8192 either way and that file's row stays
// as the pre-boot default. Arm 1 of R2c is still a host body. What the seat
// changes is WHO decides, on the ROM's own object, at the ROM's own point.
//
// THE ORDER IS THE ROM'S, not a convenience. src/func_0201a054.c's head runs
//
//     func_020427f8(); func_02042784(); func_02053a8c(); func_02053c40();
//     func_0205b858(); func_0203d740(); func_02013e64(); func_0201a4e4();
//     IRQ::EnableIRQs(1); IME = 1; IRQ::Enable();
//     func_02053be0(1); func_0203bbc0(); func_0203bb5c();
//     if (!r4) func_0201fec8();
//     func_02042f68(0xd01, data_0208ee50);
//     ...
//
// so func_02013e64 comes first, func_0201fec8 next and func_02042f68 last.
// hal/boot_os.cpp's port_boot_rom_game_init_head() still runs the ARMS AROUND
// THEM at its own point in tests/walk_window.cpp -- this file does not move
// that transcription and does not duplicate any call it makes. What it adds is
// the three the transcription refused.
//
// WHERE THIS RUNS, AND WHY THAT IS THE SAFE POINT. port_rom_a054_seam() is
// reached from tests/walk_window.cpp:7429, immediately after
// Heap::InitializeRootHeap and well after port_romdata_load() at :7365. The
// host's own boot staging -- the pending character at :7743, the opening-seen
// bit at :8134, the level boot itself -- is all BELOW it, so an arm that
// writes DS state here writes it before anything host-side has staged, which
// is exactly the ROM's own order.
//
// ===========================================================================
// THE STORAGE CHECKS
// ===========================================================================
//
// R2a. The body is one line -- func_0205a588(data_0209caa0, 0, 0x32c), a
// memset of the 796-byte save block -- and hal/level_boot.cpp hosts that block
// as ONE grouped run of five .dsstate$savblk sections:
//
//     .dsstate$savblk0000  data_0209caa0  0x14   +0x000
//     .dsstate$savblk0001  data_0209cab4  0x1e   +0x014
//     .dsstate$savblk0002  data_0209cad2  0x12   +0x032
//     .dsstate$savblk0003  data_0209cae4  0x10   +0x044
//     .dsstate$savblk0004  data_0209caf4  728    +0x054 .. +0x32c
//
// A grouped-section run is a LAYOUT the linker could reorder, and a 796-byte
// memset onto a reordered run is silent corruption rather than a fault. So
// port_r2a_saveblock_layout() asserts the five offsets on EVERY boot, and when
// one has moved the arm REFUSES and names it rather than memsetting anyway.
//
// R2c. One word of the card driver's storage was hosted nowhere:
// data_020a8764, the halfword src/func_02060364.c copies out of the device
// row. hal/globals_link100.cpp measured its boundary and left it. It is hosted
// below at its ROM SPAN -- 0x020a8764 to data_020a8780, 28 bytes -- not at the
// four bytes func_02060364's own `unsigned int` declaration writes: that width
// is the undersized-hosted-global bug this port keeps finding, and the span is
// what config/arm9/symbols.txt says the object is. It is mutable DS state, so
// it goes in the .dsstate capture the save state rolls back.
//
// ===========================================================================
// THE ONE EDGE THAT IS STILL A HOST BRIDGE
// ===========================================================================
//
// Arm 1 of func_02042f68 is func_02060890, the card driver's bring-up, and it
// stays a host body because two of its statements cannot link:
//
//     data_020a8780 = func_02060a64   the FLASH read path, hosted nowhere and
//                                     reached by nothing in this game.
//     func_02061138()                 reaches func_020610fc, hand-asm with a
//                                     deliberate `b self` MSVC cannot
//                                     assemble -- family A, permanently.
//
// Six matched TUs stay out behind those two lines and are named in
// port/slice_r2abc.txt.
//
// THE FORM IS A RENAME, NOT A SHADOW, and it is hal/rom_main.cpp's own
// precedent. port/CMakeLists.txt compiles src/func_02042f68.c with
// `func_02060890=port_rom_card_bringup`, so the matched TU stays in the link
// byte for byte, src/func_02060890.c stays unenrolled and unclaimed rather
// than becoming a new SHADOW row in linkage.py's replacement queue, and one
// place in the tree says this edge is not ROM code yet. port_rom_card_bringup
// below is that place: func_02060890's ROM body with its two unlinkable
// statements removed and nothing else changed, INCLUDING the ROM's own
// `if (state != 0) return` guard.
//
// AND THAT GUARD IS WHY THIS DOES NOT DOUBLE-CREATE THE CARD THREAD. On this
// host the guard TRIPS: ntr/backup.cpp's PortBackupFill sets the work object's
// state word to 1 at static-init time, long before this seam, so the bring-up
// finds the driver already initialised and returns without calling
// func_0206002c. hal/boot_os.cpp's own direct call to func_0206002c at its
// own point in port_boot_rom_game_init_head stays the only one, and
// port/tools/thread_create_proof.py measures exactly the thread it always did.
//
// R2b -- func_0201fec8 -- IS RUN NOW (run link100, lane R2BD), and both of the
// blockers lane R2ABC measured are answered. WHAT THAT ARM IS: the DS DOWNLOAD
// PLAY ADVERTISEMENT, published once at boot. func_0201fec8 reads the
// firmware's language field out of the shared block, picks the matching
// game-info record and hands it plus the WM parent parameters to
// func_0203db64, which clears the comms bands and copies both records into DS
// state. FOUR matched TUs come with it: src/func_0201fec8.c, src/func_0203db64.c,
// src/func_02059f2c.c and src/func_0203daac.c.
//
// BLOCKER 1 MOVED WHILE R2ABC WAS WRITING IT DOWN, and its proposed remedy is
// wrong today. R2ABC asked for `MP3_BSS(".dsstate$ymp3b9990", data_020a0fa0, 6)`
// in hal/comms_conductor.cpp because data_020a0fa0 was hosted nowhere. Lane WM3
// has since hosted it -- hal/wm_arm7.cpp:290, `WMBSS(".dsstate$ywmd11",
// data_020a0fa0, 6, 1)` -- so that line would be a DUPLICATE DEFINITION now,
// and hal/comms_conductor.cpp is not touched by this lane at all. The blocker
// itself is the same one in a new place: func_0203db64 memsets 0x18 bytes from
// data_020a0fa0, whose host object is SIX bytes, and the next section in that
// run is ".dsstate$ywmd12" -- data_020a11e4, 1180 bytes of live thread-queue
// storage. Eighteen bytes of the ROM's memset would land in it.
//
// THE REMEDY IS EIGHTEEN BYTES OF THIS FILE'S OWN, IN THE ONE SECTION NAME
// THAT SORTS BETWEEN THEM. port_r2b_fa0_tail below is allocated to
// ".dsstate$ywmd11a", which MSVC sorts after "ywmd11" and before "ywmd12", so
// data_020a0fa0's six bytes and these eighteen are one contiguous 24-byte run
// and the ROM's memset stays inside storage. hal/wm_arm7.cpp:277-288 names the
// same gap from the other side ("data_020a0fa0[idx] for idx > 0 addresses that
// neighbour on hardware and does not here"), so this closes the out-of-range
// STORE it describes as well as the memset.
//
// WHAT THOSE EIGHTEEN BYTES ARE NOT, said plainly: on the DS they are
// data_020a0fa6, the other three ids of that MAC table, and this port hosts
// that symbol in hal/comms_conductor.cpp's band C instead. So the ROM's memset
// clears the right COUNT of bytes at the right base and the last eighteen of
// them are a host tail rather than data_020a0fa6. The arm measures what that
// costs before it makes the call: it reads data_020a0fa6's eighteen bytes and
// reports whether they are zero. They are on this path -- the band is BSS and
// the lobby that fills it runs far below this seam -- so the write the port
// does not deliver to data_020a0fa6 is zeros over zeros, the same reading R2a's
// own arm records for the save block.
//
// BLOCKER 2, THE .RODATA ISLAND, IS MOUNTED. Fourteen symbols, 864 bytes,
// twenty relocated words: five 0x1c game-info records (one per firmware
// language), the 0x40 WM parent parameter block, five UTF-16 description
// strings, the UTF-16 game name and the two icon paths. port/tools/romdata.py
// carries them as NAMED rows with the relocation audit; the twenty relocated
// words are NOT byte-copied into the game, because a byte-copied relocated word
// is a DS address in a host process -- the rule that file writes for itself and
// the bug that killed data_020876e4. port_r2b_bind_mbinfo() below rebinds them
// to the host addresses of the eight objects they name, after checking each
// word still holds the DS address config/arm9/relocs.txt says it should.
// ---------------------------------------------------------------------------

#include <cstdio>
#include <cstring>

#include "dsstate_seg.h"

extern "C" {

// ---- R2a: the save block, hosted as one 0x32c run by hal/level_boot.cpp ----
// Declared as arrays rather than pointers so the offsets below are the
// linker's own addresses. hal/level_boot.cpp owns the storage; this file only
// checks it and hands the head to the ROM's body.
extern unsigned char data_0209caa0[];
extern unsigned char data_0209cab4[];
extern unsigned char data_0209cad2[];
extern unsigned char data_0209cae4[];
extern unsigned char data_0209caf4[];

// src/func_02013e64.c -- the ROM's own clear of the save block.
void func_02013e64(void);

// The layout gate. Returns 0 when the five names are the ROM's own run.
//
// THE OFFSETS ARE COMPUTED FROM INTEGER ADDRESSES, not by subtracting the
// pointers. The five names are separately declared globals that the linker
// places adjacently because their grouped sections sort that way; adjoining
// sections do not make them one C++ array, and `k[i].p - data_0209caa0` would
// be pointer subtraction across distinct array objects. Each address is
// reduced to an address-sized integer first, in the port's own spelling --
// (unsigned)(size_t), the cast hal/actor_registry.cpp and the actor-table
// files already use on a host pointer, lossless here because the port builds
// 32-bit (port/build-port.cmd calls vcvars32). The refusal path is unchanged:
// a run that is not the ROM's shape makes the arm refuse rather than memset,
// and the message still names the symbol and both offsets.
int port_r2a_saveblock_layout(void)
{
    static const struct { const unsigned char *p; unsigned want; const char *n; } k[] = {
        { data_0209caa0, 0x000, "data_0209caa0" },
        { data_0209cab4, 0x014, "data_0209cab4" },
        { data_0209cad2, 0x032, "data_0209cad2" },
        { data_0209cae4, 0x044, "data_0209cae4" },
        { data_0209caf4, 0x054, "data_0209caf4" },
    };
    const unsigned head = (unsigned)(size_t)data_0209caa0;
    int bad = 0;
    for (int i = 0; i < 5; ++i) {
        unsigned at = (unsigned)(size_t)k[i].p - head;
        if (at != k[i].want) {
            std::fprintf(stderr, "  [r2a] SAVE BLOCK RUN BROKEN: %s at +0x%x, "
                                 "ROM says +0x%x\n", k[i].n, at, k[i].want);
            bad = 1;
        }
    }
    return bad;
}

// ---- R2c: the card driver's storage the identify needs --------------------
//
// data_020a8764 is the one word src/func_02060364.c writes that nothing in
// this port hosted. The storage-check block above says why it is 28 bytes and
// not the four its own declaration writes.
DSSTATE_BEGIN
unsigned char data_020a8764[28];
DSSTATE_END

// The card driver's work object and the device table, both hosted elsewhere:
// hal/globals_link100.cpp's grouped card span and hal/boot_globals.cpp's
// arm9 .data bytes. Read here only to report what the retained pointer
// names after the call, never to say which lookup ran.
// data_020867bc carries its extent because the diagnostic below has to test
// whether the retained row is inside the ROM's table at all; 160 bytes is the
// span hal/boot_globals.cpp defines and config/arm9/symbols.txt states.
extern unsigned char data_020a8180[];
extern unsigned char data_020a8760[];
extern unsigned char data_020867bc[160];
extern unsigned char data_020a4b40[];
extern unsigned char data_0208ee50[];

// ntr/backup.cpp's SEPARATELY HOSTED default row: five words typed by hand,
// the same five the ROM's table carries at +0x28. Its static init parks a
// pointer to it in data_020a8760 before this seam runs, so that is the object
// the retained pointer names until the ROM's own identify replaces it. Named
// here only so the diagnostic below can say which object it is looking at.
extern int data_port_backup_device[10];

void func_0206002c(void);          // src/func_0206002c.c, the card thread
void func_02042f68(int a0, unsigned char *src);   // the ROM's own arm

// ===========================================================================
// R2b: THE DOWNLOAD-PLAY ADVERTISEMENT -- STORAGE, THE POINTER BIND, THE GATE
// ===========================================================================

// The two DS globals func_0201fec8's closure reads that nothing hosted, both
// at their ROM SPANS from config/arm9/symbols.txt rather than at the width
// their own declarations write:
//
//   data_0209e64c  4      the body's own once-guard. src/func_0201fec8.c
//                         declares it `unsigned char`; the span to
//                         data_0209e650 is four.
//   data_020a0fec  0x34   func_0203db64:58 copies 0x1c of the chosen game-info
//                         record into it; the span to data_020a1020 (band A's
//                         head, hal/comms_conductor.cpp) is 0x34. Nothing walks
//                         from band C's tail into it -- that file's own guard
//                         block says every walk it makes is internal to a band
//                         -- so it is hosted standalone rather than grouped.
DSSTATE_BEGIN
unsigned char data_0209e64c[4];
unsigned char data_020a0fec[0x34];
DSSTATE_END

// THE EIGHTEEN BYTES BEHIND data_020a0fa0, in the one section name that sorts
// between hal/wm_arm7.cpp's ".dsstate$ywmd11" (data_020a0fa0, six bytes) and
// its ".dsstate$ywmd12" (data_020a11e4, 1180 bytes). align(1), like both of
// its neighbours, so the linker inserts no padding and the 24 bytes the ROM's
// memset writes are one run. The header block says what these bytes are and
// are not; port_r2b_storage_layout() below refuses the arm if the linker did
// not lay them where the name says.
#pragma section(".dsstate$ywmd11a", read, write)
__declspec(allocate(".dsstate$ywmd11a")) __declspec(align(1))
unsigned char port_r2b_fa0_tail[18] = {0};

// The memset targets that belong to other files, declared with the extents
// this file needs to CHECK rather than to use. Each pair below is the
// adjacency the ROM's own memset length depends on.
extern unsigned char data_020a0fa0[];    // hal/wm_arm7.cpp   6
extern unsigned char data_020a0fa6[];    // hal/comms_conductor.cpp band C  18
extern unsigned char data_020a0fb8[];    // band C            6
extern unsigned char data_020a0fd0[];    // band C            28
extern unsigned char data_020a10a4[];    // band B            2
extern unsigned char data_020a10fc[];    // band B            2
extern unsigned char data_020a1112[];    // band B            66
extern unsigned char data_020a1040[];    // hal/camera_bridges.cpp camcomm  4
extern unsigned char data_020a1052[];    // camcomm, last member            0x12
extern unsigned char data_020a1154[];    // camera_bridges camrec           0xc
extern unsigned char data_020a11c4[];    // camrec, +0x70 into the run
extern unsigned char data_020a1064[];    // hal/comms_seam.cpp ywmc11      64

// The .rodata island, mounted by port/tools/romdata.py from
// extracted/arm9_dec.bin. Six records the ROM's own code names, and the eight
// objects the twenty relocated words point at.
extern unsigned char data_020752cc[];    // game info, one per language
extern unsigned char data_020752e8[];
extern unsigned char data_02075304[];
extern unsigned char data_02075320[];
extern unsigned char data_0207533c[];
extern unsigned char data_02075358[];    // the WM parent parameter block
extern unsigned char data_02075398[];    // the five UTF-16 descriptions
extern unsigned char data_020753f8[];
extern unsigned char data_0207545c[];
extern unsigned char data_020754c4[];
extern unsigned char data_02075530[];
extern unsigned char data_0208f278[];    // UTF-16 "Super Mario 64 DS"
extern unsigned char data_0208f29c[];    // the icon graphics path
extern unsigned char data_0208f2d0[];    // the icon palette path

// src/func_0201fec8.c -- the ROM's own body.
void func_0201fec8(void);

// THE POINTER BIND. config/arm9/relocs.txt says exactly which word of which
// record is an address and what it points at; this is that table, transcribed,
// with the host object beside each row. The bind is one-way and idempotent: a
// word already holding its host address is left alone, a word holding the DS
// address the relocation names is rewritten, and anything else REFUSES rather
// than binding, because a third value means the image or the mount changed
// under this table and the port would be writing a pointer into a record it no
// longer understands.
//
// WHY IT IS HERE AND NOT IN hal/ptr_tables.cpp. That file is the port's home
// for pointer tables the ROM CALLS THROUGH, and it is not this lane's. This
// island is read at exactly one point in the boot, by exactly one body, and
// the bind has to happen after port_romdata_load() (tests/walk_window.cpp:7365)
// and before this seam (:7429) -- which is precisely the window this arm runs
// in. Keeping the two together is what makes the ordering checkable.
static int port_r2b_bind_mbinfo(void)
{
    struct Row {
        unsigned char *rec;      // the game-info record
        const char *rn;          // its name, for the message
        unsigned off;            // the relocated word's offset in it
        unsigned ds;             // the DS address relocs.txt says is there
        unsigned char *host;     // the host object that address means
        const char *hn;
    };
    static const Row rows[] = {
        { data_020752cc, "data_020752cc", 0x04, 0x0208f278, data_0208f278, "data_0208f278" },
        { data_020752cc, "data_020752cc", 0x08, 0x02075398, data_02075398, "data_02075398" },
        { data_020752cc, "data_020752cc", 0x0c, 0x0208f29c, data_0208f29c, "data_0208f29c" },
        { data_020752cc, "data_020752cc", 0x10, 0x0208f2d0, data_0208f2d0, "data_0208f2d0" },
        { data_020752e8, "data_020752e8", 0x04, 0x0208f278, data_0208f278, "data_0208f278" },
        { data_020752e8, "data_020752e8", 0x08, 0x020753f8, data_020753f8, "data_020753f8" },
        { data_020752e8, "data_020752e8", 0x0c, 0x0208f29c, data_0208f29c, "data_0208f29c" },
        { data_020752e8, "data_020752e8", 0x10, 0x0208f2d0, data_0208f2d0, "data_0208f2d0" },
        { data_02075304, "data_02075304", 0x04, 0x0208f278, data_0208f278, "data_0208f278" },
        { data_02075304, "data_02075304", 0x08, 0x02075530, data_02075530, "data_02075530" },
        { data_02075304, "data_02075304", 0x0c, 0x0208f29c, data_0208f29c, "data_0208f29c" },
        { data_02075304, "data_02075304", 0x10, 0x0208f2d0, data_0208f2d0, "data_0208f2d0" },
        { data_02075320, "data_02075320", 0x04, 0x0208f278, data_0208f278, "data_0208f278" },
        { data_02075320, "data_02075320", 0x08, 0x020754c4, data_020754c4, "data_020754c4" },
        { data_02075320, "data_02075320", 0x0c, 0x0208f29c, data_0208f29c, "data_0208f29c" },
        { data_02075320, "data_02075320", 0x10, 0x0208f2d0, data_0208f2d0, "data_0208f2d0" },
        { data_0207533c, "data_0207533c", 0x04, 0x0208f278, data_0208f278, "data_0208f278" },
        { data_0207533c, "data_0207533c", 0x08, 0x0207545c, data_0207545c, "data_0207545c" },
        { data_0207533c, "data_0207533c", 0x0c, 0x0208f29c, data_0208f29c, "data_0208f29c" },
        { data_0207533c, "data_0207533c", 0x10, 0x0208f2d0, data_0208f2d0, "data_0208f2d0" },
    };
    const int n = (int)(sizeof rows / sizeof rows[0]);
    int bound = 0, already = 0, bad = 0;
    for (int i = 0; i < n; ++i) {
        unsigned cur = 0;
        std::memcpy(&cur, rows[i].rec + rows[i].off, sizeof cur);
        const unsigned want = (unsigned)(size_t)rows[i].host;
        if (cur == want) { ++already; continue; }
        if (cur != rows[i].ds) {
            std::fprintf(stderr,
                         "  [r2b] MBINFO WORD NOT THE ROM'S: %s+0x%02x reads "
                         "%08x, config/arm9/relocs.txt says %08x (-> %s)\n",
                         rows[i].rn, rows[i].off, cur, rows[i].ds, rows[i].hn);
            ++bad;
            continue;
        }
        std::memcpy(rows[i].rec + rows[i].off, &want, sizeof want);
        ++bound;
    }
    std::fprintf(stderr, "  [r2b] download-play records: %d of %d pointer "
                         "words bound to host objects (%d already bound, %d "
                         "refused)\n", bound, n, already, bad);
    return bad;
}

// THE LAYOUT GATE. Six memset lengths and two copy lengths in
// src/func_0203db64.c, each one an adjacency in the host image. The rows below
// are the ones this port has to prove; where a run belongs to another file,
// the row checks the LANDMARK that file's own guard also checks, so a break
// shows up here as well as there.
//
// Offsets are differences of ADDRESS-SIZED INTEGERS, not pointer subtraction:
// these are separately declared globals the linker places adjacently because
// their grouped sections sort that way, and adjoining sections do not make two
// symbols one array. Same spelling and the same reason as
// port_r2a_saveblock_layout() above.
static int port_r2b_storage_layout(void)
{
    static const struct { const char *what; unsigned char *a, *b; unsigned want; } k[] = {
        // The one this lane adds: data_020a0fa0's six bytes plus this file's
        // eighteen, so the 0x18 memset has 24 bytes of run.
        {"data_020a0fa0 -> port_r2b_fa0_tail", data_020a0fa0, port_r2b_fa0_tail, 6},
        // Band C: 0fb8's six then 0fbe's eighteen reach 0fd0, so the second
        // 0x18 memset is inside the conductor's own band.
        {"data_020a0fb8 -> data_020a0fd0",     data_020a0fb8, data_020a0fd0, 0x18},
        // Band B: 10a4 + 0x58 lands on 10fc, and 10fc + 0x16 lands on 1112,
        // whose own 66 bytes carry that memset's remaining 0x42.
        {"data_020a10a4 -> data_020a10fc",     data_020a10a4, data_020a10fc, 0x58},
        {"data_020a10fc -> data_020a1112",     data_020a10fc, data_020a1112, 0x16},
        // camera_bridges' camcomm run: 1040 + 0x12 is its last member, whose
        // own 0x12 completes the 0x24 the memset writes.
        {"data_020a1040 -> data_020a1052",     data_020a1040, data_020a1052, 0x12},
        // camera_bridges' camrec run: 1154 + 0x70 is 11c4, whose own storage
        // runs far past the 0x90 this memset needs (sixteen 0x24 records).
        {"data_020a1154 -> data_020a11c4",     data_020a1154, data_020a11c4, 0x70},
    };
    int bad = 0;
    for (int i = 0; i < (int)(sizeof k / sizeof k[0]); ++i) {
        const unsigned at = (unsigned)(size_t)k[i].b - (unsigned)(size_t)k[i].a;
        if (at == k[i].want) continue;
        std::fprintf(stderr, "  [r2b] COMMS RUN BROKEN: %s is +0x%x, the ROM "
                             "says +0x%x\n", k[i].what, at, k[i].want);
        bad = 1;
    }
    return bad;
}

// ---- ARM 1 of func_02042f68, as a host body ------------------------------
//
// src/func_02060890.c minus its last two statements, which cannot link. The
// per-source rename in port/CMakeLists.txt points the ROM's own call here.
// Every store below is the ROM's, at the ROM's offset, in the ROM's order
// (0x02060890: read +0x34, return if non-zero, then +0x34 = 1, +0x20 = 0,
// +0x1c = +0x20, +0x18 = +0x1c, +0x24 = -1, +0x28 = 0, +0x2c = 0, then
// bl func_0206002c).
void port_rom_card_bringup(void)
{
    char *g = (char *)data_020a8180;
    if (*(int *)(g + 0x34) != 0) {
        /* The port reaches this line, every boot: ntr/backup.cpp seeds the
           work object statically at start-up, so the ROM's own guard says the
           driver is already up. Said once rather than silently. */
        std::fprintf(stderr, "  [rom-a054] R2c arm 1 (func_02060890): the card "
                             "driver is already initialised (state %d), so the "
                             "ROM's own guard returns -- func_0206002c stays "
                             "hal/boot_os.cpp's single call\n",
                     *(int *)(g + 0x34));
        return;
    }
    *(int *)(g + 0x34) = 1;
    *(int *)(g + 0x20) = 0;
    *(int *)(g + 0x1c) = *(int *)(g + 0x20);
    *(int *)(g + 0x18) = *(int *)(g + 0x1c);
    *(int *)(g + 0x24) = -1;
    *(int *)(g + 0x28) = 0;
    *(int *)(g + 0x2c) = 0;
    func_0206002c();
    /* data_020a8780 = func_02060a64  -- NOT RUN: the FLASH read path, hosted
       nowhere and reached by nothing in this game.
       func_02061138()               -- NOT RUN: hand-asm `b self`, family A. */
}

// ---- the seam body hal/rom_main.cpp calls ---------------------------------
//
// One entry point, the arms inside it in the ROM's order. hal/rom_main.cpp
// keeps its own [rom-main] line; the per-arm lines below are what a captured
// run shows firing.
void port_rom_a054_arms(void)
{
    /* ARM R2a. func_02013e64() -- memset(data_0209caa0, 0, 0x32c). */
    if (port_r2a_saveblock_layout()) {
        std::fprintf(stderr, "  [rom-a054] R2a REFUSED: the save block is not "
                             "the ROM's 0x32c run, so its memset is not run\n");
    } else {
        func_02013e64();
        std::fprintf(stderr, "  [rom-a054] R2a func_02013e64: the ROM cleared "
                             "its own 0x32c save block at 0x0209caa0\n");
    }

    /* ARM R2b. func_0201fec8() -- the download-play advertisement.

       THE GUARD IS THE ROM'S OWN. src/func_0201a054.c reads r4 as
       `*(u16 *)0x027ffc40 == 2` and calls this body only when r4 is clear;
       hal/boot_os.cpp:578 records the same derivation and records that
       ntr/io.cpp writes 0 to that word at io_init, so the port takes the arm.
       The word is read here rather than assumed, so a build that ever puts a 2
       there skips this call exactly as the cartridge would. */
    {
        const unsigned short r4w = *(volatile unsigned short *)0x027ffc40u;
        if (r4w == 2) {
            std::fprintf(stderr, "  [rom-a054] R2b SKIPPED as the ROM skips "
                                 "it: 0x027ffc40 reads 2\n");
        } else {
            int refuse = port_r2b_storage_layout();
            refuse |= port_r2b_bind_mbinfo() ? 1 : 0;
            if (refuse) {
                std::fprintf(stderr, "  [rom-a054] R2b REFUSED: the comms runs "
                                     "or the download-play records are not what "
                                     "the ROM's own lengths need, so "
                                     "func_0201fec8 is not called\n");
            } else {
                /* WHAT THE SPLIT COSTS, MEASURED BEFORE THE CALL RATHER THAN
                   ARGUED. The 0x18 memset at data_020a0fa0 reaches this file's
                   own eighteen-byte tail and not data_020a0fa6, which
                   hal/comms_conductor.cpp hosts elsewhere. If those eighteen
                   bytes are already zero the write the port does not deliver
                   is a write of zeros over zeros and nothing observable is
                   lost; if they are not, this line says so and the reading
                   stops being free. */
                int fa6_nonzero = 0;
                for (int i = 0; i < 18; ++i)
                    if (data_020a0fa6[i]) ++fa6_nonzero;

                func_0201fec8();

                std::fprintf(stderr,
                             "  [rom-a054] R2b func_0201fec8: the ROM published "
                             "its own download-play record -- firmware language "
                             "%u (0x027ffce4 bits 0..2, what src/func_02059eb0.c "
                             "reads), guard data_0209e64c now %u; "
                             "data_020a0fec[0..4] %02x%02x%02x%02x, name "
                             "pointer %08x\n",
                             (unsigned)(*(volatile unsigned short *)0x027ffce4u & 7u),
                             (unsigned)data_0209e64c[0],
                             data_020a0fec[0], data_020a0fec[1],
                             data_020a0fec[2], data_020a0fec[3],
                             *(unsigned *)(data_020a0fec + 4));
                std::fprintf(stderr,
                             "  [rom-a054] R2b parent parameters into "
                             "data_020a1064: +0x30 %04x, +0x32 %04x (parent max "
                             "send), +0x34 %04x (child max send) -- "
                             "hal/wm_arm7.cpp:98-106 predicts 0x200 and 0x20\n",
                             *(unsigned short *)(data_020a1064 + 0x30),
                             *(unsigned short *)(data_020a1064 + 0x32),
                             *(unsigned short *)(data_020a1064 + 0x34));
                std::fprintf(stderr,
                             "  [rom-a054] R2b NOTE: data_020a0fa6's eighteen "
                             "bytes read %s before the call, so the memset this "
                             "port delivers to port_r2b_fa0_tail instead is %s\n",
                             fa6_nonzero ? "NON-ZERO" : "all zero",
                             fa6_nonzero ? "a write the DS object does not get"
                                         : "zeros over zeros");
            }
        }
    }

    /* ARM R2c. func_02042f68(0xd01, data_0208ee50) -- the card bring-up, the
       device identify and the record tag, all three as the ROM's own body. */
    {
        unsigned char *row_before = 0, *row_after = 0;
        std::memcpy(&row_before, data_020a8760, sizeof row_before);
        func_02042f68(0xd01, data_0208ee50);
        std::memcpy(&row_after, data_020a8760, sizeof row_after);

        /* WHICH OBJECT DOES THE RETAINED POINTER NAME? An offset into
           data_020867bc only means something if the row is inside
           data_020867bc, and it need not be: ntr/backup.cpp's
           data_port_backup_device is a separately hosted row of the same five
           words, parked in data_020a8760 at static-init time. So the span is
           tested first, on integer addresses in the port's own width, and the
           line names the object it found rather than printing a difference
           between two unrelated ones.

           AND IT NAMES THE OBJECT ONLY, NEVER THE LOOKUP. Every branch below
           reports what data_020a8760 holds after the call and stops there,
           because no reading of that pointer tells you which lookup ran.
           src/func_02042f68.c:11-12 returns before everything else when
           func_0203da3c() reads 2, so on that path the card bring-up does not
           run, func_020603c8 is never called, the device search in
           src/func_02060398.c never walks data_020867bc and the tag loop never
           writes data_020a4b40: the pointer simply keeps whatever static init
           left in it. A null reading is therefore consistent with the early
           return and with a search that found no 0xd01, and a table row is
           consistent with a search that ran now and with a row an earlier call
           left behind. The same rule the NOTE line below is written under. */
        const unsigned table = (unsigned)(size_t)data_020867bc;
        const unsigned row   = (unsigned)(size_t)row_after;
        if (!row_after) {
            std::fprintf(stderr, "  [rom-a054] R2c func_02042f68: after the "
                                 "call data_020a8760 holds NO DEVICE ROW "
                                 "(null)\n");
        } else if (row - table < (unsigned)sizeof data_020867bc) {
            std::fprintf(stderr,
                         "  [rom-a054] R2c func_02042f68: after the call "
                         "data_020a8760 holds row +0x%x of data_020867bc -- "
                         "type 0x%x, size %d bytes, kind %d; tag \"%.8s\"\n",
                         row - table,
                         *(unsigned *)row_after,
                         *(int *)(row_after + 4),
                         (int)*(unsigned short *)data_020a8764,
                         (const char *)data_020a4b40);
        } else if (row == (unsigned)(size_t)data_port_backup_device) {
            std::fprintf(stderr,
                         "  [rom-a054] R2c func_02042f68: after the call "
                         "data_020a8760 holds ntr/backup.cpp's separately "
                         "hosted default row (data_port_backup_device), not a "
                         "row of data_020867bc -- type 0x%x, size %d bytes; "
                         "data_020a8764 reads %d; tag \"%.8s\"\n",
                         *(unsigned *)row_after,
                         *(int *)(row_after + 4),
                         (int)*(unsigned short *)data_020a8764,
                         (const char *)data_020a4b40);
        } else {
            std::fprintf(stderr,
                         "  [rom-a054] R2c func_02042f68: after the call "
                         "data_020a8760 holds %08x, which is neither inside "
                         "data_020867bc nor the hosted default row\n", row);
        }

        /* OBSERVED, AND NO MORE THAN OBSERVED. An unchanged pointer does not
           prove the identify never reached src/func_02060364.c: that body
           writes data_020a8760 and data_020a8764 and nothing else, so
           re-selecting the same row writes the same two words and leaves
           exactly this reading. Measuring the call itself would need a counter
           inside ROM code, or a sentinel written over DS state before the call
           and left behind when it does not run -- a behaviour change either
           way. So the line reports the observation and stops there. */
        if (row_before == row_after)
            std::fprintf(stderr, "  [rom-a054] R2c NOTE: the device pointer in "
                                 "data_020a8760 did not move across the call\n");
    }
}

}  // extern "C"
