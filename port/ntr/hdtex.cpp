// The HD texture pack's loader. See ntr/hdtex.h for what the feature is.
//
// ------------------------------------------------------------------ THE NAME
// A pack file is named by a 64-bit hash of what the texture IS, so the same
// art file works in every level, every run and every build. hdtex_name()
// hashes, in order: format, width, height and the colour-0 bit; the texel
// bytes at the exact range the decoder reads; for format 5 the per-block
// index words and then the four palette entries EACH BLOCK INDEXES, in block
// order; and for every other palette format its own small entry run. Nothing
// about where the texture sits, or about what else is loaded, enters it.
// (Almost: those fixed palette runs could reach past a short palette, so
// hdtex_name2() further down hashes only the entries the decoder reads, and
// the loader asks for that name first and this one second.)
//
// IT IS NOT THE PARKED tangOS-SM64DS EDITION'S HASH, AND THAT WAS THE PLAN.
// That edition shipped 206 PNGs and the intent was to reproduce its hash bit
// for bit so they would load here unrenamed. Its definition, from
// recomp/vendor/melonds/GPU3D_Texcache.h, Texcache::GetTexture:
//
//     u64 texname = TextureHash[0]
//                 ^ (TextureHash[1] << 17 | TextureHash[1] >> 47)
//                 ^ (TexPalHash     << 31 | TexPalHash     >> 33)
//                 ^ ((u64)fmt << 56) ^ ((u64)widthLog2 << 48)
//                 ^ ((u64)heightLog2 << 40);
//
// each component being Texcache::MaskedHash -- XXH64 at seed 0 over a VRAM
// range, wrapping at the end of the array and reseeding with the running
// hash. That is reproduced exactly, in tangos_name() below, and it was
// measured against that edition's own 206-name dump on the castle grounds:
// 2 names agreed (both format 6), 26 carried the same picture under a
// different name (all format 5), none collided, and no variation of the
// palette range closed the gap.
//
// THE DEFECT IS IN THE ORIGINAL HASH. For format 5 -- 84% of this game's
// textures -- GetTexture sets TexPalSize = 0x10000, so the name is a hash of
// 64 KB of palette memory around the texture's own palette base: whatever
// else happens to be loaded. Proved inside ONE binary, with no second edition
// involved: level 1 dumped twice gives 97 of 97 identical names, so it is
// deterministic; but level 1 against level 6, over the pictures present in
// both, format 6 keeps the same name 3 times out of 3 while format 5 gets a
// DIFFERENT name 17 times out of 17 for the same picture. A format-5 entry in
// that pack only ever matched the level it was harvested in, and no
// implementation can reproduce from content a name that is not a function of
// content.
//
// So the loader uses the hash above, and the parked pack is renamed instead:
// port/tools/hdtex_repack.py matches pictures, and the dump's index carries
// tangos_name() beside the port's name so the mapping can also be read off a
// table. On the same rows the port's hash gives 96 of 96 identical names run
// to run and 18 of 19 shared pictures under ONE name across two levels, with
// no collisions.
//
// REPRODUCING melonDS'S ARITHMETIC AT ALL IS SOUND because the port maps the
// DS's whole video address space at its real addresses. ntr/mmio.h reserves
// VRAM_BASE 0x06000000 for VRAM_SIZE 0x900000 and ntr/io.cpp's map_fixed
// commits and MEMSETS it to zero before the game runs an instruction. So the
// texture-slot window (0x06800000, 0x80000 bytes) and the palette-slot window
// (0x06880000, 0x20000 bytes) have melonDS's VRAMFlat_Texture and
// VRAMFlat_TexPal size, shape and initial contents, and both are inside that
// one 9 MB reservation, so the wrapping arithmetic here can never read
// unmapped memory.
//
// ONE PIECE OF ARITHMETIC DIFFERS BETWEEN THE TWO. Format 5's slot-1 address:
// gx.cpp's decoder uses the simple off/2 form, which is right for the blocks
// this game actually uses, while melonDS applies GBATEK's masked form.
// tangos_name() uses melonDS's, because it answers "what did that edition
// call this"; hdtex_name() uses the DECODER's, because it answers "what does
// this port draw".
//
// A python copy of the XXH64 and of the parked mixing is in
// port/tools/hdtex_hash.py, whose selftest checks it against the canonical
// empty-string vector and against 400 random buffers through the xxhash
// module; tmp/TEX_hashtest.cpp in run hd1 checked this file's C++ against
// that python on 196 vectors.
//
// ---------------------------------------------------------------- OFF IS OFF
// hdtex_wants_work() is an int compare of two ints that are zero unless a
// pack was indexed or a dump directory was named. Everything else in this
// file hangs off it, and gx.cpp's bind path asks it first.

#include "ntr/hdtex.h"

#include <algorithm>
#include <atomic>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#if defined(_WIN32)
#include <windows.h>
#include <direct.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#endif

// stb_image and stb_image_write are public domain and vendored under
// port/third_party/stb with their licence text intact. They are header-only,
// so they add no DLL dependency and no static import -- a static DLL import
// maps over 0x02000000 and breaks this port. The implementation macros are
// defined HERE AND NOWHERE ELSE, so no other translation unit in the build
// grows a PNG codec. The include is relative rather than through a CMake
// include directory because port/CMakeLists.txt belongs to another lane.
#define STBI_ONLY_PNG
#define STBI_ONLY_GIF   /* ANIMTEX1: an animated <name>.gif, see hdtex.h */
#define STBI_NO_LINEAR
#define STBI_NO_HDR
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../third_party/stb/stb_image.h"
#include "../third_party/stb/stb_image_write.h"

namespace ntr {
namespace {

// ------------------------------------------------------------------- XXH64
// xxHash 64-bit, Yann Collet, BSD 2-Clause, transcribed from the reference
// algorithm. Every host this port builds for is little-endian, which is the
// byte order XXH64 is defined in, so the loads are plain memcpy.
const uint64_t kP1 = 0x9E3779B185EBCA87ull;
const uint64_t kP2 = 0xC2B2AE3D27D4EB4Full;
const uint64_t kP3 = 0x165667B19E3779F9ull;
const uint64_t kP4 = 0x85EBCA77C2B2AE63ull;
const uint64_t kP5 = 0x27D4EB2F165667C5ull;

inline uint64_t rotl64(uint64_t x, int r) { return (x << r) | (x >> (64 - r)); }

inline uint64_t rd64(const uint8_t *p) {
    uint64_t v;
    std::memcpy(&v, p, 8);
    return v;
}
inline uint32_t rd32(const uint8_t *p) {
    uint32_t v;
    std::memcpy(&v, p, 4);
    return v;
}
inline uint64_t xxh_round(uint64_t acc, uint64_t in) {
    acc += in * kP2;
    acc = rotl64(acc, 31);
    return acc * kP1;
}
inline uint64_t xxh_merge(uint64_t acc, uint64_t val) {
    acc ^= xxh_round(0, val);
    return acc * kP1 + kP4;
}

uint64_t xxh64(const uint8_t *p, size_t len, uint64_t seed) {
    const uint8_t *const end = p + len;
    uint64_t h;
    if (len >= 32) {
        const uint8_t *const limit = end - 32;
        uint64_t v1 = seed + kP1 + kP2, v2 = seed + kP2, v3 = seed,
                 v4 = seed - kP1;
        do {
            v1 = xxh_round(v1, rd64(p)); p += 8;
            v2 = xxh_round(v2, rd64(p)); p += 8;
            v3 = xxh_round(v3, rd64(p)); p += 8;
            v4 = xxh_round(v4, rd64(p)); p += 8;
        } while (p <= limit);
        h = rotl64(v1, 1) + rotl64(v2, 7) + rotl64(v3, 12) + rotl64(v4, 18);
        h = xxh_merge(h, v1);
        h = xxh_merge(h, v2);
        h = xxh_merge(h, v3);
        h = xxh_merge(h, v4);
    } else {
        h = seed + kP5;
    }
    h += static_cast<uint64_t>(len);
    while (p + 8 <= end) {
        h ^= xxh_round(0, rd64(p));
        p += 8;
        h = rotl64(h, 27) * kP1 + kP4;
    }
    if (p + 4 <= end) {
        h ^= static_cast<uint64_t>(rd32(p)) * kP1;
        p += 4;
        h = rotl64(h, 23) * kP2 + kP3;
    }
    while (p < end) {
        h ^= static_cast<uint64_t>(*p++) * kP5;
        h = rotl64(h, 11) * kP1;
    }
    h ^= h >> 33;
    h *= kP2;
    h ^= h >> 29;
    h *= kP3;
    h ^= h >> 32;
    return h;
}

// melonDS Texcache::MaskedHash, with a seed so the port's own hash can chain
// several ranges into one value: hash `size` bytes from `addr`, wrapping at
// the end of the array and feeding the running hash in as the next seed.
uint64_t masked_hash_seeded(const uint8_t *vram, uint32_t vram_size,
                            uint32_t addr, uint32_t size, uint64_t seed) {
    uint64_t h = seed;
    addr &= (vram_size - 1);
    while (size > 0) {
        const uint32_t piece =
            (addr + size > vram_size) ? (vram_size - addr) : size;
        h = xxh64(vram + addr, piece, h);
        addr = (addr + piece) & (vram_size - 1);
        size -= piece;
    }
    return h;
}

uint64_t masked_hash(const uint8_t *vram, uint32_t vram_size, uint32_t addr,
                     uint32_t size) {
    return masked_hash_seeded(vram, vram_size, addr, size, 0);
}

// The port's mapped slot windows, in melonDS's flat shape. gx.cpp states the
// same two bases for its decoder; they are repeated here so this file needs
// nothing out of that file's anonymous namespace.
const uintptr_t kTexSlotBase = 0x06800000u;
const uint32_t kTexVramSize = 0x80000u;
const uintptr_t kPlttSlotBase = 0x06880000u;
const uint32_t kPlttVramSize = 0x20000u;

// --------------------------------------------------------------------- state
int g_enabled;                 // hdtex_configure's answer: a pack may be served
int g_pack_indexed;            // a directory was listed and had names in it
int g_dump_on;
int g_log;
char g_pack_dir[1024];
std::string *g_dump_dir;
// The pack's PNG names, each mapped to the index in g_dirs of the directory
// that answers for it. g_dirs is highest priority first: every enabled mod's
// textures_hd (the later mod first), then the main pack LAST as the fallback.
std::unordered_map<uint64_t, int> *g_names;
std::vector<std::string> *g_dirs;
std::unordered_set<uint64_t> *g_dumped;  // names already written this run

/* MOVING TEXTURES (lane ANIMTEX1, see hdtex.h). g_kinds holds only the names
   whose winning directory answers with an animation (a folder of frames or a
   GIF); a name absent from it is a still, so a pack of stills never touches
   it. g_anims is the decoded store, one slot per animated name, filled by one
   worker thread started at index time. A slot's frames are written by the
   worker before it publishes state 1, and only read after a reader has seen
   state 1, so the atomic is the whole hand-over. */
enum { kStill = 1, kGif = 2, kFolder = 3 };
std::unordered_map<uint64_t, uint8_t> *g_kinds;
struct AnimSlot {
    std::string path;               // the folder or the .gif
    int kind = 0;
    std::shared_ptr<HdTexAnim> anim;
    std::atomic<int> state{0};      // 0 waiting, 1 ready, 2 failed
};
std::unordered_map<uint64_t, std::shared_ptr<AnimSlot>> *g_anims;
std::thread *g_anim_worker;
std::atomic<int> g_anim_cancel{0};
HdTexStats g_stats;

/* THE DUMP'S INDEX, and the lane's instrument.
   A pack author wants to know what a file in the dump actually is, and this
   lane needed to know which TERM of the hash diverged from the parked
   edition's when the whole name did. Both are the same table: one row per
   texture written, carrying the geometry words it came from, its format and
   shape, and each component hash on its own. SM64DS_HD_TEXTURES_INDEX names
   the file; without it nothing here runs.

   hdtex_name fills this and hdtex_dump writes the row, which is safe because
   gx.cpp calls them back to back on one bind and nothing runs between. */
struct NameParts {
    uint32_t teximage, plttbase, fmt, wl, hl, addr, pal_addr, tex_size, color0;
    uint64_t tangos;   // the parked edition's name for the same texture
    uint64_t v1;       // hdtex_name's name for it, beside hdtex_name2's
};
NameParts g_last;
std::string *g_index_path;
std::unordered_set<uint64_t> *g_indexed;

void make_dir(const char *p) {
#if defined(_WIN32)
    _mkdir(p);
#else
    mkdir(p, 0755);
#endif
}

// Exactly 16 hex digits then ".png": anything else in the directory is not a
// texture and is ignored rather than guessed at.
bool parse_name(const char *fn, uint64_t *out) {
    uint64_t v = 0;
    for (int i = 0; i < 16; ++i) {
        const char c = fn[i];
        int d;
        if (c >= '0' && c <= '9') d = c - '0';
        else if (c >= 'a' && c <= 'f') d = c - 'a' + 10;
        else if (c >= 'A' && c <= 'F') d = c - 'A' + 10;
        else return false;
        v = (v << 4) | static_cast<uint64_t>(d);
    }
    if (std::strcmp(fn + 16, ".png") != 0 && std::strcmp(fn + 16, ".PNG") != 0)
        return false;
    *out = v;
    return true;
}

/* The same 16 hex digits, then what kind of entry the name is: ".png" a
   still, ".gif" an animation, nothing at all (a folder) an animation of PNG
   frames. 0 for anything else. `is_dir` says which the directory listing
   found, so a FILE called <16 hex> with no extension is not a folder. */
int parse_entry(const char *fn, bool is_dir, uint64_t *out) {
    const size_t n = std::strlen(fn);
    if (n == 16 && is_dir) {
        char buf[24];
        std::memcpy(buf, fn, 16);
        std::memcpy(buf + 16, ".png", 5);
        return parse_name(buf, out) ? kFolder : 0;
    }
    if (n != 20 || is_dir) return 0;
    if (parse_name(fn, out)) return kStill;
    if (std::strcmp(fn + 16, ".gif") == 0 || std::strcmp(fn + 16, ".GIF") == 0) {
        char buf[24];
        std::memcpy(buf, fn, 16);
        std::memcpy(buf + 16, ".png", 5);
        return parse_name(buf, out) ? kGif : 0;
    }
    return 0;
}

// LIST THE DIRECTORY ONCE, AT CONFIGURE. After this a bind of an unreplaced
// texture costs one hash-set lookup and touches no file at all, which is what
// keeps the pack off the per-bind path for the large majority of this game's
// textures that a pack will never replace.
// Returns how many NEW names the directory contributed (a name a directory of
// higher priority already has keeps its entry).
int index_dir(const std::string &dir, int dir_index, bool log_missing) {
    int found = 0;
    const char *dirc = dir.c_str();
    /* ONE DIRECTORY'S NAMES, EACH AT ITS BEST KIND: a folder of frames beats
       a GIF beats a still (hdtex.h says why). Collected first and merged
       after, so the order the listing happens to return does not decide. */
    std::unordered_map<uint64_t, uint8_t> here;
    auto add = [&](uint64_t n, int kind) {
        uint8_t &k = here[n];
        if (kind > k) k = static_cast<uint8_t>(kind);
    };
#if defined(_WIN32)
    std::string pat = dir + "\\*";
    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA(pat.c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) {
        if (g_log && log_missing) std::printf("[hdtex] no pack at %s\n", dirc);
        return 0;
    }
    do {
        uint64_t n;
        const bool is_dir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        if (const int kind = parse_entry(fd.cFileName, is_dir, &n)) add(n, kind);
    } while (FindNextFileA(h, &fd));
    FindClose(h);
#else
    DIR *d = opendir(dirc);
    if (!d) {
        if (g_log && log_missing) std::printf("[hdtex] no pack at %s\n", dirc);
        return 0;
    }
    for (struct dirent *e = readdir(d); e; e = readdir(d)) {
        uint64_t n;
        struct stat st;
        const std::string full = dir + "/" + e->d_name;
        const bool is_dir = stat(full.c_str(), &st) == 0 && S_ISDIR(st.st_mode);
        if (const int kind = parse_entry(e->d_name, is_dir, &n)) add(n, kind);
    }
    closedir(d);
#endif
    for (const auto &kv : here) {
        if (!g_names->emplace(kv.first, dir_index).second) continue;
        ++found;
        if (kv.second != kStill) {
            (*g_kinds)[kv.first] = kv.second;
            auto slot = std::make_shared<AnimSlot>();
            char nm[24];
            std::snprintf(nm, sizeof nm, "%016llx",
                          static_cast<unsigned long long>(kv.first));
            slot->path = dir + "/" + nm + (kv.second == kGif ? ".gif" : "");
            slot->kind = kv.second;
            (*g_anims)[kv.first] = slot;
        }
    }
    return found;
}

// RGBA8 in file order -> the port's 0xAARRGGBB.
inline uint32_t rgba_to_argb(const unsigned char *p) {
    return (static_cast<uint32_t>(p[3]) << 24) |
           (static_cast<uint32_t>(p[0]) << 16) |
           (static_cast<uint32_t>(p[1]) << 8) | static_cast<uint32_t>(p[2]);
}

double now_ms() {
#if defined(_WIN32)
    LARGE_INTEGER f, c;
    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&c);
    return 1000.0 * static_cast<double>(c.QuadPart) /
           static_cast<double>(f.QuadPart);
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1e6;
#endif
}

/* ------------------------------------------------------- MOVING TEXTURES
   The decoders run on the worker thread only. Each fills one HdTexAnim and
   touches no shared state but the byte total below, which only the worker
   writes. */
size_t g_anim_total_bytes;   // decoded frame bytes, all animations (worker only)

/* One RGBA8 picture into a frame at (fw, fh): copied straight when the sizes
   agree (every frame of a normal animation), nearest-resampled when a frame
   came out a different size from the first one. */
void put_frame(std::vector<uint32_t> &dst, const unsigned char *img, int pw,
               int ph, int fw, int fh) {
    dst.resize(static_cast<size_t>(fw) * fh);
    if (pw == fw && ph == fh) {
        for (size_t i = 0; i < dst.size(); ++i) dst[i] = rgba_to_argb(img + i * 4);
        return;
    }
    for (int y = 0; y < fh; ++y) {
        const int sy = static_cast<int>(static_cast<int64_t>(y) * ph / fh);
        for (int x = 0; x < fw; ++x) {
            const int sx = static_cast<int>(static_cast<int64_t>(x) * pw / fw);
            dst[static_cast<size_t>(y) * fw + x] =
                rgba_to_argb(img + (static_cast<size_t>(sy) * pw + sx) * 4);
        }
    }
}

/* THE CAPS (hdtex.h). `n` frames of fw x fh: how many to skip between kept
   frames so this animation fits kAnimCapBytes, or 0 when even the first frame
   would not fit what is left of kAnimTotalCapBytes and only a still can be
   kept. */
int anim_stride(size_t n, int fw, int fh, const char *path) {
    const size_t fb = static_cast<size_t>(fw) * fh * 4u;
    int stride = 1;
    while ((n + stride - 1) / stride * fb > kAnimCapBytes && stride < 1 << 20)
        ++stride;
    const size_t need = (n + stride - 1) / stride * fb;
    if (g_anim_total_bytes + need > kAnimTotalCapBytes) {
        std::fprintf(stderr, "[hdtex] %s: animations already hold %u MB, the "
                     "total cap is %u MB: first frame only\n", path,
                     static_cast<unsigned>(g_anim_total_bytes >> 20),
                     kAnimTotalCapBytes >> 20);
        return 0;
    }
    if (stride > 1)
        std::fprintf(stderr, "[hdtex] %s: %u frames of %dx%d are over the %u MB "
                     "cap: keeping every %d%s frame, same loop length\n", path,
                     static_cast<unsigned>(n), fw, fh, kAnimCapBytes >> 20,
                     stride, stride == 2 ? "nd" : stride == 3 ? "rd" : "th");
    return stride;
}

/* "fps=24" (or "fps 24", or a bare number) from a folder's anim.txt; the
   default is 30 frames a second, the game's own rate. */
double folder_fps(const std::string &dir) {
    double fps = 30.0;
    if (FILE *f = std::fopen((dir + "/anim.txt").c_str(), "rb")) {
        char line[256];
        while (std::fgets(line, sizeof line, f)) {
            const char *q = line;
            while (*q == ' ' || *q == '\t') ++q;
            if ((q[0] == 'f' || q[0] == 'F') && (q[1] == 'p' || q[1] == 'P') &&
                (q[2] == 's' || q[2] == 'S')) {
                q += 3;
                while (*q == ' ' || *q == '\t' || *q == '=' || *q == ':') ++q;
            }
            const double v = std::atof(q);
            if (v > 0.0) { fps = v; break; }
        }
        std::fclose(f);
    }
    if (fps < 0.1) fps = 0.1;
    if (fps > 240.0) fps = 240.0;
    return fps;
}

bool load_folder(const std::string &dir, HdTexAnim &a) {
    std::vector<std::string> files;
#if defined(_WIN32)
    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA((dir + "\\*.png").c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return false;
    do {
        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            files.push_back(fd.cFileName);
    } while (FindNextFileA(h, &fd));
    FindClose(h);
#else
    DIR *d = opendir(dir.c_str());
    if (!d) return false;
    for (struct dirent *e = readdir(d); e; e = readdir(d)) {
        const size_t n = std::strlen(e->d_name);
        if (n > 4 && (std::strcmp(e->d_name + n - 4, ".png") == 0 ||
                      std::strcmp(e->d_name + n - 4, ".PNG") == 0))
            files.push_back(e->d_name);
    }
    closedir(d);
#endif
    if (files.empty()) return false;
    /* FILE-NAME ORDER, ignoring case: 0000.png, 0001.png ... or f000.png,
       f001.png ... both play in the order an author numbered them. */
    std::sort(files.begin(), files.end(),
              [](const std::string &x, const std::string &y) {
                  const size_t n = x.size() < y.size() ? x.size() : y.size();
                  for (size_t i = 0; i < n; ++i) {
                      const int cx = std::tolower(static_cast<unsigned char>(x[i]));
                      const int cy = std::tolower(static_cast<unsigned char>(y[i]));
                      if (cx != cy) return cx < cy;
                  }
                  return x.size() < y.size();
              });
    const double fps = folder_fps(dir);
    const size_t n = files.size();
    int stride = 1;
    for (size_t i = 0; i < n; ++i) {
        if (g_anim_cancel.load()) return false;
        if (a.pw && (i % static_cast<size_t>(stride)) != 0) continue;
        int pw = 0, ph = 0, comp = 0;
        unsigned char *img =
            stbi_load((dir + "/" + files[i]).c_str(), &pw, &ph, &comp, 4);
        if (!img || pw <= 0 || ph <= 0) {
            if (img) stbi_image_free(img);
            std::fprintf(stderr, "[hdtex] %s/%s: frame would not load\n",
                         dir.c_str(), files[i].c_str());
            if (!a.pw) return false;   // no first frame, no animation
            continue;                  // keep the frame before on screen
        }
        if (!a.pw) {
            a.pw = pw;
            a.ph = ph;
            stride = anim_stride(n, pw, ph, dir.c_str());
            a.stride = stride ? stride : 1;
        }
        a.frames.emplace_back();
        put_frame(a.frames.back(), img, pw, ph, a.pw, a.ph);
        stbi_image_free(img);
        if (!stride) break;
        /* THE KEPT FRAME HOLDS THE TIME OF EVERY FRAME IT STANDS FOR, and the
           ends are computed from the frame number, not summed, so the loop is
           exactly n / fps long whatever the stride. */
        size_t upto = i + static_cast<size_t>(stride);
        if (upto > n) upto = n;
        a.end_ms.push_back(static_cast<uint32_t>(upto * 1000.0 / fps + 0.5));
    }
    if (a.frames.empty()) return false;
    if (!stride) { a.end_ms.assign(1, 0); a.total_ms = 0; return true; }
    a.total_ms = a.end_ms.back();
    return true;
}

bool load_gif(const std::string &path, HdTexAnim &a) {
    FILE *f = std::fopen(path.c_str(), "rb");
    if (!f) return false;
    std::vector<unsigned char> buf;
    unsigned char chunk[65536];
    size_t got;
    while ((got = std::fread(chunk, 1, sizeof chunk, f)) > 0)
        buf.insert(buf.end(), chunk, chunk + got);
    std::fclose(f);
    if (buf.empty() || buf.size() > (1u << 30)) return false;
    int *delays = 0, w = 0, h = 0, z = 0, comp = 0;
    unsigned char *img = stbi_load_gif_from_memory(
        buf.data(), static_cast<int>(buf.size()), &delays, &w, &h, &z, &comp, 4);
    if (!img || w <= 0 || h <= 0 || z <= 0) {
        if (img) stbi_image_free(img);
        if (delays) stbi_image_free(delays);
        return false;
    }
    a.pw = w;
    a.ph = h;
    int stride = anim_stride(static_cast<size_t>(z), w, h, path.c_str());
    a.stride = stride ? stride : 1;
    const size_t fb = static_cast<size_t>(w) * h * 4u;
    uint32_t t = 0;
    for (int i = 0; i < z; ++i) {
        /* A GIF delay of 10 ms or less is played as 100 ms, which is what
           every browser does with it (authoring tools write 0 for "default"). */
        int dl = delays ? delays[i] : 100;
        if (dl <= 10) dl = 100;
        t += static_cast<uint32_t>(dl);
        if (stride && (i % stride) == 0) {
            a.frames.emplace_back();
            put_frame(a.frames.back(), img + fb * i, w, h, w, h);
            a.end_ms.push_back(0);
        }
        if (!a.end_ms.empty()) a.end_ms.back() = t;   // through the frames it stands for
        if (!stride) break;
    }
    stbi_image_free(img);
    if (delays) stbi_image_free(delays);
    if (!stride) { a.end_ms.assign(1, 0); a.total_ms = 0; return true; }
    a.total_ms = a.end_ms.back();
    return !a.frames.empty();
}

/* THE WORKER: every slot in the store, in name order, one at a time, at
   below-normal priority so a boot or a mod change does not fight the game for
   a core. A bind that needs a slot first waits for it (hdtex_lookup3). */
void anim_worker() {
#if defined(_WIN32)
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
#endif
    std::vector<uint64_t> names;
    for (const auto &kv : *g_anims) names.push_back(kv.first);
    std::sort(names.begin(), names.end());
    for (uint64_t n : names) {
        if (g_anim_cancel.load()) return;
        AnimSlot &slot = *g_anims->find(n)->second;
        auto a = std::make_shared<HdTexAnim>();
        const double t0 = now_ms();
        const bool ok = slot.kind == kGif ? load_gif(slot.path, *a)
                                          : load_folder(slot.path, *a);
        const double dt = now_ms() - t0;
        g_stats.anim_decode_ms += dt;
        if (ok) {
            const size_t bytes = a->frames.size() * static_cast<size_t>(a->pw) * a->ph * 4u;
            g_anim_total_bytes += bytes;
            g_stats.anim_bytes = g_anim_total_bytes;
            g_stats.anim_frames += static_cast<int>(a->frames.size());
            ++g_stats.anim_loaded;
            slot.anim = a;
            if (g_log)
                std::printf("[hdtex] anim %016llx: %d frame(s) %dx%d, loop %u ms, "
                            "stride %d, %.1f ms to decode (%s)\n",
                            static_cast<unsigned long long>(n),
                            static_cast<int>(a->frames.size()), a->pw, a->ph,
                            a->total_ms, a->stride, dt, slot.path.c_str());
        } else {
            std::fprintf(stderr, "[hdtex] %s: animation would not load\n",
                         slot.path.c_str());
        }
        slot.state.store(ok ? 1 : 2);
    }
}

void anim_stop_worker() {
    if (!g_anim_worker) return;
    g_anim_cancel.store(1);
    g_anim_worker->join();
    delete g_anim_worker;
    g_anim_worker = 0;
    g_anim_cancel.store(0);
}

void anim_start_worker() {
    g_anim_total_bytes = 0;
    g_stats.anim_frames = 0;
    g_stats.anim_loaded = 0;
    g_stats.anim_bytes = 0;
    if (g_anims->empty()) return;   // a pack of stills starts nothing
    g_anim_worker = new std::thread(anim_worker);
}

// LIST THE DIRECTORIES ONCE, AT CONFIGURE. After this a bind of an unreplaced
// texture costs one hash-map lookup and touches no file at all, which is what
// keeps the pack off the per-bind path for the large majority of this game's
// textures that a pack will never replace.
//
// Mods first, highest priority first, so a name a later mod has is claimed
// before an earlier mod's or the main pack's.
//
// MOD TEXTURES DO NOT WAIT FOR THE HdTextures SETTING (run mods1, MODFILES1).
// HdTextures says "show the optional HD pack"; a mod's textures_hd is the mod
// author's picture and a player who switched the mod on expects to see it
// whether or not they also run the big pack. So the mod directories are
// indexed whenever the mods list has any, and the main pack only when the
// setting is on (g_enabled). Tango's call later if he wants it gated.
void index_pack() {
    g_names = new std::unordered_map<uint64_t, int>();
    g_dirs = new std::vector<std::string>();
    g_kinds = new std::unordered_map<uint64_t, uint8_t>();
    g_anims = new std::unordered_map<uint64_t, std::shared_ptr<AnimSlot>>();
    if (hdtex_mod_dir_hook) {
        char d[1024];
        for (int i = 0; hdtex_mod_dir_hook(i, d, sizeof d); ++i) {
            g_dirs->push_back(d);
            const int n = index_dir(g_dirs->back(),
                                    static_cast<int>(g_dirs->size()) - 1, false);
            if (n > 0)
                std::fprintf(stderr, "[mods] textures_hd: %s: %d texture(s)\n", d, n);
        }
    }
    if (g_enabled) {
        g_dirs->push_back(g_pack_dir);
        index_dir(g_dirs->back(), static_cast<int>(g_dirs->size()) - 1, true);
    }
    g_stats.pack_entries = static_cast<int>(g_names->size());
    // An empty directory is not a pack: leaving g_pack_indexed at 0 keeps the
    // whole feature off the bind path rather than asking an empty set about
    // every texture in the game.
    g_pack_indexed = g_stats.pack_entries > 0 ? 1 : 0;
    g_stats.anim_entries = static_cast<int>(g_anims->size());
    anim_start_worker();
    if (g_log)
        std::printf("[hdtex] pack %s: %d texture(s) indexed (%d director%s)\n",
                    g_pack_dir, g_stats.pack_entries,
                    static_cast<int>(g_dirs->size()),
                    g_dirs->size() == 1 ? "y" : "ies");
}


}  // namespace

// ----------------------------------------------------------------- the seam
int (*hdtex_mod_dir_hook)(int i, char *out, unsigned cap);

void hdtex_configure(int enabled, const char *pack_dir)
{
    g_log = getenv("SM64DS_HD_TEXTURES_LOG") ? 1 : 0;

    g_pack_dir[0] = '\0';
    if (pack_dir && *pack_dir) {
        std::strncpy(g_pack_dir, pack_dir, sizeof g_pack_dir - 1);
        g_pack_dir[sizeof g_pack_dir - 1] = '\0';
    }
    // A pack with nowhere to load from is not a pack, so the directory is
    // part of "on" and not a separate question. (The seam's rule, kept.)
    g_enabled = (enabled && g_pack_dir[0]) ? 1 : 0;

    // THE DUMP IS INDEPENDENT OF THE PACK. Authoring a pack means running
    // with no pack and the dump on, so this is its own switch and its own
    // directory. It is a NEW variable rather than the existing
    // SM64DS_TEX_DUMP, which writes per-bind PPMs named by teximage/palette
    // and is what several boot lanes' texture rows already scan; that one
    // keeps doing exactly what it did.
    if (const char *d = getenv("SM64DS_HD_TEXTURES_DUMP")) {
        if (*d) {
            g_dump_dir = new std::string(d);
            g_dump_on = 1;
            make_dir(g_dump_dir->c_str());
            g_dumped = new std::unordered_set<uint64_t>();
        }
    }
    if (const char *ix = getenv("SM64DS_HD_TEXTURES_INDEX")) {
        if (*ix) {
            g_index_path = new std::string(ix);
            g_indexed = new std::unordered_set<uint64_t>();
        }
    }

    index_pack();
    if (g_log) {
        std::printf("[hdtex] enabled=%d pack=%s indexed=%d dump=%s\n",
                    g_enabled, g_pack_dir, g_pack_indexed,
                    g_dump_on ? g_dump_dir->c_str() : "(off)");
        // The summary belongs at the END of a run, and this file has no hook
        // in the frame loop and owns no file that does. atexit is the whole
        // mechanism, it is registered only when logging is on, and it prints.
        atexit(hdtex_report);
    }
}

/* THE RE-INDEX (run after093, lane SEAMMODS1). hal/mods.cpp switches the mod
   list at the title and the textures_hd folders it answers through
   hdtex_mod_dir_hook change with it: the index is built again from the hook
   and the main pack the same way hdtex_configure built it (the HdTextures
   setting and the dump are not touched). The caller then drops gx.cpp's decode
   cache (gx_invalidate_textures), which holds pictures this index chose.
   Returns the number of names indexed. */
int hdtex_reindex(void)
{
    anim_stop_worker();   /* ANIMTEX1: it reads the store deleted below */
    delete g_names;
    delete g_dirs;
    delete g_kinds;
    delete g_anims;
    g_names = 0;
    g_dirs = 0;
    g_kinds = 0;
    g_anims = 0;
    index_pack();
    return g_stats.pack_entries;
}

static int g_paused;   /* TEXSWAP1, see hdtex.h */
void hdtex_set_paused(int paused) { g_paused = paused ? 1 : 0; }
int hdtex_paused(void) { return g_paused; }

int hdtex_enabled(void) { return g_enabled; }

const char *hdtex_pack_dir(void) { return g_pack_dir; }

bool hdtex_wants_work(void) { return (g_pack_indexed | g_dump_on) != 0; }

// THE PARKED EDITION'S NAME, kept for one reason: the dump's index carries it
// beside the port's own, so anyone holding a pack built for that edition can
// read the mapping off a table instead of matching pixels. Nothing in the bind
// path calls this. The block at the head of this file says why it is not the
// name the loader uses.
static uint64_t tangos_name(uint32_t teximage, uint32_t plttbase)
{
    const uint32_t tp = teximage & ~0xC00F0000u;
    const uint32_t fmt = (tp >> 26) & 7u;
    const uint32_t wlog2 = (tp >> 20) & 7u;
    const uint32_t hlog2 = (tp >> 23) & 7u;
    const uint32_t w = 8u << wlog2, h = 8u << hlog2;
    const uint32_t addr = (tp & 0xFFFFu) * 8u;

    const uint8_t *const tex = reinterpret_cast<const uint8_t *>(kTexSlotBase);
    const uint8_t *const pal = reinterpret_cast<const uint8_t *>(kPlttSlotBase);

    uint64_t h0 = 0, h1 = 0, hp = 0;
    if (fmt == 7) {
        h0 = masked_hash(tex, kTexVramSize, addr, w * h * 2u);
    } else if (fmt == 5) {
        uint32_t slot1 = 0x20000u + ((addr & 0x1FFFCu) >> 1);
        if (addr >= 0x40000u) slot1 += 0x10000u;
        h0 = masked_hash(tex, kTexVramSize, addr, w * h / 16u * 4u);
        h1 = masked_hash(tex, kTexVramSize, slot1, w * h / 16u * 2u);
        // 0x10000 BYTES. This is the defect: see the file header.
        hp = masked_hash(pal, kPlttVramSize, plttbase * 16u, 0x10000u);
    } else {
        uint32_t tex_size = 0, entries = 0, pal_addr = plttbase * 16u;
        switch (fmt) {
            case 1: tex_size = w * h;      entries = 32;  break;
            case 6: tex_size = w * h;      entries = 8;   break;
            case 2: tex_size = w * h / 4u; entries = 4;   pal_addr >>= 1; break;
            case 3: tex_size = w * h / 2u; entries = 16;  break;
            case 4: tex_size = w * h;      entries = 256; break;
            default: return 0;
        }
        pal_addr &= 0x1FFFFu;
        h0 = masked_hash(tex, kTexVramSize, addr, tex_size);
        hp = masked_hash(pal, kPlttVramSize, pal_addr, entries * 2u);
    }
    return h0 ^ rotl64(h1, 17) ^ rotl64(hp, 31) ^
           (static_cast<uint64_t>(fmt) << 56) ^
           (static_cast<uint64_t>(wlog2) << 48) ^
           (static_cast<uint64_t>(hlog2) << 40);
}

// One palette byte, or 0 outside the window. The palette window is mapped and
// committed for its whole length, so this can only refuse a base the geometry
// words themselves put out of range.
static inline uint8_t pal_byte(const uint8_t *pal, uint32_t off)
{
    return off < kPlttVramSize ? pal[off] : 0u;
}

static uint64_t name_v1(uint32_t teximage, uint32_t plttbase)
{
    const uint32_t tp = teximage & ~0xC00F0000u;
    const uint32_t fmt = (tp >> 26) & 7u;
    const uint32_t wlog2 = (tp >> 20) & 7u;
    const uint32_t hlog2 = (tp >> 23) & 7u;
    const uint32_t w = 8u << wlog2, h = 8u << hlog2;
    const uint32_t addr = (tp & 0xFFFFu) * 8u;
    const uint32_t color0 = (tp >> 29) & 1u;

    const uint8_t *const tex = reinterpret_cast<const uint8_t *>(kTexSlotBase);
    const uint8_t *const pal = reinterpret_cast<const uint8_t *>(kPlttSlotBase);

    /* THE HEADER. Shape and format first, so two textures that happen to share
       bytes at different dimensions cannot share a name. color0 rides here
       because it changes what the texture LOOKS like without changing one byte
       of it, and a pack author replacing the picture is replacing the look. */
    uint32_t head[4] = {fmt, w, h, color0};
    uint64_t hash = xxh64(reinterpret_cast<const uint8_t *>(head), sizeof head,
                          0);

    uint32_t tex_size = 0, entries = 0, pal_addr = plttbase * 16u;
    switch (fmt) {
        case 1: tex_size = w * h;       entries = 32;  break;  // A3I5
        case 2: tex_size = w * h / 4u;  entries = 4;   pal_addr >>= 1; break;
        case 3: tex_size = w * h / 2u;  entries = 16;  break;
        case 4: tex_size = w * h;       entries = 256; break;
        case 5: tex_size = w * h / 16u * 4u; entries = 0; break;
        case 6: tex_size = w * h;       entries = 8;   break;  // A5I3
        case 7: tex_size = w * h * 2u;  entries = 0;   break;  // direct colour
        default: return 0;  // format 0 is "no texture" and never gets here
    }

    // THE TEXTURE'S OWN BYTES, exactly the range the decoder reads.
    hash = masked_hash_seeded(tex, kTexVramSize, addr, tex_size, hash);

    if (fmt == 5) {
        /* 4x4-COMPRESSED, AND THIS IS THE WHOLE POINT OF THIS HASH. Its
           per-block index words live in texture slot 1, at the offset gx.cpp's
           own decoder reads them from -- the DECODER's rule, not melonDS's
           masked one, because the question here is what this port draws.

           Then the PALETTE BYTES THE BLOCKS ACTUALLY REFERENCE, four entries
           per block in block order, rather than a fixed 64 KB window around
           the palette base. That window is what made the parked edition's
           format-5 name depend on whatever else was loaded, so the same
           texture had one name on the castle grounds and another in Bob-omb
           Battlefield. Gathering the referenced entries instead makes the name
           a property of the texture and nothing else. */
        const uint32_t idx_off = 0x20000u + addr / 2u;
        const uint32_t blocks = w * h / 16u;
        hash = masked_hash_seeded(tex, kTexVramSize, idx_off, blocks * 2u,
                                  hash);

        std::vector<uint8_t> refs(static_cast<size_t>(blocks) * 8u);
        for (uint32_t b = 0; b < blocks; ++b) {
            const uint32_t io = (idx_off + b * 2u) & (kTexVramSize - 1u);
            const uint32_t info =
                static_cast<uint32_t>(tex[io]) |
                (static_cast<uint32_t>(tex[(io + 1u) & (kTexVramSize - 1u)])
                 << 8);
            // Palette offset is in 4-byte units, i.e. two colours (GBATEK);
            // a block uses the four entries from there.
            const uint32_t base_byte = pal_addr + (info & 0x3FFFu) * 4u;
            for (uint32_t k = 0; k < 8u; ++k)
                refs[static_cast<size_t>(b) * 8u + k] =
                    pal_byte(pal, base_byte + k);
        }
        hash = xxh64(refs.data(), refs.size(), hash);
    } else if (entries) {
        // Every other palette format indexes a fixed run from its own base,
        // and the run is small and entirely the texture's own.
        pal_addr &= 0x1FFFFu;
        hash = masked_hash_seeded(pal, kPlttVramSize, pal_addr, entries * 2u,
                                  hash);
    }

    if (g_index_path) {
        std::memset(&g_last, 0, sizeof g_last);
        g_last.teximage = teximage;
        g_last.plttbase = plttbase;
        g_last.fmt = fmt;
        g_last.wl = wlog2;
        g_last.hl = hlog2;
        g_last.addr = addr;
        g_last.tex_size = tex_size;
        g_last.pal_addr = pal_addr;
        g_last.color0 = color0;
        g_last.tangos = tangos_name(teximage, plttbase);
    }
    return hash;
}

uint64_t hdtex_name(uint32_t teximage, uint32_t plttbase)
{
    ++g_stats.hashed;
    return name_v1(teximage, plttbase);
}

/* THE SECOND NAME (run studio1, lane HDNAME2). The name must be a property of
   the texture and nothing else, and hdtex_name above is not quite that: its
   palette part reads FIXED runs, and a fixed run can be longer than what the
   texture uses.

   Format 5 hashes four entries per block whatever the block's mode, but a
   mode-0 block reads three of them and a mode-1 or mode-3 block reads two. A
   block of a two- or three-colour mode that sits on the last entries of its
   palette therefore hashed the first entries of the NEXT model's palette, so
   the same picture took a different name in a different level: run studio1
   lane HASH1 counted 365 stage and object textures that do this. The other
   palette formats read 32, 16 or 256 entries however short the texture's own
   palette is, and 26 more textures moved with what sat after them.

   This one hashes the same header, the same texel bytes and the same index
   words, and then ONLY THE ENTRIES THE DECODER READS: for format 5, per block
   in block order, three entries for mode 0, two for modes 1 and 3, four for
   mode 2; for formats 1, 2, 3, 4 and 6, entries 0 up to the highest index
   the texels use, found by one pass over them. Entry 0 is always in, so a
   texture whose colour 0 is transparent does not change name over that bit
   alone; the bit has its own place in the header. Format 7 has no palette
   and gets the name hdtex_name gives it.

   The loader asks for this name first and for hdtex_name's on a miss, so
   every pack made before it still loads; a new pack may use either. */
uint64_t hdtex_name2(uint32_t teximage, uint32_t plttbase)
{
    const uint32_t tp = teximage & ~0xC00F0000u;
    const uint32_t fmt = (tp >> 26) & 7u;
    const uint32_t wlog2 = (tp >> 20) & 7u;
    const uint32_t hlog2 = (tp >> 23) & 7u;
    const uint32_t w = 8u << wlog2, h = 8u << hlog2;
    const uint32_t addr = (tp & 0xFFFFu) * 8u;
    const uint32_t color0 = (tp >> 29) & 1u;

    // The index row wants both names, and name_v1 fills g_last for it.
    ++g_stats.hashed;
    if (g_index_path) g_last.v1 = name_v1(teximage, plttbase);
    if (fmt == 7)
        return g_index_path ? g_last.v1 : name_v1(teximage, plttbase);

    const uint8_t *const tex = reinterpret_cast<const uint8_t *>(kTexSlotBase);
    const uint8_t *const pal = reinterpret_cast<const uint8_t *>(kPlttSlotBase);

    uint32_t head[4] = {fmt, w, h, color0};
    uint64_t hash = xxh64(reinterpret_cast<const uint8_t *>(head), sizeof head,
                          0);

    uint32_t tex_size = 0, pal_addr = plttbase * 16u;
    switch (fmt) {
        case 1: tex_size = w * h;       break;  // A3I5
        case 2: tex_size = w * h / 4u;  pal_addr >>= 1; break;
        case 3: tex_size = w * h / 2u;  break;
        case 4: tex_size = w * h;       break;
        case 5: tex_size = w * h / 16u * 4u; break;
        case 6: tex_size = w * h;       break;  // A5I3
        default: return 0;  // format 0 is "no texture" and never gets here
    }

    // THE TEXTURE'S OWN BYTES, exactly as hdtex_name hashes them.
    hash = masked_hash_seeded(tex, kTexVramSize, addr, tex_size, hash);

    if (fmt == 5) {
        // The index words, at the decoder's offset, as in hdtex_name.
        const uint32_t idx_off = 0x20000u + addr / 2u;
        const uint32_t blocks = w * h / 16u;
        hash = masked_hash_seeded(tex, kTexVramSize, idx_off, blocks * 2u,
                                  hash);

        // How many entries each block mode reads (GBATEK): mode 0 three, the
        // fourth colour being transparent; modes 1 and 3 two, the others
        // blended from them; mode 2 all four.
        static const uint8_t kModeEntries[4] = {3, 2, 4, 2};
        std::vector<uint8_t> refs;
        refs.reserve(static_cast<size_t>(blocks) * 8u);
        for (uint32_t b = 0; b < blocks; ++b) {
            const uint32_t io = (idx_off + b * 2u) & (kTexVramSize - 1u);
            const uint32_t info =
                static_cast<uint32_t>(tex[io]) |
                (static_cast<uint32_t>(tex[(io + 1u) & (kTexVramSize - 1u)])
                 << 8);
            const uint32_t base_byte = pal_addr + (info & 0x3FFFu) * 4u;
            const uint32_t n = kModeEntries[info >> 14] * 2u;
            for (uint32_t k = 0; k < n; ++k)
                refs.push_back(pal_byte(pal, base_byte + k));
        }
        hash = xxh64(refs.data(), refs.size(), hash);
    } else {
        // One pass over the texels for the highest index they use. The index
        // bits: format 1 the low 5 of each byte, 6 the low 3, 2 four 2-bit
        // texels a byte, 3 two 4-bit texels, 4 the whole byte.
        uint32_t top = 0;
        for (uint32_t i = 0; i < tex_size; ++i) {
            const uint32_t v = tex[(addr + i) & (kTexVramSize - 1u)];
            uint32_t m = 0;
            switch (fmt) {
                case 1: m = v & 0x1Fu; break;
                case 6: m = v & 0x07u; break;
                case 2: {
                    const uint32_t a = v & 3u, b = (v >> 2) & 3u,
                                   c = (v >> 4) & 3u, d = v >> 6;
                    m = a > b ? a : b;
                    if (c > m) m = c;
                    if (d > m) m = d;
                    break;
                }
                case 3: {
                    const uint32_t a = v & 0xFu, b = v >> 4;
                    m = a > b ? a : b;
                    break;
                }
                case 4: m = v; break;
            }
            if (m > top) top = m;
        }
        pal_addr &= 0x1FFFFu;
        hash = masked_hash_seeded(pal, kPlttVramSize, pal_addr,
                                  (top + 1u) * 2u, hash);
    }
    return hash;
}

void hdtex_dump(uint64_t name, int width, int height, const uint32_t *argb)
{
    if (!g_dump_on || width <= 0 || height <= 0 || !argb) return;
    if (!g_dumped->insert(name).second) return;  // one file per unique name

    char path[1200];
    std::snprintf(path, sizeof path, "%s/%016llx.png", g_dump_dir->c_str(),
                  static_cast<unsigned long long>(name));

    const size_t n = static_cast<size_t>(width) * height;
    std::vector<unsigned char> rgba(n * 4);
    for (size_t i = 0; i < n; ++i) {
        const uint32_t p = argb[i];
        rgba[i * 4 + 0] = static_cast<unsigned char>(p >> 16);
        rgba[i * 4 + 1] = static_cast<unsigned char>(p >> 8);
        rgba[i * 4 + 2] = static_cast<unsigned char>(p);
        rgba[i * 4 + 3] = static_cast<unsigned char>(p >> 24);
    }
    if (stbi_write_png(path, width, height, 4, rgba.data(), width * 4))
        ++g_stats.dumped;

    // THE INDEX ROW. See NameParts above for what it is for.
    if (g_index_path && g_indexed->insert(name).second) {
        if (FILE *f = fopen(g_index_path->c_str(), "ab")) {
            if (g_indexed->size() == 1)
                fprintf(f,
                        "name\tname_v1\tw\th\tfmt\tcolor0\tteximage\t"
                        "plttbase\taddr\ttexsize\tpaladdr\ttangos_name\n");
            fprintf(f, "%016llx\t%016llx\t%d\t%d\t%u\t%u\t%08x\t%04x\t%05x"
                       "\t%u\t%05x\t%016llx\n",
                    (unsigned long long)name, (unsigned long long)g_last.v1,
                    width, height, g_last.fmt,
                    g_last.color0, g_last.teximage, g_last.plttbase,
                    g_last.addr, g_last.tex_size, g_last.pal_addr,
                    (unsigned long long)g_last.tangos);
            fclose(f);
        }
    }
}

int hdtex_lookup(uint64_t name, int width, int height,
                 std::vector<uint32_t> &out)
{
    return hdtex_lookup2(name, 0, 0, 0, width, height, out);
}

int hdtex_lookup2(uint64_t name, int have_words, uint32_t teximage,
                  uint32_t plttbase, int width, int height,
                  std::vector<uint32_t> &out)
{
    return hdtex_lookup3(name, have_words, teximage, plttbase, width, height,
                         out, 0);
}

/* One integer scale for a pw x ph picture over a width x height texture: the
   rule hdtex_lookup2 always used, shared with the animations. */
static int scale_for(int pw, int ph, int width, int height)
{
    int su = pw / width, sv = ph / height;
    if (su < 1) su = 1;
    if (sv < 1) sv = 1;
    int s = su < sv ? su : sv;
    if (s > 8) s = 8;
    return s;
}

int hdtex_lookup3(uint64_t name, int have_words, uint32_t teximage,
                  uint32_t plttbase, int width, int height,
                  std::vector<uint32_t> &out,
                  std::shared_ptr<const HdTexAnim> *anim)
{
    if (anim) anim->reset();
    if (!g_pack_indexed || g_paused || width <= 0 || height <= 0) return 0;
    auto found = g_names->find(name);
    if (have_words && (found == g_names->end() || found->second > 0)) {
        /* THE OLD NAME, AND MOD ORDER BEFORE NAME VERSION. Every pack made
           before hdtex_name2 is named by hdtex_name, so the old name is asked
           for too. Each entry's value is the rank of the directory that
           answers for it (index_pack walks them highest priority first: the
           later mod, then the earlier ones, the main pack last), and the
           better rank wins: a higher mod's old-name file beats a lower pack's
           new-name file for the same texture. The new name wins only a tie,
           both files in one directory. A new-name hit in the top directory
           cannot be beaten, so it skips the second hash. When the index is
           on, hdtex_name2 has already computed the old name for this bind. */
        const uint64_t old_name =
            g_index_path ? g_last.v1 : name_v1(teximage, plttbase);
        const auto old_found = g_names->find(old_name);
        if (old_found != g_names->end() &&
            (found == g_names->end() || old_found->second < found->second)) {
            found = old_found;
            name = old_name;
        }
    }
    if (found == g_names->end()) {
        // THE COMMON CASE, and it is one or two hash-set lookups with no file
        // I/O.
        ++g_stats.missing;
        return 0;
    }

    /* AN ANIMATION (lane ANIMTEX1). Only a name in g_kinds can be one, so a
       pack of stills pays one empty-map lookup here and nothing else. */
    if (anim && !g_kinds->empty()) {
        const auto k = g_anims->find(name);
        if (k != g_anims->end()) {
            AnimSlot &slot = *k->second;
            if (slot.state.load() == 0) {
                /* THE WORKER HAS NOT REACHED IT YET: wait. It started at boot
                   or at the mod change, so by the time a level binds the
                   texture this is normally already done; the longest wait is
                   in the summary line so a hitch here cannot hide. */
                const double w0 = now_ms();
                while (slot.state.load() == 0) {
#if defined(_WIN32)
                    Sleep(1);
#endif
                }
                const double waited = now_ms() - w0;
                if (waited > g_stats.anim_wait_ms) g_stats.anim_wait_ms = waited;
            }
            if (slot.state.load() == 1 && slot.anim && !slot.anim->frames.empty()) {
                const HdTexAnim &a = *slot.anim;
                const int s = scale_for(a.pw, a.ph, width, height);
                const int ow = width * s, oh = height * s;
                if (a.pw != ow || a.ph != oh) {
                    /* Not a whole multiple: every frame resampled once, here,
                       laid end to end in `out`. The normal case skips this
                       and binds the decoded frames themselves. */
                    const size_t fn = static_cast<size_t>(ow) * oh;
                    out.assign(fn * a.frames.size(), 0);
                    for (size_t f = 0; f < a.frames.size(); ++f)
                        for (int y = 0; y < oh; ++y) {
                            const int sy = static_cast<int>(static_cast<int64_t>(y) * a.ph / oh);
                            for (int x = 0; x < ow; ++x) {
                                const int sx = static_cast<int>(static_cast<int64_t>(x) * a.pw / ow);
                                out[f * fn + static_cast<size_t>(y) * ow + x] =
                                    a.frames[f][static_cast<size_t>(sy) * a.pw + sx];
                            }
                        }
                } else {
                    out.clear();
                }
                *anim = slot.anim;
                ++g_stats.replaced;
                if (g_log)
                    std::printf("[hdtex] %016llx %dx%d -> anim %d frame(s) %dx%d "
                                "scale %d\n", static_cast<unsigned long long>(name),
                                width, height, static_cast<int>(a.frames.size()),
                                a.pw, a.ph, s);
                return s;
            }
            /* A broken animation falls back to a still of the same name in
               the same directory, if there is one, and otherwise to the DS. */
        }
    }

    char path[1200];
    std::snprintf(path, sizeof path, "%s/%016llx.png",
                  (*g_dirs)[found->second].c_str(),
                  static_cast<unsigned long long>(name));

    const double t0 = now_ms();
    int pw = 0, ph = 0, comp = 0;
    unsigned char *img = stbi_load(path, &pw, &ph, &comp, 4);
    if (!img || pw <= 0 || ph <= 0) {
        if (img) stbi_image_free(img);
        ++g_stats.load_failed;
        if (g_log)
            std::printf("[hdtex] %016llx: PNG would not load (%s)\n",
                        static_cast<unsigned long long>(name), path);
        return 0;
    }

    // THE SCALE IS AN INTEGER, ALWAYS. A pack PNG is normally an exact whole
    // multiple of the DS texture, and then S is that multiple and every texel
    // is copied straight across -- which is what makes the identity-pack
    // proof exact. An odd size is not refused, because a pack author should
    // not have to hit one number; it is nearest-resampled to the nearest
    // integer scale instead, the same rule the parked edition used. The
    // smaller of the two axes wins, so a replacement never claims more texels
    // than it actually has.
    const int s = scale_for(pw, ph, width, height);  // at most 8

    const int ow = width * s, oh = height * s;
    out.assign(static_cast<size_t>(ow) * oh, 0);
    if (pw == ow && ph == oh) {
        const size_t n = static_cast<size_t>(ow) * oh;
        for (size_t i = 0; i < n; ++i)
            out[i] = rgba_to_argb(img + i * 4);
    } else {
        for (int y = 0; y < oh; ++y) {
            int sy = static_cast<int>(static_cast<int64_t>(y) * ph / oh);
            if (sy >= ph) sy = ph - 1;
            for (int x = 0; x < ow; ++x) {
                int sx = static_cast<int>(static_cast<int64_t>(x) * pw / ow);
                if (sx >= pw) sx = pw - 1;
                out[static_cast<size_t>(y) * ow + x] =
                    rgba_to_argb(img + (static_cast<size_t>(sy) * pw + sx) * 4);
            }
        }
    }
    stbi_image_free(img);

    const double dt = now_ms() - t0;
    g_stats.total_load_ms += dt;
    if (dt > g_stats.worst_load_ms) {
        g_stats.worst_load_ms = dt;
        g_stats.worst_load_name = name;
        g_stats.worst_load_w = pw;
        g_stats.worst_load_h = ph;
    }
    ++g_stats.replaced;
    g_stats.replacement_bytes += out.size() * sizeof(uint32_t);
    if (g_log)
        std::printf("[hdtex] %016llx %dx%d -> %dx%d scale %d (%.2f ms)\n",
                    static_cast<unsigned long long>(name), width, height, pw,
                    ph, s, dt);
    return s;
}

int hdtex_anim_frame(const HdTexAnim &a, double now)
{
    const size_t n = a.frames.size();
    if (n <= 1 || a.total_ms == 0) return 0;
    const double t = std::fmod(now, static_cast<double>(a.total_ms));
    const uint32_t ti = t < 0.0 ? 0u : static_cast<uint32_t>(t);
    const auto it = std::upper_bound(a.end_ms.begin(), a.end_ms.end(), ti);
    const size_t i = static_cast<size_t>(it - a.end_ms.begin());
    return static_cast<int>(i < n ? i : n - 1);
}

/* SM64DS_HD_ANIM_STEP_MS=<ms> (test knob): every read of the clock advances
   it by exactly that much instead of reading real time, so a headless row
   shows the same frames on every run however fast it goes. gx.cpp reads the
   clock once per game tick (gx_reset) and once per Smooth Motion picture. */
double hdtex_clock_ms(void)
{
    static double step = -1.0, t = 0.0;
    if (step < 0.0) {
        const char *e = getenv("SM64DS_HD_ANIM_STEP_MS");
        step = (e && *e) ? std::atof(e) : 0.0;
        if (step < 0.0) step = 0.0;
    }
    if (step > 0.0) return t += step;
    return now_ms();
}

const HdTexStats *hdtex_stats(void) { return &g_stats; }

void hdtex_report(void)
{
    if (!g_log) return;
    /* STDERR, like gx.cpp's own [perf] line and for the same reason: this
       binary sends stdout to a playlog file, so a summary printed there is
       not in the output a test driver captures. Every number a gate quotes
       comes off this line. */
    std::fprintf(stderr,
        "[hdtex] SUMMARY pack=%d hashed=%d replaced=%d missing=%d "
        "load_failed=%d dumped=%d worst_load=%.2fms (%016llx %dx%d) "
        "total_load=%.2fms held=%llu bytes anim=%d/%d frames=%d "
        "anim_bytes=%llu anim_decode=%.1fms anim_wait=%.2fms\n",
        g_stats.pack_entries, g_stats.hashed, g_stats.replaced, g_stats.missing,
        g_stats.load_failed, g_stats.dumped, g_stats.worst_load_ms,
        static_cast<unsigned long long>(g_stats.worst_load_name),
        g_stats.worst_load_w, g_stats.worst_load_h, g_stats.total_load_ms,
        static_cast<unsigned long long>(g_stats.replacement_bytes),
        g_stats.anim_loaded, g_stats.anim_entries, g_stats.anim_frames,
        static_cast<unsigned long long>(g_stats.anim_bytes),
        g_stats.anim_decode_ms, g_stats.anim_wait_ms);
    std::fflush(stderr);
}

}  // namespace ntr
