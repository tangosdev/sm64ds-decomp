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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unordered_set>

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
std::unordered_set<uint64_t> *g_names;   // the pack's PNG names
std::unordered_set<uint64_t> *g_dumped;  // names already written this run
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

// LIST THE DIRECTORY ONCE, AT CONFIGURE. After this a bind of an unreplaced
// texture costs one hash-set lookup and touches no file at all, which is what
// keeps the pack off the per-bind path for the large majority of this game's
// textures that a pack will never replace.
void index_pack() {
    g_names = new std::unordered_set<uint64_t>();
#if defined(_WIN32)
    std::string pat = std::string(g_pack_dir) + "\\*.png";
    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA(pat.c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) {
        if (g_log) std::printf("[hdtex] no pack at %s\n", g_pack_dir);
        return;
    }
    do {
        uint64_t n;
        if (std::strlen(fd.cFileName) == 20 && parse_name(fd.cFileName, &n))
            g_names->insert(n);
    } while (FindNextFileA(h, &fd));
    FindClose(h);
#else
    DIR *d = opendir(g_pack_dir);
    if (!d) {
        if (g_log) std::printf("[hdtex] no pack at %s\n", g_pack_dir);
        return;
    }
    for (struct dirent *e = readdir(d); e; e = readdir(d)) {
        uint64_t n;
        if (std::strlen(e->d_name) == 20 && parse_name(e->d_name, &n))
            g_names->insert(n);
    }
    closedir(d);
#endif
    g_stats.pack_entries = static_cast<int>(g_names->size());
    // An empty directory is not a pack: leaving g_pack_indexed at 0 keeps the
    // whole feature off the bind path rather than asking an empty set about
    // every texture in the game.
    g_pack_indexed = g_stats.pack_entries > 0 ? 1 : 0;
    if (g_log)
        std::printf("[hdtex] pack %s: %d texture(s) indexed\n", g_pack_dir,
                    g_stats.pack_entries);
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

}  // namespace

// ----------------------------------------------------------------- the seam
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

    if (g_enabled) index_pack();
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

uint64_t hdtex_name(uint32_t teximage, uint32_t plttbase)
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
    ++g_stats.hashed;
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
                        "name\tw\th\tfmt\tcolor0\tteximage\tplttbase\taddr\t"
                        "texsize\tpaladdr\ttangos_name\n");
            fprintf(f, "%016llx\t%d\t%d\t%u\t%u\t%08x\t%04x\t%05x\t%u\t%05x"
                       "\t%016llx\n",
                    (unsigned long long)name, width, height, g_last.fmt,
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
    if (!g_pack_indexed || width <= 0 || height <= 0) return 0;
    if (g_names->find(name) == g_names->end()) {
        // THE COMMON CASE, and it is one hash-set lookup with no file I/O.
        ++g_stats.missing;
        return 0;
    }

    char path[1200];
    std::snprintf(path, sizeof path, "%s/%016llx.png", g_pack_dir,
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
    int su = pw / width, sv = ph / height;
    if (su < 1) su = 1;
    if (sv < 1) sv = 1;
    int s = su < sv ? su : sv;
    if (s > 8) s = 8;  // the largest array a pack may ask for

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
        "total_load=%.2fms held=%llu bytes\n",
        g_stats.pack_entries, g_stats.hashed, g_stats.replaced, g_stats.missing,
        g_stats.load_failed, g_stats.dumped, g_stats.worst_load_ms,
        static_cast<unsigned long long>(g_stats.worst_load_name),
        g_stats.worst_load_w, g_stats.worst_load_h, g_stats.total_load_ms,
        static_cast<unsigned long long>(g_stats.replacement_bytes));
    std::fflush(stderr);
}

}  // namespace ntr
