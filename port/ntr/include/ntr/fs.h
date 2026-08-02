// DS card filesystem.
//
// Two backends behind one API:
//
//   ROM       -- parse the FNT/FAT tables out of a .nds image. Useful for
//                development, and the only way to get at the data initially.
//   DIRECTORY -- read files the user has already extracted. This is the shipping
//                model: the port ships no Nintendo data, and the player supplies
//                assets from a cartridge they own. tools/extract_rom.py does the
//                extraction, once, on their machine.
//
// Files are addressed by numeric ID, because that is how the game addresses them
// -- LoadFileAt(handle, dest) takes a FAT index, not a path. Paths come from the
// FNT and exist for our benefit, not the game's. IDs below fs_overlay_count() are
// overlays, which sit before the named filesystem.

#ifndef NTR_FS_H
#define NTR_FS_H

#include <stdint.h>

namespace ntr {

bool fs_mount_rom(const char *rom_path);
bool fs_mount_dir(const char *dir_path);
void fs_unmount();
bool fs_mounted();

int32_t fs_file_count();
int32_t fs_overlay_count();          // = the root directory's first file ID

// The game does not use FAT indices directly: SharedFilePtr and the resource
// tables carry a 16-bit *asset ID*, and LoadFileAt(handle, dest) takes one of
// those. The two spaces differ by a constant.
//
// Established empirically against the decomp's recovered asset evidence
// (include/MadPianoResources.h, include/MessageBank.h) on the readable/
// asset-evidence-seed branch. Eight independent points across two distant
// regions, five of them name-confirmed:
//
//     0x040A/B/C -> data/enemy/piano/piano.bmd / .kcl / _attack.bca
//     0x0431..35 -> data/message/msg_data_{eng,frn,gmn,itl,spn}.bin
//
// The constant itself is unexplained; 430 is what the data says.
constexpr int32_t ASSET_ID_BIAS = 430;

inline int32_t fs_id_by_asset(uint32_t asset_id) {
    return static_cast<int32_t>(asset_id) - ASSET_ID_BIAS;
}

int32_t fs_size(int32_t id);
const char *fs_path(int32_t id);     // "" for overlays and unnamed entries
int32_t fs_id_by_path(const char *path);   // -1 if absent

// Read a whole file. Returns bytes read, or -1. `dst` must hold fs_size(id).
int32_t fs_read(int32_t id, void *dst, int32_t capacity);

}  // namespace ntr

#endif  // NTR_FS_H
