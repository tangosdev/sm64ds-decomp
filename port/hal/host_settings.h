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

/* ---- CameraMode: WHICH CAMERA AN INTERACTIVE RUN BOOTS INTO -------------
   "analog" (default) | "freecam" | "ds", the three modes tests/walk_window.cpp
   names CAM_ANALOG / CAM_FREE / CAM_DS. Returned as that numbering: 0 analog,
   1 freecam, 2 ds. The default is analog because that is what main has always
   promoted an interactive run to once the Camera actor is up, so a
   settings.json without the key boots exactly the program that shipped before
   the key existed. A SELFTEST IGNORES IT and stays DS-exact, for the reason
   the RunMode pin gives: a comparator run must not depend on a preferences
   file. SM64DS_ANALOG_CAMERA / SM64DS_DS_CAMERA / SM64DS_FREECAM still win
   over the file, because an environment knob is a per-run request and the
   file is a standing one. The debug menu's camera row writes the key back
   through host_setting_save_camera_mode, the way the run row writes RunMode.
   Boot-latched: F1 and the row move the live mode, the file moves the next
   boot. */
int host_setting_camera_mode(void);
int host_setting_save_camera_mode(int mode);

/* ---- THE CONTROL BINDINGS, ONE KEY PER ACTION ----------------------------
   Why they are here at all: the F5 debug menu could rebind exactly one thing
   (the run button) and only in a window a person was sitting at, and a lobby
   match boots straight into VS with no menu on the way. settings.json is read
   at boot by every copy of the game the launcher starts, single-player and
   lobby alike (the launcher runs both out of the bundle directory, and the
   file sits beside the exe), so a binding here reaches a match.

   KEYBOARD, Win32 virtual-key codes (the number GetAsyncKeyState takes; 'W'
   is 0x57, VK_SPACE is 0x20). 0 means that action has no keyboard binding.
   Defaults are the bindings this program has always hardcoded:

     KeyUp        0x57 W        KeyUpAlt     0x26 up arrow
     KeyDown      0x53 S        KeyDownAlt   0x28 down arrow
     KeyLeft      0x41 A        KeyLeftAlt   0x25 left arrow
     KeyRight     0x44 D        KeyRightAlt  0x27 right arrow
     KeyJump      0x20 space
     KeyAttack    0x58 X        (punch / kick / Yoshi's tongue: the DS B button)
     KeyCrouch    0x11 ctrl     (the DS R shoulder)
     KeyRun       0x10 shift    (the port's own run button; see RunMode)
     KeyStart     0x0d enter    (scene path only: the minigames' own menus)
     KeySelect    0x08 backspace (scene path only, keyboard only on the DS
                                 side too -- every free pad button is spoken
                                 for, see the scene loop)

   The four *Alt keys exist so the shipped "WASD and the arrows both walk"
   default is a fact the launcher can show and change rather than a rule
   hidden in the game. Either half of a pair may be 0.

   PAD, XInput button masks (XINPUT_GAMEPAD_*: A 0x1000, B 0x2000, X 0x4000,
   Y 0x8000, LB 0x0100, RB 0x0200, START 0x0010, BACK 0x0020, the d-pad
   0x0001..0x0008, the stick clicks 0x0040/0x0080). 0 means no pad binding.
   Defaults are again what the window has always done:

     PadJump      0x1000 A
     PadAttack    0x2000 B
     PadCrouch    0x20000 RT -- the right trigger, which has always crouched.
                            THE TRIGGERS ARE NOT XINPUT BUTTONS: they are 0..255
                            axes with no mask, so the file names them with two
                            bits above the XInput word, HOST_PAD_LT 0x10000 and
                            HOST_PAD_RT 0x20000, and a pull past 100 reads as
                            the button being down. A PadCrouch of 0 (the value
                            every launcher before 0.3.4 wrote) still crouches
                            on RT, so an older file keeps the trigger.
     PadRun       0x4000 X
     PadStart     0x0010 START
     PadSelect    0      -- BACK opens the debug menu, so Select has no pad
                            default; bind it here if the menu is not wanted
                            on BACK.

   The left stick and the d-pad walk, and the right stick, the bumpers and
   the right-stick click drive the camera. Those are not bindings in this
   file; they are the pad's shape.

   RunButtonKey AND RunButtonPad, the two names that existed before this
   block, KEEP WORKING AS ALIASES of KeyRun and PadRun. When both spellings
   are in the file KeyRun / PadRun win; when only the old name is there it is
   read exactly as before; the save path writes BOTH spellings so a launcher
   of either vintage reads the choice back. host_setting_run_key and
   host_setting_run_pad return the same answers as host_setting_key(HOST_KEY_RUN)
   and host_setting_pad(HOST_PAD_RUN).

   A value outside the code space (keys 0..0xff, pads 0..0xffff) is a typo,
   not a choice, and reads as the default. A fractional value such as 3.9
   truncates to 3 (json_int reads the leading integer, the behaviour every
   integer key in this file has always had) rather than defaulting. Two actions on one key is legal
   and means both happen; nothing here second-guesses it. The debug menu's
   own navigation -- the arrows, enter, escape, F5, the d-pad, A, B and BACK
   -- is fixed and is not in this table.

   Boot-latched, like RunMode: the launcher's dialog promises the restart. */
enum {
    HOST_KEY_UP = 0, HOST_KEY_DOWN, HOST_KEY_LEFT, HOST_KEY_RIGHT,
    HOST_KEY_UP_ALT, HOST_KEY_DOWN_ALT, HOST_KEY_LEFT_ALT, HOST_KEY_RIGHT_ALT,
    HOST_KEY_JUMP, HOST_KEY_ATTACK, HOST_KEY_CROUCH, HOST_KEY_RUN,
    HOST_KEY_START, HOST_KEY_SELECT,
    HOST_KEY_COUNT
};
enum {
    HOST_PAD_JUMP = 0, HOST_PAD_ATTACK, HOST_PAD_CROUCH, HOST_PAD_RUN,
    HOST_PAD_START, HOST_PAD_SELECT,
    HOST_PAD_COUNT
};
/* The two trigger pseudo-buttons a pad binding may name (see the PAD block
   above), and the widest value a pad binding may hold. */
enum { HOST_PAD_LT = 0x10000, HOST_PAD_RT = 0x20000, HOST_PAD_MASK_MAX = 0x3ffff };
/* The bound code for one action, or 0 for unbound; an index outside the enum
   is 0 too. host_setting_key_name / host_setting_pad_name give the
   settings.json spelling of an action ("KeyJump", "PadJump"), for logs. */
int host_setting_key(int action);
int host_setting_pad(int action);
const char *host_setting_key_name(int action);
const char *host_setting_pad_name(int action);

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

/* ---- NameTags: WHO THAT IS, AND HOW THEY ARE DOING ----------------------
   Default 1, and the default is ON, which is the one key here that is on by
   default and needs its reason stated.

   In an online VS match every other body on screen is a stranger, and the
   cartridge has nothing to say about which one is which: the DS's own VS mode
   was four people in a room who could look up. The port's is not, so each
   REMOTE player carries a tag over his head with the lobby nickname the room
   gave that seat and, above it, that seat's carried-star count.

   IT IS A HOST OVERLAY AND NOT A MOD, which is why it does not ride the Mods
   panel's rules. It draws into the framebuffer after the frame is composed,
   reads game state and writes none of it, and runs only while data_0209f2d8
   says a VS match. Nothing about the simulation changes with it on or off,
   which is the line GaplessMinigames and LovesMeCharacter are on the other
   side of.

   THE GLYPHS ARE THE CARTRIDGE'S. The tag is drawn with the ROM's own message
   font, decoded out of the tiles LoadFont3D uploaded, not with the debug
   overlay's ASCII font -- see tests/nametag.h for the decode and for what the
   font does and does not contain.

   Reloads live, like the gap keys and MouseCapture. SM64DS_NAME_TAGS=0 forces
   it off for a proof run and any other value forces it on; unset is the
   file's answer. */
int host_setting_name_tags(void);

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

/* ---- THE PER-CHARACTER PALETTE PICKER ----------------------------------
   Four keys, one per playable character: PaletteMario, PaletteLuigi,
   PaletteWario, PaletteYoshi. Absent or "" is the ROM's colors, and the
   ROM is the default, exactly like every other Mods key.

   WHAT A VALUE NAMES. Any other value is the BASENAME of a palette file,
   palettes/<value>.pal, looked for in the same three places a combo file
   is: beside the exe, then the asset root, then the working directory.
   The file is a palmod v2 blob, the same format CustomPalette reads, so
   one file can carry records for several characters and the Studio needs
   no second compiler.

   ONLY THAT CHARACTER'S RECORDS ARE APPLIED. A .pal file names target
   files by path; a key applies only the records whose target is one of
   THAT character's own player files -- body, head with cap, head without
   cap, and for Luigi the two texture-swap files his body and capped head
   are driven by. Records aimed at anybody else in the same file are
   skipped in silence, which is what lets one authored file serve four
   keys with four different answers. Applied records keep every rule
   CustomPalette already has: the palette is found by ITS OWN name in the
   served file's table, the color count must match exactly, and any
   disagreement refuses out loud and serves the ROM's colors.

   PaletteYoshi ALSO TAKES FOUR BUILT-IN VALUES, "yoshi:green",
   "yoshi:red", "yoshi:blue" and "yoshi:yellow", which need no file at
   all: they are the ROM's own four VS colors, taken out of the player's
   extraction. Yoshi's body palette is four stacked 16-color rows and
   adventure mode renders row 0; the built-ins put the row VS would have
   selected where row 0 is. "yoshi:green" IS row 0, so it is the ROM.
   hal/fs_mods.cpp carries the derivation, including why the head files
   get the same row rather than being left alone.

   BOOT-LATCHED, like every Mods key, because the file layer caches what
   it serves. Each active key says one plain-words line on stderr.

   THEY REPLACE CustomPalette WHEN SET. The old single-combo key still
   works on its own, but a file that sets even one of these four turns it
   off entirely, with one line saying so -- two mods writing colors into
   the same palettes would make what a player sees depend on which ran
   last, and no setting should mean that.

   character is 0 Mario, 1 Luigi, 2 Wario, 3 Yoshi. The accessor never
   returns null; "" is the ROM.

   host_setting_yoshi_builtin_row is 0..3 when PaletteYoshi named one of the
   built-in colors, in the order the rows are stacked (green, red, blue,
   yellow), and -1 when the key names a file instead. The four spellings are
   owned by host_settings.cpp, the same way LovesMeCharacter's are, so the
   word-to-index step and the mechanism that consumes the index live in one
   file each and cannot drift. A "yoshi:" value this build does not know is
   a typo -- no file system here allows a colon in a name -- so it reads as
   the ROM and says so once. */
const char *host_setting_character_palette(int character);
int host_setting_character_palette_any(void);
int host_setting_yoshi_builtin_row(void);

/* ---- PadLayouts: A LEARNED BUTTON MAP FOR ONE NON-XINPUT CONTROLLER ------
   The DirectInput backend (hal/pad_backend.cpp) maps a pad's raw button
   indices and axes onto the XInput mask through a built-in table keyed by
   the pad's USB vendor:product id, with a positional guess for anything the
   table does not know. A player reported the guess wrong on their pad: only
   the d-pad worked and the face buttons came out rotated. The fix is a map
   the player teaches the game one press at a time (the debug menu's "pad
   layout" row), and this is where that map is kept between runs.

   THE FILE SHAPE. One optional key, an array of flat objects:

     "PadLayouts": [
       { "vid": 1118, "pid": 654, "name": "USB Gamepad",
         "a": 2, "b": 1, "x": 3, "y": 0, "lb": 4, "rb": 5,
         "back": 8, "start": 9, "lthumb": 10, "rthumb": 11,
         "lt_btn": 6, "rt_btn": 7, "lt_axis": -1, "rt_axis": -1,
         "lx_axis": 0, "ly_axis": 1, "rx_axis": 2, "ry_axis": 5,
         "lx_sign": 1, "ly_sign": -1, "rx_sign": 1, "ry_sign": -1 }
     ]

   The field names are pad_backend.cpp's PadLayout, so the two cannot drift.
   vid and pid are the USB ids in DECIMAL (the file's integer reader reads
   decimal). Button fields are DirectInput button indices 0..31, -1 for "this
   pad has no such button". Axis fields are 0 X, 1 Y, 2 Z, 3 Rx, 4 Ry, 5 Rz,
   -1 for none. The four sign fields are 1 or -1 and say which way the raw
   axis moves for a rightward (or upward) push, because DirectInput's Y grows
   downward and a learned pad may wire any axis either way. name is for a
   person reading the file; the game ignores it.

   LENIENT LIKE EVERY OTHER KEY. An object whose vid or pid is missing or out
   of 1..65535 is dropped. Any other field that is missing or out of range
   reads as the generic fallback's value for that field, so a half-edited
   object still gives a usable pad rather than none. At most
   HOST_PAD_LAYOUT_MAX objects are kept; later ones are ignored. A file with no
   PadLayouts key reads exactly as before the key existed.

   THE SAVE keeps the whole array: host_setting_save_pad_layout replaces the
   object with the same vid:pid or appends one, then rewrites the PadLayouts
   value in place and carries every other key of the file untouched, the way
   the run and camera saves do. Boot-latched for the file's purposes, but the
   pad backend is told the new map directly by the caller so the pad answers
   with it at once; a restart re-reads it from here. */
struct HostPadLayout {
    int vid, pid;
    int a, b, x, y, lb, rb, back, start, lthumb, rthumb;
    int lt_btn, rt_btn, lt_axis, rt_axis;
    int lx_axis, ly_axis, rx_axis, ry_axis;
    int lx_sign, ly_sign, rx_sign, ry_sign;
    char name[64];
};
enum { HOST_PAD_LAYOUT_MAX = 8 };
/* The generic fallback's values into *out (vid and pid 0, name ""), the
   same row pad_backend.cpp uses for an unknown pad. */
void host_pad_layout_default(struct HostPadLayout *out);
/* 1 and *out filled when the file names a layout for vid:pid, else 0. */
int host_setting_pad_layout(int vid, int pid, struct HostPadLayout *out);
/* How many layouts the file carries, and the i'th of them (1, or 0 when i
   is out of range), so a caller can hand all of them to the pad backend. */
int host_setting_pad_layout_count(void);
int host_setting_pad_layout_at(int i, struct HostPadLayout *out);
/* Upsert by vid:pid and persist. Returns 1 when the file on disk now says
   so; the in-memory table moves either way. Fields out of range are
   clamped to the fallback's value before anything is written. Refuses
   (returns 0, nothing moves) when vid or pid is 0 or the table is full of
   other pads. */
int host_setting_save_pad_layout(const struct HostPadLayout *layout);

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
