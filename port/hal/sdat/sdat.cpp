// SDAT archive: load, parse, seat the root, decode SWAVs.
//
// The root object is the same shape func_02050f34 builds -- +0x7c FAT,
// +0x80 FILE, +0x84 INFO -- because six matched accessors in slice_gate10
// read exactly those three fields:
//
//   Sound::InfoSequenceEntry::GetWithID       root+0x84, rec +0x08
//   Sound::InfoInstrumentBankEntry::GetWithID root+0x84, rec +0x10
//   func_02050c14 (SEQARC)                    root+0x84, rec +0x0c
//   func_02050b4c (WAVEARC)                   root+0x84, rec +0x14
//   func_02050a84 (GROUP)                     root+0x84, rec +0x1c
//   func_020509b0 / func_02050a5c (FAT)       root+0x7c
//
// WHY NOT func_02050f34 ITSELF: it is not in any slice, and its own
// companion func_02050d54 allocates the three blocks off the game heap and
// pulls them in with FS_ReadFile through the card seam. That path is
// reproducible, but it buys nothing here -- the whole 4.4MB archive is
// resident either way, and going through it would put 4.4MB of sound data on
// the game's heap for no benefit. The root this file builds is
// field-for-field what func_02050f34 leaves behind; the difference is where
// the bytes live, and the accessors above cannot tell.
//
// RESIDENCY PRE-SEAT: func_020509b0(fileId) returns the runtime "loaded
// address" slot in the FAT record (FAT+0xc+id*16+8) and is what every
// residency check in func_02051bd0 / func_02051a98 / func_020514b4 tests.
// Filling all 282 of them with the resident address means the game correctly
// concludes nothing needs loading from the card -- which is true, the archive
// IS in memory. Load commands 0x1b/0x1c/0x1d therefore never fire, and the
// START command's data pointers land inside this image.
#include "sdat.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unordered_map>
#include <vector>

#ifndef PORT_REPO_ROOT
#define PORT_REPO_ROOT "."
#endif

SdatArchive g_sdat;

extern "C" void *data_020a5bb8;

// ---- little-endian readers ---------------------------------------------
static inline sd_u16 rd16(const sd_u8 *p) { return (sd_u16)(p[0] | (p[1] << 8)); }
static inline sd_u32 rd32(const sd_u8 *p)
{ return (sd_u32)p[0] | ((sd_u32)p[1] << 8) | ((sd_u32)p[2] << 16) | ((sd_u32)p[3] << 24); }

#ifdef PORT_ROM_CLEAN
/* A shipping build has no PORT_REPO_ROOT fallback. hal/asset_root_refuse.cpp
   is the write-up. */
extern "C" void port_asset_root_refuse(const char *wanted);
#endif

static const char *asset_root(void)
{
    const char *env = getenv("SM64DS_ASSET_ROOT");
#ifdef PORT_ROM_CLEAN
    if (!env)
        port_asset_root_refuse("extracted/dsd/files/data/sound_data.sdat");
#endif
    return env ? env : PORT_REPO_ROOT;
}

// ---- FAT ----------------------------------------------------------------

static sd_u8 *fat_entry(sd_u32 id)
{
    if (!g_sdat.fat || id >= g_sdat.fatCount) return 0;
    return g_sdat.fat + 0xc + id * 16;
}

sd_u8 *sdat_file(sd_u32 id)
{
    sd_u8 *e = fat_entry(id);
    if (!e) return 0;
    sd_u32 off = rd32(e);
    if (off >= g_sdat.size) return 0;
    return g_sdat.base + off;
}

sd_u32 sdat_file_size(sd_u32 id)
{
    sd_u8 *e = fat_entry(id);
    return e ? rd32(e + 4) : 0;
}

sd_u32 sdat_file_id_of(const void *p)
{
    if (!g_sdat.base || !p) return (sd_u32)-1;
    const sd_u8 *q = (const sd_u8 *)p;
    if (q < g_sdat.base || q >= g_sdat.base + g_sdat.size) return (sd_u32)-1;
    sd_u32 off = (sd_u32)(q - g_sdat.base);
    for (sd_u32 i = 0; i < g_sdat.fatCount; i++) {
        sd_u8 *e = g_sdat.fat + 0xc + i * 16;
        sd_u32 o = rd32(e), s = rd32(e + 4);
        if (off >= o && off < o + s) return i;
    }
    return (sd_u32)-1;
}

// The sequence's base CHANNEL PRIORITY (the SDAT's cpr byte), which is the
// priority every track of the player starts at on the ARM7. The SSEQ 0xC6
// opcode overrides it per track; a stream with no 0xC6 keeps it.
//
// This existed nowhere in the port before, and its absence inverted the ROM's
// voice-steal order in any busy scene. The opening cutscene is the worst case:
// its SFX carry cpr 96..127 (NCS_SE_VT_PEACH_LETTER and NCS_SE_VO_SLEEP1_Y are
// 127) and set no 0xC6, while NCS_BGM_OPENING carries cpr 64 and its lead
// track sets 0xC6 66. On hardware the SFX therefore dominate the music and the
// mix ducks cleanly. The port defaulted every track to 64, so the 127-priority
// voices ran at 64 -- level with the BGM -- and the 16-channel mixer stole
// between them at random, dropping the letter narration and the music in turn.
// That is the opening's "sounds overlap and cut out".
//
// seqBase is the file's DATA base (what the START command carries in b); the
// SSAR case reads the entry table beside it, the SSEQ case the INFO record.
//
// MEASURED, on this tree, with the opening harness (SM64DS_SCENE=1 +
// SM64DS_TITLE_ENTRY=1, touch past the title at f720 and the file-select at
// f800, 3000 frames, SM64DS_NO_AUDIO=1 so the mixer is clocked at exactly
// 1/60 s per frame and the run is deterministic). Counts are from
// SM64DS_VOICE_TRACE=1: "chan N start" is a note that got a channel,
// "chan ALLOC FAILED" is a note that got nothing and is simply not heard.
// The cutscene column is the 3000-frame run minus an 850-frame run that
// stops before the cutscene begins, so it is the opening's own share:
//
//                          cutscene: notes sounded / notes dropped
//   cpr discarded (before)              652 / 384
//   cpr seated (this commit)            820 / 216
//   cpr inverted (127-cpr, control)     605 / 431
//
// The inverted row is the detector control: it breaks the same byte in the
// other direction and the dropped-note count moves the other way, so the
// 216 is a real reading and not a counter that always says the same thing.
// Before, EVERY voice in the opening ran at 64..69 -- the trace histogram has
// no value above 69 in it -- because 64 was the hardcoded default and the
// cpr byte never reached the player. After, the SDAT's own 96 and 127 appear.
int sdat_seq_priority(const sd_u8 *seqBase, sd_u32 startOff)
{
    if (!seqBase || !g_sdat.base || !g_sdat.info) return 64;
    sd_u32 fid = sdat_file_id_of(seqBase);
    if (fid == (sd_u32)-1) return 64;
    sd_u8 *f = sdat_file(fid);
    if (!f) return 64;

    if (!memcmp(f, "SSAR", 4)) {
        // entry table: count at +0x1c, entries at +0x20, 12 bytes each:
        // { seqOff u32, bank u16, vol u8, cpr u8, ppr u8, ply u8 }. seqOff is
        // relative to the DATA base, which is exactly startOff's frame.
        sd_u32 count = rd32(f + 0x1c);
        for (sd_u32 i = 0; i < count; i++) {
            sd_u8 *e = f + 0x20 + i * 12;
            if (rd32(e) == startOff) return e[7];
        }
        return 64;
    }
    if (!memcmp(f, "SSEQ", 4)) {
        // A standalone sequence carries its cpr in the INFO SEQ record whose
        // fileId names this file: { fileId u16, unk u16, bank u16, vol u8,
        // cpr u8, ... }.
        sd_u32 recOff = rd32(g_sdat.info + 8 + SDAT_REC_SEQ * 4);
        if (!recOff) return 64;
        sd_u8 *tbl = g_sdat.info + recOff;
        sd_u32 n = rd32(tbl);
        for (sd_u32 i = 0; i < n; i++) {
            sd_u32 v = rd32(tbl + 4 + i * 4);
            if (!v) continue;
            sd_u8 *e = g_sdat.info + v;
            if (rd16(e) == fid) return e[7];
        }
    }
    return 64;
}

// ---- INFO ---------------------------------------------------------------

sd_u8 *sdat_info_entry(int rec, sd_u32 id)
{
    if (!g_sdat.info || rec < 0 || rec >= SDAT_REC_COUNT) return 0;
    sd_u32 recOff = rd32(g_sdat.info + 8 + rec * 4);
    if (!recOff) return 0;
    sd_u8 *tbl = g_sdat.info + recOff;
    if (id >= rd32(tbl)) return 0;
    sd_u32 v = rd32(tbl + 4 + id * 4);
    return v ? g_sdat.info + v : 0;
}

// ---- SWAV decode --------------------------------------------------------

static const int kAdpcmIndexTab[8] = { -1, -1, -1, -1, 2, 4, 6, 8 };
static const sd_s16 kAdpcmStepTab[89] = {
    7,8,9,10,11,12,13,14,16,17,19,21,23,25,28,31,34,37,41,45,50,55,60,66,73,
    80,88,97,107,118,130,143,157,173,190,209,230,253,279,307,337,371,408,449,
    494,544,598,658,724,796,876,963,1060,1166,1282,1411,1552,1707,1878,2066,
    2272,2499,2749,3024,3327,3660,4026,4428,4871,5358,5894,6484,7132,7845,
    8630,9493,10442,11487,12635,13899,15289,16818,18500,20350,22385,24623,
    27086,29794,32767
};

struct WaveCache {
    std::vector<sd_s16> pcm;
    SdatWave w;
};
static std::unordered_map<const void *, WaveCache> g_waves;

/* Drop every decoded wave. The cache is keyed by SWAV record ADDRESSES in the
   game arena, and an address only names the same sample while the arena around
   it stands still. A save-state restore that crossed an area rolls the arena
   back to different sample data at the same addresses, so the keys would hand
   out the other area's audio. lk6_savestate_load calls this beside the other
   sd_*_reset calls; the cache refills on demand at the next note-on. */
void sd_waves_reset(void)
{
    g_waves.clear();
}

/* Put THIS process's SDAT root back into data_020a5bb8. The global is a hosted
   DS symbol, so a save state captures and restores it like any other -- but
   what it holds is a HOST BOOT POINTER (sdat_init's calloc'd root object), not
   game state: it is written once at boot and never changes during play. An
   in-process restore rewrites it with the same value, harmless. A DISK state
   from an earlier run rewrites it with the OTHER process's heap address, and
   the first sound lookup (func_02050c14 walking root+0x84) dereferences a
   pointer into a heap that no longer exists. lk6_savestate_load calls this
   after the section copy so the live root always wins. */
void sd_sdat_reseat(void)
{
    if (g_sdat.root)
        data_020a5bb8 = g_sdat.root;   /* the extern "C" decl at the top */
}

// Decode one SWAV record (12-byte header + payload) into mono s16.
static const WaveCache *decode_swav(const sd_u8 *rec)
{
    auto it = g_waves.find(rec);
    if (it != g_waves.end()) return &it->second;

    int fmt = rec[0];
    int loop = rec[1];
    sd_u32 rate = rd16(rec + 2);
    sd_u32 loopWords = rd16(rec + 6);
    sd_u32 nonLoopWords = rd32(rec + 8);
    const sd_u8 *d = rec + 12;
    sd_u32 bytes = (loopWords + nonLoopWords) * 4;

    WaveCache wc;
    wc.w.sampleRate = rate ? rate : 32768;
    wc.w.loop = loop;
    wc.w.loopStart = 0;

    if (fmt == 0) {                       // PCM8, signed
        wc.pcm.resize(bytes);
        for (sd_u32 i = 0; i < bytes; i++)
            wc.pcm[i] = (sd_s16)((sd_s8)d[i] << 8);
        wc.w.loopStart = loopWords * 4;
    } else if (fmt == 1) {                // PCM16
        sd_u32 n = bytes / 2;
        wc.pcm.resize(n);
        for (sd_u32 i = 0; i < n; i++)
            wc.pcm[i] = (sd_s16)rd16(d + i * 2);
        wc.w.loopStart = loopWords * 2;
    } else if (fmt == 2) {                // IMA-ADPCM
        if (bytes < 4) return 0;
        sd_s32 pred = (sd_s16)rd16(d);
        int index = d[2] & 0x7f;
        if (index > 88) index = 88;
        sd_u32 nib = (bytes - 4) * 2;
        wc.pcm.reserve(nib);
        for (sd_u32 i = 0; i < nib; i++) {
            int code = (d[4 + (i >> 1)] >> ((i & 1) * 4)) & 0xf;
            sd_s32 step = kAdpcmStepTab[index];
            sd_s32 diff = step >> 3;
            if (code & 1) diff += step >> 2;
            if (code & 2) diff += step >> 1;
            if (code & 4) diff += step;
            pred += (code & 8) ? -diff : diff;
            if (pred < -0x8000) pred = -0x8000;
            if (pred > 0x7fff) pred = 0x7fff;
            index += kAdpcmIndexTab[code & 7];
            if (index < 0) index = 0;
            if (index > 88) index = 88;
            wc.pcm.push_back((sd_s16)pred);
        }
        // The 4-byte ADPCM header sits inside the "loop" word count, so the
        // loop point is (loopWords*4 - 4) bytes of nibbles in.
        wc.w.loopStart = loopWords >= 1 ? (loopWords * 4 - 4) * 2 : 0;
    } else {
        static int warned;
        if (!warned++) fprintf(stderr, "[sdat] unknown SWAV format %d\n", fmt);
        return 0;
    }

    wc.w.totalSamples = (sd_u32)wc.pcm.size();
    if (wc.w.loopStart >= wc.w.totalSamples) wc.w.loopStart = 0;
    auto res = g_waves.emplace(rec, std::move(wc));
    res.first->second.w.pcm = res.first->second.pcm.data();
    return &res.first->second;
}

int sdat_swar_wave(const sd_u8 *swar, sd_u32 index, SdatWave *out)
{
    if (!swar || memcmp(swar, "SWAR", 4) != 0) return 0;
    sd_u32 count = rd32(swar + 0x38);
    if (index >= count) return 0;
    sd_u32 off = rd32(swar + 0x3c + index * 4);
    if (!off) return 0;
    const WaveCache *wc = decode_swav(swar + off);
    if (!wc || wc->pcm.empty()) return 0;
    *out = wc->w;
    out->pcm = wc->pcm.data();
    return 1;
}

// ---- SBNK ---------------------------------------------------------------

static void read_note_rec(const sd_u8 *p, SdatNote *n)
{
    n->swav     = rd16(p);
    n->swar     = rd16(p + 2);
    n->baseNote = p[4];
    n->attack   = p[5];
    n->decay    = p[6];
    n->sustain  = p[7];
    n->release  = p[8];
    n->pan      = p[9];
}

int sdat_bank_note(const sd_u8 *sbnk, int prog, int note,
                   SdatNote *out, const sd_u8 **swarOut)
{
    if (!sbnk || memcmp(sbnk, "SBNK", 4) != 0) return 0;
    sd_u32 nInst = rd32(sbnk + 0x38);
    if (prog < 0 || (sd_u32)prog >= nInst) return 0;

    const sd_u8 *ir = sbnk + 0x3c + prog * 4;
    int type = ir[0];
    sd_u32 off = rd16(ir + 1);
    if (!type || !off) return 0;
    const sd_u8 *rec = sbnk + off;

    if (type >= 1 && type <= 5) {
        read_note_rec(rec, out);
    } else if (type == 0x10) {              // drum set
        int lo = rec[0], hi = rec[1];
        if (note < lo || note > hi) return 0;
        read_note_rec(rec + 2 + (note - lo) * 12 + 2, out);
    } else if (type == 0x11) {              // key split
        int region = -1;
        for (int i = 0; i < 8; i++) {
            if (rec[i] == 0) break;
            if (note <= rec[i]) { region = i; break; }
        }
        if (region < 0) return 0;
        read_note_rec(rec + 8 + region * 12 + 2, out);
    } else {
        return 0;
    }

    // The linked SWAR comes out of the SBNK's own reserved wave-link slots,
    // which func_0205b78c patched when the game started the sequence -- the
    // same 32 bytes the hardware ARM7 reads.
    if (out->swar > 3) return 0;
    const sd_u8 *swar = *(const sd_u8 *const *)(sbnk + 0x18 + out->swar * 8);
    if (!swar) return 0;
    *swarOut = swar;
    return 1;
}

int sdat_link_bank_waves(sd_u8 *sbnk)
{
    if (!sbnk || !g_sdat.info || memcmp(sbnk, "SBNK", 4) != 0) return 0;
    sd_u32 fid = sdat_file_id_of(sbnk);
    if (fid == (sd_u32)-1) return 0;

    // Find the BANK info entry that names this file.
    sd_u32 recOff = rd32(g_sdat.info + 8 + SDAT_REC_BANK * 4);
    if (!recOff) return 0;
    sd_u8 *tbl = g_sdat.info + recOff;
    sd_u32 n = rd32(tbl);
    sd_u8 *entry = 0;
    for (sd_u32 i = 0; i < n; i++) {
        sd_u32 v = rd32(tbl + 4 + i * 4);
        if (!v) continue;
        sd_u8 *e = g_sdat.info + v;
        if (rd32(e) == fid) { entry = e; break; }
    }
    if (!entry) return 0;

    int filled = 0;
    for (int k = 0; k < 4; k++) {
        sd_u16 id = rd16(entry + 4 + k * 2);
        if (id == 0xffff) continue;
        sd_u8 **slot = (sd_u8 **)(sbnk + 0x18 + k * 8);
        if (*slot) continue;                       // the game already linked it
        sd_u8 *we = sdat_info_entry(SDAT_REC_WAVEARC, id);
        if (!we) continue;
        sd_u8 *swar = sdat_file(rd32(we));
        if (!swar || memcmp(swar, "SWAR", 4) != 0) continue;
        *slot = swar;
        filled++;
    }
    return filled;
}

// ---- volume curve -------------------------------------------------------

// 0..127 -> tenths of a dB in [-723, 0]. The DS carries a 128-entry table in
// its ARM7 binary, which is not ours; this is the log curve that table
// approximates (20*log10(v/127), in 0.1dB units, floored at the DS's own
// -72.3dB bottom). Audibly equivalent, and honestly not the ROM's bytes.
int sd_cnv_vol(int v)
{
    static sd_s16 lut[128];
    static int built;
    if (!built) {
        built = 1;
        lut[0] = -723;
        for (int i = 1; i < 128; i++) {
            double db10 = 200.0 * log10((double)i / 127.0);
            if (db10 < -723.0) db10 = -723.0;
            lut[i] = (sd_s16)(db10 < 0 ? db10 - 0.5 : db10 + 0.5);
        }
    }
    if (v < 0) v = 0;
    if (v > 127) v = 127;
    return lut[v];
}

// ---- init ---------------------------------------------------------------

int sdat_init(void)
{
    if (g_sdat.base) return 1;

    char path[512];
    const char *env = getenv("SM64DS_SDAT");
    if (env) snprintf(path, sizeof path, "%s", env);
    else snprintf(path, sizeof path,
                  "%s/extracted/dsd/files/data/sound_data.sdat", asset_root());

    FILE *f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "[sdat] cannot open %s -- sound stays silent\n", path);
        return 0;
    }
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (len < 0x40) { fclose(f); return 0; }
    sd_u8 *buf = (sd_u8 *)malloc((size_t)len);
    if (!buf) { fclose(f); return 0; }
    if (fread(buf, 1, (size_t)len, f) != (size_t)len) {
        fclose(f); free(buf);
        fprintf(stderr, "[sdat] short read on %s\n", path);
        return 0;
    }
    fclose(f);

    if (memcmp(buf, "SDAT", 4) != 0) {
        fprintf(stderr, "[sdat] %s is not an SDAT\n", path);
        free(buf);
        return 0;
    }

    g_sdat.base = buf;
    g_sdat.size = (size_t)len;

    int nblocks = rd16(buf + 0x0e);
    sd_u8 *symb = 0, *file = 0;
    for (int i = 0; i < nblocks && i < 8; i++) {
        sd_u32 off = rd32(buf + 0x10 + i * 8);
        if (!off || off >= (sd_u32)len) continue;
        sd_u8 *b = buf + off;
        if (!memcmp(b, "SYMB", 4)) symb = b;
        else if (!memcmp(b, "INFO", 4)) g_sdat.info = b;
        else if (!memcmp(b, "FAT ", 4)) g_sdat.fat = b;
        else if (!memcmp(b, "FILE", 4)) file = b;
    }
    (void)symb;
    if (!g_sdat.info || !g_sdat.fat) {
        fprintf(stderr, "[sdat] missing INFO or FAT block\n");
        free(buf);
        g_sdat.base = 0;
        return 0;
    }
    g_sdat.fatCount = rd32(g_sdat.fat + 8);

    // The root object. 0x100 bytes covers everything func_02050f34 touches
    // (+0x30 FSFile, +0x74 handle pair, +0x7c/0x80/0x84 the three blocks);
    // the first 0x30 is the SDAT header itself, as on hardware.
    sd_u8 *root = (sd_u8 *)calloc(1, 0x100);
    if (!root) { free(buf); g_sdat.base = 0; return 0; }
    memcpy(root, buf, 0x30);
    *(sd_u8 **)(root + 0x7c) = g_sdat.fat;
    *(sd_u8 **)(root + 0x80) = file;
    *(sd_u8 **)(root + 0x84) = g_sdat.info;
    g_sdat.root = root;
    data_020a5bb8 = root;

    // Pre-seat residency for every file (see the header comment).
    for (sd_u32 i = 0; i < g_sdat.fatCount; i++) {
        sd_u8 *e = g_sdat.fat + 0xc + i * 16;
        sd_u32 off = rd32(e);
        *(sd_u8 **)(e + 8) = (off < (sd_u32)len) ? (buf + off) : 0;
    }

    fprintf(stderr,
            "[sdat] %s: %ld bytes, %u files, seq=%u seqarc=%u bank=%u "
            "wavearc=%u group=%u; root seated at data_020a5bb8\n",
            path, len, g_sdat.fatCount,
            rd32(g_sdat.info + rd32(g_sdat.info + 8 + SDAT_REC_SEQ * 4)),
            rd32(g_sdat.info + rd32(g_sdat.info + 8 + SDAT_REC_SEQARC * 4)),
            rd32(g_sdat.info + rd32(g_sdat.info + 8 + SDAT_REC_BANK * 4)),
            rd32(g_sdat.info + rd32(g_sdat.info + 8 + SDAT_REC_WAVEARC * 4)),
            rd32(g_sdat.info + rd32(g_sdat.info + 8 + SDAT_REC_GROUP * 4)));
    return 1;
}
