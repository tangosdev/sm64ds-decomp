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

// PALSPLIT1 (run coopfix1, 11-13): the undershirt is its own row. It was one pick with the hat (the cap in the head
// file, the undershirt in the body file) until Tango asked for two; it is row 3, after the three rows the identity's
// bro block, settings.json and the wire have always carried in that order, so none of them moves. An undershirt that
// is not set wears the hat's pick (pick_of), so a player, a settings file or a console older than the split looks
// exactly as it did when the two were one row.
enum { kRowHat = 0, kRowOveralls = 1, kRowShoes = 2, kRowUndershirt = 3, kRows = 4 };

// One player's picks for the three bros: rgb[chr][row] as 0xRRGGBB, set[chr][row] nonzero when picked.
// chr: 0 Mario, 1 Luigi, 2 Wario. An unset undershirt follows the hat.
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

// BROSCOLOR1 (run coopfix1): THE COLOUR CHOICES A PLAYER HAS IN A ROOM, the character-palettes mod's own list, offered
// without the mod: Mario, Luigi and Wario each "Original" or "Custom" (Custom = his own Hat, Overalls, Shoes and
// Undershirt colours),
// Yoshi "Green" "Red" "Blue" "Yellow" (the cartridge's four multiplayer rows), "Pink" "Cyan" "Purple" "Orange" (the profile popup's
// other four cells) or "Custom" (his own Body and Shoes). The
// choice is stored in settings.json (a bro: OnlineMarioHat ...; Yoshi: OnlineBodyColor / OnlineShoeColor, the profile
// popup's keys), so the popup and these calls agree; the identity carries a bro's rows to every console (SYNI bro block)
// and the join exports Yoshi's pair. Logic thread only. chr: 0 Mario, 1 Luigi, 2 Wario, 3 Yoshi.
extern "C" {
int         port_bro_choice_count(int chr);                        // 2 for a bro, 9 for Yoshi; 0 for a bad chr
const char *port_bro_choice_name(int chr, int i);                  // the choice's label, or 0 out of range
int         port_bro_choice_current(int chr);                      // the local player's current choice (index)
int         port_bro_choice_set(int chr, int i);                   // store it; 1 ok. Custom brings back his last Custom colours
int         port_bro_choice_is_default(int chr);                   // 1 while chr wears the default colours (chr -1: all four)
int         port_bro_custom_get(int chr, int part, unsigned *rgb); // a Custom colour, 0xRRGGBB; part: bro 0 Hat 1 Overalls
                                                                   // 2 Shoes 3 Undershirt, Yoshi 0 Body 1 Shoes. 1 when it
                                                                   // answered
int         port_bro_custom_set(int chr, int part, unsigned rgb);  // set one part; chr becomes Custom. 1 ok
unsigned    port_bro_choice_swatch(int chr, int i);                // 0xRRGGBB for a choice's chip (hat / body colour)
// The bro row this console's player wears in a room: his online pick (settings.json), else, when he has no online pick
// for that bro, the character-palettes mod's Custom colour (hal/fs_mods.cpp port_fs_mods_bro_custom). 1 when *rgb is set.
// Row 3, the undershirt, with none of its own set answers the hat's (the colour it wears).
int         port_bro_my_row(int chr, int row, unsigned *rgb);
}

#endif  // PORT_HAL_BRO_COLORS_H
