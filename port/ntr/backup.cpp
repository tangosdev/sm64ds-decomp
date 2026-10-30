// THE DS BACKUP MEDIUM, HOSTED, AND THE ARM7 THAT ANSWERS FOR IT.
// Run link100, lane SAVE.
//
// ============================================================================
// 1. WHAT THIS REPLACES
// ============================================================================
//
// port/README.md used to say "There is no save medium yet ... Persistence is
// deliberately out of scope", and hal/scene_boot.cpp made that true by DEFINING
// the two leaves of the game's save system itself:
//
//     _ZN8SaveData14SaveDataToCartEPcjj     SaveData::SaveDataToCart
//     _ZN8SaveData16ReadDataFromCartEPcjj   SaveData::ReadDataFromCart
//
// Both are byte-matched translation units sitting in src/, and both were host
// SHADOWS: a host body of the same name ran and the ROM's own body did not
// reach the binary. The host bodies stored the game's SaveData buffers verbatim
// into a container file with a host header, a host CRC32 and a host valid-slot
// bitmap, and the block above them said the ROM's own record framing "cannot be
// reproduced bit-for-bit on the host: BOTH data_020a4b40 (the magic) and
// data_020a8760 (the medium size) are bss the absent CARD driver fills at
// runtime, so a framed image would have to GUESS a magic and a size the host
// never has."
//
// NEITHER WAS A GUESS AND BOTH ARE IN THE ROM. src/func_0201a054.c -- the
// game's own boot spine, decompiled and committed -- is one line:
//
//     func_02042f68(0xd01, data_0208ee50);
//
// func_02042f68 is the save system's init (src/func_02042f68.c): it runs the
// card driver's init, identifies the backup device as type 0xd01, and then
// copies eight bytes out of its second argument into data_020a4b40, stopping at
// the first zero. So the magic is whatever data_0208ee50 holds and the size is
// whatever row 0xd01 of the driver's device table says.
//
//     data_0208ee50   arm9 .rodata   "ds mario"    (64 73 20 6d 61 72 69 6f)
//     0xd01           arm9 .rodata   device table row at 0x020867bc + 0x28:
//                     type 0x00000d01, total 0x2000 bytes, page 0x20,
//                     kind 2 -- a 64 Kbit serial EEPROM, 8192 bytes.
//
// Both were read straight out of extracted/arm9_dec.bin (flat, based at
// 0x02004000 -- the same base port/tools/romdata.py uses and states) and both
// are quoted below with their addresses. The magic is an eight-character tag
// and the device row is five words of chip geometry; they are named constants
// here rather than a romdata.py mount ONLY because a ROM-CLEAN kit zeroes every
// romdata array, and a zeroed device row is a zero-byte save chip -- i.e. a
// shipped kit that silently cannot save. See THE ROMDATA PROPOSAL at the foot.
//
// So this file hosts the MEDIUM and nothing above it. The game's save logic --
// ReadFileData, SaveFile, EraseSaveFile, SaveMinigames, ReadMinigameData, the
// checksum, the record tag, the primary copy and the mirror, the retry loops --
// is the ROM's own matched code, linked from src/ through port/slice_gate215.txt.
//
// ============================================================================
// 2. THE ROM'S PATH FROM SaveData DOWN TO A CHIP, AND WHERE IT IS CUT
// ============================================================================
//
//   SaveData::SaveFile / EraseSaveFile / SaveMinigames        (already linked)
//     -> SaveData::SaveDataToCart          src/_ZN8SaveData14SaveDataToCartEPcjj.cpp
//   SaveData::ReadFileData / ReadMinigameData                 (already linked)
//     -> SaveData::ReadDataFromCart        src/_ZN8SaveData16ReadDataFromCartEPcjj.cpp
//
//        both then:
//          func_0203da3c()      the wireless role byte; 2 == "no cartridge"
//          func_0206045c()      the chip size, data_020a8760[1]
//          func_02057020()      take an OS lock id                 <-- HOSTED
//          func_0205ff80/70()   claim / release the card
//            -> func_02060188 / func_020600e0
//          func_02060484()      write span      -> func_0206062c
//          func_02060558()      read span       -> func_0206071c
//            -> func_02060f60(work, cmd, retries)                  <-- HOSTED
//          func_02057078()      release the lock id                <-- HOSTED
//
// func_02060f60 IS THE ARM7 BOUNDARY. Its ROM body (src/func_02060f60.cpp)
// pushes a command number down PXI channel 0xB with IPCSend, then sleeps on
// data_020a8180+0x34 bit 0x20 until the ARM7's interrupt clears it and reads
// the ARM7's answer out of the shared command block. The port has no ARM7 and
// no PXI reply, so that sleep never ends: the port's OS_SleepThread(0)
// (hal/os_thread.cpp) runs one pump turn and returns, and the ROM loop spins
// forever. THIS FILE IS THE ARM7's HALF OF THAT CONVERSATION. Everything above
// func_02060f60 is the ROM's.
//
// The two lock primitives are cut for a second, unrelated reason: func_02057020
// and func_02057078 are hand-written ARM (`asm { ... }` blocks MSVC cannot
// assemble) that allocate an OS lock id out of the bitmask at 0x027FFFB0, a
// page ntr maps nothing at. hal/fs_names.cpp already faces exactly this for
// FS_Init and says so; the same ruling, the same page, one place.
//
// ============================================================================
// 3. THE COMMAND BLOCK, AS THE ROM FILLS IT
// ============================================================================
//
// data_020a8180 is the card driver's work object and data_020a8160 the 32-byte
// command block it shares with the ARM7 (src/func_0206002c.c wires the two
// together and this file reproduces that wiring statically). The three words
// the transfer commands use are the same three in both directions:
//
//     block + 0x00   result written by the ARM7; 0 = done, 4 = busy/retry
//     block + 0x04   backup device type (func_020607e8 puts it there)
//     block + 0x0c   SOURCE       medium byte offset on a read,
//                                 ARM9 pointer on a write
//     block + 0x10   DESTINATION  ARM9 pointer on a read,
//                                 medium byte offset on a write
//     block + 0x14   length, at most 0x100 -- the staging buffer's size
//
// and the commands, read off the four call sites (there are only four):
//
//     0   src/func_02060f60.cpp   hand the ARM7 the block pointer (init)
//     2   src/func_020607e8.c     identify the backup device
//     6   src/func_0206071c.c     read a span
//     8   src/func_0206062c.cpp   program a span   (retries 0xa)
//     9   src/func_0206062c.cpp   verify what 8 just programmed
//
// Command 8 and command 9 arrive with the SAME block still filled in, which is
// why verify below compares the medium against the same source pointer.
//
// ============================================================================
// 4. THE FILE
// ============================================================================
//
// It is a cartridge image and nothing else: exactly 8192 bytes, no host header,
// no host checksum, no host version word. Every byte in it was put there by the
// ROM's own SaveDataToCart. A fresh file is filled with 0xFF, which is what an
// erased EEPROM cell reads back as, and 0xFF fails the ROM's record-tag compare
// on both the primary copy and the mirror -- which is exactly the "2" answer
// ReadDataFromCart gives for a blank medium, the answer that makes every caller
// run SetDefaultValues. Deleting the file is buying a new cartridge.
//
// LAYOUT, and this is the ROM's, not ours (src/_ZN8SaveData14SaveDataToCartEPcjj.cpp):
//
//     primary copy   at  slot << 7
//     mirror copy    at  (8192 / 2) + (slot << 7)
//
//     record + 0     u16  checksum, little endian
//     record + 2     8    the record tag, data_020a4b40 = "ds mario"
//     record + 10    n    the payload -- the game's SaveData bytes verbatim
//
//     checksum = sum of the eight tag bytes as a u16, then for every payload
//                byte: rotate the u16 left by one, xor in the byte.
//
//     slots 0,1,2   FileSaveData      0x44 bytes   (ReadFileData / SaveFile)
//     slot  3       MinigameSaveData  0x2e4 bytes  (ReadMinigameData / SaveMinigames)
//
// The whole layout is verifiable from outside the game, and port/tools/save_proof.py
// does verify it -- tag, checksum, primary and mirror, on a file the game wrote.
//
// WHERE IT LIVES:
//     1. $SM64DS_SAVE_PATH        exact path, if set (what the proof uses); its
//                                 parent directory is created too, same as the
//                                 other three candidates below
//     2. <folder holding the exe>/save/sm64ds.sav        -- a player's kit
//     3. $SM64DS_ASSET_ROOT/save/sm64ds.sav
//     4. ./save/sm64ds.sav
// The save/ directory is created if it does not exist. Writes go to a
// per-process sibling temp and are moved over the real file, so a reader sees
// the old image or the new one and never a half-written mixture.
//
// ============================================================================
// 5. WHAT THIS DOES NOT DO
// ============================================================================
//
// NO ERASE, NO SECTOR MAP, NO WRITE ENDURANCE. A 64 Kbit EEPROM is byte
// writable and the ROM never sends an erase command to one. (An older note here
// named func_02060a64 and its neighbours as the FLASH erase family: they are the
// card's CPU ROM read path, linked since run linkfull lane S4CARD over
// ntr/card.cpp, and they never touch the backup chip.) If a later lane links a
// real erase family, its commands land in the `default:` arm below and are
// refused honestly rather than silently succeeding.
//
// NO SAVESTATE COUPLING FOR THE MEDIUM. g_image and its file are plain host
// storage OUTSIDE every DSSTATE_BEGIN/END block, so the lk6/lk7 dev savestate
// never captures or restores them: a savestate is a snapshot of RAM and a
// cartridge is not RAM, so loading a state must not un-write a save. The card
// driver's arm9 bss at the foot of this file IS RAM and IS captured, which is
// the line dsstate_guard draws and enforces.

#include "ntr/backup.h"

/* The card driver's arm9 bss is hosted at the foot of this file and it IS DS
   RAM, so the lk6/lk7 dev savestate has to capture it like any other hosted
   global -- dsstate_guard enforces that. The MEDIUM above is the other side of
   the line and stays out: a savestate is a snapshot of RAM and a cartridge is
   not RAM, so restoring a state must not un-write a save. */
#include "hal/dsstate_seg.h"
#include "hal/host_settings.h"   /* adventure_ghost_mode() */
#include "hal/comms_hub.h"       /* SEAMCLEAN1: port::online_session_get, the session record */
#include <random>

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <ctime>

#ifdef _WIN32
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <windows.h>
#else
#  include <sys/stat.h>
#  include <sys/types.h>
#  include <unistd.h>
#endif

/* defined in the ROM-side block at the foot of this file */
extern "C" unsigned char data_020a4b40[8];
/* CAREER1: told of every write before it lands (image = the chip as it is now), so hal/career.cpp can follow a file copy or erase. Null by default. */
extern "C" { void (*port_backup_write_hook)(unsigned addr, const void *src, unsigned len, const unsigned char *image) = 0; }

namespace ntr {
namespace backup {
namespace {

bool          g_open = false;      // the image has been resolved and loaded
bool          g_fresh = true;      // no file existed: this is a new cartridge
char          g_path[1024];
unsigned char g_image[kSize];
Stats         g_stats;

bool env_on(const char *name)
{
    const char *e = std::getenv(name);
    return e && *e && *e != '0';
}

/* SEAMCOOP1: a coop room entered or left in a live process (hal/coop_boot.cpp) chooses at run time through select_coop below:
   -1 = the environment as at boot, 0 = solo, 1 = coop guest, 2 = coop host */
int  g_force_coop = -1;
bool coop_mode() { if (g_force_coop >= 0) return g_force_coop != 0; static int v = -1; if (v < 0) v = env_on("SM64DS_COOP") ? 1 : 0; return v != 0; }
bool coop_host() { if (g_force_coop >= 0) return g_force_coop == 2; static int v = -1; if (v < 0) v = env_on("SM64DS_COOP_HOST") ? 1 : 0; return v != 0; }

/* The ROM copies "ds mario" into data_020a4b40 at boot (func_02042f68, run by
   hal/boot_arms.cpp R2c), after this file static init. A coop room therefore
   puts its own tag back before the first card access and before every write. */
void coop_assert_tag()
{
    if (coop_mode() && std::memcmp(data_020a4b40, "ds coop!", 8) != 0)
        std::memcpy(data_020a4b40, "ds coop!", 8);
}

const unsigned char kTagNormal[8] = { 'd', 's', ' ', 'm', 'a', 'r', 'i', 'o' };
const unsigned char kTagCoop[8]   = { 'd', 's', ' ', 'c', 'o', 'o', 'p', '!' };

bool ends_with_coopsav(const char *p)
{
    size_t n = std::strlen(p);
    if (n < 8) return false;
    const char *e = p + n - 8;
    static const char want[] = ".coopsav";
    for (int i = 0; i < 8; ++i) {
        char c = e[i];
        if (c >= 'A' && c <= 'Z') c = (char)(c - 'A' + 'a');
        if (c != want[i]) return false;
    }
    return true;
}

[[noreturn]] void fence_die(int code, const char *what, const char *why)
{
    std::fprintf(stderr,
                 "[backup] COOP FENCE REFUSED (%s): %s -- nothing was read as a "
                 "save and nothing was written. Exiting with code %d.\n",
                 what, why, code);
    std::fflush(stderr);
    std::fflush(stdout);
#ifdef _WIN32
    ExitProcess((UINT)code);
    for (;;) {}
#else
    _exit(code);
#endif
}

struct ImageKind { int normal; int coop; bool all_erased; };

ImageKind classify(const unsigned char *img)
{
    ImageKind k = { 0, 0, true };
    for (int i = 0; i < kSize; ++i)
        if (img[i] != kErased) { k.all_erased = false; break; }
    for (int slot = 0; slot < 4; ++slot) {
        for (int copy = 0; copy < 2; ++copy) {
            const unsigned char *t = img + (copy ? kSize / 2 : 0) + (slot << 7) + 2;
            if (std::memcmp(t, kTagNormal, 8) == 0) ++k.normal;
            else if (std::memcmp(t, kTagCoop, 8) == 0) ++k.coop;
        }
    }
    return k;
}

// The kind of save this process is: refuse an image that is the other kind.
void fence_image(const unsigned char *img, const char *path, bool partial = false)
{
    ImageKind k = classify(img);
    if (!coop_mode()) {
        if (k.coop)
            fence_die(7, path, "this is a COOP save and this is the normal game");
    } else {
        if (k.normal)
            fence_die(8, path, "this is a NORMAL save and this is a coop room");
        if (!partial && !k.coop && !k.all_erased)
            fence_die(8, path, "this file is not a coop save");
    }
}

// Names: normal mode never opens a *.coopsav, coop mode only does.
void fence_path(const char *path)
{
    const bool cs = ends_with_coopsav(path);
    if (!coop_mode() && cs)
        fence_die(7, path, "a *.coopsav is a coop save and this is the normal game");
    if (coop_mode() && !cs)
        fence_die(8, path, "a coop room only opens a *.coopsav file");
}

#ifdef _WIN32
char g_guest_dir[1024];

bool pid_alive(unsigned long pid)
{
    HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, (DWORD)pid);
    if (!h) return GetLastError() == ERROR_ACCESS_DENIED;
    DWORD code = 0;
    BOOL ok = GetExitCodeProcess(h, &code);
    CloseHandle(h);
    return ok && code == STILL_ACTIVE;
}

void remove_guest_dir(const char *dir)
{
    char pat[1100];
    std::snprintf(pat, sizeof pat, "%s\\*", dir);
    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA(pat, &fd);
    if (h != INVALID_HANDLE_VALUE) {
        do {
            if (fd.cFileName[0] == '.') continue;
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
            char f[1200];
            std::snprintf(f, sizeof f, "%s\\%s", dir, fd.cFileName);
            DeleteFileA(f);
        } while (FindNextFileA(h, &fd));
        FindClose(h);
    }
    RemoveDirectoryA(dir);
}

void guest_tmp_root(char *out, size_t cap)
{
    const char *o = std::getenv("SM64DS_COOP_TEMP_ROOT");
    if (o && *o && std::strlen(o) + 1 < cap) { std::snprintf(out, cap, "%s", o); return; }
    DWORD n = GetTempPathA((DWORD)cap, out);
    if (n == 0 || n >= cap) std::snprintf(out, cap, ".");
    size_t l = std::strlen(out);
    while (l > 1 && (out[l - 1] == '\\' || out[l - 1] == '/')) out[--l] = '\0';
}

// Remove every sm64ds-coop-<pid> folder whose process is gone.
void sweep_stale_guest_dirs(const char *root)
{
    char pat[1100];
    std::snprintf(pat, sizeof pat, "%s\\sm64ds-coop-*", root);
    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA(pat, &fd);
    if (h == INVALID_HANDLE_VALUE) return;
    do {
        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) continue;
        const char *num = fd.cFileName + std::strlen("sm64ds-coop-");
        char *end = nullptr;
        unsigned long pid = std::strtoul(num, &end, 10);
        if (!*num || *end || pid == 0) continue;
        if (pid == (unsigned long)GetCurrentProcessId() || pid_alive(pid)) continue;
        char d[1200];
        std::snprintf(d, sizeof d, "%s\\%s", root, fd.cFileName);
        remove_guest_dir(d);
        std::fprintf(stderr, "[backup] swept a stale guest save folder (pid %lu)\n", pid);
    } while (FindNextFileA(h, &fd));
    FindClose(h);
}

void guest_cleanup_atexit()
{
    if (g_guest_dir[0]) remove_guest_dir(g_guest_dir);
    g_guest_dir[0] = '\0';
}

void resolve_guest_path()
{
    char root[900];
    guest_tmp_root(root, sizeof root);
    sweep_stale_guest_dirs(root);
    char dir[1000];
    std::snprintf(dir, sizeof dir, "%s\\sm64ds-coop-%lu", root,
                  (unsigned long)GetCurrentProcessId());
    CreateDirectoryA(dir, NULL);
    std::snprintf(g_guest_dir, sizeof g_guest_dir, "%s", dir);
    std::snprintf(g_path, sizeof g_path, "%s\\guest.coopsav", dir);
    std::atexit(guest_cleanup_atexit);
}
#else
void resolve_guest_path()
{
    std::snprintf(g_path, sizeof g_path, "/tmp/sm64ds-coop-%lu.coopsav",
                  (unsigned long)getpid());
}
#endif

bool trace_on()
{
    static int v = -1;
    if (v < 0) v = std::getenv("SM64DS_SAVE_TRACE") ? 1 : 0;
    return v != 0;
}

void make_dir(const char *dir)
{
#ifdef _WIN32
    CreateDirectoryA(dir, NULL);
#else
    mkdir(dir, 0777);
#endif
}

// ===========================================================================
// ADVENTURE ONLINE SAVE (run hunt8, lane ONLINESAVE1)
// An Adventure Online session (world or party room) never touches the
// single-player save. Its medium is <save folder>/online.sav: a 56 byte header
// (magic "SOLS", version, this install's random id, HMAC-SHA256 over id + the
// 8192 save bytes) followed by the image. A missing, wrong-size, mis-signed or
// foreign-install file is renamed online.sav.rejected-<time> and the run starts
// blank. SM64DS_ONLINE_SAVE=0 turns it off. Honest limit: the key is inside the
// exe, so this stops copying and casual edits, not someone who reads the exe.
// ===========================================================================
bool          g_online = false;
unsigned char g_oid[16];
const size_t  kOnHdr = 56;

struct Sha256 {
    uint32_t h[8]; unsigned char buf[64]; uint64_t len; size_t n;
    static uint32_t rr(uint32_t x, int c) { return (x >> c) | (x << (32 - c)); }
    void init() {
        static const uint32_t i0[8] = {0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,
                                       0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19};
        std::memcpy(h, i0, sizeof h); len = 0; n = 0;
    }
    void block(const unsigned char *p) {
        static const uint32_t k[64] = {
0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2};
        uint32_t w[64];
        for (int i = 0; i < 16; ++i)
            w[i] = ((uint32_t)p[i*4] << 24) | ((uint32_t)p[i*4+1] << 16) |
                   ((uint32_t)p[i*4+2] << 8) | p[i*4+3];
        for (int i = 16; i < 64; ++i) {
            uint32_t s0 = rr(w[i-15],7) ^ rr(w[i-15],18) ^ (w[i-15] >> 3);
            uint32_t s1 = rr(w[i-2],17) ^ rr(w[i-2],19) ^ (w[i-2] >> 10);
            w[i] = w[i-16] + s0 + w[i-7] + s1;
        }
        uint32_t a=h[0],b=h[1],c=h[2],d=h[3],e=h[4],f=h[5],g=h[6],hh=h[7];
        for (int i = 0; i < 64; ++i) {
            uint32_t t1 = hh + (rr(e,6)^rr(e,11)^rr(e,25)) + ((e&f)^(~e&g)) + k[i] + w[i];
            uint32_t t2 = (rr(a,2)^rr(a,13)^rr(a,22)) + ((a&b)^(a&c)^(b&c));
            hh=g; g=f; f=e; e=d+t1; d=c; c=b; b=a; a=t1+t2;
        }
        h[0]+=a;h[1]+=b;h[2]+=c;h[3]+=d;h[4]+=e;h[5]+=f;h[6]+=g;h[7]+=hh;
    }
    void update(const unsigned char *p, size_t m) {
        len += m;
        while (m) {
            size_t t = 64 - n; if (t > m) t = m;
            std::memcpy(buf + n, p, t); n += t; p += t; m -= t;
            if (n == 64) { block(buf); n = 0; }
        }
    }
    void finish(unsigned char out[32]) {
        uint64_t bits = len * 8;
        unsigned char one = 0x80, z = 0;
        update(&one, 1);
        while (n != 56) update(&z, 1);
        unsigned char l[8];
        for (int i = 0; i < 8; ++i) l[i] = (unsigned char)(bits >> (56 - 8 * i));
        update(l, 8);
        for (int i = 0; i < 8; ++i) {
            out[i*4] = (unsigned char)(h[i] >> 24); out[i*4+1] = (unsigned char)(h[i] >> 16);
            out[i*4+2] = (unsigned char)(h[i] >> 8); out[i*4+3] = (unsigned char)h[i];
        }
    }
};

void online_hmac(const unsigned char *img, unsigned char out[32])
{
    static const char key[] = "sm64ds-adventure-online-save/v1/2c9f7e41b8d3a605";
    unsigned char k[64], ip[64], op[64], inner[32];
    std::memset(k, 0, sizeof k);
    std::memcpy(k, key, sizeof key - 1);
    for (int i = 0; i < 64; ++i) { ip[i] = k[i] ^ 0x36; op[i] = k[i] ^ 0x5c; }
    Sha256 a; a.init(); a.update(ip, 64); a.update(g_oid, 16); a.update(img, kSize); a.finish(inner);
    Sha256 b; b.init(); b.update(op, 64); b.update(inner, 32); b.finish(out);
}

int           g_force_online = -1;   // NAME1: the in-game online menu's own choice (-1 = decide from the environment as at boot, 0 solo save, 1 online save)

bool online_wanted()
{
    if (coop_mode()) return false;
    if (g_force_online >= 0) return g_force_online == 1;
    const char *k = std::getenv("SM64DS_ONLINE_SAVE");
    if (k && *k == '0') return false;
    const char *vs = std::getenv("SM64DS_VS_MAP");
    if (vs && *vs) return false;
    /* SEAMCLEAN1: the relay and the test mesh's port come off the session record, not getenv */
    const char *r = port::online_session_get(port::kSessRelay);
    const char *m = port::online_session_get(port::kSessHubPort);
    if (!((r && *r) || (m && *m))) return false;
    return port::adventure_ghost_mode();
}

// raw = header + image (kOnHdr + kSize bytes): true if it is ours and intact.
bool online_verify(const unsigned char *raw)
{
    if (std::memcmp(raw, "SOLS", 4) != 0) return false;
    if (raw[4] != 1 || raw[5] || raw[6] || raw[7]) return false;
    if (std::memcmp(raw + 8, g_oid, 16) != 0) return false;
    unsigned char mac[32];
    online_hmac(raw + kOnHdr, mac);
    unsigned char d = 0;
    for (int i = 0; i < 32; ++i) d |= (unsigned char)(mac[i] ^ raw[24 + i]);
    return d == 0;
}

void online_load_id(const char *dir)
{
    char idp[1000];
    std::snprintf(idp, sizeof idp, "%s/online.id", dir);
    unsigned char hex[32];
    if (std::FILE *f = std::fopen(idp, "rb")) {
        size_t n = std::fread(hex, 1, 32, f);
        std::fclose(f);
        if (n == 32) {
            bool ok = true;
            for (int i = 0; i < 16 && ok; ++i) {
                int v = 0;
                for (int j = 0; j < 2; ++j) {
                    int c = hex[i*2+j];
                    int d = -1;
                    if (c >= '0' && c <= '9') d = c - '0';
                    else if (c >= 'a' && c <= 'f') d = c - 'a' + 10;
                    if (d < 0) { ok = false; break; }
                    v = v * 16 + d;
                }
                g_oid[i] = (unsigned char)v;
            }
            if (ok) return;
        }
    }
    std::random_device rd;
    for (int i = 0; i < 16; ++i) g_oid[i] = (unsigned char)(rd() ^ (rd() >> 8));
    if (std::FILE *f = std::fopen(idp, "wb")) {
        for (int i = 0; i < 16; ++i) std::fprintf(f, "%02x", g_oid[i]);
        std::fclose(f);
    }
}

void online_reject(const char *why)
{
    char dst[1100];
    std::snprintf(dst, sizeof dst, "%s.rejected-%lu", g_path, (unsigned long)std::time(NULL));
    std::fprintf(stderr, "[online-save] %s refused (%s); moved to %s, starting blank\n",
                 g_path, why, dst);
    std::remove(dst);
    if (std::rename(g_path, dst) != 0) std::remove(g_path);
}

// Fill g_path. Candidate order is documented in the header block above.
void resolve_path_sp()
{
    /* COOP (section 6): a guest runs on its own temp image; the host on the
       file the launcher named. The fence refuses a coop room with no file. */
    if (coop_mode()) {
        if (!coop_host()) { resolve_guest_path(); return; }
        const char *c = std::getenv("SM64DS_COOP_SAVE");
        if (!c || !*c) c = std::getenv("SM64DS_SAVE_PATH");
        if (!c || !*c || std::strlen(c) + 1 >= sizeof g_path)
            fence_die(8, "(none)", "a coop host has no SM64DS_COOP_SAVE file");
        std::snprintf(g_path, sizeof g_path, "%s", c);
        char parent[sizeof g_path];
        std::snprintf(parent, sizeof parent, "%s", g_path);
        char *bs = std::strrchr(parent, '\\');
        char *fs = std::strrchr(parent, '/');
        if (fs && (!bs || fs > bs)) bs = fs;
        if (bs && bs != parent) { *bs = '\0'; make_dir(parent); }
        return;
    }
    const char *env = std::getenv("SM64DS_SAVE_PATH");
    if (env && *env && std::strlen(env) + 1 < sizeof g_path) {
        std::snprintf(g_path, sizeof g_path, "%s", env);
        /* The other three candidates below create their save/
           directory; this one returned before make_dir, so a
           shortcut that pointed SM64DS_SAVE_PATH at a directory
           that did not exist yet got fopen "wb" failing, a
           flush_failure, SaveFile answering 0 -- and the game's
           own "Saved!" box regardless, because Message's
           countdown owns the box and not the write. */
        char parent[sizeof g_path];
        std::snprintf(parent, sizeof parent, "%s", g_path);
        char *bs = std::strrchr(parent, '\\');
        char *fs = std::strrchr(parent, '/');
        if (fs && (!bs || fs > bs)) bs = fs;
        if (bs && bs != parent) { *bs = '\0'; make_dir(parent); }
        return;
    }

    char dir[900];
    dir[0] = '\0';

#ifdef _WIN32
    {
        char exe[MAX_PATH];
        DWORD n = GetModuleFileNameA(NULL, exe, MAX_PATH);
        if (n > 0 && n < MAX_PATH) {
            char *bs = std::strrchr(exe, '\\');
            char *fs = std::strrchr(exe, '/');
            if (fs && (!bs || fs > bs)) bs = fs;
            if (bs) {
                *bs = '\0';
                if (std::strlen(exe) + 1 < sizeof dir)
                    std::snprintf(dir, sizeof dir, "%s", exe);
            }
        }
    }
#endif
    if (!dir[0]) {
        const char *root = std::getenv("SM64DS_ASSET_ROOT");
        if (root && *root && std::strlen(root) + 1 < sizeof dir)
            std::snprintf(dir, sizeof dir, "%s", root);
    }
    if (!dir[0])
        std::snprintf(dir, sizeof dir, ".");

    char sub[960];
    std::snprintf(sub, sizeof sub, "%s/save", dir);
    make_dir(sub);
    std::snprintf(g_path, sizeof g_path, "%s/sm64ds.sav", sub);
}

void resolve_path()
{
    resolve_path_sp();
    g_online = online_wanted();
    if (!g_online) return;
    char dir[1024];
    std::snprintf(dir, sizeof dir, "%s", g_path);
    char *bs = std::strrchr(dir, '\\');
    char *fs = std::strrchr(dir, '/');
    if (fs && (!bs || fs > bs)) bs = fs;
    if (bs) *bs = '\0'; else std::snprintf(dir, sizeof dir, ".");
    make_dir(dir);
    online_load_id(dir);
    std::snprintf(g_path, sizeof g_path, "%s/online.sav", dir);
    std::fprintf(stderr, "[online-save] adventure online: save medium is %s\n", g_path);
}

// Read the image, or manufacture an erased chip. A file that is not exactly
// kSize bytes is NOT a save this port wrote; it is left on disk untouched and
// the run gets an erased chip in memory, so a truncated or foreign file can
// never be read as half a save and is never silently deleted.
void open_once()
{
    coop_assert_tag();
    if (g_open) return;
    g_open = true;

    resolve_path();
    fence_path(g_path);
    std::memset(g_image, kErased, sizeof g_image);
    g_fresh = true;

    if (g_online) {
        std::FILE *of = std::fopen(g_path, "rb");
        if (!of) return;
        unsigned char raw[kOnHdr + kSize];
        size_t got = std::fread(raw, 1, sizeof raw, of);
        int extra = (std::fgetc(of) != EOF);
        std::fclose(of);
        if (got != sizeof raw || extra) { online_reject("wrong size"); return; }
        if (!online_verify(raw)) { online_reject("bad header or signature"); return; }
        std::memcpy(g_image, raw + kOnHdr, sizeof g_image);
        g_fresh = false;
        return;
    }
    std::FILE *f = std::fopen(g_path, "rb");
    if (!f) {
        if (trace_on())
            std::fprintf(stderr, "[backup] fresh chip, no file at %s\n", g_path);
        return;
    }
    unsigned char buf[kSize];
    size_t got = std::fread(buf, 1, sizeof buf, f);
    int extra = (std::fgetc(f) != EOF);
    std::fclose(f);
    if (coop_mode() && (got != (size_t)kSize || extra))
        fence_die(8, g_path, "the coop save is not a whole 8192-byte image");
    if (got != (size_t)kSize || extra) {
        std::fprintf(stderr,
                     "[backup] %s is %u bytes%s, not %u -- treating this run as a "
                     "fresh cartridge and leaving the file alone\n",
                     g_path, (unsigned)got, extra ? "+" : "", (unsigned)kSize);
        return;
    }
    fence_image(buf, g_path);
    std::memcpy(g_image, buf, sizeof g_image);
    g_fresh = false;
    if (trace_on())
        std::fprintf(stderr, "[backup] loaded %s\n", g_path);
}

// Persist the whole image through a per-process temp and a replacing move.
bool flush()
{
    // Once per process: a failing medium must not spam a play log.
    static bool said = false;

    // PROOF KNOB (coop save retry rows): SM64DS_COOPSAVE_FAIL_WRITES=N makes the
    // first N flushes of this process fail the way a locked temp file does.
    static int forced_left = -1;
    if (forced_left < 0) {
        const char *fw = std::getenv("SM64DS_COOPSAVE_FAIL_WRITES");
        forced_left = (fw && *fw) ? std::atoi(fw) : 0;
    }
    if (forced_left > 0) {
        --forced_left;
        ++g_stats.flush_failures;
        std::fprintf(stderr, "[backup] FORCED flush failure (%d left)\n", forced_left);
        return false;
    }

    char tmp[1088];
#ifdef _WIN32
    unsigned long pid = (unsigned long)GetCurrentProcessId();
#else
    unsigned long pid = (unsigned long)getpid();
#endif
    if (std::strlen(g_path) + 32 >= sizeof tmp) return false;
    std::snprintf(tmp, sizeof tmp, "%s.%lu.tmp", g_path, pid);

    std::FILE *f = std::fopen(tmp, "wb");
    if (!f) {
        ++g_stats.flush_failures;
        if (!said) {
            said = true;
            std::fprintf(stderr,
                         "[backup] COULD NOT WRITE %s -- the game's save did "
                         "not reach the disk\n", g_path);
        }
        return false;
    }
    bool ok;
    if (g_online) {
        unsigned char hdr[kOnHdr];
        std::memset(hdr, 0, sizeof hdr);
        std::memcpy(hdr, "SOLS", 4);
        hdr[4] = 1;
        std::memcpy(hdr + 8, g_oid, 16);
        online_hmac(g_image, hdr + 24);
        ok = std::fwrite(hdr, 1, kOnHdr, f) == kOnHdr &&
             std::fwrite(g_image, 1, sizeof g_image, f) == (size_t)kSize;
    } else {
        ok = std::fwrite(g_image, 1, sizeof g_image, f) == (size_t)kSize;
    }

    if (std::fflush(f) != 0) ok = false;
    if (std::fclose(f) != 0) ok = false;
    if (!ok) {
        std::remove(tmp);
        ++g_stats.flush_failures;
        if (!said) {
            said = true;
            std::fprintf(stderr,
                         "[backup] COULD NOT WRITE %s -- the game's save did "
                         "not reach the disk\n", g_path);
        }
        return false;
    }

#ifdef _WIN32
    if (!MoveFileExA(tmp, g_path, MOVEFILE_REPLACE_EXISTING)) {
        std::remove(tmp);
        ++g_stats.flush_failures;
        if (!said) {
            said = true;
            std::fprintf(stderr,
                         "[backup] COULD NOT WRITE %s -- the game's save did "
                         "not reach the disk\n", g_path);
        }
        return false;
    }
#else
    if (std::rename(tmp, g_path) != 0) {
        std::remove(tmp);
        ++g_stats.flush_failures;
        if (!said) {
            said = true;
            std::fprintf(stderr,
                         "[backup] COULD NOT WRITE %s -- the game's save did "
                         "not reach the disk\n", g_path);
        }
        return false;
    }
#endif
    ++g_stats.flushes;
    return true;
}

// THE FILE IS THE CHIP, NOT THIS PROCESS'S COPY OF IT (run hunt2, lane
// SAVELOSS1). g_image is loaded once, at the first card access -- for a
// title boot that is the file select's read of all three files -- and
// flush() writes all 8192 bytes of it. Two game processes on one save file
// (a second window, or a copy left running) therefore undid each other:
// whichever wrote LAST put back every slot as it stood when THAT process
// started. Measured on the 0.5.1 exe: A opens file A, B opens file B and
// saves a Lethal Lava Land star, then A saves file A -- and file B's star is
// gone, although A never touched file B. A 64 Kbit EEPROM programs exactly
// the bytes it is sent (command 8 carries an address and a length, section 3
// above) and every other cell keeps what is in it.
// So every command re-reads the file first: a program changes only its own
// span of what is on disk now, and a read sees what is on disk now, the way
// the chip would answer. A file that is not a whole image (absent, or not
// exactly kSize bytes: open_once's foreign-file rule) is left alone and the
// image in memory stands, as before. One named mutex per save path keeps a
// read-modify-write from interleaving with another process's.
void refresh_from_disk()
{
    std::FILE *f = std::fopen(g_path, "rb");
    if (!f) return;
    unsigned char buf[kSize];
    size_t got, extra;
    if (g_online) {
        unsigned char raw[kOnHdr + kSize];
        got = std::fread(raw, 1, sizeof raw, f);
        extra = (std::fgetc(f) != EOF);
        std::fclose(f);
        if (got != sizeof raw || extra || !online_verify(raw)) return;
        std::memcpy(buf, raw + kOnHdr, kSize);
        got = kSize;
        extra = 0;
    } else {
        got = std::fread(buf, 1, sizeof buf, f);
        extra = (std::fgetc(f) != EOF);
        std::fclose(f);
    }
    if (got != (size_t)kSize || extra) return;
    if (std::memcmp(g_image, buf, sizeof g_image) != 0) {
        fence_image(buf, g_path);   /* a file swapped under a running game */
        std::memcpy(g_image, buf, sizeof g_image);
        if (trace_on())
            std::fprintf(stderr, "[backup] the file changed on disk since this "
                                 "process last touched it; reading it again\n");
    }
    g_fresh = false;
}

#ifdef _WIN32
/* the name is the save path, folded to a hash: one lock per file, whatever
   spelling or case each process resolved it to */
HANDLE open_medium_mutex()
{
    unsigned hsh = 2166136261u;
    for (const char *p = g_path; *p; ++p) {
        char c = *p;
        if (c >= 'A' && c <= 'Z') c = (char)(c - 'A' + 'a');
        if (c == '\\') c = '/';
        hsh = (hsh ^ (unsigned char)c) * 16777619u;
    }
    char name[64];
    std::snprintf(name, sizeof name, "Local\\sm64ds-save-%08x", hsh);
    HANDLE h = CreateMutexA(NULL, FALSE, name);
    if (h) WaitForSingleObject(h, 5000);
    return h;
}

struct MediumLock {
    HANDLE h;
    MediumLock() : h(open_medium_mutex()) {}
    ~MediumLock() {
        if (h) { ReleaseMutex(h); CloseHandle(h); }
    }
};

// ONE TRANSFER, ONE HOLDER (run hunt3, lane POLISH2). The lock above covers one
// card command, and one save is sixteen of them (SaveDataToCart for the file,
// then for the minigame block: crc, tag and payload, primary then mirror). Every
// SaveCurrentFile also rewrites the minigame block in slot 3, which every
// window shares, so two windows saving in the same few milliseconds could land
// one's crc under the other's payload in BOTH copies; ReadMinigameData then
// finds no valid copy and writes the defaults, and every minigame record is
// gone. The ROM brackets each transfer with a lock id of its own
// (func_02057020 ... func_02057078) and takes the card only by that id
// inside it, so the medium is held over the same span: from the first
// read or program command of a transfer, by the id that owns the card,
// until that id is released (hal/os_lockid.cpp calls
// port_backup_transfer_end). The ids other callers take and never release
// never reach a card command, so they never start a hold. Windows mutexes are
// recursive per thread, so the per-command lock inside a transfer is free.
HANDLE g_xfer = NULL;
int    g_xfer_id = -1;

void transfer_hold(int owner)
{
    if (owner < 0 || g_xfer_id >= 0) return;   /* no id owns the card, or held */
    open_once();                                /* the name is the save path */
    g_xfer = open_medium_mutex();
    g_xfer_id = owner;
}

void transfer_end(int lock_id)
{
    if (g_xfer_id < 0 || g_xfer_id != lock_id) return;
    if (g_xfer) { ReleaseMutex(g_xfer); CloseHandle(g_xfer); }
    g_xfer = NULL;
    g_xfer_id = -1;
}
#else
struct MediumLock {};
void transfer_hold(int) {}
void transfer_end(int) {}
#endif

// A span the chip actually has. No clamping: a chip without those cells says
// no, and the ROM's own retry-then-fail path is what handles the answer.
bool span_ok(unsigned addr, unsigned len)
{
    if (len > (unsigned)kSize) return false;
    if (addr > (unsigned)kSize) return false;
    return len <= (unsigned)kSize - addr;
}

}  // namespace

const char *path()
{
    open_once();
    return g_path;
}

bool was_fresh()
{
    open_once();
    return g_fresh;
}

bool read(unsigned addr, void *dst, unsigned len)
{
    open_once();
    if (!dst || !span_ok(addr, len)) return false;
    MediumLock lock;
    refresh_from_disk();
    std::memcpy(dst, g_image + addr, len);
    ++g_stats.reads;
    if (trace_on())
        std::fprintf(stderr, "[backup] read  %04x +%u\n", addr, len);
    return true;
}

bool write(unsigned addr, const void *src, unsigned len)
{
    open_once();
    if (!src || !span_ok(addr, len)) return false;
    MediumLock lock;
    refresh_from_disk();
    coop_assert_tag();
    {
        /* the safety net: a write that would leave a record of the other
           kind in the image is refused before it reaches the disk */
        unsigned char cand[kSize];
        std::memcpy(cand, g_image, sizeof cand);
        std::memcpy(cand + addr, src, len);
        /* a record is written in pieces (checksum, tag, payload), so only the
           other kind's tag can be judged here, not "neither" */
        fence_image(cand, g_path, true);
    }
    if (port_backup_write_hook) port_backup_write_hook(addr, src, len, g_image);   /* CAREER1 */
    std::memcpy(g_image + addr, src, len);
    ++g_stats.writes;
    if (trace_on())
        std::fprintf(stderr, "[backup] write %04x +%u\n", addr, len);
    return flush();
}

bool verify(unsigned addr, const void *src, unsigned len)
{
    open_once();
    ++g_stats.verifies;
    if (!src || !span_ok(addr, len)) { ++g_stats.verify_failures; return false; }
    if (std::memcmp(g_image + addr, src, len) != 0) {
        ++g_stats.verify_failures;
        return false;
    }
    return true;
}

Stats stats() { return g_stats; }

// NAME1 (run after093): the title's online menu picks the save medium at run time. on=1: the signed online save (a missing or foreign file
// starts blank, as at boot); on=0: the single player save; on<0: back to what the environment says (the boot's own choice). The image is
// reloaded from the chosen medium, so the file select the ROM builds next reads that medium. The image in memory is always already on disk
// (every write flushes), so nothing is lost by the switch.
void select_online(int on)
{
    if (coop_mode()) return;
    const int want = on < 0 ? -1 : (on ? 1 : 0);
    if (g_open && g_force_online == want) return;
    g_force_online = want;
    g_open = false;
    open_once();
    std::fprintf(stderr, "[online-save] medium is now %s (%s)\n", g_online ? "the online save" : "the single player save", g_path);
}
bool online_selected() { return g_online; }

/* SEAMCOOP1: THE COOP MEDIUM AT RUN TIME (run after093; the one-process plan's M5b). A coop room member used to be a process started
   with SM64DS_COOP (and SM64DS_COOP_HOST + SM64DS_COOP_SAVE on the host), which this file read once: the host on its .coopsav, a guest
   on a temp image of its own. A coop room is now entered and left inside a running game (hal/coop_boot.cpp), so the same choice is
   made here at the title, where no level holds the image: 1 = a guest's temp image, 2 = the host's SM64DS_COOP_SAVE file, 0 = back to
   the single player save. The fence (section 6) applies exactly as at boot. Leaving coop puts the ROM's own record tag back
   ("ds mario", arm9 .rodata data_0208ee50, which func_02042f68 copies to data_020a4b40 at boot) and deletes the guest's temp image. */
void select_coop(int mode)
{
    const int want = mode < 0 ? -1 : (mode > 2 ? 2 : mode);
    if (g_open && g_force_coop == want) return;
    const bool was_guest = coop_mode() && !coop_host();
    g_force_coop = want;
#ifdef _WIN32
    if (was_guest && want != 1) guest_cleanup_atexit();
#else
    (void)was_guest;
#endif
    if (want == 0) {
        g_force_online = 0;
        if (std::memcmp(data_020a4b40, "ds coop!", 8) == 0) std::memcpy(data_020a4b40, "ds mario", 8);
    }
    g_open = false;
    open_once();
    std::fprintf(stderr, "[coop-save] medium is now %s (%s)\n",
                 !coop_mode() ? "the single player save" : coop_host() ? "the coop host's file" : "a coop guest's temp image", g_path);
}
bool coop_selected() { return coop_mode(); }

}  // namespace backup
}  // namespace ntr

// ============================================================================
// THE ROM SIDE: the card driver's storage, and the three faces
// ============================================================================

extern "C" {

DSSTATE_BEGIN

// ---- the 64 Kbit EEPROM row of the card driver's device table --------------
//
// arm9 .rodata 0x020867bc is a 4-row, 0x28-byte-per-row table and
// func_02060398 looks a row up by its type word. This is row 1, the row
// func_02042f68(0xd01, ...) selects, transcribed with its ROM offsets:
//
//     +0x00  0x00000d01   type: serial EEPROM, 64 Kbit
//     +0x04  0x00002000   total size, 8192 bytes  <- func_0206045c returns this
//     +0x08  0x00002000   largest single transfer the driver will issue
//     +0x0c  0x0020 u16   page size, 32 bytes         } one word, 0x00020020
//     +0x0e  0x0002 u16   device kind                 } little endian
//                         <- func_02060364 copies +0x0e to data_020a8764
//     +0x10  0x0000004f   the command opcode set for this kind
//
// The other three rows (0x901 = 4 Kbit EEPROM, 0x1202 = 2 Mbit FLASH, and the
// zero terminator) are not reachable: the game names 0xd01 and nothing else.
int data_port_backup_device[10] = {
    0x00000d01, 0x00002000, 0x00002000, 0x00020020, 0x0000004f,
    0, 0, 0, 0, 0
};

// data_020a8760 is a POINTER, not the table: src/func_02060364.c assigns the
// row to it and src/func_0206045c.c reads element [1] through it.
// src/func_020603c8.cpp -> func_02060364 is what does this on hardware, off the
// back of the card driver's own thread; the port has no card thread, so the
// same assignment is made statically here.
// The STORAGE for that pointer word is hal/globals_link100.cpp's grouped card
// span (data_020a8180 / data_020a81bc / data_020a8760 as one contiguous
// object, so the card thread's downward stack stays inside the ROM's own
// bytes); this file only fills it. Two hosts of one name collided at
// integration (LNK2005), and the contiguous one owns the bytes.
extern "C" unsigned char data_020a8760[4];
extern "C" unsigned char data_020a8180[60];

// The record tag every save record carries, and the tag ReadDataFromCart
// compares a record against before it will believe the payload. On hardware
// src/func_02042f68.c copies it out of arm9 .rodata 0x0208ee50 ("ds mario",
// 64 73 20 6d 61 72 69 6f) eight bytes at a time, stopping at the first zero;
// there are no zeros in the eight, so all eight land.
unsigned char data_020a4b40[8] = { 'd', 's', ' ', 'm', 'a', 'r', 'i', 'o' };

/* COOP (section 6, layer 2): a coop room frames every record with "ds coop!".
   Constant-initialised above, patched here before main. */
namespace {
struct PortCoopTag {
    PortCoopTag() {
        const char *e = std::getenv("SM64DS_COOP");
        if (e && *e && *e != '0')
            std::memcpy(data_020a4b40, "ds coop!", 8);
    }
};
PortCoopTag g_port_coop_tag;
}  // namespace

// The 32-byte command block the ARM9 and the ARM7 share. src/func_0206002c.c
// zeroes it, flushes it out of the data cache and hangs it off the work object.
int data_020a8160[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

// The card driver's work object, arm9 bss 0x020a8180. Field names are read off
// the matched TUs that touch each offset; the block at +0x3c is the driver's
// own OSThread (the ROM's separate symbol data_020a81bc) and the port creates
// no such thread, so it is reserved and never used.
struct PortCardWork {
    int            *cmd;        /* 0x00 the shared command block             */
    int             f04;        /* 0x04                                      */
    int             owner;      /* 0x08 lock id holding the card, -3 = free  */
    int             depth;      /* 0x0c lock recursion depth                 */
    unsigned short  waitq;      /* 0x10 sleep queue for the lock             */
    unsigned short  pad12;
    int             kind;       /* 0x14 what the lock was taken for          */
    int             src;        /* 0x18                                      */
    int             dst;        /* 0x1c                                      */
    int             len;        /* 0x20                                      */
    int             f24;        /* 0x24                                      */
    int             cbfn;       /* 0x28 async completion callback            */
    int             cbarg;      /* 0x2c                                      */
    int             f30;        /* 0x30 async worker entry                   */
    unsigned        state;      /* 0x34 bit0 inited, bit2 busy, bit4 async   */
    int             prio;       /* 0x38 the card thread's priority           */
    unsigned char   thread[0xd0 - 0x3c];  /* 0x3c data_020a81bc, unused here */
    int             fd0;        /* 0xd0                                      */
    unsigned short  fd4;        /* 0xd4 sleep queue for "card busy"          */
    unsigned short  padd6;
    unsigned char   padd8[0xe0 - 0xd8];
    unsigned char   staging[0x100];       /* 0xe0 the DMA staging buffer     */
};

// The state src/func_02060890.c and src/func_0206002c.c leave behind, which is
// the state every entry point below assumes. Two of these are load-bearing and
// a zeroed object deadlocks without them:
//   owner = ~2 (-3)  "nobody holds the card". src/func_02060188.c spins in
//                    `while (owner != -3) OS_SleepThread(&waitq)` until it sees
//                    that value, and a zeroed owner never becomes -3 because
//                    nothing on the host is going to release a lock nobody took.
//   state = 0        NOT SEEDED ANY MORE (run linkfull, lane S4CARD). Bit 0 is
//                    the driver's "initialised" bit and the ROM's own bring-up,
//                    src/func_02060890.c, is what sets it: it now runs in the
//                    ROM's order inside main (hal/boot_arms.cpp's R2c arm),
//                    before any card read or save, and its first line returns
//                    when the word is already set -- a seed of 1 made it skip
//                    the card thread and the page reader
//                    (`data_020a8780 = func_02060a64`), and the ROM's card reads
//                    need both. func_02060484 / 02060558 or in bit 2 for the
//                    duration of a transfer and func_0206081c ands out bits 2
//                    and 3 at the end. The other seeds stay: they are the
//                    values that body writes, and smoke_player, which has no
//                    bring-up, keeps them.
static const PortCardWork kCardWorkInit = {
    data_020a8160,  /* cmd    */
    0,              /* f04    */
    ~2,             /* owner  */
    0,              /* depth  */
    0, 0,           /* waitq, pad12 */
    0,              /* kind   */
    0, 0, 0,        /* src, dst, len */
    -1,             /* f24    */
    0, 0,           /* cbfn, cbarg */
    0,              /* f30    */
    0u,             /* state: set by src/func_02060890.c (see above) */
    4               /* prio   */
};

// Fill hal/globals_link100.cpp's storage once, before anything reads it: the
// work object's initial state and the device-row pointer that
// src/func_02060364.c would assign off the card thread on hardware.
namespace {
struct PortBackupFill {
    PortBackupFill() {
        /* The work object is bigger than the 60 bytes dsd named data_020a8180:
           on the ROM its own writes reach +0xd4, straight into the bytes dsd
           named data_020a81bc (hal/globals_link100.cpp says the same), and the
           two host arrays are laid out contiguously in that order for exactly
           this reason. The copy below spans both names the way the ROM does. */
        static_assert(sizeof(PortCardWork) <= 60 + 1444,
                      "PortCardWork exceeds the card span data_020a8180..data_020a8760");
        unsigned char *card = data_020a8180;
        std::memcpy(card, &kCardWorkInit, sizeof kCardWorkInit);
        int *row = data_port_backup_device;
        std::memcpy(data_020a8760, &row, sizeof row);
    }
};
PortBackupFill g_port_backup_fill;
}  // namespace


DSSTATE_END

// ---- OS_GetLockID / OS_ReleaseLockID (ROM: func_02057020 / func_02057078) ---
// Both live in hal/os_lockid.cpp: the ROM's search over the two lock words at
// 0x027fffb0 and its release, which sets the id's bit back. This file once
// carried a constant-id face for the first (return 0x40) and a do-nothing face
// for the second. The constant face went at integration; the do-nothing one
// stayed after the words became real, so every SaveDataToCart and
// ReadDataFromCart kept its id for good. Four go at the title and two per
// save, so from the 23rd save of one process on, SaveDataToCart took its -3
// arm and returned before writing anything, and nothing said so.

// ---- FACE: the ARM7's card-backup server (ROM: func_02060f60) --------------
// PORT_HOST_ABI: the ROM body pushes `cmd` down PXI channel 0xB with IPCSend
// and then sleeps on data_020a8180+0x34 bit 0x20 until the ARM7's IPC receive
// interrupt clears it. The port has no ARM7, no PXI and no such interrupt, so
// that sleep never ends. This IS the ARM7's answer, at the seam where the ARM7
// would give it: same command block, same result word, same return convention.
//
// Returns 1 when the command completed (block[0] == 0) and 0 otherwise, which
// is what the ROM returns and what src/func_0206062c.cpp and src/func_0206071c.c
// break their loops on. `retries` is how many times the ROM would re-send after
// a "busy" (4); a host medium is never busy, so it is not consulted.
int func_02060f60(void *self, int cmd, int retries)
{
    PortCardWork *w = (PortCardWork *)self;
    (void)retries;
    if (!w || !w->cmd) return 0;

    int *blk = w->cmd;
    blk[0] = 0;                       /* the ARM7's result word: 0 == done   */

    switch (cmd) {
    case 0:
        /* "here is the command block" -- the ARM9's very first request, sent
           by the ROM body itself before any other. Nothing to do: the block is
           already where this file can see it. */
        return 1;

    case 2:
        /* Identify the backup device. blk[1] carries the type word the ARM9
           read out of the device row (func_020607e8 -> func_02060470). The
           only device this file has is the one that row describes, so the
           answer is yes for that word and no for anything else. */
        if (blk[1] != 0 && blk[1] != data_port_backup_device[0]) {
            blk[0] = 1;
            return 0;
        }
        return 1;

    case 6: {
        /* Read: source is a medium offset, destination an ARM9 pointer. */
        ntr::backup::transfer_hold(w->owner);
        unsigned addr = (unsigned)blk[3];
        void *dst = (void *)(std::size_t)(unsigned)blk[4];
        unsigned len = (unsigned)blk[5];
        if (!ntr::backup::read(addr, dst, len)) { blk[0] = 1; return 0; }
        return 1;
    }

    case 8: {
        /* Program: source is an ARM9 pointer, destination a medium offset. */
        ntr::backup::transfer_hold(w->owner);
        const void *src = (const void *)(std::size_t)(unsigned)blk[3];
        unsigned addr = (unsigned)blk[4];
        unsigned len = (unsigned)blk[5];
        if (!ntr::backup::write(addr, src, len)) { blk[0] = 1; return 0; }
        return 1;
    }

    case 9: {
        /* Verify what command 8 just programmed. src/func_0206062c.cpp sends
           it with the SAME block still filled in, so the comparison is the
           same source against the same destination. */
        const void *src = (const void *)(std::size_t)(unsigned)blk[3];
        unsigned addr = (unsigned)blk[4];
        unsigned len = (unsigned)blk[5];
        if (!ntr::backup::verify(addr, src, len)) { blk[0] = 1; return 0; }
        return 1;
    }

    default:
        /* The erase / sector family and anything else this game never sends.
           Refused, not faked: a caller that gets here has found a path this
           file has not modelled and should see a failure, not a lie. */
        std::fprintf(stderr, "[backup] unmodelled card command %d\n", cmd);
        blk[0] = 1;
        return 0;
    }
}

/* The end of a transfer: hal/os_lockid.cpp's func_02057078 (OS_ReleaseLockID)
   hands every released id here, and the one that owned the card's transfer
   lets the medium go (ntr::backup::transfer_end, above). */
void port_backup_transfer_end(int lock_id)
{
    ntr::backup::transfer_end(lock_id);
}

}  // extern "C"

// ============================================================================
// THE ROMDATA PROPOSAL, for whoever picks this up next
// ============================================================================
//
// Six more matched TUs would link if arm9 .rodata 0x020867bc (0xa0 bytes, the
// device table) and 0x0208ee50 (8 bytes, the tag) were mounted through
// port/tools/romdata.py: func_02060398, func_02060364, func_020603c8,
// func_02060470, func_020607e8 and func_02042f68 -- i.e. the ROM would do its
// own device identify and its own tag copy instead of the two static
// definitions above, and func_02042f68 has a real ROM caller in
// src/func_0201a054.c's boot spine for whoever hosts that.
//
// IT WAS NOT TAKEN HERE FOR ONE REASON. port/CMakeLists.txt's ROM-CLEAN mode
// makes romdata.py emit ZEROED arrays for a shipped kit, which the kit then
// fills from the player's own ROM at boot. A zeroed device row is size 0, and
// size 0 makes func_0206045c return 0, and SaveDataToCart's very next line is
// `if (half <= off) return 1` -- a kit that cannot save at all, failing
// silently, on the exact build a player runs. Settling that (either an
// exemption for these two spans or a boot-order guarantee that the blob is
// loaded before the first save) is the work that has to come first.
