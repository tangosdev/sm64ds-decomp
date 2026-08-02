// SM64DS model files.
//
// BMD is EAD's own container, not NSBMD -- the loader checks no magic and the
// header is a flat count/offset table. Layout comes from include/BMD_File.h in
// the decomp, which derives it from matched code rather than format docs.
//
// The payload is what matters here: BMD stores its geometry as *DS display
// lists*, the same packed command streams the geometry engine consumes. So
// drawing a real model is not a conversion -- it is handing the bytes to
// gx_write_fifo, which is exactly what the game does.

#ifndef NTR_BMD_H
#define NTR_BMD_H

#include <stdint.h>

#include <array>
#include <string>
#include <vector>

namespace ntr {

// Decoded texture, ready for the rasteriser.
struct BmdTexture {
    std::vector<uint32_t> rgba;
    int width = 0, height = 0, format = 0;
    std::string name;
};

// Material record: index into textures at +0x04, palettes at +0x08. Confirmed
// against Mario, whose four materials name themselves mat_bm_body / _eye /
// _head / _head_c and point at textures mario_body / _eye_1 / _head / _head.
struct BmdMaterial {
    uint32_t texture = 0, palette = 0;
    uint32_t dif_amb = 0;      // +0x28 -- diffuse, ambient, set-vertex-colour
    uint32_t spe_emi = 0;      // +0x24 -- specular, emission
    std::string name;
};

struct BmdModel {
    std::vector<uint8_t> data;        // decompressed file image
    uint32_t scale_shift = 0;
    uint32_t num_bones = 0, num_textures = 0, num_palettes = 0, num_materials = 0;

    // Display list `i` draws with material material_ids[i]; the mapping comes
    // from the bone's material-ID list at +0x34, which BMD_File.h documents.
    struct DisplayList { uint32_t offset, size; };
    std::vector<DisplayList> dlists;
    std::vector<uint8_t> material_ids;

    std::vector<BmdMaterial> materials;
    std::vector<BmdTexture> textures;   // decoded per material, indexed by material
};

// Load and decompress by filesystem id or path. Returns false if absent.
bool bmd_load(int32_t file_id, BmdModel &out);
bool bmd_load_path(const char *path, BmdModel &out);
bool bmd_load_asset(uint32_t asset_id, BmdModel &out);   // the game's own handle

// Push every display list into the geometry engine, in order.
void bmd_submit(const BmdModel &m);

// Per-bone world matrices, indexed by bone (= MTX_RESTORE slot).
//
// A BMD carries no bind pose: the bone transforms live in a BCA animation, and
// the game always drives them through ModelComponents::UpdateBones. Pass a BCA
// to pose the model; pass none and every bone is identity, which stacks the
// parts on top of each other.
void bmd_bone_matrices(const BmdModel &m, const std::vector<uint8_t> &bca,
                       std::vector<std::array<float, 16>> &out);

// Load and decompress a BCA animation.
bool bca_load_path(const char *path, std::vector<uint8_t> &out);

// Bind an animation for subsequent bmd_submit calls. Empty clears it.
void bmd_set_animation(const std::vector<uint8_t> *bca);

}  // namespace ntr

#endif
