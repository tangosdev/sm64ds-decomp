// THE HD TEXTURE PACK: replace a cartridge texture with a bigger picture of
// the same thing, chosen by what the texture IS rather than where it sits.
//
// OFF IS OFF. With HdTextures absent or 0 and no dump directory set,
// hdtex_wants_work() is one int compare and gx.cpp's bind path takes exactly
// the route it took before this file existed: no hash is computed, no
// directory is listed, no file is opened and nothing is allocated. That is a
// measured claim, not a hope -- run hd1's out/TEX/ carries the capture hashes.
//
// THE NAME IS THE CONTENT, AND ONLY THE CONTENT. A pack file is called
// <16 hex digits>.png, and those digits are a 64-bit hash of the texture's
// format, its width and height, its colour-0 bit, its own VRAM bytes and the
// palette bytes it actually references. Nothing about WHERE it sits or what
// else is loaded enters it, so a name is the same in every level, every run
// and every build.
//
// That last part is the whole reason this is not the parked tangOS-SM64DS
// edition's hash. That one is reproduced here too (hdtex.cpp, tangos_name)
// and it is stable for every format except 4x4-compressed, which is 84% of
// this game's textures: for those it hashes 64 KB of palette memory around
// the texture's own palette base, so the same texture had one name on the
// castle grounds and a different one in Bob-omb Battlefield. The dump's index
// carries both names per texture, and port/tools/hdtex_repack.py renames an
// old pack by matching pictures.
//
// THE SCALE. A replacement is bound at its own pixel dimensions with a scale
// factor S = png_width / texture_width, and GxTriangle::tex_scale (ntr/gx.h)
// multiplies the sampler's texel coordinates by S while tw/th carry the real
// buffer size. So a 4x image of a 32x32 texture binds as 128x128 with S = 4
// and every wrap, clamp and flip rule keeps working on the DS's own texel
// grid. A PNG that is not a whole multiple is not refused; it is
// nearest-resampled to the nearest integer scale, so a pack author does not
// have to hit one exact number.
//
// TO AUTHOR A PACK: set SM64DS_HD_TEXTURES_DUMP to an empty directory, play
// (or run port/tools/hdtex_harvest.py, which boots every level and scene
// headless), and every texture the game bound lands there as <name>.png at
// the cartridge's own size. Edit or upscale those files, keep the names, drop
// them in the pack directory. port/tools/hdtex_upscale.py builds a baseline.
// out/TEX/pack_authors.txt in run hd1 says the same thing in plain words.

#ifndef NTR_HDTEX_H
#define NTR_HDTEX_H

#include <stdint.h>

#include <vector>

namespace ntr {

// THE SEAM (run hd1, lane RES). Called once at boot beside configure_aspect,
// with the HdTextures settings value and the pack directory. `enabled` 0 is
// off, which is the default and the cartridge's own textures. A null or empty
// directory reads as off whatever `enabled` says: a pack with nowhere to load
// from is not a pack. The pointer is copied, so its lifetime is this file's
// business rather than an assumption about the caller's storage.
void hdtex_configure(int enabled, const char *pack_dir);

// What hdtex_configure was told. Unchanged from the seam's contract.
int hdtex_enabled(void);
const char *hdtex_pack_dir(void);

// True when a bind must compute the content hash: a pack is indexed, or the
// dump is on. THIS IS THE WHOLE COST WHEN THE FEATURE IS OFF.
bool hdtex_wants_work(void);

// The content name of the texture the two latched geometry words describe.
// Only meaningful while hdtex_wants_work(); it reads DS video memory.
// SM64DS_HD_TEXTURES_INDEX=<file> makes the dump write a table beside the
// PNGs: one row per texture with its format, shape, geometry words and the
// parked edition's name for the same texture.
uint64_t hdtex_name(uint32_t teximage, uint32_t plttbase);

// Write one decoded texture to the dump directory as <name>.png (RGBA8, one
// file per unique name, never overwritten, nothing at all when no dump
// directory is set). `argb` is width*height texels, 0xAARRGGBB.
void hdtex_dump(uint64_t name, int width, int height, const uint32_t *argb);

// Look a name up in the pack. A miss costs one hash-set lookup and NO file
// I/O. A hit fills `out` with (width*S)*(height*S) texels in 0xAARRGGBB and
// returns S (1..8). 0 means no replacement: bind the port's own decode.
int hdtex_lookup(uint64_t name, int width, int height,
                 std::vector<uint32_t> &out);

// What the run did. SM64DS_HD_TEXTURES_LOG=1 prints the per-texture lines and
// the summary; these numbers are what run hd1's measurements are taken from.
struct HdTexStats {
    int pack_entries;          // PNG names indexed at configure
    int hashed;                // textures hashed (one per cache miss)
    int replaced;              // misses that took a pack PNG
    int missing;               // misses whose name was not in the pack
    int load_failed;           // the name was indexed but the PNG would not load
    int dumped;                // PNG files written by the dump
    double worst_load_ms;      // slowest single pack load: the bind hitch
    uint64_t worst_load_name;
    int worst_load_w, worst_load_h;
    double total_load_ms;
    size_t replacement_bytes;  // texels held by replacements, in bytes
};
const HdTexStats *hdtex_stats(void);
void hdtex_report(void);  // one summary line, when logging is on

}  // namespace ntr

#endif  // NTR_HDTEX_H
