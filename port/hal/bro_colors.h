// run after093, lane BROCOLOR1: A BRO IN HIS PLAYER'S OWN COLOURS.
//
// Mario, Luigi and Wario each draw with ONE palette per model file, shared by
// every body that wears that file, so a per-player colour cannot be written
// into the file's own palette. Instead the palette span a material reads is
// copied into a port-only window of palette VRAM above everything the DS
// uses (hal/bro_colors.cpp kWinBase..kWinEnd), recoloured there with the
// category table hal/vs_char_groups.h, and the material's palette base is
// pointed at the copy around that body's draw (and put back after it), the
// way hal/player_bridges.cpp stamps a Yoshi ghost's row.
//
// A pick that equals the cartridge colour of its row is no pick: a body whose
// rows are all unset or all cartridge stamps nothing, so its draw is the
// cartridge's, byte for byte.
#ifndef PORT_HAL_BRO_COLORS_H
#define PORT_HAL_BRO_COLORS_H

namespace port {
namespace bro {

enum { kRowHat = 0, kRowOveralls = 1, kRowShoes = 2, kRows = 3 };

// One player's picks for the three bros: rgb[chr][row] as 0xRRGGBB, set[chr][row] nonzero when picked.
// chr: 0 Mario, 1 Luigi, 2 Wario.
struct Picks {
    unsigned rgb[3][kRows];
    unsigned char set[3][kRows];
};

// The material palette bases one draw changed, for unstamp.
enum { kMaxMat = 64 };
struct Stamp {
    char *rec[2];
    const unsigned char *hdr[2];    // the model's BMD_File, for restamp
    unsigned n[2];
    int old[2][kMaxMat];
    int now[2][kMaxMat];
    unsigned char hit[2][kMaxMat];
};

// true when any row of any bro is a real pick (set and not the cartridge colour)
bool any(const Picks &p);

// Point the materials of model `mdl` (slot k = 0 body, 1 head) at recoloured copies. Nothing changes
// (st.n[k] = 0) when no palette of the model has a picked category.
void stamp_model(Stamp &st, int k, char *mdl, const Picks &p);

// Stamp again every material of a stamped model whose base moved since the stamp (BROSTAMP1: a texture
// sequence's Update between the body's draw and its mirror draw writes the cartridge base back on the
// materials it drives, the eyes' blink on every head). The moved base becomes the one unstamp restores.
void restamp(Stamp &st, const Picks &p);

// Put back every base stamp_model changed that still holds the stamped value (a texture sequence that
// moved a base during the draw keeps its own value).
void unstamp(Stamp &st);

}  // namespace bro
}  // namespace port

// The picks a body draws with, or 0 for none (hal/comms_sync.cpp: the local body's own picks in an
// online world, a ghost's from its identity). Fills *out when it returns 1.
extern "C" int port_bro_picks_for_body(void *body, port::bro::Picks *out);

#endif  // PORT_HAL_BRO_COLORS_H
