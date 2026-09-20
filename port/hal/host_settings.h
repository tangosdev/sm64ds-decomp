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
   screen-gap keys, Volume, MouseCapture and the five VoiceChat keys reload
   live; everything else keeps its boot value, because the dialog's own Mods
   panel promises a restart for those.
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
   "analog" | "freecam" | "ds" (default), the three modes tests/walk_window.cpp
   names CAM_ANALOG / CAM_FREE / CAM_DS. Returned as that numbering: 0 analog,
   1 freecam, 2 ds. The default is ds, on Tango's order, because that is the
   cartridge's own stepped rotate and it is the mode the bumpers turn in;
   analog and freecam stay one F1 press (or one menu row) away. A SELFTEST
   IGNORES IT and stays DS-exact, for the reason the RunMode pin gives: a
   comparator run must not depend on a preferences file. SM64DS_ANALOG_CAMERA
   / SM64DS_DS_CAMERA / SM64DS_FREECAM still win over the file, because an
   environment knob is a per-run request and the file is a standing one. The
   debug menu's camera row writes the key back through
   host_setting_save_camera_mode, the way the run row writes RunMode.
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

/* ---- AdventureGhosts: PLAY THE SINGLE-PLAYER ADVENTURE TOGETHER ----------
   Default 0, and 0 is the ROM: an ordinary solo game with nobody else in it.
   With it on the game is still a solo game -- your own level, your own single
   local player -- but it opens the comms carrier and the state-sync aux
   channel and draws every OTHER player who is in the same level as a
   see-through, walk-through ghost with a name tag. No shared world, no
   collision, no interference; the only thing shared is "here is my body,
   drawn faintly in your world" (status/ADVENTURE.md).

   IT IS A MODE, NOT A PRESENTATION PREFERENCE, which is why the default is OFF
   where NameTags' is ON. NameTags decides how an already-online body is
   labelled; this decides whether the session is an adventure-ghost one at all.
   It is NOT the VS flag data_0209f2d8==1: several draw and collide gates key
   off VS mode, so ghost mode is its own scope and VS mode is byte-unaffected
   by it.

   Read like NameTags in every other respect: reloads live, and SM64DS_ADVENTURE
   overrides the file (0 forces it off, any other value forces it on; unset is
   the file's answer). */
int host_setting_adventure_ghosts(void);

/* Aspect: the presentation ratio, WIDTH DIVIDED BY HEIGHT. 0 (the default) is
   the explicit native sentinel -- the 4:3 window the port has always opened;
   1.7777778 is 16:9. A NUMBER rather than a Widescreen boolean because a
   boolean cannot express ultrawide, and a ratio means an odd monitor needs no
   new mode name. Absent, unparseable, negative or otherwise not a positive
   number all read as 0; a positive value is CLAMPED into [1.0, 4.0], so 9.0 is
   4.0 and not an error. The ceiling is 4.0 rather than 3.0 so that 32:9
   (3.5555556) survives the sanitiser as itself instead of being letterboxed to
   3.0; aspect_sanitise in the .cpp carries why 4.0 is the right stop.

   BOOT-LATCHED, unlike the keys around it: the aspect is chosen once and
   threaded into the framebuffer, so a mid-run reload cannot move it.
   walk_window's boot is the only reader and hands the answer to
   ntr::configure_aspect. SM64DS_ASPECT overrides the file with a ratio;
   SM64DS_WIDESCREEN still works as the legacy boolean (0 native, anything else
   16:9) and is consulted only when SM64DS_ASPECT is unset. */
double host_setting_aspect(void);

/* ---- VsLuigiInfection: the Luigi Infection VS MODE, launcher toggle ------
   Default 0, and 0 is a normal VS match, byte-identical to a build without
   the key. Luigi Infection is a not-ROM-accurate VS mode (one player starts
   as a hidden Luigi and tags survivors, who convert and join his team until
   the clock runs out), so it rides the same rule every mode here does: default
   OFF, reachable only when a player asks for it by name.

   THE TWO HALVES AND THE ONE CHANNEL BETWEEN THEM. The mode's logic is the
   game-side module hal/luigi_infection.cpp (its own lane), which arms off the
   environment variable SM64DS_VS_LUIGI_INFECTION and reads SM64DS_VS_LUIGI_TIME
   for an optional test clock. This key is the LAUNCHER'S channel to that flag:
   the launcher's Settings dialog writes "VsLuigiInfection" (and optionally
   "VsLuigiInfectionSeconds") to settings.json, and host_settings BRIDGES the
   file to the environment -- when the key is on and the player has not already
   exported the variable, load_once publishes SM64DS_VS_LUIGI_INFECTION=1 (and
   SM64DS_VS_LUIGI_TIME from the seconds key) so the game side reads ONE channel
   and the file is one way of writing it. This is the Aspect contract exactly:
   the environment wins, so SM64DS_VS_LUIGI_INFECTION set for a proof run beats
   the file, and an unset environment is the file's answer.

   host_setting_vs_luigi_infection returns 1 when the mode is on, else 0, with
   the same env override in front: SM64DS_VS_LUIGI_INFECTION unset is the file's
   answer, empty or "0" forces it off, any other value forces it on (the grammar
   hal/luigi_infection.cpp's own reader uses).
   host_setting_vs_luigi_infection_seconds is the optional test clock in whole
   seconds; 0 (the default, and an absent or non-positive key) means the ROM's
   own match clock, the faithful one. SM64DS_VS_LUIGI_TIME overrides the file
   the same way. */
int host_setting_vs_luigi_infection(void);
int host_setting_vs_luigi_infection_seconds(void);

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

/* ---- PROXIMITY VOICE CHAT, lane VOICE ----------------------------------
   Six keys, and every one of them RELOADS LIVE -- they are host preferences
   with no cached artefact behind them, so the Mods panel's restart promise
   does not apply and the launcher's toggle is expected to work mid-match.

   VoiceEnabled     bool, default false. The master switch, and OFF MEANS NO
                    RECORDING DEVICE IS OPENED. Not opened and muted, not
                    opened and discarded: hal/voice_chat.cpp asks this before
                    it touches winmm at all, and turning the key off while the
                    game is running closes the device it had.
   VoiceMicDevice   string, default "". "" (and the literal "auto") is
                    AUTO-PICK: hal/voice_capture_win.cpp opens each recording
                    device in turn for a fraction of a second, measures the
                    peak sample, and takes the first one that is actually
                    producing audio. That is the default because a device that
                    opens fine and captures nothing -- Windows-muted, privacy
                    blocked, or a live headset mic sitting behind the wrong
                    Windows default -- is indistinguishable from a working one
                    until something records from it, and what the player sees
                    is a silent channel with no error anywhere. The literal
                    "default" (or "system") forces the Windows default device
                    with no scan. Any other value is matched case-insensitively
                    as a SUBSTRING against the names winmm reports, first match
                    wins, and the scan is skipped; no match falls back to the
                    default with one line on stderr. Substring because winmm
                    truncates device names to 31 characters and a launcher
                    listing them cannot always show the whole name. A value
                    longer than the buffer reads as "" -- the same "a typo is
                    not a choice" rule the palette keys follow.
   VoiceMicIndex    int, default -1. -1 means "no index given", which is what
                    leaves VoiceMicDevice in charge. 0 or above is a winmm
                    device id used VERBATIM: no scan, no name match, and no
                    quiet fall back to the default if it will not open. It is
                    the escape hatch for a machine with two devices whose
                    truncated 31-character names are identical, which a
                    substring match cannot tell apart, and it OUTRANKS
                    VoiceMicDevice when both are set. An id past the end of the
                    machine's device list fails the open like any other bad
                    device and is latched, not retried sixty times a second.
   VoiceVolume      int 0..100, default 80. A linear gain on decoded remote
                    audio, on top of the distance falloff. INDEPENDENT of the
                    game's Volume key, and the voice mix runs after the master
                    trim so that stays true: muting the game does not mute
                    the people you are playing with.
   VoiceNearRadius  int world units, default 512. Full volume inside this.
   VoiceFarRadius   int world units, default 3072. Silence at and beyond it,
                    with a log falloff between the two. far <= near is not a
                    choice anybody made, so BOTH go back to their defaults.

   The radii are the game's own world units -- the integer part of the Fix12
   position at Actor+0x5c. port/status/VOICE.md carries the arena measurement
   the defaults come from. host_setting_voice_mic_device never returns null. */
int host_setting_voice_enabled(void);
int host_setting_voice_volume(void);
const char *host_setting_voice_mic_device(void);
int host_setting_voice_mic_index(void);
int host_setting_voice_near_radius(void);
int host_setting_voice_far_radius(void);

/* NetMode: 0 lockstep, 1 rollback. LOCKSTEP IS THE DEFAULT (0.3.6 hotfix,
   2026-09-03): an absent settings.json, one that will not parse, or one
   without the key all read as lockstep; "rollback" is still a choice.
   Rollback shipped 0.3.4 as the default but re-simulates the world every
   frame a remote peer moves (the prediction carries only the heading), which
   halves the effective tick rate; lockstep never predicts, so it never
   re-sims. The comms transport reads it once at install; SM64DS_NETMODE
   overrides it there. See status/ROLLBACK_SHIP.md. */
int host_setting_net_mode(void);

/* THE WIDTH ROLLBACK IS PROVEN TO. Determinism (the DET rung) and cost (the
   COST rung) are measured up to eight seated players; past that the wide
   lane's Minimap::Render faults at frame 2 on player numbers 8 and up, in
   lockstep as much as in rollback, so nothing past eight has been shown to
   hold. A seated session wider than this runs lockstep whatever NetMode
   says, and the transport says so on one line at install. Lives here, next
   to the NetMode parse, so the number and the default are read together. */
enum { kRollbackMaxPlayers = 8 };

/* ---- FrameRate: HOW MANY PICTURES A SECOND. PRESENTATION ONLY. ----------
   Default 0, and 0 is the ROM: one picture per game tick, exactly what the
   port has always drawn. A NUMBER rather than a boolean for the Aspect
   reason -- 60, 90, 120 and 144 are all real monitors and a boolean cannot
   name them.

   IT DOES NOT MOVE THE GAME'S CLOCK, and that is the whole point. The game
   owns its own rate and writes it down: data_0208ee44 is vblanks per tick
   (1 = 60, 2 = 30, 3 = 20) and every scene sets it in its own InitResources,
   so the 3D levels tick 30 times a second and the minigames 60 whatever this
   key says. What the key buys is EVEN HOLD TIMES. The port presents through a
   plain StretchDIBits with no vsync anywhere, so a 30 Hz picture on a 144 Hz
   display is held 5, 5, 5, 5, 4 refreshes, and the eye reads that unevenness
   as judder on top of the 30.

   ACCEPTED: 0 for native, the string "display" for the primary display's
   refresh rate read once at boot, or a whole number of pictures a second
   CLAMPED TO 240. Absent, unparseable, negative, and any positive value
   BELOW 60 all read as 0. 60 is the floor because 60 is the fastest the
   ROM's own clock ever runs (divider 1, every minigame), so a smaller number
   would ask the port to present less often than the game ticks, which a
   presentation layer may not do. Above 240 is CLAMPED rather than rejected,
   the Aspect rule again: 1000 becomes 240, which is a picture, where
   rejecting it would be a surprise.

   BOOT-LATCHED, like Aspect and CustomPalette: the presentation clock's shape
   is decided once, at the pacer's first turn, and a mid-run flip would be a
   second code path nobody tests. The launcher's Settings row promises a
   restart. SM64DS_FRAME_RATE overrides the file with the same grammar,
   "display" included; an unparseable override is still an override and says
   native, which is the answer an unparseable file value gives.

   WHAT IT LEAVES ALONE, and this is the contract the vanilla proof rests on:
   the game tick, the input sample, the geometry, the raster, the fader, the
   sound frame and the netplay round accounting. The extra presents hand
   Windows the SAME finished framebuffer again. NOTHING IS INTERPOLATED at
   this rung -- with the key on, the picture repeats -- so the only thing that
   changes is WHEN the pixels are handed over.

   host_setting_frame_rate returns the target pictures per second, or 0 for
   native. */
int host_setting_frame_rate(void);

/* ---- THE THREE PICTURE-QUALITY KEYS (run hd1) ---------------------------
   One block, because they are one promise: with every one of them absent the
   game's picture, timing and behaviour are what the build without them
   produced, byte for byte where it can be measured. They are opt-in host
   renderer settings and never fixes to the ROM's own behaviour.

   RenderScale: HOW MANY HOST ROWS PER DS ROW the 3D picture is rendered at.
   A vertical multiplier of the DS panel's 192 rows, so 2 is 512x384, 3 is
   768x576 and 4 is 1024x768, and the width follows the run's aspect. 0 is
   the explicit default sentinel and means exactly what the port does today
   (2 at the native 4:3 Aspect; whatever ntr::configure_aspect derives under
   a wide one), so a settings.json written before this key existed, and a
   file that will not parse, both read as the shipped picture. Absent,
   unparseable and negative all read as 0; a positive value is CLAMPED into
   1..4, the Aspect rule, so 9 is 4 and not an error. 1 is the DS's own
   256x192 and is a real choice, which is why it is not folded into 0.
   BOOT-LATCHED: the size is threaded into the framebuffer at boot and a
   mid-run change would be a reallocation nobody tests. SM64DS_RENDER_SCALE
   overrides the file with the same grammar.

   HdTextures: 1 turns on the replacement texture pack, 0 (the default) is
   the ROM's own textures. The pack's directory is "textures_hd" under the
   asset root (SM64DS_ASSET_ROOT), or the working directory's own
   "textures_hd" when there is no asset root, and SM64DS_HD_TEXTURES_DIR
   names a different one outright. SM64DS_HD_TEXTURES overrides the on/off
   key: unset is the file's answer, empty or "0" forces it off, anything
   else forces it on, the grammar the mod keys already use.
   host_setting_hd_textures_dir never returns null; it returns the directory
   the pack would be loaded from whether or not the key is on, and the
   pointer is a static buffer that is valid for the whole run.

   SmoothModels: the model subdivision level, 0..3. 0 is the default and is
   the ROM's own geometry. Absent, unparseable and negative read as 0 and
   anything above 3 is clamped to 3, the Aspect rule again.
   SM64DS_SMOOTH_MODELS overrides the file.

   All three are read once and latched, like Aspect and FrameRate, and the
   launcher's rows promise a restart. */
int host_setting_render_scale(void);
int host_setting_hd_textures(void);
const char *host_setting_hd_textures_dir(void);
int host_setting_smooth_models(void);

/* ---- THE TWO PICTURE-SMOOTHING KEYS (run hd2) ---------------------------
   The same promise as the block above and the same grammar: absent means the
   picture the build without them produced, byte for byte where it can be
   measured, and both are opt-in host renderer settings rather than fixes to
   anything the ROM does.

   TextureFilter: HOW A TEXEL IS CHOSEN when the raster samples a texture.
   0 is the default and is the sampler the port has always used -- nearest,
   one texel per pixel, the DS's own look. 1 is bilinear: the four texels
   around the sample point, blended. 2 is trilinear: bilinear plus a chain of
   progressively halved copies of each texture, with the two nearest sizes
   blended, which is what stops a floor running away into the distance from
   sparkling. Absent, unparseable and negative read as 0 and anything above 2
   is clamped to 2, the Aspect rule. At 0 no chain is built and no extra
   memory is held. SM64DS_TEXTURE_FILTER overrides the file.

   AntiAliasing: HOW THE STAIRCASE ALONG A POLYGON EDGE IS SOFTENED.
   0 is the default and is no pass at all. 1 is edge smoothing: after the 3D
   picture is drawn and before any 2D layer is composited over it, a filter
   walks the pixels the 3D engine drew, finds the ones on a contrast edge and
   blends them along it. It touches only pixels the 3D engine drew, so text,
   the HUD and the touch-screen art are untouched. Absent, unparseable and
   negative read as 0 and anything above 1 is clamped to 1.
   SM64DS_ANTI_ALIASING overrides the file.

   Both are read once and latched, for the reason the block above is: the
   filter mode sizes the texture cache's mip chains at the first bind and the
   smoothing pass sizes a full-picture scratch buffer, so both are settled
   before the first frame and the launcher's rows promise a restart. */
int host_setting_texture_filter(void);
int host_setting_anti_aliasing(void);

/* ---- THE THREE PRESENT KEYS (run hd2, lane GPU1) ------------------------
   The same promise as the two blocks above and the same grammar: absent means
   the picture the build without them produced, byte for byte where it can be
   measured. These three are not about what the picture IS, only about how the
   finished picture reaches the screen.

   PresentBackend: WHICH PATH HANDS THE FINISHED PICTURE OVER. 0 is the
   default and is the one the port has always used -- one GDI call,
   StretchDIBits, straight into the window. 1 is Direct3D 11: the same
   finished picture is uploaded to the graphics card and drawn once as a
   rectangle, which is what makes a real vsync and a real scaling filter
   possible at all. Absent, unparseable, 0 itself and negative read as 0 and
   anything above 1 is clamped to 1, the Aspect rule. It NEVER FAILS: if the
   card, the driver or the swap chain will not have it, the run says so in one
   plain line and finishes on the GDI path. SM64DS_PRESENT_BACKEND overrides.

   PresentFilter: HOW THE PICTURE IS STRETCHED to the window, and READ ONLY
   WHEN THE BACKEND IS 1. 0 is the default and is nearest, the DS's hard pixel
   edges, which is what the GDI path does today. 1 is smooth, an ordinary
   bilinear stretch. 2 is sharp: the picture is blown up by a whole number
   with hard edges first and only the leftover fraction is blended, which
   keeps the blocky look at a window size that is not a whole multiple.
   Clamped into 0..2. SM64DS_PRESENT_FILTER_D3D overrides -- NOT
   SM64DS_PRESENT_FILTER, which has belonged to the GDI path's halftone knob
   since before this key existed.

   VSync: WHETHER THE PICTURE WAITS FOR THE MONITOR, and READ ONLY WHEN THE
   BACKEND IS 1, because the GDI path has no vsync to switch on. 0 is the
   default and is no wait, which is what the port has always done. 1 waits,
   which removes tearing. Clamped into 0..1. SM64DS_VSYNC overrides.
   IT CAN STAND ITSELF DOWN: this game advances one fixed step per tick and
   never catches up, so a wait that does not fit inside the frame's budget
   would slow the GAME rather than drop a picture. The present path measures
   that and switches the wait off, with one line, rather than let it happen.
   It also stands down for the whole run when FrameRate is set, because that
   key presents the same picture several times inside one tick.

   All three are read once and latched, like the blocks above, and the
   launcher's rows promise a restart. */
int host_setting_present_backend(void);
int host_setting_present_filter(void);
int host_setting_vsync(void);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
namespace port {
/* The adventure-ghost mode flag as a C++ predicate, so the ghost render and
   no-collision paths read one name rather than the extern "C" accessor's
   int. True exactly when host_setting_adventure_ghosts() is nonzero, env
   override included. Nothing in src/ ever sees this; it gates host-layer
   drawing and per-body interaction state only. */
bool adventure_ghost_mode();
}
#endif

#endif /* PORT_HOST_SETTINGS_H */
