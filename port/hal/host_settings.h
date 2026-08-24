/* Host player settings, read from settings.json in the bundle directory.
   This is the launcher's file: its Settings dialog writes it, the game reads
   it. It holds host preferences only, never anything about the game itself.

   Not an environment variable. For launcher-driven runs the launcher builds
   the child environment itself, so anything a player exported is silently
   overridden and never reaches the game. The file next to the exe is the
   only channel that works.

   Read once, on first use, and then WATCHED: host_settings_poll below
   re-reads the file when it changes on disk, so the launcher's dialog can
   adjust the gap and the volume while the game is running. Only the four
   screen-gap keys, Volume and MouseCapture reload live; everything else keeps
   its boot value, because the dialog's own Mods panel promises a restart for
   those.
   A missing, unreadable or malformed file falls back to the defaults in
   silence at boot, and a reload that cannot read the file keeps the values
   it has rather than falling back -- the launcher swaps the file in with a
   rename, but a reader that punished a torn write with the defaults would
   flash the player's picture black for a frame. */
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
   GapFillMode    1 ambient (default), 0 solid, 2 the scene's own hand-drawn
                  band, gap/scene_<id>.bmp. Ignored when GapPeek is 1 -- except
                  that the art is peek's backdrop either way; see below.
   GapColor       0xFFRRGGBB for the solid fill. Default 0xFF000000. Ignored
                  when GapPeek is 1.
   GapPeek        1 draws the band's own hidden sprites over the scene's own
                  art if there is any and over PLAIN BLACK if there is not.
                  Default 0.

   PEEK IGNORES THE FILL MODE. With peek off the band is decoration and the two
   fill keys are the player's taste in it; with peek on the band is a view of
   what is really in those rows, and the true state of a row with no sprite in
   it is empty. Neither fill runs behind that, because a wash blurs into the
   sprites and a flat colour cannot be told from one. The scene's hand-drawn
   art DOES, because a picture is read as a picture: peek's backdrop is
   gap/scene_<id>.bmp when that file is there and black when it is not, and
   everything over the backdrop is still only what the game submitted.
   hal/host_settings.cpp and hal/gap_art.h carry the long form. */
int host_setting_minigame_gap(void);
int host_setting_gap_fill_mode(void);
unsigned host_setting_gap_color(void);
int host_setting_gap_peek(void);

/* ---- GaplessMinigames: THE ONE KEY THAT IS A MOD -----------------------
   Default 0, and the default is the only setting that is the ROM. This is
   the game half of the launcher's Mods section, where its label reads "use
   gapless version of minigame when able".

   The four keys above are about the PICTURE. Every one of them leaves the
   game's own G alone, so the simulation runs exactly as the DS runs it and
   the argument between them is only how many rows of image sit between the
   halves and what is drawn in them.

   This one is not that. With it on, a SUPPORTED minigame's G is set to zero
   in the game's own word once its InitResources has finished, so the ROM's
   OAM router puts the two screens edge to edge in the SIMULATION: a thing
   crossing the seam leaves the bottom screen's top row and arrives on the
   top screen's bottom row on the very next frame, with no hidden band and
   no rows of travel behind plastic.

   THAT IS A DIFFERENT GAME, and the difference is measurable rather than
   cosmetic. A crossing loses the G rows it used to spend in the hinge --
   32 of them in Bob-omb Squad, three frames at the ball's own speed -- so a
   shot arrives sooner than it does on hardware, and the top 32 world rows
   the top screen used to show are pushed off the top of it. The port's
   north star is that the port BE the decomp, so this ships off and stays
   off unless a player asks for it by name.

   "WHEN ABLE" IS A TABLE, NOT A HOPE. It applies only to a minigame whose G
   consumers have actually been enumerated and whose gapless behaviour has
   been proven, and every other minigame keeps its full gap simulation with
   this key on. The table and the audit behind its one row live in
   hal/screen_gap.cpp, and every minigame launch says on stderr which of the
   three it got: engaged, unsupported, or off.

   IT MOOTS THE OTHER FOUR for the scene it engages on, and not by a rule
   written here: it works by zeroing the same word the layout reads, so
   there is no band, and a band that does not exist cannot be filled, drawn
   on, or peeked into. */
int host_setting_gapless_minigames(void);

/* ---- LovesMeCharacter: the second Mods key ------------------------------
   Default 0, and the default is the only setting that is the ROM. Which
   character stars in the Loves Me...? petal minigame: 0 is Yoshi, 1 is the
   minigame archive's own Mario model, asked for by the file's "mario".

   Unlike GaplessMinigames this one changes only what the scene LOOKS like,
   but it is a mod all the same -- the ROM never shows anyone but Yoshi
   there -- so it rides the same rules: default off, boot-latched (the file
   layer caches what it serves, so the launcher's Mods panel promises a
   restart), and announced in plain words on stderr when on. Returns 0 for
   Yoshi/off, else a 1-based index into host_settings.cpp's LOVESME_KEY
   spellings (mario, the three VS Yoshi colors, luigi, wario, metal_wario).
   The mechanism is a file-layer substitution owned by hal/fs_mods.cpp;
   that file's header carries the derivation that makes the swaps safe. */
int host_setting_lovesme_character(void);

/* ---- MouseCapture: HOLD THE POINTER AND STEER WITH IT --------------------
   Default 0, which is exactly what this program has always done: the mouse is
   a stylus, and the camera only follows it while the RIGHT BUTTON IS HELD (see
   the MOUSE banner in tests/walk_window.cpp). With this key on, an ordinary
   adventure window HOLDS the pointer -- hidden, pinned to the middle of the
   picture and clipped to the window -- and bare mouse movement turns the
   camera with no button at all, the way a modern third-person game does.

   THIS IS A HOST PREFERENCE AND NOT A MOD. Nothing about the game changes:
   the deltas land on the same two variables the right-button drag has always
   fed, at the same 48-and-24 binangs a pixel, through the same
   host_camera_turn_sign. With the key off not one win32 call is made and the
   two extra tests read false, so off is the old program.

   WHERE IT DOES NOT ENGAGE, and every one of these is a pointer the player
   would otherwise have to go and find:
     * while the debug menu is open -- which is what makes ESCAPE the release,
       since escape opens the menu;
     * while the window is not the foreground one, so alt-tab hands the
       pointer back and coming back takes it again;
     * on the SCENE path at all (the minigames), where the mouse IS the
       stylus and taking it would take the game;
     * in a STACKED window, where the bottom half of the picture is a live
       touchscreen for the same reason;
     * in the DS-EXACT camera mode, where the mouse does not steer anything,
       so holding the pointer would cost the player something and buy nothing;
     * while the run-button rebind row is capturing;
     * in a selftest, which has no pointer and no player.

   It reloads live like the gap and the volume, so the launcher's dialog can
   turn it on and off with the game already running. */
int host_setting_mouse_capture(void);

/* ---- CustomPalette: the third Mods key ---------------------------------
   Default 0, and the default is the only setting that is the ROM. 1..3
   pick a palette combo file, palettes/combo<N>.pal in the same folder
   settings.json is looked for, whose color sets are written into the
   playable characters' model palettes as their files load. The key picks
   the FILE and nothing else: every target file name, palette name and
   color comes out of the combo file, so newer combos need no new build.
   Out-of-range
   values read as 0. Boot-latched (the file layer caches what it serves,
   so the launcher's Mods panel promises a restart), announced in plain
   words on stderr when on. The file format and the patching live in
   hal/fs_mods.cpp. */
int host_setting_custom_palette(void);

/* ---- THE LIVE RE-READ -------------------------------------------------
   host_settings_poll: call once per frame from the host loop. Internally it
   looks at the file's write time only every 30th call, so its steady-state
   cost is a counter compare. When the file HAS changed it re-reads the four
   screen-gap keys, Volume and MouseCapture, and returns 1 exactly when one of
   them now answers differently; every other return is 0. Nothing else
   reloads: the Mods panel tells the player those need a restart, and making
   the file watcher agree with the dialog is the whole contract.

   host_settings_gen steps once per poll that returned 1. hal/screen_gap.cpp
   folds it into its layout latch key, which is how a fill-mode or colour
   change rebuilds a layout whose every other input is unchanged.

   host_setting_volume is the file's Volume key, 0..100, or -1 when the file
   has never named one. -1 means the boot-time SM64DS_VOLUME environment
   value (the launcher passes the same number both ways) stays in charge. */
int host_settings_poll(void);
int host_settings_gen(void);
int host_setting_volume(void);

#ifdef __cplusplus
}
#endif

#endif /* PORT_HOST_SETTINGS_H */
