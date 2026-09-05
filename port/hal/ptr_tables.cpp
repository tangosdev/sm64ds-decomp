// Two ROM tables that romdata.py was emitting as raw bytes, hosted.
//
// ---- the bug class ---------------------------------------------------------
//
// port/tools/romdata.py hands the port the ROM's own bytes for every symbol in
// its NAMED list. That is right for constants and wrong, silently and for a
// long time, for any symbol whose bytes contain a RELOCATED word: a relocated
// word holds a DS address, and a DS address called through on the host is a
// jump into unmapped memory. data_02099fb4 / data_02099fbc were exactly this
// and killed a play session about ten thousand frames in (see
// hal/particle_vtable.cpp).
//
// port/tools/ptr_audit.py is the sweep for the rest. It intersects NAMED with
// config/arm9/relocs.txt and reports every named symbol carrying a relocation.
// Of the forty it flagged as "code pointers, named by no host TU", every one
// belongs to one of the two tables below -- and both are provably indexed and
// CALLED by matched code that is already in the link.
//
// The reason this hid so well is worth writing down: the callees are matched
// src that is in NO SLICE, so they were never compiled. "The symbol is not in
// the build" is evidence FOR this bug, not against it, because the only thing
// naming those functions was a table of bytes. port/slice_ptrsweep.txt is that
// list; nothing in it is new decomp.
//
// ---- table 1: data_02086758, the backup-media command table ----------------
//
// src/func_0205c5e4.c, the engine's per-command entry:
//
//     if (ret == 7)
//         ret = data_02086758[idx](self);
//
// -- nine plain C function pointers, indexed by the command id its callers
// pass (func_0205cf5c passes self+0x10, func_0205cfa4 passes self->f10, and
// the handlers themselves re-enter with 2, 3 and 7). Emitted as ROM bytes the
// port's copy held nine DS addresses.
//
// ---- table 2: the 39 records at 0x02086bc0.., the camera script commands ---
//
// src/func_02008550.cpp seeds data_0209b138[39] from them once and then
// dispatches:
//
//     (cam->*data_0209b138[msg[6]])(msg + 7, beg, end)
//
// and its one caller is src/ProcessKuppaScript.cpp, command 4, with
// data_0209f318 -- the Camera -- as the object. That is the camera half of the
// kuppa/cutscene script language: ProcessKuppaScript is reached from the level
// boot (Stage::LoadClsnAndObjects calls ContinueKuppaScriptIfNecessary) and
// from the star-get cutscene (func_ov002_020c7ff8 -> func_0200d4b0 ->
// func_0200ee8c -> RunKuppaScript).
//
// Each record is the ROM's 8-byte mwcc pointer-to-member pair, {code address,
// this-delta}, and every delta in the run is zero. They sit INTERLEAVED with
// the camera STATE machine's 38 pairs in the same address run -- those are the
// ones hal/camera_states.cpp bakes in, at 0x02086bc8, 0x02086bd0, 0x02086be0
// and so on. Same region, different table, different consumer.
//
// The dispatch itself cannot survive the ABI: MSVC's PMF for a complete
// single-inheritance class is a 4-byte __thiscall address, and the callees are
// C-linkage cdecl functions taking the camera as their first stack argument.
// So the consumer, src/func_02008550.cpp, goes through hostgen's MG_PMF_CALL
// patch (lane shadow-A retired the host copy that used to stand in for it):
// the 39 seeding assignments copy word 0 out of each record exactly as the
// ROM's copy does, and the one call reads it back as a plain function. Same
// hop hal/camera_states.cpp already makes for the state machine next door.
//
// ---- how to re-derive this file --------------------------------------------
//
// Every entry below quotes its own line out of config/arm9/relocs.txt. Nothing
// here is a guess about layout: `from` is the word the ROM relocates and `to`
// is where it points, and the symbol names come straight from
// config/arm9/symbols.txt. Both tables are removed from romdata.py's NAMED
// list in exchange -- putting either name back there would define the symbol
// twice and, if the byte copy won, restore the crash.
//
// port_ptr_tables_check() re-asserts the whole shape before the first frame.
#include <cstdio>
#include <cstdlib>

#include "dsstate_seg.h"

extern "C" {

/* THE ONE PRIMITIVE THE CALLEES REACH THAT HAS NO C BODY. src/CpuCopy8.c is
   an asm-primitive TU: a hand-tuned unaligned block copy that aligns the
   destination with an edge-preserving read-modify-write so no 8-bit store is
   ever issued. That trick exists for the DS's VRAM bus, which cannot take a
   byte write; on the host a plain byte copy is the same function. Its only
   caller in this slice is func_0205bc88, and it is not in
   port/slice_ptrsweep.txt for exactly this reason.
   PORT_HOST_ABI: ARM asm primitive (unaligned block copy), MSVC cannot assemble. */
void CpuCopy8(const void *src, void *dst, unsigned int size)
{
    const unsigned char *s = (const unsigned char *)src;
    unsigned char *d = (unsigned char *)dst;
    while (size--)
        *d++ = *s++;
}

// ---- table 1: data_02086758 ------------------------------------------------
//
// The handlers are declared here the way the table uses them -- one argument,
// an int result. func_0205c410 and func_0205c448 are decompiled `void` and let
// their result ride out of the tail call they end in, which is an ARM
// convention the host does not share; they are in the port's ride-through
// catalog and the two slots are typed through anyway, because the alternative
// is not calling them at all.
unsigned int func_0205bbdc(char *);
unsigned int func_0205bbe4(char *);
unsigned int func_0205bc0c(char *);
unsigned int func_0205bc88(char *);
unsigned int func_0205c048(char *);
unsigned int func_0205c264(char *);
unsigned int func_0205c378(char *);
unsigned int func_0205c410(char *);
unsigned int func_0205c448(char *);

/* from:0x02086758 to:0x0205c448 | 0x0208675c to:0x0205c410
   from:0x02086760 to:0x0205c378 | 0x02086764 to:0x0205c264
   from:0x02086768 to:0x0205c048 | 0x0208676c to:0x0205bc88
   from:0x02086770 to:0x0205bc0c | 0x02086774 to:0x0205bbe4
   from:0x02086778 to:0x0205bbdc */
DSSTATE_BEGIN
unsigned int (*data_02086758[9])(char *) = {
    func_0205c448,
    func_0205c410,
    func_0205c378,
    func_0205c264,
    func_0205c048,
    func_0205bc88,
    func_0205bc0c,
    func_0205bbe4,
    func_0205bbdc,
};

// ---- table 2: the 39 camera-script records ---------------------------------
//
// Declared through the widest shape the dispatch can present: the ROM enters
// them with the camera in r0 and (msg + 7, beg, end) in r1..r3, and the
// individual handlers spell as many of those as they read. Host-side that is
// one cdecl signature, and the extra arguments a one-parameter handler ignores
// are the caller's to clean up.
typedef int (*CamScriptFn)(void *cam, unsigned char *arg, int beg, int end);

int func_020071e0(void *, unsigned char *, int, int);
int func_020072c4(void *, unsigned char *, int, int);
int func_020073a0(void *, unsigned char *, int, int);
int func_02007414(void *, unsigned char *, int, int);
int func_02007450(void *, unsigned char *, int, int);
int func_02007484(void *, unsigned char *, int, int);
int func_020074e8(void *, unsigned char *, int, int);
int func_0200751c(void *, unsigned char *, int, int);
int func_02007548(void *, unsigned char *, int, int);
int func_020075b4(void *, unsigned char *, int, int);
int func_0200762c(void *, unsigned char *, int, int);
int func_02007698(void *, unsigned char *, int, int);
int func_02007758(void *, unsigned char *, int, int);
int func_020077f0(void *, unsigned char *, int, int);
int func_02007870(void *, unsigned char *, int, int);
int func_020078c4(void *, unsigned char *, int, int);
int func_02007910(void *, unsigned char *, int, int);
int func_020079ac(void *, unsigned char *, int, int);
int func_02007a8c(void *, unsigned char *, int, int);
int func_02007b0c(void *, unsigned char *, int, int);
int func_02007b98(void *, unsigned char *, int, int);
int func_02007d58(void *, unsigned char *, int, int);
int func_02007e5c(void *, unsigned char *, int, int);
int func_02007f14(void *, unsigned char *, int, int);
int func_02007fcc(void *, unsigned char *, int, int);
int func_02008008(void *, unsigned char *, int, int);
int func_02008028(void *, unsigned char *, int, int);
int func_02008080(void *, unsigned char *, int, int);
int func_020080b0(void *, unsigned char *, int, int);
int func_020080f0(void *, unsigned char *, int, int);
int func_0200814c(void *, unsigned char *, int, int);
int func_02008200(void *, unsigned char *, int, int);
int func_020082a0(void *, unsigned char *, int, int);
int func_020082f8(void *, unsigned char *, int, int);
int func_0200840c(void *, unsigned char *, int, int);
int func_02008450(void *, unsigned char *, int, int);
int func_02008474(void *, unsigned char *, int, int);
int func_020084b0(void *, unsigned char *, int, int);
int func_02008500(void *, unsigned char *, int, int);

/* Each record keeps the ROM's own two-word shape, {handler, this-delta}, so
   the host copy of func_02008550 reads word 0 the same way the matched source
   copies the pair. The index in the comment is the data_0209b138 slot, which
   is the script command byte. */
/* [ 0] from:0x02086c80 to:0x02008500 */
void *data_02086c80[2] = { (void *)&func_02008500, 0 };
/* [ 1] from:0x02086d80 to:0x020084b0 */
void *data_02086d80[2] = { (void *)&func_020084b0, 0 };
/* [ 2] from:0x02086c60 to:0x02008474 */
void *data_02086c60[2] = { (void *)&func_02008474, 0 };
/* [ 3] from:0x02086c90 to:0x02008450 */
void *data_02086c90[2] = { (void *)&func_02008450, 0 };
/* [ 4] from:0x02086bd8 to:0x0200840c */
void *data_02086bd8[2] = { (void *)&func_0200840c, 0 };
/* [ 5] from:0x02086d18 to:0x020082f8 */
void *data_02086d18[2] = { (void *)&func_020082f8, 0 };
/* [ 6] from:0x02086d98 to:0x020082a0 */
void *data_02086d98[2] = { (void *)&func_020082a0, 0 };
/* [ 7] from:0x02086c88 to:0x02008200 */
void *data_02086c88[2] = { (void *)&func_02008200, 0 };
/* [ 8] from:0x02086d00 to:0x0200814c */
void *data_02086d00[2] = { (void *)&func_0200814c, 0 };
/* [ 9] from:0x02086bc0 to:0x020080f0 */
void *data_02086bc0[2] = { (void *)&func_020080f0, 0 };
/* [10] from:0x02086e30 to:0x020080b0 */
void *data_02086e30[2] = { (void *)&func_020080b0, 0 };
/* [11] from:0x02086ce8 to:0x02008080 */
void *data_02086ce8[2] = { (void *)&func_02008080, 0 };
/* [12] from:0x02086e38 to:0x02008028 */
void *data_02086e38[2] = { (void *)&func_02008028, 0 };
/* [13] from:0x02086e20 to:0x02008008 */
void *data_02086e20[2] = { (void *)&func_02008008, 0 };
/* [14] from:0x02086cd0 to:0x02007fcc */
void *data_02086cd0[2] = { (void *)&func_02007fcc, 0 };
/* [15] from:0x02086e08 to:0x02007f14 */
void *data_02086e08[2] = { (void *)&func_02007f14, 0 };
/* [16] from:0x02086df0 to:0x02007e5c */
void *data_02086df0[2] = { (void *)&func_02007e5c, 0 };
/* [17] from:0x02086cb8 to:0x02007d58 */
void *data_02086cb8[2] = { (void *)&func_02007d58, 0 };
/* [18] from:0x02086dd8 to:0x02007b98 */
void *data_02086dd8[2] = { (void *)&func_02007b98, 0 };
/* [19] from:0x02086dc0 to:0x02007b0c */
void *data_02086dc0[2] = { (void *)&func_02007b0c, 0 };
/* [20] from:0x02086c20 to:0x02007a8c */
void *data_02086c20[2] = { (void *)&func_02007a8c, 0 };
/* [21] from:0x02086da0 to:0x020079ac */
void *data_02086da0[2] = { (void *)&func_020079ac, 0 };
/* [22] from:0x02086d60 to:0x02007910 */
void *data_02086d60[2] = { (void *)&func_02007910, 0 };
/* [23] from:0x02086d88 to:0x020078c4 */
void *data_02086d88[2] = { (void *)&func_020078c4, 0 };
/* [24] from:0x02086cf8 to:0x02007870 */
void *data_02086cf8[2] = { (void *)&func_02007870, 0 };
/* [25] from:0x02086d68 to:0x020077f0 */
void *data_02086d68[2] = { (void *)&func_020077f0, 0 };
/* [26] from:0x02086c78 to:0x02007758 */
void *data_02086c78[2] = { (void *)&func_02007758, 0 };
/* [27] from:0x02086d40 to:0x02007698 */
void *data_02086d40[2] = { (void *)&func_02007698, 0 };
/* [28] from:0x02086d38 to:0x0200762c */
void *data_02086d38[2] = { (void *)&func_0200762c, 0 };
/* [29] from:0x02086d20 to:0x020075b4 */
void *data_02086d20[2] = { (void *)&func_020075b4, 0 };
/* [30] from:0x02086c50 to:0x02007548 */
void *data_02086c50[2] = { (void *)&func_02007548, 0 };
/* [31] from:0x02086d08 to:0x0200751c */
void *data_02086d08[2] = { (void *)&func_0200751c, 0 };
/* [32] from:0x02086cf0 to:0x020074e8 */
void *data_02086cf0[2] = { (void *)&func_020074e8, 0 };
/* [33] from:0x02086c38 to:0x02007484 */
void *data_02086c38[2] = { (void *)&func_02007484, 0 };
/* [34] from:0x02086cd8 to:0x02007450 */
void *data_02086cd8[2] = { (void *)&func_02007450, 0 };
/* [35] from:0x02086cc0 to:0x02007414 */
void *data_02086cc0[2] = { (void *)&func_02007414, 0 };
/* [36] from:0x02086cb0 to:0x020073a0 */
void *data_02086cb0[2] = { (void *)&func_020073a0, 0 };
/* [37] from:0x02086ca8 to:0x020072c4 */
void *data_02086ca8[2] = { (void *)&func_020072c4, 0 };
/* [38] from:0x02086ca0 to:0x020071e0 */
void *data_02086ca0[2] = { (void *)&func_020071e0, 0 };

/* The roster the checker and the host copy of func_02008550 both walk, in
   data_0209b138 index order -- which is the order src/func_02008550.cpp
   assigns them, not address order. */
/* `extern` spelled on the definition on purpose: a const array at namespace
   scope has INTERNAL linkage in C++ even inside extern "C", and without this
   the host copy of func_02008550 cannot see it. */
extern void *const *const port_cam_script_records[39];
void *const *const port_cam_script_records[39] = {
    data_02086c80, data_02086d80, data_02086c60, data_02086c90,
    data_02086bd8, data_02086d18, data_02086d98, data_02086c88,
    data_02086d00, data_02086bc0, data_02086e30, data_02086ce8,
    data_02086e38, data_02086e20, data_02086cd0, data_02086e08,
    data_02086df0, data_02086cb8, data_02086dd8, data_02086dc0,
    data_02086c20, data_02086da0, data_02086d60, data_02086d88,
    data_02086cf8, data_02086d68, data_02086c78, data_02086d40,
    data_02086d38, data_02086d20, data_02086c50, data_02086d08,
    data_02086cf0, data_02086c38, data_02086cd8, data_02086cc0,
    data_02086cb0, data_02086ca8, data_02086ca0,
};

static const unsigned g_cam_script_addr[39] = {
    0x02086c80, 0x02086d80, 0x02086c60, 0x02086c90, 0x02086bd8, 0x02086d18,
    0x02086d98, 0x02086c88, 0x02086d00, 0x02086bc0, 0x02086e30, 0x02086ce8,
    0x02086e38, 0x02086e20, 0x02086cd0, 0x02086e08, 0x02086df0, 0x02086cb8,
    0x02086dd8, 0x02086dc0, 0x02086c20, 0x02086da0, 0x02086d60, 0x02086d88,
    0x02086cf8, 0x02086d68, 0x02086c78, 0x02086d40, 0x02086d38, 0x02086d20,
    0x02086c50, 0x02086d08, 0x02086cf0, 0x02086c38, 0x02086cd8, 0x02086cc0,
    0x02086cb0, 0x02086ca8, 0x02086ca0,
};

// ---- table 3: data_020876e4, the Kuppa/star-get script-pointer table -------
//
// src/func_0200ee8c.c, the star-get camera-script launcher:
//
//     RunKuppaScript(data_020876e4[arg0]);
//
// reached from func_ov002_020c7ff8 (star-collect Player state; the s==3 boss
// case passes index 5) -> func_0200d4b0 -> func_0200ee8c. Each slot is a
// RELOCATED word pointing at a Kuppa-script bytecode blob; byte-copied by
// romdata.py the port's data_020876e4[i] held six raw DS addresses, and
// RunKuppaScript seated data_0209fc48 with one, so ProcessKuppaScript faulted
// walking a garbage pointer (`while (*s != 0) { ...; s += *s; }`).
//
// Same bug class as tables 1 and 2. The difference is the targets are DATA, not
// code: the six words point at five unique script blobs (slots 2 and 4 share
// 0x0208776c). No relocation lands inside any blob, so each is byte-hosted
// verbatim from arm9_dec.bin. Each blob is a run of length-prefixed records
// terminated by a 0 byte -- ProcessKuppaScript's own walk -- so its size is
// exactly the byte offset of that terminator plus one, never a boundary guess:
//   0x02088388 -> 69, 0x020888f0 -> 92, 0x0208776c -> 44 (ends at the next
//   symbol data_02087798), 0x020889b0 -> 103, 0x02088b14 -> 139.
//
// data_020876e4 is removed from romdata.py's NAMED list in exchange; putting
// it back would define the symbol twice and, if the byte copy won, restore the
// crash. func_0200ee8c.c reaches it as `extern void *data_020876e4[]` (C
// linkage), which this definition satisfies the same way table 1's array does.
//
/* Byte-hosted script blobs, content from extracted/arm9_dec.bin (base
   0x02004000), sizes walked to each blob's own 0 terminator. */
static unsigned char kuppa_blob_02088388[69] = { 7,4,0,0,0,0,25,14,4,0,0,15,39,18,0,0,125,0,0,0,51,7,4,0,0,22,0,26,16,4,23,0,15,39,20,240,0,102,0,16,5,255,255,8,11,4,23,0,255,255,4,113,28,62,1,13,4,30,0,30,0,24,0,3,48,0,0,128,0 };
static unsigned char kuppa_blob_020888f0[92] = { 7,4,0,0,0,0,17,14,4,0,0,15,39,18,0,0,125,0,0,0,51,16,4,0,0,30,0,20,88,2,76,0,16,16,255,255,8,11,4,55,0,15,39,21,227,255,29,0,15,4,55,0,15,39,22,122,19,4,0,0,0,0,0,15,4,55,0,15,39,22,0,0,0,0,0,0,128,0,13,4,30,0,30,0,24,0,4,48,0,0,128,0 };
static unsigned char kuppa_blob_0208776c[44] = { 14,4,0,0,20,0,18,0,0,100,0,156,255,51,16,4,0,0,15,39,20,88,2,76,0,16,16,0,0,0,13,4,30,0,30,0,24,0,2,40,0,0,128,0 };
static unsigned char kuppa_blob_020889b0[103] = { 14,4,0,0,20,0,19,0,0,100,0,156,255,51,16,4,0,0,39,0,20,88,2,76,0,16,16,0,0,0,13,4,30,0,30,0,24,0,2,40,0,0,128,14,4,75,0,102,0,19,0,0,100,0,56,255,7,15,4,50,0,15,39,22,0,0,0,0,0,0,0,2,15,4,50,0,80,0,22,0,136,255,255,0,0,0,0,15,4,70,0,90,0,22,0,160,0,0,0,0,0,0,0 };
static unsigned char kuppa_blob_02088b14[139] = { 16,4,0,0,10,0,20,88,2,76,0,16,16,0,0,0,14,4,0,0,10,0,18,0,0,100,0,0,0,51,19,4,0,0,0,0,2,199,255,51,0,187,0,38,0,171,0,8,255,19,4,11,0,19,0,27,199,255,51,0,187,0,38,0,171,0,8,255,19,4,20,0,24,0,27,43,1,91,0,58,0,78,255,62,0,124,255,19,4,35,0,51,0,27,0,0,127,0,0,0,89,0,117,1,208,254,19,4,52,0,15,39,27,236,255,135,0,58,255,135,0,158,0,95,253,13,4,30,0,30,0,24,128,1,48,0,0,128,0 };

/* from:0x020876e4 to:0x02088388 | 0x020876e8 to:0x020888f0
   from:0x020876ec to:0x0208776c | 0x020876f0 to:0x020889b0
   from:0x020876f4 to:0x0208776c | 0x020876f8 to:0x02088b14
   Slots 2 and 4 share the 0x0208776c blob. */
void *data_020876e4[6] = {
    kuppa_blob_02088388,
    kuppa_blob_020888f0,
    kuppa_blob_0208776c,
    kuppa_blob_020889b0,
    kuppa_blob_0208776c,
    kuppa_blob_02088b14,
};
// ---- table 4: data_020756f0, the twelve shared-model SharedFilePtrs --------
//
// src/_ZN5Stage13InitResourcesEv.cpp, between LoadGraphics2D and LoadModel:
//
//     for (i = 0; i < 0xC; i++) Model::LoadFile(data_020756f0[i]);
//
// Twelve relocated words -- the coin, the mushroom, the shared pickup models --
// loaded once per level entry so the classes that read SharedFilePtr::filePtr
// straight out (OneUpMushroom::InitResources for types 11 and 12, Coin for its
// first two kinds) find a loaded file with no LoadFile of their own in front.
//
// SAME BUG CLASS AS TABLES 1 TO 3, WITH ONE DIFFERENCE WORTH SAYING: the
// twelve targets are not arm9 at all. Every relocation in the span points into
// OVERLAY 2 BSS, so the ROM bytes are twelve ov002 DS addresses and on the host
// each of those symbols is a separate array at an unrelated address. Emitted by
// romdata.py the loop would hand Model::LoadFile twelve DS addresses; that is
// why the name is not on its NAMED list and must not be put there.
//
// hal/level_boot.cpp's port_stage_preload_shared_models ALREADY SPELLS THESE
// TWELVE BY NAME, in this order, for exactly this reason -- it is the port's
// hand-rolled stand-in for the ROM's loop, written when the table itself could
// not be mounted. This row is the same twelve names as the table the ROM's own
// loop indexes, so seating Stage::InitResources retires that stand-in rather
// than doubling it. The order is the ROM's, read out of the arm9 relocation
// table and quoted per slot below; it is not this file's choice.
//
// ptr_audit STAYS AT ZERO through this change: the audit's subject is
// romdata.py's NAMED list, and this name is not on it before or after.
extern unsigned char data_ov002_0210da48[], data_ov002_0210d9b8[],
    data_ov002_0210da50[], data_ov002_0210d9f8[], data_ov002_0210da40[],
    data_ov002_0210d9a0[], data_ov002_0210d9c0[], data_ov002_0210e7d8[],
    data_ov002_0210e3a0[], data_ov002_0211094c[], data_ov002_0211095c[],
    data_ov002_0210d9a8[];

/* from:0x020756f0 to:0x0210da48 | 0x020756f4 to:0x0210d9b8
   from:0x020756f8 to:0x0210da50 | 0x020756fc to:0x0210d9f8
   from:0x02075700 to:0x0210da40 | 0x02075704 to:0x0210d9a0
   from:0x02075708 to:0x0210d9c0 | 0x0207570c to:0x0210e7d8
   from:0x02075710 to:0x0210e3a0 | 0x02075714 to:0x0211094c
   from:0x02075718 to:0x0211095c | 0x0207571c to:0x0210d9a8
   all kind:load module:overlay(2) */
void *data_020756f0[12] = {
    data_ov002_0210da48, data_ov002_0210d9b8, data_ov002_0210da50,
    data_ov002_0210d9f8, data_ov002_0210da40, data_ov002_0210d9a0,
    data_ov002_0210d9c0, data_ov002_0210e7d8, data_ov002_0210e3a0,
    data_ov002_0211094c, data_ov002_0211095c, data_ov002_0210d9a8,
};
DSSTATE_END

static const unsigned g_shared_model_addr[12] = {
    0x0210da48, 0x0210d9b8, 0x0210da50, 0x0210d9f8, 0x0210da40, 0x0210d9a0,
    0x0210d9c0, 0x0210e7d8, 0x0210e3a0, 0x0211094c, 0x0211095c, 0x0210d9a8,
};

static const unsigned g_kuppa_script_addr[6] = {
    0x02088388, 0x020888f0, 0x0208776c, 0x020889b0, 0x0208776c, 0x02088b14,
};

/* Called once before the first frame, the sibling of
   port_particle_vtables_check(). It catches the two ways this file can rot: a
   slot left null (something else seated the symbol and this file lost the
   tie-break) and a slot holding a word in the DS address range, which is what
   a raw ROM word looks like and is the whole bug class. Cheap, and it says so
   on frame 0 instead of ten thousand frames into a play session. */
static int port_ptr_slot_bad(const void *p)
{
    const unsigned v = (unsigned)(size_t)p;
    return !v || (v >= 0x01ff0000u && v < 0x02400000u);
}

void port_ptr_tables_check(void)
{
    for (int i = 0; i < 9; ++i)
        if (port_ptr_slot_bad((const void *)data_02086758[i])) {
            std::fprintf(stderr, "FATAL: card command table word %08x holds "
                         "%08x, not a host pointer\n", 0x02086758u + i * 4,
                         (unsigned)(size_t)data_02086758[i]);
            std::abort();
        }
    for (int i = 0; i < 39; ++i)
        if (port_ptr_slot_bad(port_cam_script_records[i][0])) {
            std::fprintf(stderr, "FATAL: camera script record %08x (command "
                         "%d) holds %08x, not a host pointer\n",
                         g_cam_script_addr[i], i,
                         (unsigned)(size_t)port_cam_script_records[i][0]);
            std::abort();
        }
    for (int i = 0; i < 6; ++i)
        if (port_ptr_slot_bad(data_020876e4[i])) {
            std::fprintf(stderr, "FATAL: kuppa script table slot %d (ROM "
                         "%08x) holds %08x, not a host pointer\n", i,
                         g_kuppa_script_addr[i],
                         (unsigned)(size_t)data_020876e4[i]);
            std::abort();
        }
    for (int i = 0; i < 12; ++i)
        if (port_ptr_slot_bad(data_020756f0[i])) {
            std::fprintf(stderr, "FATAL: shared-model table slot %d (ov002 "
                         "%08x) holds %08x, not a host pointer\n", i,
                         g_shared_model_addr[i],
                         (unsigned)(size_t)data_020756f0[i]);
            std::abort();
        }
}

/* ---- table 3: data_0209a03c, one word, and it is the same bug -------------
 *
 * config/arm9/relocs.txt has exactly three rows for this address:
 *
 *     from:0x0201a4b4 kind:load to:0x0209a03c    src/func_0201a490.c reads it
 *     from:0x020417c4 kind:load to:0x0209a03c    src/func_0204175c.c reads it
 *     from:0x0209a03c kind:load to:0x02057e30    THE WORD ITSELF
 *
 * The third row is the one that matters: the word AT 0x0209a03c is relocated,
 * so the four bytes romdata.py would hand over are the DS address 0x02057e30
 * and calling through them on the host is a jump into nothing. Both readers
 * CALL it -- src/func_0201a490.c is one line, `data_0209a03c(data_0208ee60)`
 * -- which is why hal/boot_os.cpp refused func_0201a490 outright rather than
 * hosting the word: "hosting the raw word would call into nothing".
 *
 * 0x02057e30 is src/func_02057e30.c, and the whole ROM body is `{}`. So the
 * binding is the ordinary one this file makes everywhere above: the host
 * pointer to the host function, not the DS address of the DS function. The
 * cast is because the ROM's call site spells the pointer `void (*)(void *)`
 * while the callee is decompiled `void (void)` -- the DS passes the argument
 * in r0 and the empty body never reads it, and cdecl lets the caller clean up,
 * so the one-argument shape is the faithful spelling of the call.
 *
 * It is .data, not .bss, so it goes inside the DSSTATE bracket with the tables
 * above and the save state rolls it back like the rest. */
extern void func_02057e30(void);
DSSTATE_BEGIN
void (*data_0209a03c)(void *) = (void (*)(void *))func_02057e30;
DSSTATE_END

}  /* extern "C" */

/* src/func_02008b4c.c, reached from commands 12 and 38 through func_02009414,
   calls the spline evaluator by its bare address name. The config knows it as
   func_ov002_020effb8 -- 0x020effb8 is inside the Player overlay -- and
   src/func_ov002_020effb8.c is the same matched function. A weak alias ties
   the two spellings together without a forwarding thunk; it only binds if
   nothing else defines the bare name. */
#pragma comment(linker, "/alternatename:_func_020effb8=_func_ov002_020effb8")
