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
//     1. $SM64DS_SAVE_PATH        exact path, if set (what the proof uses)
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
// writable and the ROM never sends an erase command to one; the driver's erase
// family (func_02060a64 and its neighbours) is the FLASH path and no call site
// in this game reaches it. If a later lane links that family, its commands land
// in the `default:` arm below and are refused honestly rather than silently
// succeeding.
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

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>

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

namespace ntr {
namespace backup {
namespace {

bool          g_open = false;      // the image has been resolved and loaded
bool          g_fresh = true;      // no file existed: this is a new cartridge
char          g_path[1024];
unsigned char g_image[kSize];
Stats         g_stats;

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

// Fill g_path. Candidate order is documented in the header block above.
void resolve_path()
{
    const char *env = std::getenv("SM64DS_SAVE_PATH");
    if (env && *env && std::strlen(env) + 1 < sizeof g_path) {
        std::snprintf(g_path, sizeof g_path, "%s", env);
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

// Read the image, or manufacture an erased chip. A file that is not exactly
// kSize bytes is NOT a save this port wrote; it is left on disk untouched and
// the run gets an erased chip in memory, so a truncated or foreign file can
// never be read as half a save and is never silently deleted.
void open_once()
{
    if (g_open) return;
    g_open = true;

    resolve_path();
    std::memset(g_image, kErased, sizeof g_image);
    g_fresh = true;

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
    if (got != (size_t)kSize || extra) {
        std::fprintf(stderr,
                     "[backup] %s is %u bytes%s, not %u -- treating this run as a "
                     "fresh cartridge and leaving the file alone\n",
                     g_path, (unsigned)got, extra ? "+" : "", (unsigned)kSize);
        return;
    }
    std::memcpy(g_image, buf, sizeof g_image);
    g_fresh = false;
    if (trace_on())
        std::fprintf(stderr, "[backup] loaded %s\n", g_path);
}

// Persist the whole image through a per-process temp and a replacing move.
bool flush()
{
    char tmp[1088];
#ifdef _WIN32
    unsigned long pid = (unsigned long)GetCurrentProcessId();
#else
    unsigned long pid = (unsigned long)getpid();
#endif
    if (std::strlen(g_path) + 32 >= sizeof tmp) return false;
    std::snprintf(tmp, sizeof tmp, "%s.%lu.tmp", g_path, pid);

    std::FILE *f = std::fopen(tmp, "wb");
    if (!f) { ++g_stats.flush_failures; return false; }
    bool ok = std::fwrite(g_image, 1, sizeof g_image, f) == (size_t)kSize;
    if (std::fflush(f) != 0) ok = false;
    if (std::fclose(f) != 0) ok = false;
    if (!ok) { std::remove(tmp); ++g_stats.flush_failures; return false; }

#ifdef _WIN32
    if (!MoveFileExA(tmp, g_path, MOVEFILE_REPLACE_EXISTING)) {
        std::remove(tmp);
        ++g_stats.flush_failures;
        return false;
    }
#else
    if (std::rename(tmp, g_path) != 0) {
        std::remove(tmp);
        ++g_stats.flush_failures;
        return false;
    }
#endif
    ++g_stats.flushes;
    return true;
}

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
// src/func_020603c8.c -> func_02060364 is what does this on hardware, off the
// back of the card driver's own thread; the port has no card thread, so the
// same assignment is made statically here.
int *data_020a8760 = data_port_backup_device;

// The record tag every save record carries, and the tag ReadDataFromCart
// compares a record against before it will believe the payload. On hardware
// src/func_02042f68.c copies it out of arm9 .rodata 0x0208ee50 ("ds mario",
// 64 73 20 6d 61 72 69 6f) eight bytes at a time, stopping at the first zero;
// there are no zeros in the eight, so all eight land.
unsigned char data_020a4b40[8] = { 'd', 's', ' ', 'm', 'a', 'r', 'i', 'o' };

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
//   state = 1        the driver's "initialised" bit. func_02060484 / 02060558
//                    or in bit 2 for the duration of a transfer and
//                    func_0206081c ands out bits 2 and 3 at the end; bit 0 is
//                    what says the object below them is real.
PortCardWork data_020a8180 = {
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
    1u,             /* state  */
    4               /* prio   */
};

DSSTATE_END

// ---- FACE: OS_GetLockID (ROM: func_02057020) -------------------------------
// PORT_HOST_ABI: hand-written ARM (`asm` block, MSVC cannot assemble) over the
// OS lock bitmask at 0x027FFFB0, a page ntr maps nothing at. hal/fs_names.cpp
// faces the same primitive for FS_Init and states the same reason.
//
// The ROM allocates the lowest free bit of two 32-bit words and numbers them
// 0x40..0x5f and 0x60..0x7f, returning -3 when both are full. The port has one
// caller that keeps an id (this one) and nothing that can exhaust the pool, so
// the answer is a stable id from the first word. It must NOT be -3: both
// SaveDataToCart and ReadDataFromCart return failure on -3 before touching the
// medium.
int func_02057020(void)
{
    return 0x40;
}

// ---- FACE: OS_ReleaseLockID (ROM: func_02057078) ---------------------------
// PORT_HOST_ABI: the `asm` sibling of the above, clearing the same bit in the
// same unmapped bitmask. Nothing to clear.
void func_02057078(int lock_id)
{
    (void)lock_id;
}

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
        unsigned addr = (unsigned)blk[3];
        void *dst = (void *)(std::size_t)(unsigned)blk[4];
        unsigned len = (unsigned)blk[5];
        if (!ntr::backup::read(addr, dst, len)) { blk[0] = 1; return 0; }
        return 1;
    }

    case 8: {
        /* Program: source is an ARM9 pointer, destination a medium offset. */
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
