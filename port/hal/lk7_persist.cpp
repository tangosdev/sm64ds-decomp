// Disk-backed save state for the PC port (lane lk7). Makes the single lk6 slot
// survive the game closing and reopening.
//
// WHAT THIS ADDS ON TOP OF lk6
// ----------------------------
// hal/lk6_savestate.cpp keeps one IN-MEMORY slot: the hosted DS arena, the
// .dsstate section, and the hardware content stores, valid for the life of the
// process. lk7 writes that same content to a file next to the exe on every
// successful save, and reads it back at startup so the slot is populated from
// disk. Everything about WHAT is captured stays in lk6; lk7 only moves those
// bytes to and from disk and guards the move with a header.
//
// THE HARD PART: ABSOLUTE POINTERS ACROSS A RESTART
// -------------------------------------------------
// The snapshot is full of raw absolute pointers, and they come in TWO kinds.
// Pointers INTO THE ARENA (actor-list heads, model records, intrusive links)
// relocate only if the arena comes up at the same base, which is why
// hal/os_arena.cpp pins it at a fixed address (0x30000000) with VirtualAlloc,
// the same technique ntr/io.cpp uses for the DS ranges. Pointers INTO THE EXE
// IMAGE (every vtable pointer in every actor, every code pointer in a hosted
// table) relocate only if the IMAGE comes up at the same base -- and ASLR
// rebases a Windows exe once per boot, so a state written before a reboot
// would come back with every vtable pointing into the void. The original lane
// draft never checked that; the header now carries the image base and refuses
// a mismatch, and the build links the window targets with /DYNAMICBASE:NO so
// the base is the same every boot and the refusal never fires in practice.
//
// THE HEADER, AND WHAT EACH FIELD REFUSES
// ---------------------------------------
// A disk state is only loaded if every header field matches this process:
//   magic         wrong bytes           -> not our file, refuse
//   format        version bump          -> old on-disk layout, refuse
//   gittip        different build       -> another version's world, refuse
//   romdata       other game data       -> the world was built from other
//                                          bytes, refuse (see below)
//   image_base    exe rebased           -> code pointers would dangle, refuse
//   arena_base    arena at a diff base  -> arena pointers would dangle, refuse
//   dsstate_base  section moved         -> hosted globals moved, refuse
// and then, in header_lengths_SAFE and not in the policy list above, because
// they are the lengths the body copies are sized by and not a judgement about
// compatibility at all:
//   arena_size    arena a diff size     -> the copy would overrun, refuse
//   dsstate_size  section grew/shrank   -> the copy would overrun, refuse
//   hw_size       regions differ        -> the copy would overrun, refuse
// THE ORDER MATTERS AND IT IS THE ORDER ABOVE. header_matches compares field
// by field and returns on the first mismatch, so a genuinely old state file --
// one written by any earlier build -- is refused at GITTIP, which cannot match
// across builds and therefore fires before anything below it is even looked at.
// dsstate_base and the length checks sit BEHIND gittip as backstops, not as the
// front line: each one independently trips on a state whose gittip somehow
// agreed, which is what makes them worth keeping even though gittip is the
// strictly tighter test. So a change that grows or moves the .dsstate section
// costs no compatibility that a commit did not already cost, and the release
// note for such a change should say "refused, not corrupted" and name gittip
// as the field a player will actually see.
//
// A refusal names the field on stderr and leaves the file untouched. A gittip
// mismatch is the common one: it is how a state written by an older build of
// the game is turned away instead of loaded into a world it no longer
// describes. (gittip, image_base and dsstate_base overlap in what they catch;
// they are all kept because each refusal message tells the reader something
// different about WHY the file cannot load.)
//
// THE HALF THE HEADER WAS MISSING: THE DATA (format 3, 2026-08-26)
// ----------------------------------------------------------------
// Every field listed above is an EXE-SIDE fact. Read the list again with that
// in mind: which build, where its image landed, where its arena landed, how big
// its hosted-global section is. Not one of them can see the ASSET FOLDER.
//
// And a save state is not a snapshot of an exe. It is a snapshot of a WORLD,
// and that world was built out of build/assets/romdata.bin -- every ROM table
// the game reads, the level and path and floor records included. The same exe
// pointed at two different asset folders produces two different worlds, and
// every field above says they are the same.
//
// NOT YET OBSERVED IN THE FIELD. The hole was found by reading this header
// against hal/romdata_loader.cpp, not by a report, and the write-up says so
// because a guard that claims a victim it does not have is a guard the next
// reader stops trusting. What it can do is demonstrated on demand:
// port/tools/install_mismatch_probe.py saves a state under one asset folder's
// game data and boots the window on another's, and with the guard off the
// restored world takes an access violation.
//
// WHAT THE 2026-08-26 BURST ACTUALLY WAS, since this file was written during it
// and first blamed for it. Six crash reports from one player in two minutes,
// every launch, ending in a path assert -- "binding 0 has -1 nodes and the node
// walk holds 3 ... the floor record is wrong" -- and a null dereference. The
// install looked mixed because the folder was NAMED for an old release. It was
// not. The exe, the game data and the save state in that folder were all the
// same build: the state passed the gittip field, which only a state written by
// that same exe can do, and the shipped bundles carry no romdata.bin at all
// (the player's own machine builds it from a recipe), so the data in the folder
// was the data that exe asked for.
//
// SO THE REAL DEFECT IS UNTOUCHED BY THIS FILE AND STILL OPEN: a save state
// written by one build and reloaded by THAT SAME BUILD, with every header field
// agreeing -- gittip, image base, arena base, section bounds, and now the game
// data too -- restoring a world with an invalid floor record. That is a
// save-state FIDELITY bug, the half-rollback shape the lk6 write-up catalogues,
// and no stamp in this header can see it: every stamp matches, correctly. It
// has its own lane. Do not read format 3 as having closed it, and do not read a
// clean stamp as evidence a restored world is sound.
//
// So the header now carries the identity of the game data the world was built
// from: the whole-file sha256 of romdata.bin and its length, as
// hal/romdata_loader.cpp recorded them at boot. It also carries the asset root
// PATH, which is never compared (a player is free to move or rename the folder)
// and exists so the refusal message can name the folder a human has to go fix.
//
// This is what makes the guard complete rather than merely long: gittip answers
// "same code?", romdata answers "same data?", and a world needs both to be the
// same world.
//
// FORMAT 2 STATES ARE REFUSED, NOT MISREAD. The version bump is the point: a
// format-2 file has no romdata fields, so there is no way to ask it the
// question, so it does not get to load. It is refused on `format` before a byte
// of its body is read. This costs players NOTHING THEY WERE NOT ALREADY PAYING:
// gittip changes every release, so every release has always refused every save
// state written before it. Format 3 refuses the same files one field earlier.
//
// SM64DS_SAVESTATE_NO_GUARD=1 turns the POLICY refusals off for forensics --
// load a state the guard would have turned away, to see what it does. It does
// NOT turn off the three LENGTH checks (arena, dsstate, hw), which are not
// policy: the load copies disk.arena_size bytes into the live arena, and a
// longer file with the guard off would be a buffer overrun. Those three stay
// hard in every configuration.

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

/* run mg16 lane MP2: the instance suffix on savestate.bin */
#include "instance_tag.h"

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

typedef unsigned int u32;

extern "C" {
// os_arena window + whether it is at the fixed base.
void *port_arena_base(void);
void *port_arena_end(void);
void *port_arena_cursor(void);
void  port_arena_set_cursor(void *p);
int   port_arena_is_fixed(void);

// the .dsstate section bounds (hal/dsstate_seg.cpp sentinels).
extern char dsstate_lo;
extern char dsstate_hi;

// hal/lk6_savestate.cpp: the fix-off arm's bracket, round whichever memcpy
// actually rolls .dsstate back. Returns non-zero if it stashed.
int  port_ss_rollguard_begin(void);
void port_ss_rollguard_end(int began);

// The world's own two singletons, for the runnability check below. Both are
// hosted DS globals inside .dsstate: data_0209f394[0] is the local Player
// (hal/cxxname_bridge.cpp) and data_0209f318 the Camera (hal/actor_vtables.cpp),
// and every target that links this file links both of those.
extern int data_0209f394[];
extern void *data_0209f318;

// the hardware content stores (ntr/io.cpp).
unsigned port_hw_regions_size(void);
void port_hw_regions_copy_out(void *dst);
void port_hw_regions_copy_in(const void *src);

// lk6: the in-memory slot this layer mirrors to and from disk.
int lk6_savestate_save(void);
int lk6_savestate_load(void);
int lk6_savestate_has(void);

// the build's git tip, embedded by CMake (host-src/port_gittip.c). Weakly
// aliased in fault_probe.h for builds without it; here we only read it.
extern const char port_build_gittip[];

// hal/os_arena.cpp: the one-line install verdict the crash dump carries.
void port_install_verdict_add(const char *line);

// WHICH GAME DATA this process booted from. The storage is in hal/os_arena.cpp
// -- the one hal file every target that links this one also links -- and
// hal/romdata_loader.cpp fills it at boot in a ROM-CLEAN build. A build with no
// loader (tests/smoke_persist.cpp is one) leaves it empty and zero, and so does
// a ROM-CLEAN build before port_romdata_load has run. Both halves of every
// comparison below are then the same empty string, which is the right answer
// for two runs of the same binary reading no blob at all.
//
// Plain externs on purpose. The weak-symbol idiom (__declspec(selectany) plus
// /alternatename) would work, but it costs a row in
// tools/alternatename_baseline.txt per symbol -- an allowlist entry saying "this
// alias is deliberately defeated in the full link" -- and three rows of frozen
// allowlist is a bad price for storage that could simply live somewhere both
// targets already reach.
extern char               port_romdata_sha[65];
extern unsigned long long port_romdata_bytes;
extern char               port_asset_root_seen[512];
}

namespace {

// Bumping any field layout below is a FORMAT_VERSION bump; an older file is
// then refused on the format field rather than misread. Format 2 was the
// .dsstate-section world (format 1 was the never-shipped hand-list draft).
// Format 3 adds the game-data identity (romdata sha + length + the asset root
// path) -- see "THE HALF THE HEADER WAS MISSING" above.
const char   MAGIC[8]        = { 'S','M','6','4','D','S','S','T' };
const u32    FORMAT_VERSION  = 3;
const u32    GITTIP_FIELD    = 64;   // fixed-width, NUL-padded, always NUL-terminated
const u32    SHA_FIELD       = 65;   // 64 hex digits + NUL, or all-NUL for "none"
const u32    ROOT_FIELD      = 512;  // the asset root path; informational only

#pragma pack(push, 1)
struct Header {
    char     magic[8];
    u32      format;
    char     gittip[GITTIP_FIELD];   // stays at offset 12: smoke_persist pokes it there
    uint64_t image_base;             // exe base: vtable/code pointers relocate iff equal
    uint64_t arena_base;
    uint64_t arena_size;
    uint64_t arena_cursor;   // low-water carve pointer at save time (host state)
    uint64_t dsstate_base;   // &dsstate_lo: where the hosted globals sit this build
    uint64_t dsstate_size;   // &dsstate_hi - &dsstate_lo
    uint64_t hw_size;        // palette + video + sprite memory, 0 if not reserved
    /* FORMAT 3. Appended at the END on purpose: every field above keeps the
       offset it had, so smoke_persist's "gittip is at offset 12" poke and any
       hand analysis of an existing dump still read the same words. */
    char     romdata_sha[SHA_FIELD]; // sha256 of the romdata.bin this world was built from
    uint64_t romdata_bytes;          // its length; 0 with an empty sha means "none"
    char     asset_root[ROOT_FIELD]; // where that data came from. NEVER COMPARED --
                                     // a player may move or rename the folder. It is
                                     // here so the refusal can name it.
};
#pragma pack(pop)

size_t dsstate_size_live()
{
    char *lo = &dsstate_lo, *hi = &dsstate_hi;
    return hi > lo ? (size_t)(hi - lo) : 0;
}

// Fill hdr for THIS process. Returns 0 (and leaves hdr zeroed) if there is no
// usable arena window, which is the same guard lk6_savestate_save uses.
int fill_header(Header *hdr)
{
    memset(hdr, 0, sizeof *hdr);
    char *base = (char *)port_arena_base();
    char *end  = (char *)port_arena_end();
    if (!base || end <= base) return 0;
    memcpy(hdr->magic, MAGIC, sizeof hdr->magic);
    hdr->format = FORMAT_VERSION;
    snprintf(hdr->gittip, sizeof hdr->gittip, "%s", port_build_gittip);
#if defined(_WIN32)
    hdr->image_base = (uint64_t)(uintptr_t)GetModuleHandleA(NULL);
#endif
    hdr->arena_base   = (uint64_t)(uintptr_t)base;
    hdr->arena_size   = (uint64_t)(size_t)(end - base);
    hdr->arena_cursor = (uint64_t)(uintptr_t)port_arena_cursor();
    hdr->dsstate_base = (uint64_t)(uintptr_t)&dsstate_lo;
    hdr->dsstate_size = (uint64_t)dsstate_size_live();
    hdr->hw_size      = (uint64_t)port_hw_regions_size();
    /* format 3: WHICH GAME DATA this world was built out of. Read straight off
       hal/romdata_loader.cpp's record of the blob it verified at boot. */
    snprintf(hdr->romdata_sha, sizeof hdr->romdata_sha, "%s", port_romdata_sha);
    hdr->romdata_bytes = port_romdata_bytes;
    snprintf(hdr->asset_root, sizeof hdr->asset_root, "%s", port_asset_root_seen);
    return 1;
}

// The state file path: next to the exe, the folder the launcher puts the bundle
// in (same idea as host_settings.cpp find_settings). Returns 0 if it cannot be
// determined. Non-Windows never gets here because port_arena_is_fixed is 0
// there, so this is Windows-only.
int state_path(char *out, size_t cap)
{
#if defined(_WIN32)
    char exe[MAX_PATH];
    DWORD n = GetModuleFileNameA(NULL, exe, MAX_PATH);
    if (n > 0 && n < MAX_PATH) {
        char *slash = strrchr(exe, '\\');
        char *fwd   = strrchr(exe, '/');
        if (fwd && (!slash || fwd > slash)) slash = fwd;
        if (slash) {
            *slash = '\0';
            /* run mg16 lane MP2: THE DANGEROUS ONE. Two copies of the game in
               one folder share this path, and smoke_persist.cpp:249 already
               names the hazard -- "a separate process picking up the first
               one's savestate.bin". SM64DS_INSTANCE gives each its own; unset,
               the name is unchanged and every existing save still loads. The
               +16 headroom check becomes +32 to cover the suffix. */
            if (strlen(exe) + 32 < cap) {
                snprintf(out, cap, "%s\\savestate%s.bin", exe,
                         port_instance_tag());
                return 1;
            }
        }
    }
#endif
    (void)out; (void)cap;
    return 0;
}

// ---- IS THE SAVED WORLD A WORLD? -------------------------------------------
//
// Every header field is an EXE-SIDE or an ASSET-SIDE fact: which build, which
// game data, where the image and the arena landed, how big the section is. A
// file can agree on all of them and still hold a world the game cannot take a
// single tick of. That is what a player hit on 2026-08-26: launcher and game
// both 0.2.13, the state written by that same build, every field matched, the
// state loaded -- and the first frame died in func_0200ca50 +0x12 reading
// address 0x24, which is `mode->+0x24` with a NULL camera mode, under
// Camera::Behavior (resolved against a layout-matched rebuild of that commit).
//
// So the load asks three questions of the WORLD before it commits to it, and
// they are the three the first tick asks:
//
//   1. is there a Player  -- data_0209f394[0], the actor the frame loop ticks
//      and the camera follows;
//   2. is there a Camera  -- data_0209f318, ticked every frame by
//      Camera::Behavior;
//   3. does that Camera have a MODE -- its +0x13c, which func_0200ca50
//      dereferences at +0x24 before it does anything else. The mode objects
//      are ROM data in the .cammod run, so a live Camera's is never null; a
//      null one means the bytes at that address are not a Camera.
//
// Read out of the FILE's bytes, at the same offsets they occupy live, before a
// single byte is copied into place -- so a refusal costs the player their save
// state and not their launch, which is the same trade every other refusal in
// this file makes.
//
// SKIPPED WHEN THIS PROCESS DID NOT BOOT A LEVEL. tests/smoke_persist and
// tests/smoke_savestate build a bare actor world on purpose -- they park their
// own object in data_0209f394[0] and it is not an arena Player -- so asking
// them for one would refuse the very states those tests exist to prove
// round-trip. The gate is the LIVE world and it is the weakest thing that
// distinguishes the game from a harness: a Player pointer that actually
// addresses the hosted arena. If THIS process booted a level, the file has to
// have booted one too.
static int in_arena(const void *p)
{
    const char *c = (const char *)p;
    return c >= (const char *)port_arena_base()
        && c <  (const char *)port_arena_end();
}
static int world_check_applies(void)
{
    void *p = (void *)(size_t)data_0209f394[0];
    return p && in_arena(p);
}
/* Is this word a vtable pointer into this exe? Both singletons are C++ objects
   with virtuals -- the frame loop dispatches through them on the first tick --
   so an object whose first word does not address the image is not one of them,
   whatever else it might be. /DYNAMICBASE:NO pins the image, which is what
   makes a bare range test meaningful across a restart. Executable and
   read-only sections both count: MSVC puts vtables in .rdata. */
#if defined(_WIN32)
extern "C" IMAGE_DOS_HEADER __ImageBase;
#endif
static int in_image(const void *p)
{
#if defined(_WIN32)
    const char *base = (const char *)&__ImageBase;
    IMAGE_NT_HEADERS32 *nt = (IMAGE_NT_HEADERS32 *)(base +
        ((IMAGE_DOS_HEADER *)base)->e_lfanew);
    const char *c = (const char *)p;
    return c >= base && c < base + nt->OptionalHeader.SizeOfImage;
#else
    (void)p;
    return 1;
#endif
}
static const char *world_fault(const char *abuf, size_t asz,
                               const char *dbuf, size_t dsz)
{
    if (!abuf || !dbuf) return 0;
    if (!world_check_applies())
        return 0;                      /* harness world: nothing to compare */

    const char *dlo = &dsstate_lo;
    size_t off_player = (const char *)&data_0209f394[0] - dlo;
    size_t off_cam    = (const char *)&data_0209f318   - dlo;
    if (off_player + 4 > dsz || off_cam + 4 > dsz)
        return 0;                      /* section shape moved; not our call */

    void *player = *(void *const *)(dbuf + off_player);
    void *cam    = *(void *const *)(dbuf + off_cam);
    if (!player)
        return "it has no Player (data_0209f394[0] is null)";
    if (!in_arena(player))
        return "its Player is not in the hosted arena";
    if (!cam)
        return "it has no Camera (data_0209f318 is null)";
    if (!in_arena(cam))
        return "its Camera is not in the hosted arena";
    {
        size_t off = (const char *)player - (const char *)port_arena_base();
        if (off + 4 > asz || !in_image(*(void *const *)(abuf + off)))
            return "its Player is not an object of this build (no vtable)";
    }
    {
        size_t off = (const char *)cam - (const char *)port_arena_base();
        if (off + 0x140 > asz)
            return "its Camera runs off the end of the saved arena";
        if (!in_image(*(void *const *)(abuf + off)))
            return "its Camera is not an object of this build (no vtable)";
        if (!*(void *const *)(abuf + off + 0x13c))
            return "its Camera has no mode -- the first frame would fault "
                   "reading address 0x24";
    }
    return 0;
}

// ---- the refusal record ----------------------------------------------------
// A refusal has to reach three places, so it is recorded once and read three
// times: the playlog (the long form, the line an operator greps for), the
// on-screen toast walk_window draws (SHORT -- ss_toast is 64 bytes, and a
// player who never opens a log otherwise sees nothing at all), and the crash
// dump's install block (so a report from a mixed install says so on its face).
char g_refuse_short[64];
char g_refuse_long[640];

/* The dump's install block is a three-label table (asset root / romdata /
   savestate) and a refusal has to arrive wearing its label, or a reader has to
   work out for themselves which subsystem the sentence came from. The long form
   is trimmed to its first line here: the multi-line detail belongs in the
   playlog, and the dump wants the one sentence that classifies the report. */
void verdict_savestate_refused(void)
{
    char line[256];
    unsigned n = 0;
    while (n < sizeof line - 24 && g_refuse_long[n] &&
           g_refuse_long[n] != '\n')
        ++n;
    snprintf(line, sizeof line, "savestate   REFUSED -- %.*s", (int)n,
             g_refuse_long);
    port_install_verdict_add(line);
}

void refuse(const char *shrt, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(g_refuse_long, sizeof g_refuse_long, fmt, ap);
    va_end(ap);
    snprintf(g_refuse_short, sizeof g_refuse_short, "%s", shrt);
    fprintf(stderr, "[savestate] %s\n", g_refuse_long);
}

// THE THREE LENGTH CHECKS, WHICH ARE NOT POLICY AND CANNOT BE OVERRIDDEN.
// lk7_persist_read copies disk.arena_size bytes into the live arena, disk
// .dsstate_size bytes over the live section, and disk.hw_size bytes into the
// hardware stores. If any of those exceeds what this process actually has, the
// copy is a buffer overrun -- so these three are checked before the guard's
// kill switch is consulted, and SM64DS_SAVESTATE_NO_GUARD does not reach them.
int header_lengths_safe(const Header *disk, const Header *self)
{
    if (disk->arena_size != self->arena_size) {
        refuse("save state refused: wrong size",
               "disk state refused: arena size mismatch (file %llu, this run %llu)",
               (unsigned long long)disk->arena_size,
               (unsigned long long)self->arena_size);
        return 0;
    }
    if (disk->dsstate_size != self->dsstate_size) {
        refuse("save state refused: layout changed",
               "disk state refused: dsstate size mismatch (file %llu, this run %llu)",
               (unsigned long long)disk->dsstate_size,
               (unsigned long long)self->dsstate_size);
        return 0;
    }
    if (disk->hw_size != self->hw_size) {
        refuse("save state refused: layout changed",
               "disk state refused: hardware-store size mismatch (file %llu, this run %llu)",
               (unsigned long long)disk->hw_size,
               (unsigned long long)self->hw_size);
        return 0;
    }
    return 1;
}

// Compare the on-disk header field by field against this process. On the first
// mismatch, name the field on stderr and return 0. Returns 1 only if every
// field matches.
int header_matches(const Header *disk, const Header *self)
{
    if (memcmp(disk->magic, self->magic, sizeof disk->magic) != 0) {
        refuse("save state refused: not a save file",
               "disk state refused: magic mismatch (not a save file)");
        return 0;
    }
    if (disk->format != self->format) {
        /* "different", not "older": a downgrade is rare but real (a player who
           reinstalls the previous release over the same folder), and a toast
           that says "older" would be a lie in exactly the case where somebody
           is already confused about which version they are running. */
        refuse("save state refused: different format",
               "disk state refused: format mismatch (file %u, build %u) -- "
               "written by a build with a different on-disk layout",
               disk->format, self->format);
        return 0;
    }
    if (strncmp(disk->gittip, self->gittip, GITTIP_FIELD) != 0) {
        refuse("save state refused: another build",
               "disk state refused: gittip mismatch (file %.*s, build %.*s)",
               (int)GITTIP_FIELD, disk->gittip, (int)GITTIP_FIELD, self->gittip);
        return 0;
    }
    /* THE GAME-DATA HALF (format 3). Placed right behind gittip because it is
       the same KIND of question -- "is this the same world" -- and because a
       version-mixed install is the one shape where gittip agrees and the world
       still does not: same exe, other asset folder. Everything below this line
       is about where things landed in memory, which is a different subject.

       Compared verbatim, empty included. Two runs of a non-ROM-clean build both
       stamp "" and match; a ROM-clean build never stamps "", so a state carried
       across that line is refused, which is correct -- those two exes do not
       read their tables from the same place at all. */
    if (strncmp(disk->romdata_sha, self->romdata_sha, SHA_FIELD) != 0 ||
        disk->romdata_bytes != self->romdata_bytes) {
        refuse("save state refused: other game data",
               "disk state refused: the save state was made from DIFFERENT GAME "
               "DATA.\n"
               "            state: %llu bytes, sha %.16s, from %s\n"
               "            now:   %llu bytes, sha %.16s, from %s\n"
               "            The exe matches; the asset folder does not. A world "
               "built from other\n"
               "            ROM tables cannot be restored onto these, so the "
               "game is booting fresh\n"
               "            instead. Your save state file has been left alone.",
               (unsigned long long)disk->romdata_bytes,
               disk->romdata_sha[0] ? disk->romdata_sha : "(none)          ",
               disk->asset_root[0] ? disk->asset_root : "(unrecorded)",
               (unsigned long long)self->romdata_bytes,
               self->romdata_sha[0] ? self->romdata_sha : "(none)          ",
               self->asset_root[0] ? self->asset_root : "(unrecorded)");
        return 0;
    }
    if (disk->image_base != self->image_base) {
        refuse("save state refused: exe moved in memory",
               "disk state refused: exe image base mismatch (file 0x%llx, this "
               "run 0x%llx) -- code pointers would not relocate",
               (unsigned long long)disk->image_base,
               (unsigned long long)self->image_base);
        return 0;
    }
    if (disk->arena_base != self->arena_base) {
        refuse("save state refused: memory moved",
               "disk state refused: arena base mismatch (file 0x%llx, this run 0x%llx)",
               (unsigned long long)disk->arena_base,
               (unsigned long long)self->arena_base);
        return 0;
    }
    if (disk->dsstate_base != self->dsstate_base) {
        refuse("save state refused: layout changed",
               "disk state refused: dsstate base mismatch (file 0x%llx, this run 0x%llx)",
               (unsigned long long)disk->dsstate_base,
               (unsigned long long)self->dsstate_base);
        return 0;
    }
    /* arena_size, dsstate_size and hw_size are checked by header_lengths_safe,
       which runs FIRST and is not overridable. They are deliberately not
       repeated here: a duplicate check that the kill switch could skip would
       read as if the sizes were policy, and they are memory safety. */
    return 1;
}

} // namespace

extern "C" {

// 1 if disk states are usable this run: Windows and the arena is at its fixed
// base. When 0, save and load are memory-only (the caller stays on lk6).
int lk7_persist_available(void)
{
    return port_arena_is_fixed();
}

// Write the current live world to <exedir>\savestate.bin. The caller invokes
// this right after a successful lk6_savestate_save, so live memory IS the slot
// content. Returns 1 if the file was written, 0 otherwise (disk states off, no
// arena, path or IO failure). A failure to write the disk copy never fails the
// in-memory save.
int lk7_persist_write(void)
{
    if (!lk7_persist_available()) return 0;

    Header hdr;
    if (!fill_header(&hdr)) return 0;

    char path[512];
    if (!state_path(path, sizeof path)) {
        fprintf(stderr, "[savestate] could not resolve disk state path; disk save skipped\n");
        return 0;
    }

    char  *base = (char *)port_arena_base();
    size_t asz  = (size_t)hdr.arena_size;
    size_t dsz  = (size_t)hdr.dsstate_size;
    size_t hsz  = (size_t)hdr.hw_size;

    /* THE SAME THREE QUESTIONS, ASKED OF THE LIVE WORLD BEFORE IT IS WRITTEN.
       The load refuses a state whose world cannot take a tick; this is the
       other end of it, and it is the better end -- a file that could never be
       loaded should not exist. A player who presses F8 in a moment the world
       cannot answer for gets "state NOT saved", which the toast already says,
       instead of a file that turns itself away three launches later. Reading
       LIVE memory through the same function the load uses, at the same
       offsets, so the two ends can never drift apart. */
    {
        const char *bad = world_fault(base, asz, &dsstate_lo, dsz);
        if (bad) {
            fprintf(stderr, "[savestate] disk save skipped: this world is not "
                    "in a state that can be reloaded -- %s. Nothing was "
                    "written; any earlier savestate.bin is untouched.\n", bad);
            return 0;
        }
    }

    // The hardware stores go through the same copy-out the slot uses, into one
    // temporary blob, so the file's byte order is the hook's fixed region order.
    char *hbuf = 0;
    if (hsz) {
        hbuf = (char *)malloc(hsz);
        if (!hbuf) { fprintf(stderr, "[savestate] out of memory for disk save; skipped\n"); return 0; }
        port_hw_regions_copy_out(hbuf);
    }

    // Write to a temp file then rename, so a crash mid-write cannot leave a
    // truncated savestate.bin that the next launch would try to load.
    char tmp[540];
    snprintf(tmp, sizeof tmp, "%s.tmp", path);
    FILE *f = fopen(tmp, "wb");
    if (!f) {
        fprintf(stderr, "[savestate] cannot open %s for write; disk save skipped\n", tmp);
        free(hbuf);
        return 0;
    }
    int ok = 1;
    ok &= (fwrite(&hdr, sizeof hdr, 1, f) == 1);
    ok &= (asz == 0 || fwrite(base, 1, asz, f) == asz);
    ok &= (dsz == 0 || fwrite(&dsstate_lo, 1, dsz, f) == dsz);
    ok &= (hsz == 0 || fwrite(hbuf, 1, hsz, f) == hsz);
    fclose(f);
    free(hbuf);
    if (!ok) {
        fprintf(stderr, "[savestate] write to %s failed; disk save skipped\n", tmp);
        remove(tmp);
        return 0;
    }

    remove(path);          // MoveFile/rename won't overwrite on Windows
    if (rename(tmp, path) != 0) {
        fprintf(stderr, "[savestate] rename %s -> %s failed; disk save skipped\n", tmp, path);
        remove(tmp);
        return 0;
    }
    fprintf(stderr, "[savestate] wrote disk state: %zu arena + %zu dsstate + "
                    "%zu hw bytes -> %s\n", asz, dsz, hsz, path);
    return 1;
}

// Read <exedir>\savestate.bin and, if its header matches this process in every
// field, copy the arena, the .dsstate section and the hardware stores straight
// into place and hand the world to lk6 (cursor, cache drops, audio reset, all
// of it) via lk6_savestate_save + lk6_savestate_load. Returns 1 if a disk state
// was loaded, 0 if there was no file, disk states are off, or the header was
// refused. A refusal names the field on stderr, records a short reason for the
// on-screen toast (lk7_persist_refusal) and a line for the crash dump's install
// block, and LEAVES THE FILE ALONE -- the caller boots fresh, which is the whole
// point: a state that cannot be trusted costs a player their save slot, not
// their launch.
int lk7_persist_read(void)
{
    g_refuse_short[0] = 0;
    g_refuse_long[0]  = 0;

    if (!lk7_persist_available()) return 0;

    char path[512];
    if (!state_path(path, sizeof path)) return 0;

    FILE *f = fopen(path, "rb");
    if (!f) return 0;   // no saved disk state; silent, this is the normal case

    Header disk;
    if (fread(&disk, sizeof disk, 1, f) != 1) {
        /* NOT where an older-format file lands. A format-2 savestate.bin is
           the header plus an 8MB arena, so this fread succeeds on it -- it
           reads a short header followed by arena bytes -- and the FORMAT field
           at offset 8 then turns it away, which is the door that reports it
           accurately. This one is for a file that is genuinely truncated: a
           write killed part-way through by something the .tmp+rename dance
           cannot cover (a full disk, a killed process on a filesystem that
           does not make the rename atomic). */
        refuse("save state refused: file is damaged",
               "disk state refused: file too short even for a header (%s is "
               "truncated); left untouched, booting fresh", path);
        fclose(f);
        return 0;
    }

    Header self;
    if (!fill_header(&self)) { fclose(f); return 0; }
    int overridden = 0;

    /* POLICY FIRST, so the message a reader gets is the most informative one.
       The field order in header_matches is chosen (gittip and romdata ahead of
       the memory-layout fields) precisely so that a stale state is turned away
       by the reason a human cares about; running the length checks first would
       report "arena size mismatch" for a file whose real problem is that it
       came from another build. */
    if (!header_matches(&disk, &self)) {
        /* THE FORENSIC HATCH. Off by default; when it is on, the refusal has
           already been printed in full, so the log says what the guard would
           have done as well as what it did. The game may well die a few frames
           later -- that is the point of asking. */
        if (!getenv("SM64DS_SAVESTATE_NO_GUARD")) {
            verdict_savestate_refused();
            fclose(f);
            return 0;
        }
        fprintf(stderr, "[savestate] SM64DS_SAVESTATE_NO_GUARD=1: loading it "
                        "anyway. This is a forensic option, not a fix.\n");
        port_install_verdict_add("savestate   guard OVERRIDDEN "
                                 "(SM64DS_SAVESTATE_NO_GUARD=1) -- the stamp "
                                 "did NOT match and the state was loaded anyway");
        /* The toast is cleared (the game did NOT boot fresh, so the on-screen
           "refused" note would be a lie) but overridden is remembered, so the
           success path below does not go on to claim the stamp matched. That
           claim in a dump from an overridden run would be the worst possible
           line: the one fact the reader needed, inverted. */
        g_refuse_short[0] = 0;
        overridden = 1;
    }
    /* ...and memory safety after it, unconditionally: the copies below are
       sized by the DISK header, so a longer file would run off the end of the
       live arena or section. The kill switch above deliberately does not reach
       this. */
    if (!header_lengths_safe(&disk, &self)) {
        verdict_savestate_refused();
        fclose(f);
        return 0;
    }

    size_t asz = (size_t)disk.arena_size;
    size_t dsz = (size_t)disk.dsstate_size;
    size_t hsz = (size_t)disk.hw_size;
    char  *base = (char *)port_arena_base();

    // Read the arena and the section into buffers rather than straight into
    // place. THE HEADER CANNOT SEE INSIDE THE WORLD, and this is the last point
    // at which the freshly booted world still exists: the copies below are
    // destructive, so a state that turns out not to describe a runnable world
    // has to be turned away BEFORE it lands, not diagnosed after. The bases
    // matched the header, so every absolute pointer in these bytes addresses
    // what it did at save, which is what lets world_fault() below read them at
    // their own offsets.
    char *abuf = asz ? (char *)malloc(asz) : 0;
    char *dbuf = dsz ? (char *)malloc(dsz) : 0;
    if ((asz && !abuf) || (dsz && !dbuf)) {
        fprintf(stderr, "[savestate] out of memory reading disk state\n");
        free(abuf); free(dbuf); fclose(f);
        return 0;
    }
    if (asz && fread(abuf, 1, asz, f) != asz) {
        fprintf(stderr, "[savestate] disk state refused: arena body truncated\n");
        free(abuf); free(dbuf); fclose(f);
        return 0;
    }
    if (dsz && fread(dbuf, 1, dsz, f) != dsz) {
        fprintf(stderr, "[savestate] disk state refused: dsstate body truncated\n");
        free(abuf); free(dbuf); fclose(f);
        return 0;
    }
    {
        /* The kill switch covers this refusal too, and on its own terms: the
           header may have matched every field (so `overridden` is still 0) and
           the thing being waved through is the WORLD. Reading the env directly
           is what lets a forensic run load a state this check would turn away
           and watch what it does. */
        const char *bad = world_fault(abuf, asz, dbuf, dsz);
        if (bad && getenv("SM64DS_SAVESTATE_NO_GUARD"))
            overridden = 1;
        if (bad && !overridden) {
            refuse("save state refused: it does not describe a world this "
                   "game can run",
                   "disk state refused: the saved world is not runnable -- %s. "
                   "The header matched on every field (same build, same game "
                   "data, same memory layout), so this is the world INSIDE the "
                   "file, not the file itself. Booting fresh and leaving %s "
                   "untouched.", bad, path);
            verdict_savestate_refused();
            free(abuf); free(dbuf); fclose(f);
            return 0;
        }
        if (bad)
            fprintf(stderr, "[savestate] SM64DS_SAVESTATE_NO_GUARD=1: the "
                            "saved world is not runnable (%s), loading it "
                            "anyway.\n", bad);
    }
    /* SM64DS_SS_NO_ROLLGUARD=1, the fix-off arm: THIS is the copy that rolls a
       disk state's .dsstate over the live one -- lk6_savestate_load is only
       reached afterwards, through the slot round-trip below -- so the bracket
       belongs here as well as there. See port_ss_rollguard_begin in
       hal/lk6_savestate.cpp for what a knob on the wrong copy site looked
       like. */
    const int norg = port_ss_rollguard_begin();
    if (asz) memcpy(base, abuf, asz);
    if (dsz) memcpy(&dsstate_lo, dbuf, dsz);
    port_ss_rollguard_end(norg);
    free(abuf);
    free(dbuf);
    if (hsz) {
        char *hbuf = (char *)malloc(hsz);
        if (!hbuf) { fprintf(stderr, "[savestate] out of memory reading disk state\n"); fclose(f); return 0; }
        if (fread(hbuf, 1, hsz, f) != hsz) {
            fprintf(stderr, "[savestate] disk state refused: hardware-store body truncated\n");
            free(hbuf);
            fclose(f);
            return 0;
        }
        port_hw_regions_copy_in(hbuf);   /* also drops the texture decode cache */
        free(hbuf);
    }
    fclose(f);

    // Put the arena carve cursor back where it stood at save. It is host state
    // in os_arena.cpp, NOT part of the arena bytes, so a fresh boot left it
    // wherever this session's own allocations reached; without this the lk6
    // snapshot below would capture the wrong high-water mark and the next
    // allocation would stomp restored data.
    port_arena_set_cursor((void *)(uintptr_t)disk.arena_cursor);

    // The bytes and the cursor are in place. Snapshot them into the lk6 slot so
    // lk6 owns the world exactly as if F8 had run this session, then load it:
    // that path re-asserts the cursor, drops the wave cache and resets audio,
    // and it leaves the slot valid so the menu shows a state is present and F9
    // works without touching the disk again.
    lk6_savestate_save();
    lk6_savestate_load();

    fprintf(stderr, "[savestate] loaded disk state: %zu arena + %zu dsstate + "
                    "%zu hw bytes from %s\n", asz, dsz, hsz, path);
    if (!overridden)
        port_install_verdict_add("savestate   loaded from disk, stamp matched");
    return 1;
}

// THE REFUSAL, FOR THE PLAYER AND FOR THE DUMP.
//
// A refusal that only reaches stderr is a refusal a player never learns about:
// the game boots fresh, their save state is silently gone, and the log they
// would have to open to find out why is one they do not know exists. So the
// reason is offered in two lengths and tests/walk_window.cpp shows the short
// one on screen next to where the "state loaded" note would have gone.
//
// Both return an empty string when the last read had nothing to refuse, so a
// caller can use non-empty as the test. lk7_persist_read clears them on entry,
// which matters because the reproducer harnesses call it more than once.
const char *lk7_persist_refusal(void)        { return g_refuse_short; }
const char *lk7_persist_refusal_detail(void) { return g_refuse_long; }

}
