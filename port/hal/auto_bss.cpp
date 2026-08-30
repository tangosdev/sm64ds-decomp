// GENERATED-ACCUMULATED zero storage for gate-10/11 BSS ring symbols.
// Grown by the link-sweep loop; sizes are generous defaults.
//
// These are hosted DS BSS globals -- mutable game state (message and cutscene
// mode flags, the Process cleanup list, the particle cursors, and much more) --
// so they are routed into the .dsstate section the save state captures. The
// DSSTATE_BEGIN/END markers set the default bss/data segment for every global
// between them; the link-sweep loop that grows this file emits new symbols
// inside the bracket, so anything it adds is captured with no further work. See
// hal/dsstate_seg.h.
#include "dsstate_seg.h"
DSSTATE_BEGIN
extern "C" {
/* MSG_GEN_TEXT_FUNCS is NOT zeroed storage: it is a 3-entry function-pointer
   table func_0201b7cc calls through on a 0xfe message control byte. Seated with
   real host addresses in hal/message_gen_text.cpp -- a zeroed array here was a
   null call (eip=0) on every multi-page line carrying an embedded-text escape. */
int VT1[8];
/* VS wiring lane (ov075): the lobby's selection block (map pick at b2e4, an
   allocation pointer and two neighbours) and two wireless-status bytes.
   Generous [8] is the file's own convention; the ROM spans are 4 each. */
int data_0209b2e4[8];
int data_0209b2e8[8];
/* data_0209b2ec MOVED to hal/scene_vs_menu.cpp (run rel0215 lane prop15), for
   the reason data_0209f310 moved out of hal/actor_vtables.cpp: dsd splits the
   four-byte run at 0x0209b2ec into data_0209b2ec (1 byte) and data_0209b2ed
   (3), src/func_ov075_021165b0.c walks BOTH names over those same four bytes,
   and a split run cannot be hosted as two disjoint objects. It is a
   grouped-section pair there, gated by gxband_guard's 'vsrank' band. */
int data_0209b2f0[8];
int data_0209fc54[8];
int data_0209fc58[8];
int data_0209fcfc[8];
int data_0209fd04[8];
int data_0209fc80[8];
int data_0209fcf4[8];
int data_0209fd18[8];
int data_0209fc70[8];
int data_0209fc90[8];
int data_0209fc98[8];
int data_0209fca4[8];
int data_0209fca8[8];
int data_0209fcbc[8];
int data_0209fcc4[8];
int data_0209fcd0[8];
int data_0209fcd4[8];
int data_0209fcf8[8];
int data_0209fd00[8];
int data_0209fd08[8];
int data_0209fd0c[8];
int data_0209fd10[8];
int data_0209fd14[8];
int data_0209fd20[8];
int data_020a0f40[8];
int data_020a0f70[8];
int data_020a6148[8];
int data_0209d3c4[8];
int data_0209d4f8[8];
int data_0209d518[8];
int data_0209d5b8[8];
int data_0209f278[8];
int data_0209fc60[8];
int data_0209fc78[8];
int data_0209fc7c[8];
int data_0209fc84[8];
int data_0209fc88[8];
int data_0209fc94[8];
int data_0209fca0[8];
int data_0209fcb0[8];
int data_0209fccc[8];
int data_0209fcd8[8];
int data_0209fcdc[8];
int data_0209fce8[8];
int data_0209fd1c[8];
int data_0209fd5c[8];
int data_0209b454[8];
int data_0209b490[8];
int data_0209b494[8];
int data_0209b49c[8];
int data_0209cee8[8];
int data_0209d454[8];
int data_0209d45c[8];
int data_0209d480[8];
int data_0209d484[8];
int data_0209d48c[8];
int data_0209d490[8];
int data_0209d4a0[8];
int data_0209d4a4[8];
int data_0209d4c8[8];
int data_0209d64c[8];
int data_0209d650[8];
int data_0209d654[8];
int data_0209d658[8];
int data_0209d65c[8];
int data_0209d660[8];
int data_0209d664[8];
int data_0209d668[8];
int data_0209d66c[8];
int data_0209d670[8];
int data_0209d678[8];
int data_0209d67c[8];
int data_0209d680[8];
int data_0209d684[8];
int data_0209d688[8];
int data_0209d68c[8];
int data_0209d690[8];
int data_0209d694[8];
int data_0209d698[8];
int data_0209d69c[8];
int data_0209d6a0[8];
int data_0209d6a4[8];
int data_0209d6a8[8];
int data_0209d6ac[8];
int data_0209d6b0[8];
int data_0209d6b4[8];
int data_0209d6b8[8];
int data_0209d6bc[8];
int data_0209d6c0[8];
int data_0209d6c4[8];
int data_0209d6c8[8];
int data_0209d6cc[8];
int data_0209d6d0[8];
int data_0209d6d4[8];
int data_0209d6d8[8];
int data_0209d6dc[8];
int data_0209d6e0[8];
int data_0209d6e4[8];
int data_0209d6f0[8];
int data_0209d6f4[8];
int data_0209d6fc[8];
int data_0209d700[8];
int data_0209d704[8];
int data_0209d708[8];
int data_0209d70c[8];
/* SIZED BY ROM SPAN, not by the generous default. The eight symbols marked
   this way are the ones whose delta to the next ROM symbol is bigger than the
   32-byte default -- they are buffers, and a ROM routine that writes the WHOLE
   object runs off the end of the host copy and into whatever the linker put
   next. func_0201b7cc opens the message box with
   MultiStore_Int(0, &data_0209d74c, 0xf00) and cleared 3808 bytes past the
   default straight through hal_wipes[0]'s vtable pointer, which HUD::Behavior
   then dispatched IsAtStart on. Same class as the undersized data_0209f4ae
   that strayed into MovingCylinderClsn's vtable (hal/actor_vtables.cpp).
   Re-audit with the delta-to-next-symbol rule after adding any symbol here. */
int data_0209d710[0x3c / 4];    /* ROM span 0x3c */
int data_0209d74c[0xf00 / 4];   /* ROM span 0xf00 -- the message text buffer */
int data_0209f204[8];
int data_0209f20c[8];
int data_0209f27c[8];
int data_0209f284[8];
int data_0209f294[8];
int data_0209f2c4[8];
/* the red-coin counter NumRedCoins reads. Sized against the symbol table
   rather than left at this file's generic int[8]: config puts data_0209f310
   at 0x0209f310, so the extent is 3 bytes, and 32 was 29 bytes of slack over
   the next four symbols. Gate 32 defined its own copy of this in
   hal/bob_enemy_bridges.cpp; the definition lives here, which is where the
   link sweep grows BSS. */
unsigned char data_0209f30d[4];
/* run mg16 lane MP3: FULL-EXTENT, 0x18 * 4 bytes. These are per-player Ctrl
   fields (stick nx at Ctrl+0x0a, ny at +0x0c, world angle at +0x0e) and
   every matched reader indexes them by player * 0x18 as BYTES -- e.g.
   Player::Behavior's `*(s16 *)((char *)&data_0209f4a6 + data_020a0e40 *
   0x18)`. int[8] is 32 bytes, which held players 0 and 1 and put player 2
   at byte 48, off the end. That was the two-player ceiling the port used to
   refuse at; the storage is the whole record now, so the refusal goes. */
unsigned char data_0209f4a2[0x18 * 4];
unsigned char data_0209f4a4[0x18 * 4];
unsigned char data_0209f4a6[0x18 * 4];
/* Stage::CheckInput's own view of the pad records: the matched TU
   accesses the whole 0x18-stride Ctrl block through this ONE symbol
   while older TUs read the per-field splits above -- the harness
   copies fields out after each CheckInput call (see walk_window) */
int data_0209f498[24];
int data_0209f350[8];
/* the actor the player is CARRYING. Rabbit::Behavior parks itself here when
   it is caught and Minimap::Behavior reads it back; engine BSS either way. */
int data_0209f33c[8];
/* data_0209fc5c MOVED to hal/scene_vs_menu.cpp (fc5c width reconciliation),
   for the reason data_0209b2ec moved: dsd splits the four-byte run at
   0x0209fc5c into data_0209fc5c (1 byte) and data_0209fc5d (3), the ROM walks
   both names over those same four BYTES, and the generic int[8] here let
   hal/level_boot.cpp seat it at int stride -- which read as byte
   fc5c[1] == 0 and froze player 2 on every VS map (Player::Behavior's VS
   gate). Grouped-section pair there, gated by gxband_guard's 'ready' band. */
int data_0209fc68[8];
int data_020a0d84[8];
int data_020a0d88[8];
int data_020a0db0[8];
/* run mg11 lane TTE: the two shorts dScMgTrampoline2_c's and
   dScMgTrampoline_c's slot 18 seed their stroke endpoints from
   (src/func_ov006_021242cc.cpp and src/func_ov006_02121fa4.c both read
   data_020a0dbc[0] and [1] with ldrsh, and src/__sinit_02075054.c is what
   writes them). SIZED BY ROM SPAN rather than by the generous default above:
   config/arm9/symbols.txt puts the next symbol at 0x020a0dc0, four bytes on,
   which is exactly the two halfwords the three TUs read. */
short data_020a0dbc[2];
/* ---- THE STYLUS RECORD, 0x020a0de8 .. 0x020a0df7 -------------------------
   ONE 16-byte block, not three independent arrays.

   On hardware this is TouchInfo[4] -- one 4-byte record per controller,
   {u8 touched, u8 edge, u8 x, u8 y} -- and dsd named a symbol at each of the
   FIRST record's four bytes, because code reached the fields through separate
   addresses. config/arm9/symbols.txt has the four one byte apart

       data_020a0de8 0x020a0de8   (+0 touched)
       data_020a0de9 0x020a0de9   (+1 edge, touch ^ prev)
       data_020a0dea 0x020a0dea   (+2 x)
       data_020a0deb 0x020a0deb   (+3 y)

   the next named symbol is data_020a0df8, and delinks.txt puts all of it in
   .bss (0x0209b000..0x020aa420), so the block is exactly 16 bytes = 4
   records and there are no image bytes to read: the LAYOUT evidence is the
   readers. Every one of them indexes ITS OWN name by slot*4 --
   Message::Update's data_020a0de9[idx * 4], Stage::CheckCameraInput's
   data_020a0de8[i].held, TouchArea_Update's data_020a0dea[i4] -- so the four
   names have to BE the same storage, one byte apart.

   WHAT WAS HERE BEFORE. The link sweep grew three separate `int [8]` arrays
   (de8, de9, deb) and nothing at all for dea. hal/sub_screen.cpp writes the
   packed record through data_020a0de8, so Message::Update's
   data_020a0de9[idx*4] and data_020a0deb[idx*4] read two other arrays that
   nothing ever writes: measured with SM64DS_TOUCH_PROBE before this change,
   the block held 01 01 78 96 while de9 and deb read 0, and the two names sat
   +32 and +64 bytes from de8 instead of +1 and +3. `held` and `y` were zero
   on every frame of every run. Same class as the pad pair data_020a0e58 /
   data_020a0e5a that hal/input_probe.cpp works around by writing both; this
   fixes the storage instead.

   HOW THE LAYOUT IS RESTORED. The mechanism tools/ovdata.py uses to
   reproduce ROM spacing and dsstate_guard names in its own fix-it text: one
   ordered .dsstate$<family><NNNN> slot per symbol, each with its ROM
   alignment, so the linker concatenates the four into one contiguous run.
   $touch0000..0003 sorts after the $aaa sentinel and before $zzz, so the run
   stays inside the captured span -- the stylus record is DS state a save
   state has to roll back. Sizes are the DS symbol spacing (next symbol minus
   this one) and the LAST name owns the rest of the block, which is ovdata's
   rule too.

   DO NOT give these "sizes that look right" (de8[16], de9[16], ...). They
   OVERLAP by construction, and MSVC treats two named globals as disjoint
   objects: a TU that stores through one and loads through another IN THE
   SAME FUNCTION can have the load folded away. Nothing in the game path can
   hit it -- the one writer only stores, every reader only loads.

   VOLATILE IS NOT THE GUARD, and an earlier revision of this comment said it
   was. A review disassembled a probe built on that premise and found MSVC
   hoisting all three volatile sibling loads ABOVE the store, because volatile
   orders accesses to the SAME object and these names are disjoint objects to
   the compiler. sub_screen's probe happens to emit its pre-reads, then the
   stores, then its post-reads, so it reports honestly today -- but anything
   new that needs a store on one name to be seen through another must use
   _ReadWriteBarrier() (or read through the same name it wrote), not volatile. */
__pragma(section(".dsstate$touch0000", read, write)) __declspec(allocate(".dsstate$touch0000")) __declspec(align(1)) unsigned char data_020a0de8[1] = { 0 };
__pragma(section(".dsstate$touch0001", read, write)) __declspec(allocate(".dsstate$touch0001")) __declspec(align(1)) unsigned char data_020a0de9[1] = { 0 };
__pragma(section(".dsstate$touch0002", read, write)) __declspec(allocate(".dsstate$touch0002")) __declspec(align(1)) unsigned char data_020a0dea[1] = { 0 };
/* the last name carries the tail of the block: 0x020a0deb..0x020a0df7, which
   is slot 0's y plus records 1..3 */
__pragma(section(".dsstate$touch0003", read, write)) __declspec(allocate(".dsstate$touch0003")) __declspec(align(1)) unsigned char data_020a0deb[13] = { 0 };
int data_020a0e5a[8];
/* data_020a1052 moved to hal/camera_bridges.cpp: it is a field INSIDE the
   local comms record at data_020a1040, not storage of its own */
/* data_0209cab4 moved to hal/level_boot.cpp: it is the second symbol of
   the save block, which the entrance loader reads across */
int data_0209d6f8[8];
int data_0209e650[8];
int data_0209f37c[8];
int data_0209f40c[0x30 / 4];    /* ROM span 0x30 */
int data_0209f224[8];
int data_0209b274[8];
int data_0209b294[8];
/* data_0209f5bc (the installed fader) moved to hal/fader_wipes.cpp: the
   two FUN_0202xxxx wipe helpers deref it with no null check. */
int data_0209fc4c[8];
int data_020a0e58[8];
int data_0209b004[8];
int data_0209b138[0x138 / 4];   /* ROM span 0x138 */
int data_0209b270[8];
int data_0209b284[8];
int data_0209b2a4[0x40 / 4];    /* ROM span 0x40 */
int data_0209d4b4[8];
int data_0209f1f0[8];
int data_0209f24c[8];
int data_0209f268[8];
int data_0209f26c[8];
int data_0209f270[8];
/* data_0209f5e8 (the COLOR fader) moved to hal/fader_wipes.cpp at gate 31:
   Scene::SetFaders dispatches two of its virtuals, so zeroed storage is a
   null vptr and the first LoadLevel faults on it. */
int data_020a4d84[0x368 / 4];   /* ROM span 0x368 */
/* data_02099fb0 moved to romdata (gate 35): it is file-backed arm9 data with
   the value 4 in it, and it is the COUNT func_02048720 walks when it looks
   for a free 3D voice. At 0 that loop never runs, pick stays -1, and every
   type-9 sound effect in the game comes back "no positional voice free" --
   which Sound::Play answers with a silent return. */
int data_0209d4ac[8];
int data_020a4c64[8];
int data_020a4c70[0xc0 / 4];    /* ROM span 0xc0 */
/* tier-2 state wave: DeadHit/Hurt read this arm9 bss word */
int data_0209f28c[8];
/* tier-2 round 2: arm9 bss the death/hurt/quicksand ring reads */
int data_0209b4b0[8];
int data_0209f330[8];
int data_0209b470[8];
int data_0209b474[8];
/* ov100 (the message-box overlay) bss reached from St_Talk_Main; bss is
   zero at load, so host storage is the whole of it. */
int data_020a4bf8[8];
/* Sound::Play's pooled 3D voice slots, 8 bytes each. func_02048f34 clears
   SIX of them (0x020a4c18..0x020a4c48 = 0x30) and func_02048720 indexes the
   pool by data_02099fb0, so int[8] was two entries short. */
int data_020a4c18[0x30 / 4];
/* death states: KillPlayer's remaining-lives byte, and the pending-scene
   argument Scene::SetSceneToSpawn parks next to it */
int data_0209f2f4[8];
int data_0209f5b8[8];
/* gate 14: the sound-group bookkeeping the kuppa tail's Sound::
   LoadGroupAndSetBank reads (dead on the port's boot, live at link time) */
int data_0209b47c[8];
int data_0209b4a8[8];
/* gate 14 A2: the entrance step handlers' own bss, arm9 and ov002. bss is
   zero at load, so host storage is the whole of it. */
int data_0209f2bc[8];
int data_0209f2ac[8];
int data_0209f4f8[0xc0 / 4];   /* the per-level death table; ROM span 0xc0 */
int data_ov002_0210e14c[8];
int data_ov002_0210f350[8];
int data_ov002_0210f3b0[8];
/* data_ov089_02132880 was the same fiction; gate 22 mounts ov089 for
   real and it is the overlay's own bytes now. */
/* gate 16: the two processing-list globals the other three did not already
   need. data_020a4ba8 is the cleanup list (head, tail, callback pair);
   data_020a4b5c is the id hook func_0204302c calls after a Process tears an
   actor down, and null is what the ROM's boot leaves it at. */
int data_020a4ba8[8];
int data_020a4b5c[4];
/* gate 22: the DOOR's two. data_020a0ebc is the zero Vector3
   func_ov100_02145370 rotates the player offset around. kind:bss in config,
   so zero is what the boot leaves it at; ov089's own bss comes from the
   overlay mount. */
int data_020a0ebc[3];
/* gate 180: the QUESTION_BLOCK content body func_ov102_021492d4 reads it.
   kind:bss at 0x020a0edc, real span 8 bytes (to data_020a0ee4); zero is the
   boot value. */
int data_020a0edc[2];
/* gate 20: the EXIT's own scratch. func_ov002_020b0a0c stores the spawn
   record's entrance byte here on its way into LoadLevel; config/arm9 calls
   it kind:bss, so zero is what the boot leaves it at. */
unsigned char data_0209f2c0[4];
/* gate 18 (RABBIT_KEY): StartMinigameMenu's return-to-rec-room flag -- the
   caught chain's 8th-catch terminal writes it before the scene fade.
   kind:bss at 0x0209f298, span 4 (to data_0209f29c), zero is the boot value. */
unsigned char data_0209f298[4];
/* gate 27, the HUD: the star-count cache HUD::Render and RenderStarCount
   share, the red-coin counter, and the VS-mode 'results are up' flag. All
   kind:bss, so zero is the boot value. */
unsigned char data_0209f2d4[4];
unsigned char data_0209f30c[4];
unsigned char data_0209fc9c[4];
unsigned char data_0209f248[4];
/* gate 25: the bottom screen. The three SetSubBgyOffset scroll shadows and
   SetBg0Offset's pair (the 2D layer's own copy of the BGxHOFS/VOFS words),
   the owner-language byte GetOwnerLanguage returns, the per-slot camera-
   button state Stage::CheckCameraInput latches, and the ov002 byte it sets
   when the zoom button is pressed. All kind:bss, so zero is the boot value. */
int data_0209d468[4];
int data_0209d46c[4];
int data_0209d470[4];
int data_0209d47c[4];
int data_0209d494[4];
int data_0209d498[4];
unsigned char data_020a0f00[4];
unsigned short data_0209f368[8];
unsigned char data_ov002_02111180[4];
/* gate 26, the boot spine: Stage::LoadModel's last line parks the Stage's own
   ModelComponents pointer (Stage+0x874) here, and CopyTexPalFromLevelModel
   reads it back. kind:bss in config, so zero until LoadModel runs. */
int data_0209f320;
/* gate 31: StartFile's last global before the scene fade. kind:bss in config,
   so zero is the boot value; the port's boot never set it because nothing
   called StartFile.

   Gate 35 also wants this one, as the id of the star just collected, and
   declared it int[8] in its own branch. FOUR BYTES IS THE RIGHT SIZE: config
   puts data_0209f22c at 0x0209f22c, so the extent is 4, and 32 would have
   covered seven adjacent symbols. star_flow.cpp reads it as a single
   `extern unsigned char`, which this serves. Same shape as the data_020a0e68
   stomp: when two streams want one symbol, the symbol table decides, not
   whichever declaration is more generous. */
unsigned char data_0209f228[4];
/* gate 31: the two SetNumPlayers seats beside data_0209fc5c (hosted in
   hal/scene_vs_menu.cpp since the width reconciliation) -- the player count
   and the per-slot controller index. kind:bss. */
unsigned char data_0209fc50[4];
char data_0209fc64[4];
/* gate 31: the second word CleanCommonModelDataArr resets. Its two siblings
   (the count at 0x0209cef8 and the array at 0x0209cefc) already have storage
   in hal/model_host.cpp; this one had no reader until the level teardown
   called the ROM's own reset. kind:bss, so zero is the boot value. */
int data_0209cef0;
/* wave 3 lane b: the slot the Scene head chain clears.
   Scene::ResetFadersAndSound (src/_ZN5Scene19ResetFadersAndSoundEv.c) writes
   `data_0209f1e4 = 0` right after Scene::SetFaders installs the brightness
   fader, and Scene::BeforeBehavior (src/_ZN5Scene14BeforeBehaviorEv.cpp) is
   the reader: on zero it parks &data_0209f5d0 here, and func_0202345c loads
   it back and walks it as a `void **`. So the slot holds a POINTER, and null
   is both the boot value and the value the reset writes, which is what makes
   kind:bss storage the whole of it.

   FOUR BYTES, sized by the delta-to-next-symbol rule the buffer comment
   above sets out, not by this file's generic int[8]. config/arm9/symbols.txt:

       data_0209f1e0 kind:bss addr:0x0209f1e0
       data_0209f1e4 kind:bss addr:0x0209f1e4
       data_0209f1e8 kind:bss addr:0x0209f1e8

   so the ROM extent is 0x0209f1e8 - 0x0209f1e4 = 4, and the generous default
   would have been 28 bytes of slack lying across the next seven symbols --
   the data_0209f228 shape exactly. There is no overrun to leave room for
   either: all three touches in the tree are single-word. include/decl_common.h
   already declares it `void *`, which is what this definition is. */
void *data_0209f1e4;

/* ---- wave 4 lane d: the five globals Scene::BeforeBehavior's closure reads --
   Slot 7 of _ZTV5Scene (config/arm9/relocs.txt from:0x0209269c to:0x0202e3d4)
   is Scene::BeforeBehavior, and its callee closure -- func_02023544,
   func_0202ed48, func_0202ed14, func_0202fb30 -- reads five DS BSS symbols the
   port hosted nowhere. Every one is kind:bss in config/arm9/symbols.txt, so
   zero is the value the ROM's own boot leaves them at, EXCEPT data_0209f5d0
   (see its own note below).

   EVERY SIZE IS THE DELTA TO THE NEXT SYMBOL, the rule the buffer comment
   further up this file sets out, not this file's generic int[8]. Read out of
   config/arm9/symbols.txt, one subtraction each:

       data_0209f1e0 .. data_0209f1e4   4 bytes
       data_0209b2fc .. data_0209b300   4 bytes
       data_0209b300 .. data_0209b304   4 bytes
       data_0209f5d0 .. data_0209f5dc  12 bytes  (0xc)
       data_0209f61c .. data_0209f648  44 bytes  (0x2c)

   THE LAST ONE IS THE INTERESTING SIZE, and it runs the opposite way from the
   data_0209d74c overrun the header describes: the generic int[8] would be too
   SMALL, not too large. src/func_0202ed14.c writes its argument's byte at
   +0x28, so the object has to be at least 0x29 bytes; 32 would have put that
   store one byte past the end and into whatever the linker parked next.
   src/func_0202fb30.c reads the same +0x28 and copies +0x18 into +0x14. 44
   covers both with the ROM's own extent and no guessing. */

/* the paused/suspended flag. src/func_02023498.c is the only writer that ever
   sets it (`data_0209f1e0 = 1` on the pause edge) and that TU is not in the
   link, so on the port it stays 0 -- which is what makes the whole first
   branch of Scene::BeforeBehavior unreachable. decl_common.h does not declare
   this one; src/func_02023544.c and src/func_02023498.c both spell it
   `extern unsigned char`, which this serves. */
unsigned char data_0209f1e0[4];

/* the two bytes func_02023544 clears beside it. decl_common.h:1781 already
   declares data_0209b2fc as `u8`, which this serves. */
unsigned char data_0209b2fc[4];
unsigned char data_0209b300[4];

/* ---- run mg15 lane MENU: the two the minigame MENU scene reads ------------
   dScMiniGm_c (scene 5, ov005) is the first thing in the port to touch either.
   Both are kind:bss in config/arm9/symbols.txt, so zero IS the ROM's boot
   value, and both are sized by the ROM delta to the next symbol -- the rule
   this file's header sets out -- rather than by the generic int[8]:

       data_0209b304 .. data_0209b308   4 bytes
       data_0209f1d8 .. data_0209f1dc   4 bytes

   data_0209b304 is the menu's PAGE flag: func_ov005_020c14a0 sets it to 0 or 1
   as the two dwell counters expire, and func_ov005_020c0378 branches its whole
   hit test on it -- 0 is the six-cell grid (3 columns x 2 rows), 1 is the
   three-cell strip. src/func_ov005_020c1a20.c reads it during InitResources.
   Sits beside data_0209b300 and data_0209b2fc above because it IS the next
   word of that run; the ROM spacing is contiguous.

   data_0209f1d8 is the WHOLE-GAME PAUSE-ADJACENT flag. Four arm9 TUs already
   spell it (func_02019ac4 gates its work on it, func_02030aa4 and
   func_02023498 write it), so it is not an ov005 invention -- ov005 is just
   the first spelling of it that reaches this link. func_ov005_020c1a20 sets it
   to 1 at the tail of InitResources and src/func_ov005_020bff4c.cpp clears it
   on the way out, which is the menu declaring itself open and then closed.
   Every TU in the tree spells it `unsigned char`, which this serves. */
unsigned char data_0209b304[4];
unsigned char data_0209f1d8[4];

/* the record func_0202ed48 resets: 44 bytes by the rule above, and the ROM
   writes nine of its fields through func_0202ed14 (eight fresh values plus
   +0x18 copied from +0x14). */
int data_0209f61c[0x2c / 4];   /* ROM span 0x2c */

/* THE BRIGHTNESS FADER, and the one symbol here where zero is NOT the ROM's
   boot value. src/__sinit_02074edc.c is the ROM's static initialiser for it:
   it installs a vtable, sets currInterp to 0x1000, and registers a destructor.
   The port does not link that sinit, so this storage is zeroed BSS and its
   FIRST WORD -- the vptr -- is null.

   THAT IS SAFE HERE AND IT IS NOT LUCK, but it is a real constraint and the
   next lane through has to keep it. Only two dispatch-relevant touches of
   this object reach the binary, both from Scene::BeforeBehavior (the head
   also writes field4 and parks the address in data_0209f1e4, behind the same
   guard), and both are behind
   `data_0209f1e0 != 0`, which no linked writer ever makes true (above). One of
   the two, the matched FaderBrightness::SetForwardTime, ENDS IN AN UNQUALIFIED
   IsAtEnd() -- a virtual call through this vptr -- so on a null vptr it would
   fault. See the note in hal/method_faces.cpp beside its face, and
   slice_w1l2.txt's blocked list: giving this object a real vptr is the
   ROM-class swap slice_w1l3.txt already names, not a line here.

   Twelve bytes, the ROM extent, which is also exactly the C view of the class
   in include/FaderBrightness.h (vtable 0x0, currInterp 0x4, speed 0x8). */
int data_0209f5d0[0xc / 4];    /* ROM span 0xc */
}
DSSTATE_END

/* Sound:: is a NAMESPACE in the TU that calls this one (YAX mangling) */
namespace Sound { void UnsetPlayerVoiceGroup(); }
void Sound::UnsetPlayerVoiceGroup() {}

DSSTATE_BEGIN
/* ---- gate 29: the particle engine's own BSS -------------------------------
   data_0209ee78/7c/80 are the three cursors of the arena SysTracker::
   Initialise carves out (base, end, current) and func_02023178 bump-allocates
   from. 84/88/8c are the VRAM bases it caches off func_02045ce0/cf0/d10.
   data_020a4d30 is the engine's scratch slot.

   LCG_STATE_0204da4c is the particle RNG's state -- func_0204da4c multiplies
   by 0x5eedf715 and adds 0x1b0cb173 three times to pick a random emission
   direction. The additive constant means a zero seed is a perfectly good
   seed, and starting at zero is what keeps a selftest frame reproducible. */
extern "C" {
int data_0209ee78[8];
int data_0209ee7c[8];
int data_0209ee80[8];
int data_0209ee84[8];
int data_0209ee88[8];
int data_0209ee8c[8];
int data_020a4d30[8];
int LCG_STATE_0204da4c;

/* THE TWO THE FAULT PROBE WAS ANSWERING FOR. Retiring
   port/unmatched/Player_InitResources.cpp (w6-c, HOSTABI_RETIRE_SYMS) took
   away the definition site for the level id and the local player index, and
   nothing failed to link: tests/fault_probe.h carries
   /alternatename:_data_0209f2f8=_port_fault_no_level and the matching
   _data_0209f250=_port_fault_no_pidx, weak fallbacks meant to fire only in
   the reduced probe binaries. They fired in the full walk_window instead, so
   the game would have read the level it was standing in out of a fault-probe
   placeholder.

   The alias guard could not see it: both aliases FIRED, which is the shape it
   reads as healthy, and both pairs sit in alternatename_baseline.txt as
   reviewed weak-symbol idiom. dsstate_guard is what caught it, by noticing
   two hosted DS globals had landed outside the captured section.

   Four bytes each by next-symbol delta in config/arm9/symbols.txt
   (0209f250/0209f254, 0209f2f8/0209f2fc), not the one byte the retired copy
   declared. Here rather than in hal/level_boot.cpp beside the other six of
   that retirement because level_boot re-declares data_0209f2f8 as a scalar
   `signed char` further down its own file. */
unsigned char data_0209f250[4];   /* local player index */
signed char   data_0209f2f8[4];   /* level / sublevel id */
}
DSSTATE_END

/* gate 50: ov080's PAINTING (daPicGate_c, 307) bss is NOT here -- it is
   mounted with the rest of the overlay in ov080_syms.txt so the DATA table
   data_ov080_0212775c's pointers into it get rebased to host addresses. See
   the header of port/ov080_syms.txt for why auto_bss would break the Shared
   FilePtr walk. */
