/* The gap band's per-scene continuity table. See hal/gap_continuity.cpp for
   what it is and ntr/ppu.h's BandTrack note for the mechanism it feeds. */
#ifndef PORT_GAP_CONTINUITY_H
#define PORT_GAP_CONTINUITY_H

/* Point the band's continuity hook at this scene's reader, or clear it when the
   scene has none. Called from hal/screen_gap.cpp at every latch -- which is
   every time the scene or its G changes -- because installing is also what
   clears the cached OAM attributes, and a cached entry must never outlive the
   scene whose OAM it was read out of. */
void hal_band_continuity_latch(int scene_id);

#endif /* PORT_GAP_CONTINUITY_H */
