/* Host player settings, read from settings.json in the bundle directory.
   This is the launcher's file: its Settings dialog writes it, the game reads
   it. It holds host preferences only, never anything about the game itself.

   Not an environment variable. For launcher-driven runs the launcher builds
   the child environment itself, so anything a player exported is silently
   overridden and never reaches the game. The file next to the exe is the
   only channel that works.

   Read once, on first use. A change takes effect the next time the player
   presses Play, which is how the volume setting already behaves. A missing,
   unreadable or malformed file falls back to the defaults in silence, the
   same way the launcher's own loader does. */
#ifndef PORT_HOST_SETTINGS_H
#define PORT_HOST_SETTINGS_H

#ifdef __cplusplus
extern "C" {
#endif

/* SwapCameraTurnDirection: 1 when the player turned it on. Default 0. */
int host_setting_swap_camera_turn(void);

/* ---- WHICH WAY THE CAMERA TURNS ---------------------------------------
   The signed step a RIGHTWARD push of a camera control makes to the
   camera's heading, the angle from Mario to the eye that the Camera actor
   keeps at +0x17c.

   There is no hardware answer to copy here. The DS turns its camera with
   the L and R shoulder buttons and has no E key, no Q key, no right stick
   and no mouse, so every host binding this program has is the port's own
   choice and always was. The rotation MATH is the ROM's and is byte-gated
   in src/func_02009e70.cpp; only the question of which host control feeds
   which of its two directions is ours.

   MEASURED, so the default is not a guess. Walking Mario with the camera
   left alone, his facing minus the published angle-to-camera is exactly
   0x8000 on every forward frame, and holding LEFT drives that offset
   strongly positive while the camera's own heading climbs monotonically as
   it swings in behind him. A RISING heading is therefore the view panning
   LEFT. Before this setting existed every rightward control on the host
   raised it, so pushing right panned the view left. That is the modern
   third person convention backwards, and a player reported it as exactly
   that.

   So the default is -1: push right, pan right. The launcher's "Swap left
   and right camera turning" returns +1, which is what this program did
   before, for players who prefer it or who got used to it.

   Every horizontal camera control multiplies by this one value, so the
   keyboard, the stick, the bumpers, the mouse and all three camera modes
   cannot drift out of step with each other. */
int host_camera_turn_sign(void);

/* ---- THE DS SCREEN GAP ------------------------------------------------
   Four optional keys, each with its own default, so a settings.json written
   before any of them existed reads exactly as one that sets all four to
   their defaults.

   MinigameGap    1 (default): the hinge between the DS's two screens is
                  simulated, which is what the game's own code already does
                  and what makes an object crossing between the screens move
                  at one speed. 0 pulls the halves together for LAYOUT
                  purposes only -- the simulation is untouched, so objects go
                  back to jumping at the seam. The launcher's checkbox is
                  "remove minigame gap" and is the inverse of this key.
   GapFillMode    1 ambient (default), 0 solid.
   GapColor       0xFFRRGGBB for the solid fill. Default 0xFF000000.
   GapPeek        1 draws the band's own hidden sprites over the fill.
                  Default 0. */
int host_setting_minigame_gap(void);
int host_setting_gap_fill_mode(void);
unsigned host_setting_gap_color(void);
int host_setting_gap_peek(void);

#ifdef __cplusplus
}
#endif

#endif /* PORT_HOST_SETTINGS_H */
