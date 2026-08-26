// Disk save-state verification (lane lk7).
//
// This proves the save state survives the process ending. It reuses the actor
// world smoke_savestate builds (SetupRootHeap, spawn one real actor, tick it),
// then drives the DISK path in hal/lk7_persist.cpp across a real process
// boundary:
//
//   PHASE "save" (the parent):
//     1. boots the actor world and ticks it so real state lives in the arena;
//     2. saves the in-memory slot (lk6) and mirrors it to savestate.bin
//        (lk7_persist_write);
//     3. records the arena hash and prints it;
//     4. re-execs ITSELF with SM64DS_PERSIST_PHASE=load and the expected hash in
//        SM64DS_PERSIST_HASH, then returns the child's exit code.
//
//   PHASE "load" (the child, a genuinely separate process):
//     1. boots the SAME way but does NOT rebuild the evolved actor state;
//     2. calls lk7_persist_read, which validates the header, copies the arena
//        and globals into place and hands the world to lk6;
//     3. asserts the restored arena hash equals the parent's saved hash. A
//        second process loading the first one's disk state and landing on the
//        identical arena hash is the core proof.
//
// The refusal cases all run in-process in the parent after the child returns,
// so they need no second boot. Each doctors ONE field of a freshly written,
// otherwise-valid file, so the field named is provably the field that refused:
//   A  corrupted header (magic byte flipped)            -> refused, file left
//   B  gittip mismatch (the build-tip field overwritten) -> refused
//   C  DIFFERENT GAME DATA (the romdata identity field)  -> refused
//   D  the same file as C with SM64DS_SAVESTATE_NO_GUARD=1 -> LOADED, and the
//      override does not persist to the next read
//   E  a legacy format-2 file                            -> refused on format
//   F  a truncated file                                  -> refused, says so
// and then a clean valid file must load with NOTHING refused, so a green run
// cannot be one where every read happens to fail.
//
// C is the case format 2 could not express at all: same exe, same build, same
// image and arena and section bounds -- and a world built out of a different
// asset folder's ROM tables. Every EXE-side field agrees and the state loads.
// The header now carries which romdata.bin the world was built from, so it
// does not. No player has reported this; it is a hole found by reading the
// loader (hal/romdata_loader.cpp has the write-up) rather than by a report.
//
// The arena is pinned at a fixed host base (hal/os_arena.cpp), so both the
// parent and the child bring the arena up at the SAME base and the saved
// pointers relocate. If that pin ever fails, lk7_persist_available returns 0 and
// this test reports the run as skipped rather than failing, because a disk state
// is legitimately off when the arena is not fixed.

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "ntr/gx.h"
#include "ntr/mmio.h"
#include "ntr/ppu.h"

#include "fault_probe.h"

typedef unsigned int u32;

extern "C" {
int *ArrowSignRight_Spawn(void);
void *_ZN4Heap13SetupRootHeapEv(void);
extern int data_0209b3ec[12];       /* camera matrix */
struct SharedFilePtrC { unsigned short fileID; unsigned char numRefs;
                        unsigned char pad; void *filePtr; };
SharedFilePtrC *_ZN13SharedFilePtr9ConstructEj(SharedFilePtrC *s, u32 id);
extern unsigned short data_020a4b54;    /* pending actor ID */
extern void **data_020a4bb8;            /* actorID -> SpawnInfo* */
void *data_ov098_0213c380[6];
char data_ov098_0213c384[0x18];
extern void *data_020a0eac_c;           /* Memory::gameHeapPtr */
extern void *data_020a0ea0;             /* defaultHeapPtr */
void _ZN4Heap18InitializeGameHeapEjPS_(unsigned size, void *root);
unsigned _ZN22ExpandingHeapAllocator10MemoryLeftEv(void *alloc);
extern void *data_0209f394[];           /* the player array */
extern unsigned char data_0209f21c;     /* player count */
void hal_fill_model_vtable(void);
void hal_fill_shadow_vtable(void);
void hal_fill_mmc_vtable(void);

// the in-memory + disk save-state layers under test
int lk6_savestate_save(void);
int lk6_savestate_load(void);
int lk6_savestate_has(void);
int lk7_persist_write(void);
int lk7_persist_read(void);
int lk7_persist_available(void);
const char *lk7_persist_refusal(void);
const char *lk7_persist_refusal_detail(void);

// arena window
void *port_arena_base(void);
void *port_arena_end(void);
void *port_arena_cursor(void);
int   port_arena_is_fixed(void);
extern int LCG_STATE_0204da4c;
}

// lk7_persist_read hands off to lk6_savestate_load, which calls the sdat
// resets. A headless smoke opens no device, so they are no-ops here, the same
// four smoke_savestate stubs. The hardware-store hooks need no stubs: this
// smoke links ntr, so the real port_hw_regions_* in ntr/io.cpp serve both lk6
// and lk7 against the real reservations.
void sd_seq_reset(void) {}
void sd_mix_reset(void) {}
void sd_consumer_reset(void) {}
void sd_waves_reset(void) {}
void sd_sdat_reseat(void) {}

typedef int (__thiscall *Fn0)(void *);
static int vcall0(void *actor, int slot)
{
    void **vt = *(void ***)actor;
    return ((Fn0)vt[slot])(actor);
}

static int g_failures;
#define CHECK(cond) \
    do { if (!(cond)) { ++g_failures; \
        fprintf(stderr, "FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); } } while (0)

static void ident_fx(void *m)
{
    memset(m, 0, 48);
    ((int *)m)[0] = ((int *)m)[4] = ((int *)m)[8] = 0x1000;
}

static void reset_scene()
{
    ntr::gx_reset();
    NTR_MMIO(uint32_t, 0x04000440) = 0;
    NTR_MMIO(uint32_t, 0x04000454) = 0;
    NTR_MMIO(uint32_t, 0x04000440) = 1;
    NTR_MMIO(uint32_t, 0x04000454) = 0;
    NTR_MMIO(uint32_t, 0x04000580) = 0u | (0u << 8) | (255u << 16) | (191u << 24);
    ntr::gx_set_light(0, -0.4f, -0.6f, -0.7f, 0x7FFF);
    ntr::gx_enable_lights(0x1);
}

static uint64_t arena_hash()
{
    const unsigned char *b = (const unsigned char *)port_arena_base();
    const unsigned char *e = (const unsigned char *)port_arena_end();
    uint64_t h = 1469598103934665603ULL;
    for (; b < e; ++b) { h ^= *b; h *= 1099511628211ULL; }
    return h;
}

// Boot the shared actor world both phases stand on. Returns the spawned actor.
static int *boot_actor_world()
{
    CHECK(_ZN4Heap13SetupRootHeapEv() != NULL);
    ident_fx(data_0209b3ec);

    hal_fill_model_vtable();
    hal_fill_shadow_vtable();
    hal_fill_mmc_vtable();
    data_020a4b54 = 0x12b;
    static unsigned short spawn_info[4] = { 0, 0, 100, 100 };
    data_020a4bb8[0x12b] = spawn_info;
    /* THE GAME HEAP, the ROM's own chain instead of an alias -- the same
       bring-up walk_window.cpp, smoke_player.cpp, smoke_actor.cpp and
       smoke_savestate.cpp do. This line used to be
       `data_020a0eac_c = data_020a0ea0;`, the game-heap word aliased onto the
       root heap, and the retained-hand-seed comment here said the conversion
       goes with the port/CMakeLists.txt line that carries
       src/_ZN4Heap18InitializeGameHeapEjPS_.c onto this target. That line is
       in, so this is that conversion.
       BOTH PROCESSES CONVERT TOGETHER, which is what that comment required and
       what this placement gives for free: the parent and the spawned child are
       the SAME exe and both reach the game heap only through this one
       function, so there is no build in which one side carves and the other
       aliases. The gittip header refuses a disk state from any other build
       anyway, so a savestate.bin written before this change is rejected rather
       than loaded against a differently-shaped arena. */
    _ZN4Heap18InitializeGameHeapEjPS_(0x3b000, 0);
    CHECK(data_020a0eac_c != NULL);
    if (!data_020a0eac_c) {
        fprintf(stderr, "InitializeGameHeap returned null -- no game heap\n");
        return NULL;
    }
    fprintf(stderr, "[heap] game heap %p, 0x%x bytes, %u free after carve\n",
            data_020a0eac_c, 0x3b000u,
            _ZN22ExpandingHeapAllocator10MemoryLeftEv(
                *(void **)((char *)data_020a0eac_c + 0x14)));
    static SharedFilePtrC sign_model, sign_kcl;
    _ZN13SharedFilePtr9ConstructEj(&sign_model, 1177);
    _ZN13SharedFilePtr9ConstructEj(&sign_kcl, 1178);
    data_ov098_0213c380[0] = &sign_model;
    data_ov098_0213c380[1] = &sign_kcl;
    *(void **)(data_ov098_0213c384 + 0) = &sign_kcl;

    static char fake_player[0x800];
    data_0209f394[0] = fake_player;
    *(unsigned char *)&data_0209f21c = 1;

    int *actor = ArrowSignRight_Spawn();
    CHECK(actor != NULL);
    if (!actor) return NULL;
    /* where the actor landed, printed for the same reason smoke_actor prints
       it: under the old hand-seed this came back at the ROOT heap's top, and
       the carve above is only doing something if the actor is inside the
       0x3b000 payload the game heap owns. Both processes print their own, so
       the two-process half of this smoke shows the carve twice. */
    printf("  spawned at %p (game heap %p)\n", (void *)actor, data_020a0eac_c);
    CHECK(vcall0(actor, 0) == 1);           // InitResources
    reset_scene();
    return actor;
}

static const char *state_file_path(char *buf, size_t cap)
{
    char exe[MAX_PATH];
    DWORD n = GetModuleFileNameA(NULL, exe, MAX_PATH);
    if (n == 0 || n >= MAX_PATH) return NULL;
    char *slash = strrchr(exe, '\\');
    char *fwd = strrchr(exe, '/');
    if (fwd && (!slash || fwd > slash)) slash = fwd;
    if (!slash) return NULL;
    *slash = '\0';
    snprintf(buf, cap, "%s\\savestate.bin", exe);
    return buf;
}

// ---- PHASE "load": the spawned child --------------------------------------
static int phase_load()
{
    setvbuf(stdout, NULL, _IONBF, 0);
    if (!ntr::io_init()) { fprintf(stderr, "io_init failed\n"); return 2; }
    if (!port_arena_is_fixed()) {
        fprintf(stderr, "  [child] arena not at fixed base; disk load cannot run\n");
        return 3;   // signalled to the parent as a skip
    }

    const char *hs = getenv("SM64DS_PERSIST_HASH");
    if (!hs) { fprintf(stderr, "  [child] no expected hash passed\n"); return 4; }
    uint64_t expect = strtoull(hs, NULL, 16);

    int *actor = boot_actor_world();
    if (!actor) { fprintf(stderr, "  [child] actor world did not boot\n"); return 1; }

    // Load the disk state the parent wrote. This is the cross-process load: a
    // separate process picking up the first one's savestate.bin.
    int loaded = lk7_persist_read();
    CHECK(loaded == 1);
    CHECK(lk6_savestate_has() == 1);

    uint64_t got = arena_hash();
    printf("  [child] loaded disk state, arena hash=%016llx expected=%016llx\n",
           (unsigned long long)got, (unsigned long long)expect);
    CHECK(got == expect);

    // keep running after the load: the actor still dispatches
    for (int f = 0; f < 8; ++f) vcall0(actor, 6);
    (void)vcall0(actor, 9);
    printf("  [child] ran 8 frames after the cross-process load: no crash\n");

    if (g_failures) { fprintf(stderr, "  [child] %d FAILURE(S)\n", g_failures); return 1; }
    printf("  [child] cross-process load byte-exact: ok\n");
    return 0;
}

// ---- PHASE "save": the parent ---------------------------------------------
static int phase_save()
{
    PORT_INSTALL_FAULT_PROBE();
    setvbuf(stdout, NULL, _IONBF, 0);
    if (!ntr::io_init()) { fprintf(stderr, "io_init failed\n"); return 2; }

    if (!port_arena_is_fixed()) {
        // Legitimately off: the fixed base was taken. A disk state cannot
        // relocate, so report a skip rather than a failure.
        printf("smoke_persist: SKIPPED (arena not at fixed base; disk states off)\n");
        return 0;
    }
    CHECK(lk7_persist_available() == 1);

    char path[512];
    if (!state_file_path(path, sizeof path)) { fprintf(stderr, "no state path\n"); return 2; }
    remove(path);   // start from no file so the write is provably ours

    int *actor = boot_actor_world();
    if (!actor) { fprintf(stderr, "smoke_persist: no actor, abort\n"); return 1; }

    // evolve real state, then move a few words so there is unmistakable live
    // state to carry across the restart
    for (int f = 0; f < 8; ++f) vcall0(actor, 6);
    *(int *)((char *)actor + 0x5c) = 0x11110000;
    *(int *)((char *)actor + 0x60) = 0x22220000;
    *(int *)((char *)actor + 0x64) = 0x33330000;
    *(int *)((char *)actor + 0x70) = 0x0000abcd;
    LCG_STATE_0204da4c = 0x5eedf715;

    // save to the in-memory slot, then mirror to disk
    CHECK(lk6_savestate_save() == 1);
    CHECK(lk7_persist_write() == 1);

    // the file must now exist and be non-trivial
    FILE *chk = fopen(path, "rb");
    CHECK(chk != NULL);
    long fsz = 0;
    if (chk) { fseek(chk, 0, SEEK_END); fsz = ftell(chk); fclose(chk); }
    CHECK(fsz > (long)(8 << 20));   // header + 8MB arena + globals
    printf("  [parent] wrote savestate.bin (%ld bytes) at %s\n", fsz, path);

    uint64_t saved = arena_hash();
    printf("  [parent] saved arena hash=%016llx\n", (unsigned long long)saved);

    // ---- spawn a genuinely separate process to load it --------------------
    char exe[MAX_PATH];
    GetModuleFileNameA(NULL, exe, MAX_PATH);
    char hashenv[64];
    snprintf(hashenv, sizeof hashenv, "%016llx", (unsigned long long)saved);
    SetEnvironmentVariableA("SM64DS_PERSIST_PHASE", "load");
    SetEnvironmentVariableA("SM64DS_PERSIST_HASH", hashenv);

    STARTUPINFOA si; memset(&si, 0, sizeof si); si.cb = sizeof si;
    PROCESS_INFORMATION pi; memset(&pi, 0, sizeof pi);
    printf("  [parent] spawning a second process to load the disk state...\n");
    BOOL ok = CreateProcessA(exe, NULL, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
    CHECK(ok);
    int child_rc = -1;
    if (ok) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        DWORD rc = 0; GetExitCodeProcess(pi.hProcess, &rc); child_rc = (int)rc;
        CloseHandle(pi.hProcess); CloseHandle(pi.hThread);
    }
    SetEnvironmentVariableA("SM64DS_PERSIST_PHASE", NULL);
    printf("  [parent] child exited %d\n", child_rc);
    if (child_rc == 3) {
        printf("smoke_persist: SKIPPED (child could not pin the fixed base)\n");
        return 0;
    }
    CHECK(child_rc == 0);

    // ---- refusal case A: a corrupted header is refused cleanly ------------
    // Flip the first magic byte, then try to read: the read must refuse and
    // return 0, and it must NOT touch the (already valid) in-memory slot.
    {
        FILE *f = fopen(path, "r+b");
        CHECK(f != NULL);
        if (f) { unsigned char bad = 'X'; fseek(f, 0, SEEK_SET); fwrite(&bad, 1, 1, f); fclose(f); }
        int r = lk7_persist_read();
        CHECK(r == 0);
        printf("  [parent] corrupted-magic header refused: r=%d\n", r);
    }

    // ---- refusal case B: a gittip mismatch is refused cleanly -------------
    // Rewrite a valid file (write() again), then overwrite the gittip field with
    // a different build string. Header magic/format/base/size still match, so
    // the ONLY thing that can turn it away is the gittip field.
    {
        CHECK(lk7_persist_write() == 1);
        // header layout: magic[8], format(u32), gittip[64] at offset 12
        FILE *f = fopen(path, "r+b");
        CHECK(f != NULL);
        if (f) {
            const char fake[] = "deadbeef-not-this-build";
            fseek(f, 12, SEEK_SET);
            fwrite(fake, 1, sizeof fake, f);   // includes the terminating NUL
            fclose(f);
        }
        int r = lk7_persist_read();
        CHECK(r == 0);
        CHECK(lk7_persist_refusal()[0] != 0);
        printf("  [parent] gittip-mismatch header refused: r=%d  \"%s\"\n",
               r, lk7_persist_refusal());
    }

    // ---- refusal case C: DIFFERENT GAME DATA is refused cleanly -----------
    // A folder that outlived the build that filled it. Everything about the
    // EXE agrees -- same binary, same build, same image base, same arena,
    // same section -- and the asset folder underneath it is another release's.
    // Format 2 could not ask the question at all; format 3 stamps the romdata
    // identity, so this is the field that turns the file away.
    //
    // The offset is derived, not guessed: magic[8] + format(4) + gittip[64] +
    // seven u64s (image_base, arena_base, arena_size, arena_cursor,
    // dsstate_base, dsstate_size, hw_size) = 8+4+64+56 = 132, and the struct is
    // #pragma pack(1).
    //
    // WHAT KEEPS THIS HONEST IS THE strstr, NOT THE OFFSET. An earlier note here
    // claimed a field inserted above romdata_sha would push this poke into
    // asset_root and turn the CHECK red. That is wrong: any insertion smaller
    // than 65 bytes still lands INSIDE romdata_sha, the refusal still fires, and
    // the case would go green while testing a field it does not name. The
    // assertion on "DIFFERENT GAME DATA" below is what pins the case to the
    // right refusal site; the offset only has to be close enough to hit the
    // field. If a field is added above, fix the constant.
    const long ROMDATA_SHA_OFFSET = 132;
    {
        CHECK(lk7_persist_write() == 1);
        FILE *f = fopen(path, "r+b");
        CHECK(f != NULL);
        if (f) {
            // a plausible sha of somebody else's romdata.bin
            const char other[] =
                "0f1e2d3c4b5a69788796a5b4c3d2e1f00f1e2d3c4b5a69788796a5b4c3d2e1f0";
            fseek(f, ROMDATA_SHA_OFFSET, SEEK_SET);
            fwrite(other, 1, sizeof other, f);   // includes the terminating NUL
            fclose(f);
        }
        int r = lk7_persist_read();
        CHECK(r == 0);
        CHECK(lk7_persist_refusal()[0] != 0);
        CHECK(strstr(lk7_persist_refusal_detail(), "DIFFERENT GAME DATA") != NULL);
        printf("  [parent] other-game-data header refused: r=%d  \"%s\"\n",
               r, lk7_persist_refusal());
    }

    // ---- case D: the kill switch loads the SAME doctored file -------------
    // SM64DS_SAVESTATE_NO_GUARD=1 is the forensic hatch. It must actually
    // reach the load (otherwise it is a comment, not a switch), and it must
    // still be honest about what it skipped. The file from case C is left on
    // disk deliberately: the only thing that changes between the two cases is
    // the environment variable.
    {
        /* _putenv, NOT SetEnvironmentVariableA. The guard reads the switch with
           getenv, and MSVC's getenv answers out of the CRT's own copy of the
           environment, which SetEnvironmentVariableA does not touch -- it edits
           the Win32 block, which is what a CHILD process inherits (that is why
           the PERSIST_PHASE handoff above uses it and is right to). Setting it
           the Win32 way here would leave getenv answering null and this case
           would "prove" the switch does nothing. */
        _putenv("SM64DS_SAVESTATE_NO_GUARD=1");
        int r = lk7_persist_read();
        _putenv("SM64DS_SAVESTATE_NO_GUARD=");
        CHECK(r == 1);
        printf("  [parent] SM64DS_SAVESTATE_NO_GUARD=1 loaded the refused "
               "state: r=%d\n", r);
        // ...and with the switch back off, the same file is refused again, so
        // the override is per-run and leaves nothing behind.
        CHECK(lk7_persist_write() == 1);
        FILE *f = fopen(path, "r+b");
        if (f) {
            const char other[] =
                "0f1e2d3c4b5a69788796a5b4c3d2e1f00f1e2d3c4b5a69788796a5b4c3d2e1f0";
            fseek(f, ROMDATA_SHA_OFFSET, SEEK_SET);
            fwrite(other, 1, sizeof other, f);
            fclose(f);
        }
        CHECK(lk7_persist_read() == 0);
        printf("  [parent] switch off again: refused again\n");
    }

    // ---- case E: a LEGACY (format 2) file is refused, not misread ---------
    // What every savestate.bin written before this change is: a full-size file,
    // header plus an 8MB arena, from a build whose header had no romdata fields.
    // The read of a format-3-sized header off it SUCCEEDS -- it gets the short
    // header followed by arena bytes -- so the thing that has to turn it away
    // is the FORMAT field, before a byte of the body is trusted.
    {
        CHECK(lk7_persist_write() == 1);
        FILE *f = fopen(path, "r+b");
        CHECK(f != NULL);
        if (f) {
            unsigned old_format = 2;            // the pre-format-3 layout
            fseek(f, 8, SEEK_SET);              // magic[8], then format(u32)
            fwrite(&old_format, sizeof old_format, 1, f);
            fclose(f);
        }
        int r = lk7_persist_read();
        CHECK(r == 0);
        CHECK(strstr(lk7_persist_refusal_detail(), "format mismatch") != NULL);
        CHECK(strstr(lk7_persist_refusal_detail(), "file 2, build 3") != NULL);
        printf("  [parent] legacy format-2 state refused: r=%d  \"%s\"\n",
               r, lk7_persist_refusal());
    }

    // ---- case F: a TRUNCATED file is refused, and says so ------------------
    // The other legacy shape: a file too short to be a header at all, which is
    // what a write killed part-way through leaves behind.
    {
        CHECK(lk7_persist_write() == 1);
        FILE *src = fopen(path, "rb");
        CHECK(src != NULL);
        static char head[64];
        size_t got = src ? fread(head, 1, sizeof head, src) : 0;
        if (src) fclose(src);
        FILE *dst = fopen(path, "wb");
        CHECK(dst != NULL);
        if (dst) { fwrite(head, 1, got, dst); fclose(dst); }
        int r = lk7_persist_read();
        CHECK(r == 0);
        CHECK(strstr(lk7_persist_refusal_detail(), "truncated") != NULL);
        printf("  [parent] truncated state refused: r=%d  \"%s\"\n",
               r, lk7_persist_refusal());
    }

    // leave a clean valid file behind (tidy, and matches shipped behaviour)
    CHECK(lk7_persist_write() == 1);
    // ...and a valid file must still say nothing to refuse, so a passing run
    // cannot be one where every read refuses for some unrelated reason.
    CHECK(lk7_persist_read() == 1);
    CHECK(lk7_persist_refusal()[0] == 0);
    remove(path);   // do not leave a savestate.bin in the build tree

    if (g_failures) { fprintf(stderr, "smoke_persist: %d FAILURE(S)\n", g_failures); return 1; }
    /* The summary names EVERY case. It used to name four of six, which is the
       same under-reporting this smoke exists to catch: a line that reads as a
       full account and is not one. If a case is added below, add it here. */
    printf("smoke_persist: all checks passed (wrote disk state, a second process "
           "loaded it byte-exact; corrupted-magic, stale-build, other-game-data, "
           "legacy-format and truncated headers all refused; the forensic "
           "override loads one and does not stick; a clean file refuses "
           "nothing)\n");
    return 0;
}

int main(void)
{
    const char *phase = getenv("SM64DS_PERSIST_PHASE");
    if (phase && strcmp(phase, "load") == 0) return phase_load();
    return phase_save();
}
