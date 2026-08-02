// BMD model loading.
//
// Header offsets are from include/BMD_File.h in the decomp, which pins them from
// matched code. The display-list table is the unk_0c/unk_10 chain that
// Model::UpdateFileOffsets walks:
//
//     unk_10 -> unk_0c records of {count, ptr}
//               ptr    -> count records of 0x10 bytes
//                         +0x08 display list size
//                         +0x0c display list data
//
// UpdateFileOffsets rebases the pointers at +0x04 and +0x0c of each 0x10-byte
// record, which is the evidence that both are file-relative offsets.

#include "ntr/bmd.h"

#include "ntr/fs.h"
#include "ntr/gx.h"
#include "ntr/lz77.h"
#include "ntr/texture.h"

#include <array>
#include <cmath>
#include <cstring>
#include <string>

namespace ntr {
namespace {

uint32_t rd32(const std::vector<uint8_t> &b, size_t off) {
    if (off + 4 > b.size()) return 0;
    return static_cast<uint32_t>(b[off]) | (static_cast<uint32_t>(b[off + 1]) << 8)
           | (static_cast<uint32_t>(b[off + 2]) << 16)
           | (static_cast<uint32_t>(b[off + 3]) << 24);
}

std::string read_name(const std::vector<uint8_t> &b, uint32_t off) {
    if (off == 0 || off >= b.size()) return {};
    std::string s;
    for (size_t i = off; i < b.size() && b[i] && s.size() < 32; ++i)
        s.push_back(static_cast<char>(b[i]));
    return s;
}

int16_t rd16s(const std::vector<uint8_t> &b, size_t off) {
    if (off + 2 > b.size()) return 0;
    return static_cast<int16_t>(b[off] | (b[off + 1] << 8));
}

int32_t rd32s(const std::vector<uint8_t> &b, size_t off) {
    return static_cast<int32_t>(rd32(b, off));
}

using Mat16 = std::array<float, 16>;
const Mat16 kIdentity = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};

// Row-vector convention, matching the geometry engine.
Mat16 mat_mul(const Mat16 &a, const Mat16 &b) {
    Mat16 r{};
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j) {
            float s = 0;
            for (int k = 0; k < 4; ++k) s += a[i * 4 + k] * b[k * 4 + j];
            r[i * 4 + j] = s;
        }
    return r;
}

}  // namespace

bool bmd_load(int32_t file_id, BmdModel &out) {
    const int32_t packed_size = fs_size(file_id);
    if (packed_size <= 8) return false;

    std::vector<uint8_t> packed(static_cast<size_t>(packed_size));
    if (fs_read(file_id, packed.data(), packed_size) != packed_size) return false;

    const int32_t unpacked = lz77_size(packed.data(), packed_size);
    if (unpacked > 0) {
        out.data.assign(static_cast<size_t>(unpacked), 0);
        if (lz77_unpack(packed.data(), packed_size, out.data.data(), unpacked) != unpacked)
            return false;
    } else {
        out.data = std::move(packed);       // some models ship uncompressed
    }

    const std::vector<uint8_t> &b = out.data;
    if (b.size() < 0x38) return false;

    out.scale_shift = rd32(b, 0x00);
    out.num_bones = rd32(b, 0x04);
    out.num_textures = rd32(b, 0x14);
    out.num_palettes = rd32(b, 0x1c);
    out.num_materials = rd32(b, 0x24);

    const uint32_t groups = rd32(b, 0x0c);
    const uint32_t table = rd32(b, 0x10);
    out.dlists.clear();
    if (groups > 4096) return false;                    // obviously-bad header

    for (uint32_t gi = 0; gi < groups; ++gi) {
        const size_t rec = table + gi * 8u;
        if (rec + 8 > b.size()) break;
        const uint32_t count = rd32(b, rec);
        const uint32_t ptr = rd32(b, rec + 4);
        if (count > 65536) break;
        for (uint32_t k = 0; k < count; ++k) {
            const size_t sub = ptr + k * 0x10u;
            if (sub + 0x10 > b.size()) break;
            const uint32_t size = rd32(b, sub + 0x08);
            const uint32_t data = rd32(b, sub + 0x0c);
            if (size == 0 || data == 0) continue;
            if (data + size > b.size() || (size & 3)) continue;
            out.dlists.push_back({data, size});
        }
    }

    // --- materials -> textures ------------------------------------------------
    const uint32_t p_tex = rd32(b, 0x18), p_pal = rd32(b, 0x20), p_mat = rd32(b, 0x28);

    out.materials.clear();
    for (uint32_t i = 0; i < out.num_materials && i < 4096; ++i) {
        const size_t o = p_mat + i * 0x30u;
        if (o + 0x30 > b.size()) break;
        BmdMaterial m;
        m.texture = rd32(b, o + 0x04);
        m.palette = rd32(b, o + 0x08);
        m.spe_emi = rd32(b, o + 0x24);
        m.dif_amb = rd32(b, o + 0x28);
        m.name = read_name(b, rd32(b, o));
        out.materials.push_back(std::move(m));
    }

    // Which material each display list uses: the bone's material-ID list.
    out.material_ids.clear();
    if (out.num_bones > 0) {
        const uint32_t p_bone = rd32(b, 0x08);
        const uint32_t ids = rd32(b, p_bone + 0x34);
        for (size_t i = 0; i < out.dlists.size(); ++i) {
            const size_t o = ids + i;
            out.material_ids.push_back(o < b.size() ? b[o] : 0);
        }
    }

    // Decode one texture per material, so binding at draw time is a lookup.
    out.textures.assign(out.materials.size(), BmdTexture{});
    for (size_t mi = 0; mi < out.materials.size(); ++mi) {
        const BmdMaterial &mat = out.materials[mi];
        if (mat.texture >= out.num_textures || mat.palette >= out.num_palettes) continue;

        const size_t to = p_tex + mat.texture * 0x14u;
        const size_t po = p_pal + mat.palette * 0x10u;
        if (to + 0x14 > b.size() || po + 0x10 > b.size()) continue;

        const uint32_t tdata = rd32(b, to + 0x04), tsize = rd32(b, to + 0x08);
        const uint32_t tflags = rd32(b, to + 0x10);
        const uint32_t pdata = rd32(b, po + 0x04), psize = rd32(b, po + 0x08);
        if (tdata + tsize > b.size() || pdata + psize > b.size()) continue;

        TextureDesc d;
        d.format = static_cast<int>((tflags >> 26) & 7);
        d.width = 8 << ((tflags >> 20) & 7);
        d.height = 8 << ((tflags >> 23) & 7);
        d.color0_transparent = ((tflags >> 29) & 1) != 0;
        d.data = &b[tdata];
        d.data_len = static_cast<int32_t>(tsize);
        d.pal = &b[pdata];
        d.pal_len = static_cast<int32_t>(psize);
        // Format 5 keeps its per-block palette-index table immediately after the
        // main block, half its length.
        if (d.format == 5 && tdata + tsize + tsize / 2 <= b.size()) {
            d.index = &b[tdata + tsize];
            d.index_len = static_cast<int32_t>(tsize / 2);
        }

        BmdTexture &bt = out.textures[mi];
        bt.width = d.width;
        bt.height = d.height;
        bt.format = d.format;
        bt.name = read_name(b, rd32(b, to));
        if (!texture_decode(d, bt.rgba)) bt.rgba.clear();
    }
    return true;
}

// Sample a BCA track at frame 0.
//
// func_020457f0 / func_020456a0 are the general keyframe samplers, but every
// branch of both collapses to tbl[0] when the frame index is 0: the constant
// case returns tbl[0] outright, the per-frame case returns tbl[0], and the
// interpolated case computes a zero remainder and returns the low key. So the
// bind pose costs no interpolation code at all.
template <class T>
T track0(const std::vector<uint8_t> &b, uint32_t pool, uint16_t index) {
    const size_t o = pool + static_cast<size_t>(index) * sizeof(T);
    if (o + sizeof(T) > b.size()) return T{};
    T v{};
    std::memcpy(&v, &b[o], sizeof(T));
    return v;
}

// Build each bone's world matrix and preload it into the stack slot the display
// lists will MTX_RESTORE.
//
// Hierarchy from the decomp's walker func_02045074, which reads the bone record
// as s16 fields: f0 (0x00) is the bone index and the slot, f8 (0x08) a parent
// delta, fa/fc (0x0a/0x0c) sibling and child links in 0x40-byte units.
//
//     MulMat4x3Mat4x3(m, (a + f8)*0x30 + t, a*0x30 + t, t)   // world = local * parent
//
// The local transform is NOT in the BMD. func_0204547c fills a per-bone Elem
// record (stride 0x34) out of a BCA animation:
//
//     0x0C/0x10/0x14  scale, defaulting to 0x1000 when no scale track exists
//     0x18            flag: any scale track present
//     0x1A/0x1C/0x1E  rotation angles, stored <<4
//     0x20/0x24/0x28  translation
//
// and func_02045178 turns that into a 4x3 matrix as an XYZ Euler rotation with
// the translation in row 3. The BCA header holds three value pools and a track
// array at 0x08/0x0c/0x10/0x14 -- the four offsets Animation::UpdateFileOffsets
// rebases -- with one 0x24-byte track record per bone.
void bmd_bone_matrices(const BmdModel &m, const std::vector<uint8_t> &bca,
                       std::vector<std::array<float, 16>> &out) {
    const std::vector<uint8_t> &b = m.data;
    const uint32_t p_bone = rd32(b, 0x08);
    out.assign(m.num_bones ? m.num_bones : 1, kIdentity);

    const bool posed = bca.size() >= 0x18;
    uint32_t pool_scale = 0, pool_rot = 0, pool_trans = 0, tracks = 0;
    if (posed) {
        pool_scale = rd32(bca, 0x08);
        pool_rot   = rd32(bca, 0x0c);
        pool_trans = rd32(bca, 0x10);
        tracks     = rd32(bca, 0x14);
    }

    for (uint32_t i = 0; i < m.num_bones; ++i) {
        const size_t o = p_bone + i * 0x40u;
        if (o + 0x40 > b.size()) break;

        const int16_t idx = rd16s(b, o + 0x00);
        const int16_t parent_delta = rd16s(b, o + 0x08);
        if (idx < 0 || static_cast<size_t>(idx) >= out.size()) continue;

        float sx = 1, sy = 1, sz = 1, rx = 0, ry = 0, rz = 0, tx = 0, ty = 0, tz = 0;
        if (posed) {
            const size_t tr = tracks + static_cast<size_t>(idx) * 0x24u;
            if (tr + 0x24 <= bca.size()) {
                auto u16at = [&](size_t off) { return static_cast<uint16_t>(
                    bca[tr + off] | (bca[tr + off + 1] << 8)); };
                // Scale is present only if any of the six selector fields is set,
                // exactly as func_0204547c decides it.
                const uint16_t any_scale = u16at(0x02) | u16at(0x06) | u16at(0x0a)
                                           | bca[tr + 0x01] | bca[tr + 0x05] | bca[tr + 0x09];
                if (any_scale) {
                    sx = track0<int32_t>(bca, pool_scale, u16at(0x02)) / 4096.0f;
                    sy = track0<int32_t>(bca, pool_scale, u16at(0x06)) / 4096.0f;
                    sz = track0<int32_t>(bca, pool_scale, u16at(0x0a)) / 4096.0f;
                }
                // Rotation is stored as a 12-bit table index; <<4 makes it a
                // 16-bit angle, so a full turn is 4096 stored units.
                const float k = 6.28318530718f / 4096.0f;
                rx = track0<int16_t>(bca, pool_rot, u16at(0x0e)) * k;
                ry = track0<int16_t>(bca, pool_rot, u16at(0x12)) * k;
                rz = track0<int16_t>(bca, pool_rot, u16at(0x16)) * k;

                tx = track0<int32_t>(bca, pool_trans, u16at(0x1a)) / 4096.0f;
                ty = track0<int32_t>(bca, pool_trans, u16at(0x1e)) / 4096.0f;
                tz = track0<int32_t>(bca, pool_trans, u16at(0x22)) / 4096.0f;
            }
        }

        // XYZ Euler, matching func_02045178's term-by-term construction.
        const float a0 = std::sin(rx), a1 = std::cos(rx);
        const float b0 = std::sin(ry), b1 = std::cos(ry);
        const float c0 = std::sin(rz), c1 = std::cos(rz);
        Mat16 local{};
        local[0] = b1 * c1;                 local[1] = b1 * c0;                 local[2]  = -b0;
        local[4] = b0 * a0 * c1 - a1 * c0;  local[5] = a1 * c1 + b0 * a0 * c0;  local[6]  = b1 * a0;
        local[8] = a0 * c0 + b0 * a1 * c1;  local[9] = b0 * a1 * c0 - a0 * c1;  local[10] = b1 * a1;
        local[12] = tx; local[13] = ty; local[14] = tz; local[15] = 1.0f;
        for (int r = 0; r < 3; ++r) {
            const float s = (r == 0) ? sx : (r == 1) ? sy : sz;
            local[r * 4 + 0] *= s; local[r * 4 + 1] *= s; local[r * 4 + 2] *= s;
        }

        // Parents precede children in the array, so one forward pass composes.
        const int parent = idx + parent_delta;
        if (parent >= 0 && parent != idx && static_cast<size_t>(parent) < out.size())
            out[idx] = mat_mul(local, out[parent]);
        else
            out[idx] = local;
    }

    // Vertices are in units of 2^-scaleShift relative to bone space. The piano
    // makes this unmissable: its vertices span ~9 units while its bone
    // translations are 17.5, a 4x mismatch, and scaleShift is 2. Without this
    // the parts fly apart -- which is exactly what two earlier attempts did.
    if (m.scale_shift) {
        const float s = static_cast<float>(1u << m.scale_shift);
        Mat16 pre = kIdentity;
        pre[0] = pre[5] = pre[10] = s;
        for (Mat16 &w : out) w = mat_mul(pre, w);   // v' = (v * S) * W
    }
}

bool bca_load_path(const char *path, std::vector<uint8_t> &out) {
    const int32_t id = fs_id_by_path(path);
    const int32_t packed_size = fs_size(id);
    if (id < 0 || packed_size <= 8) return false;
    std::vector<uint8_t> packed(static_cast<size_t>(packed_size));
    if (fs_read(id, packed.data(), packed_size) != packed_size) return false;
    const int32_t n = lz77_size(packed.data(), packed_size);
    if (n <= 0) { out = std::move(packed); return true; }
    out.assign(static_cast<size_t>(n), 0);
    return lz77_unpack(packed.data(), packed_size, out.data(), n) == n;
}

namespace {
const std::vector<uint8_t> *g_anim = nullptr;
}

void bmd_set_animation(const std::vector<uint8_t> *bca) { g_anim = bca; }

bool bmd_load_asset(uint32_t asset_id, BmdModel &out) {
    return bmd_load(fs_id_by_asset(asset_id), out);
}

bool bmd_load_path(const char *path, BmdModel &out) {
    const int32_t id = fs_id_by_path(path);
    return id >= 0 && bmd_load(id, out);
}

void bmd_submit(const BmdModel &m) {
    static const std::vector<uint8_t> kNoAnim;
    std::vector<std::array<float, 16>> bones;
    bmd_bone_matrices(m, g_anim ? *g_anim : kNoAnim, bones);
    for (size_t i = 0; i < bones.size() && i < 32; ++i)
        gx_set_matrix_slot(static_cast<int>(i), bones[i].data());

    for (size_t di = 0; di < m.dlists.size(); ++di) {
        // Material state is set around the display list, not inside it -- the
        // lists carry only geometry, UVs and normals. This is what the game's
        // render path does with G3_TexImageParam / G3_TexPlttBase.
        const uint8_t mid = di < m.material_ids.size() ? m.material_ids[di] : 0;
        if (mid < m.materials.size())
            gx_set_material(m.materials[mid].dif_amb, m.materials[mid].spe_emi);
        if (mid < m.textures.size() && !m.textures[mid].rgba.empty()) {
            const BmdTexture &t = m.textures[mid];
            gx_bind_texture(t.rgba.data(), t.width, t.height);
        } else {
            gx_bind_texture(nullptr, 0, 0);
        }

        const BmdModel::DisplayList &dl = m.dlists[di];
        const uint32_t words = dl.size / 4u;
        for (uint32_t i = 0; i < words; ++i)
            gx_write_fifo(rd32(m.data, dl.offset + i * 4u));
    }
    gx_bind_texture(nullptr, 0, 0);
}

}  // namespace ntr
