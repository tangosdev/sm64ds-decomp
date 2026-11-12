/* HOST-SIDE 16-WIDE per-player minimap storage, shared by the two host copies
 * of Minimap::Behavior and Minimap::Render.
 *
 * THE WALL. include/Minimap.h sizes the three PER-PLAYER minimap tables at
 * FOUR -- the cartridge's own player cap:
 *
 *     mPlayerIconX[4]  @ 0x070      the blip X of each player
 *     mPlayerIconY[4]  @ 0x080      the blip Y of each player
 *     mPlayerMapIDs[4] @ 0x21e      which sub-map each player is on
 *
 * Both src methods index these by PLAYER NUMBER. Minimap::Behavior seats the
 * local player at self->a70[data_0209f250] and every player in a `for i < 4`
 * loop; Minimap::Render draws the local blip from self->unk70[idx] /
 * OAM::MM_VS_PLAYER_ICONS[pl->unk8 + idx*4] and the others in its own `i < 4`
 * loop. On the DS idx and i are 0..3. This port hosts SIXTEEN players
 * (port/hal/vs_width.h), and a console seated on slot 8/12/14/15 walks the
 * self-icon index off the end of the four-entry tables and off the end of the
 * sixteen-entry OAM::MM_VS_PLAYER_ICONS table, hands OAM::RenderSub a garbage
 * attribute pointer and faults (c0000005) in Minimap::Render the frame the
 * minimap first draws. Measured on the unfixed base at seats 8/12/15.
 *
 * src/ is byte-frozen (the matching gate), and the tables live INSIDE the
 * 0x258-byte Minimap object, so they cannot be widened in the header without
 * moving every field after them and breaking the eight other matched Minimap
 * methods. The two methods that touch these three arrays are host copies
 * (port/unmatched/Minimap_Behavior.cpp / Minimap_Render.cpp, excluded from
 * slice_gate28.txt) that keep the storage HERE, sixteen wide, keyed by the
 * real slot. Nothing else in src/ or the HAL reads these three arrays -- the
 * only reader outside the two methods was map_render's SM64DS_MM_TRACE debug
 * line, updated to read g_mm_icon* too.
 *
 * WHY A GLOBAL IS SOUND. There is exactly one Minimap actor (id 0x14f), so a
 * process-global holds the same data the single instance's members did. The
 * arrays are zero-initialised and Behavior reseats every live slot each frame
 * before Render reads it, exactly as the object's own members were.
 */
#ifndef PORT_MINIMAP_WIDE_H
#define PORT_MINIMAP_WIDE_H

enum { kMinimapWidePlayers = 32 };   /* HUB32: == vs_width.h kPortBodySlots (VS still only fills 0..15) */

#ifdef __cplusplus
extern "C" {
#endif

extern int         g_mm_iconX[kMinimapWidePlayers];   /* was Minimap +0x070 */
extern int         g_mm_iconY[kMinimapWidePlayers];   /* was Minimap +0x080 */
extern signed char g_mm_mapID[kMinimapWidePlayers];   /* was Minimap +0x21e */

/* SMALLS9 / hal/coop_minimap.cpp installs this in a party / coop / world room.
 * Asked per non-local slot in the ADVENTURE (non-VS) minimap: 0 = not a hub
 * ghost (the ROM's own path, nothing drawn in adventure), 1 = a ghost drawn
 * this frame (pos[3] = its Fix12 world position, the follower's draw position,
 * because a ghost body is parked 200000 units up between draws), 2 = a hub
 * ghost that is not in this level, 3 = (MAPDOTS1) a ghost in another AREA of this level (pos[3] = his
 * newest synced position; the icon is clamped to the map's edge). NULL = never installed (solo, VS). */
extern int (*g_mm_ghost_pos)(int slot, int pos[3]);
extern int (*g_mm_coop_room)(void);   /* coopfix1 HOSTSTART1: the bug 25 guard asks this; 0 outside coop builds */

/* Both loops run the full sixteen slots and lean on the ROM's own per-slot
 * presence gates -- data_0209f394[i] != 0 in Behavior (absent slot -> mapID
 * -1), and `current-map == g_mm_mapID[i]` in Render (an absent slot's -1 never
 * matches a real map, so its actor pointer is never dereferenced). At four
 * players slots 4..15 are gated off and the output is byte-identical to the DS.
 */

#ifdef __cplusplus
}

/* The ROM icon tables OAM::MM_VS_PLAYER_ICONS and OAM::MM_VS_PLAYER_ICONS_S are
 * each 0x40 bytes = [4 slots][4 chars]. The blip index is slot*4 + char. Wrap
 * the SLOT component to 0..3 so slots 4..15 reuse a valid colour row instead of
 * indexing past the table. Identical for slots 0..3, so the four-player path is
 * byte-faithful; groundwork too for the per-slot minimap hat colour. */
static inline int mm_icon_slot(int slot) { return slot & 3; }
#endif

#endif
