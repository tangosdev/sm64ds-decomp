#!/usr/bin/env python3
"""Emit the ROM data constants the gate-4b slice references.

These are read-only tables in the arm9 image (sine table, identity matrices,
texture-matrix constants). They are Nintendo's bytes, so they are never
committed: this reads the local decompressed arm9 (extracted/arm9_dec.bin,
flat image based at 0x02000000) and writes a C file under the build tree.
"""

import pathlib
import struct
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import romblob_common  # noqa: E402

# extracted/arm9_dec.bin: the DECOMPRESSED arm9, flat at base 0x02004000
# (verified by two code anchors: Copy36Bytes @0x0205a548, DecompressLZ16
# @0x0205a754). extracted/dsd/arm9/arm9.bin is the COMPRESSED payload
# (arm9.yaml: compressed true) and must not be read directly. Addresses at
# or past bss_start are runtime-initialized and belong in the HAL.
#
# BSS_START WAS 0x02094640 AND THAT WAS TOO LOW BY 0x69c0, which quietly
# pushed a whole run of file-backed .data into the HAL as zeroed storage.
# The ROM settles it twice over: extracted/dsd/arm9/arm9.yaml records
# bss_start 34162240 = 0x0209b000, and config/arm9/delinks.txt lays the
# sections out as .data 0x02086bc0..0x0209b000, .bss 0x0209b000..0x020aa420.
# The image itself reaches 0x020a0f78, so every address below 0x0209b000 has
# real bytes behind it. Two of the symbols the old boundary excluded had
# already been reconstructed by hand in port/hal/model_host.cpp, and the ROM
# bytes agree with both reconstructions exactly -- which is the check that
# the new boundary is the right one.
BASE = 0x02004000
BSS_START = 0x0209B000

# (symbol address, byte length, C element type). BSS addresses do not belong
# here -- they are runtime-initialized and live in the HAL as storage.
#
# THE ELEMENT TYPE IS LOAD-BEARING and was wrong for the two OBJ size tables
# until 2026-08-05. The emitter below only knew two widths, 4 for "int" and 2
# for everything else, so "char" read the twelve-byte tables as SIX 16-BIT
# values and declared them `char[6]`. Both the count and the values came out
# wrong: MSVC truncated each halfword to its low byte, so the width table read
# {8, 32, 16, 32, 8, 16} where the ROM has {8,16,32,64, 16,32,32,64, 8,8,16,32}.
# OAM::GetObjWidth indexes it at `size + shape * 4`, up to 15, so every sprite
# past shape 1 size 1 also read off the end of a six-byte array.
#
# The two are 16 rather than 12 for the same reason the ROM gets away with 12:
# 0x020755ac is 0x020755a0 + 12, so the shape-3 row of the width table IS the
# first row of the height table. Emitting 16 bytes each keeps every index the
# code can form inside its own object and reading exactly the bytes the DS
# reads. Shape 3 is the prohibited OBJ shape and no sprite uses it.
TABLES = [
    (0x02082128, 48, "int"),        # Model ctor default Matrix4x3
    (0x02082190, 48, "int"),        # matrix Render uses for a NULL argument
    (0x02082214, 0x4000, "short"),  # s16 trig table the material bind indexes
    (0x020755A0, 16, "unsigned char"),  # OBJ width table  (size + shape * 4)
    (0x020755AC, 16, "unsigned char"),  # OBJ height table
    # ONE ROW THAT DELIBERATELY OVERLAPS THE TRIG TABLE, run mg11 lane SNW.
    # 0x02082314 is 0x100 bytes into the s16 table above -- entry 128 -- and
    # dScMgSnowball_c reads it BY NAME rather than by index:
    # src/func_ov006_02126948.c does fdiv(0xc0000, data_02082314), and the ROM
    # at 0x02126958 is `ldrsh r1,[r1]` off a pool word holding 0x02082314, so
    # the read is one SIGNED HALFWORD and include/decl_common.h declares it
    # `extern short data_02082314;` to match.
    #
    # THE OVERLAP IS THE ESTABLISHED SHAPE IN THIS LIST, not a new liberty:
    # the two OBJ tables above overlap by four bytes and the header explains
    # why. A separate two-byte emission is right here for the same reason it
    # is there -- both hosts hold the ROM's own bytes for their own address,
    # nothing writes either (the trig table is read-only), and the alternative
    # would be a hand-typed constant in a HAL file, which is a value nobody
    # can re-derive from the image.
    (0x02082314, 2, "short"),       # trig table entry 128, read by name
]


# Initialized arm9 data referenced by name: address + size resolve from
# config/arm9/symbols.txt (size = delta to the next symbol, the ovdata.py
# convention). Emitted as aligned byte arrays so any struct view works.
NAMED = [
    # Run mg15 lane MENU: the minigame SELECTION menu's two row words. Both are
    # arm9 .data (config/arm9/delinks.txt: .data 0x02086bc0..0x0209b000), four
    # bytes each by the delta rule, and NEITHER carries a relocation -- checked
    # over both spans in config/arm9/relocs.txt, no `from:0x0208a170` and no
    # `from:0x0208a174` line exists. Their ROM values are 0x00000003 and
    # 0xffffffff, read out of extracted/arm9_dec.bin at 0x0208a170.
    #
    # THE -1 IS THE WHOLE REASON THEY COME FROM HERE AND NOT FROM A HAL LINE.
    # data_0208a174 is the menu's SELECTION and its boot value is the "nothing
    # picked" sentinel; src/func_ov005_020c0378.c only ever writes a row index
    # into it, so a host that zeroed it would boot the menu with row 0 already
    # chosen and nothing in the game would ever say so. data_0208a170 is the
    # highlighted row base, advanced by func_ov005_020c0878, and 3 is where the
    # ROM starts it. The port does not write Nintendo's numbers down; it reads
    # them, which is the same argument every other row in this list makes.
    #
    # ONE HONEST GAP, NAMED RATHER THAN LEFT TO BE FOUND. data_0208a174 is
    # MUTABLE DS state -- the menu writes the player's selection into it -- and
    # nothing this emitter produces lands in .dsstate. Every symbol in this
    # file goes into romdata.c's own section (measured: walk_window.map puts
    # _data_0208a170 at 0003:00004660, outside the captured span), which is the
    # mechanism's existing behaviour for all 30-odd NAMED entries and not
    # something these two introduced. The consequence is bounded and specific:
    # a save state taken while the minigame menu is open does not roll back the
    # highlighted row or the selection. Moving the emitter's output into
    # .dsstate is a change to every row here and is not this lane's to make.
    "data_0208a170", "data_0208a174",
    "data_0208a178", "data_0208c178",
    "data_0208e504", "data_0208e538", "data_0208e548", "data_0208e54c",
    "data_0208e55c", "data_0208e56c", "data_0208e57c", "data_0208e58c",
    "data_0208e59c", "data_0208e5b0", "data_0208e5c0", "data_0208e5d4",
    "data_0208e5e4", "data_0208e5f0", "data_0208e5fc", "data_0208e608",
    "data_0208e610", "data_0208e624", "data_0208e634", "data_0208e648",
    "data_02092120",
    # run linkw wave 5 (lane w5-c): the two identity-quaternion constants
    # (x,y,z,w) = (0,0,0,0x1000) the restored TILTING_PLATFORM_BFS chain
    # reads -- Quaternion_FromVector3's fallback (data_02075b04) and the
    # tilting Behavior's slerp rest pose (data_02092768). Both 0x10, both
    # reloc-free (checked config/arm9/relocs.txt over the full spans).
    # Lane w5-b reached the same pair from level 13: the WorkElevator
    # rider-push chain reads them too -- func_ov021_02112024 resets its
    # orientation from data_02092768 and Quaternion_FromVector3 falls
    # back to data_02075b04.
    "data_02075b04", "data_02092768",
    # Run link60 lane NFS: the NitroSDK archive NAME. func_0205d96c passes
    # &data_0209a048 and a length of 3 to func_0205cc80, which packs the three
    # characters into the archive record's first word and compares against it
    # on every "arch:" prefixed path. It is four bytes of arm9 rodata ("rom\0",
    # the next symbol is 0x0209a04c), and it comes from here rather than being
    # spelled as a C literal for the reason every other row here exists: the
    # port does not write Nintendo's bytes down, it reads them.
    "data_0209a048",
    # Run mg11 lane TTI: the one s16 dScMgTrampoline_c's InitResources divides
    # by. src/func_ov006_02122198.cpp reads it as `(s32)data_02082414` and the
    # ROM does `ldrsh r1,[r1]` at 0x02122210 with the pool word 0x02082414,
    # feeding cstd::fdiv(0xc0000, that) -- the scene's field of view. It is an
    # INTERIOR ADDRESS of the trig SPAN already hosted above
    # ((0x02082214, 0x4000, "short")), 0x200 bytes in, and /alternatename
    # cannot express an offset from a symbol, so the name is hosted here in its
    # own right. The duplication is safe and cannot drift: the span is a
    # read-only table (this file's own opening line says so) and both copies
    # come from the same extracted image on every build. ROM span 0x200, the
    # delta to the next config symbol data_02082614.
    "data_02082414",
    # Run link60 lane FDR2: the two BG palette words the dWipe_c time setters
    # push through GX/GXS::LoadBGPltt. func_0202f708 loads data_020926c8 and
    # func_0202f928 loads data_020926cc, each with a length of 2, on the
    # `type != 2` branch; the type == 2 branch uses their bss counterparts
    # data_0209f600 / data_0209f604, which are runtime storage and are hosted
    # in port/hal/fdr_arm9_fader_seat.cpp instead. Four bytes each by ROM span
    # (the next symbols are 0x020926cc and 0x020926d0) and reloc-free: the
    # nearest `from:` rows in config/arm9/relocs.txt are 0x020926c4 below and
    # 0x020926dc above, so nothing inside either span relocates. They are the
    # cartridge's own colours and are read here rather than written down.
    "data_020926c8", "data_020926cc",
    # run mg11 lane TTE: the fov divisor dScMgTrampoline2_c's and
    # dScMgTrampoline_c's InitResources read. Both do
    # `fdiv(0xc0000, (s32)data_02082414)` and the ROM is `ldrsh r1,[r1]` on that
    # exact address, so it is a single signed halfword and decl_common.h:1662
    # declares it `extern s16 data_02082414;`. It is an INTERIOR ADDRESS of the
    # trig table TABLES already emits at 0x02082214 (0x4000 bytes), 0x100
    # halfwords in; the span to the next symbol is 0x200 and nothing inside it
    # relocates (config/arm9/relocs.txt has no `from:` row in 0x02082414..
    # 0x02082614). Emitting it by name duplicates 0x200 read-only bytes that
    # already sit inside the trig table object, which is the cost of giving the
    # decomp's own symbol name a definition; the alternative was a synthesised
    # host global with no ROM provenance, which this file exists to avoid.
    #
    # THE ROW ITSELF IS ABOVE, NOT HERE. Two mg11 lanes needed this symbol and
    # each added it with its own derivation; the merge keeps ONE row and BOTH
    # notes, because the two lanes checked different things (that one reasons
    # from the /alternatename limitation, this one from relocs.txt) and neither
    # reading is redundant. A second entry would emit the 0x200 bytes twice.
    "data_02082178", "data_02090e80", "data_020914a0",
    "data_02092584", "data_02092654", "data_02092668", "data_0208e500", "data_02086a58", "data_0208e430", "data_02086b58",
    "data_0208e434", "data_0208e438", "data_0208e43c", "data_0208e440",
    "data_0208e444", "data_02092124", "data_02086384", "data_0208e448", "data_02088fb8", "data_020890a0", "data_02092118", "data_02092110", "data_0208eecc", "data_02092134",
    # data_020876e4 WAS HERE and was six RELOCATED words: the Kuppa/star-get
    # script-pointer table func_0200ee8c indexes as data_020876e4[arg0] and
    # RunKuppaScript runs. Byte-copied it held six raw DS addresses, so
    # ProcessKuppaScript faulted walking a garbage pointer. Hosted with real
    # host pointers to byte-copied script blobs in port/hal/ptr_tables.cpp
    # (table 3); putting the name back here would define the symbol twice and,
    # if the byte copy won, restore the crash.
    "data_0208e42c",
# ---- run lvled, lane intro-cutscene: the NEW-FILE OPENING's script data ----
#
# src/__sinit_02073e6c.c is what makes the opening's FOUR-script chain work:
# the chain arguments and the argument-blob pointers are stored as ZERO on
# disc and it writes them at boot, byte by byte because the fields are
# unaligned. On the host it writes HOST addresses, which is exactly why it
# has to run rather than the pointers being byte-copied.
#
# EVERY SYMBOL BELOW WAS AUDITED INDIVIDUALLY against config/arm9/relocs.txt
# before being put here, because this list BYTE-COPIES and a byte-copied
# relocated word carries a DS address into the host -- the data_020876e4
# crash this file's own header banks. The audit is
# out/intro-cutscene/reloc_audit.txt; the closure is 155 symbols and it
# TERMINATES (level 2 is 54 clean, 1 record, 1 further target).
#
# The 96 names here have ZERO relocated words in their spans, with ONE
# reasoned exception:
#
#   data_02089af8 (the third script, the one carrying the closing
#   LoadLevelNoReturn) is flagged with six relocated words. All six are
#   FALSE POSITIVES and none is dereferenced. dsd's arm9 relocations are
#   INFERRED -- arm9 loads at a fixed address, so a data word whose value
#   looks like an address gets marked. Walking the script record by record
#   out of arm9_dec.bin (ProcessKuppaScript's own rule: s[0] is the record
#   length, +1 cmd, +2 beg, +4 end, +6 args) puts all six at ARGUMENT
#   offsets inside cmd 0x00/0x01/0x02 records -- the player-movement
#   commands, which func_0200e5ac consumes as VALUES. The values are
#   0x020b0000, 0x020a0244, 0x02091000, 0x02090800, 0x020f0000 and
#   0x020f1000: position coordinates that happen to look like DS
#   addresses. The only kuppa command that dereferences an argument as a
#   pointer is cmd 6 (the chain), and its arg is ZERO on disc and written
#   by the sinit. Decode in out/intro-cutscene/script_decode.txt.
#
# The 55 POINTER RECORDS that really do carry pointers are NOT here. They
# are hosted with real host pointers in hal/intro_script_records.cpp, the
# way table 2 of hal/ptr_tables.cpp hosts the 39 camera-script records.
#
# NAMES THE AUDIT PRODUCED THAT ARE DELIBERATELY NOT BELOW:
#   data_02087c00, data_02088610, data_02088fb8, data_020890a0, data_02089608
#     already carried by this file above; a second entry is a redefinition.
#   data_0209b278, data_0209b294
#     NOT romdata. Both live in the 0x0209xxxx working region and their
#     spans come out NEGATIVE here (the next symbol is below them).
#     data_0209b294 is the FaderBrightness OBJECT ProcessKuppaScript drives
#     through SetToStart/SetForwardTime -- live state hosted as BSS, so
#     byte-copying it here would be a category error.
    "data_020876d4", "data_020876fc", "data_0208771c", "data_02087740",
    "data_02087798", "data_020877c8", "data_020877f8", "data_02087828",
    "data_02087858", "data_0208788c", "data_020878c0", "data_020878f4",
    "data_02087928", "data_0208795c", "data_02087990", "data_020879c4",
    "data_020879f8", "data_02087a2c", "data_02087a60", "data_02087a94",
    "data_02087ac8", "data_02087afc", "data_02087b30", "data_02087b64",
    "data_02087b98", "data_02087bcc", "data_02087c34", "data_02087c68",
    "data_02087ca0", "data_02087cd8", "data_02087d10", "data_02087d48",
    "data_02087d80", "data_02087db8", "data_02087df0", "data_02087e28",
    "data_02087e60", "data_02087e98", "data_02087ed0", "data_02087f08",
    "data_02087f40", "data_02087f80", "data_02087fc0", "data_02088000",
    "data_02088040", "data_02088080", "data_020880c0", "data_02088100",
    "data_02088140", "data_02088180", "data_020881c0", "data_02088200",
    "data_02088240", "data_02088280", "data_020882c0", "data_02088300",
    "data_02088340", "data_020883d0", "data_02088418", "data_02088460",
    "data_020884a8", "data_020884f0", "data_02088538", "data_02088580",
    "data_020885c8", "data_02088660", "data_020886b0", "data_02088700",
    "data_02088750", "data_020887a0", "data_020887f0", "data_02088840",
    "data_02088898", "data_0208894c", "data_02088a18", "data_02088a94",
    "data_02088ba0", "data_02088c2c", "data_02088ccc", "data_02088d80",
    "data_02088e38", "data_02088ef0", "data_020891a8", "data_020892c0",
    "data_020893d8", "data_020894f0", "data_02089850", "data_02089af8",
    
    # FORTY NAMES CAME OUT OF THE LINE ABOVE ON 2026-08-05, and every one of
    # them was a called-through pointer table rather than data.
    # port/tools/ptr_audit.py found them by intersecting this list with
    # config/arm9/relocs.txt: data_02086758 is nine backup-media command
    # handlers that src/func_0205c5e4.c indexes and calls, and the 39 records
    # from data_02086bc0 to data_02086e38 are the kuppa script's camera
    # commands, copied into data_0209b138 and dispatched by func_02008550.
    # Emitted here they were the ROM's own bytes -- forty-eight DS code
    # addresses -- which is the shape that killed a play session when the
    # billboard's two tables had it (see the note by data_02099fc4 below).
    # They are hosted with real host function pointers in
    # port/hal/ptr_tables.cpp; putting any of the names back on this list would
    # define the symbol twice and, if the byte copy won, restore the crash.
    #
    # ...and three plain constants the newly-linked handlers read, which had
    # never been referenced before for the same reason. Two camera-space offset
    # vectors, {0, 150, -600} and {600, 0, -400} in Fix12i, and the two ASCII
    # bytes func_0205bc88 copies into its header buffer. No relocations in any
    # of them; they really are data.
    "data_02086ec0", "data_02086ed8", "data_0209a044",
    # gate 89: a 779-byte arm9 .rodata table func_ov002_020e88a8 (a PowerStar
    # state helper) reads a s16 out of at +0x56. No relocations in it -- pure
    # data, byte-hosted the same as every table above.
    "data_02082714",
    # run mg11 lane BNP: the 0x100 bytes at 0x02082614, which
    # src/func_ov006_020ee690.cpp (dScMgJump_c::InitResources, id 0x174) reads
    # ONE s16 out of, as the divisor of cstd::fdiv(0xc0000, data_02082614) --
    # the projection constant it parks in the scene's two camera records. It is
    # an INTERIOR address of the TABLES entry above (data_02082214, the s16 trig
    # table), exactly as data_02082714 already is, so this row duplicates those
    # bytes into a second host array rather than aliasing into the first. That
    # is correct for a scalar read and would NOT be correct for anything doing
    # pointer arithmetic between the two symbols; nothing in the tree does.
    # No relocations land in the span (config/arm9/relocs.txt), so it is data.
    # run mg11 lane BNT: dScMgJump2_c's InitResources (func_ov006_020ef834)
    # loads 0x02082614 and reads a SIGNED HALFWORD out of it at 0x020ef8c8
    # (ldrsh r1,[r1]) before calling func_02053258. 0x100 bytes, the delta to
    # data_02082714 above it, and no relocations in the span -- pure data,
    # byte-hosted the same as every table here.
    "data_02082614",
    # gate 32: the coin tables Actor::GivePlayerCoins indexes (id and amount),
    # and the per-character cap-model constant data_0208a0e0 sits beside them.
    "data_02075230", "data_02075238",
    "data_0208ee44",
    # The three MSG_GEN_TEXT_FUNCS embedded-text data tables (func_0201aca4 /
    # func_0201adac read them on a 0xfe message escape): data_0208ee64 is the
    # {100,10,1} decimal place-value divisors, data_0208ee6c the button-glyph
    # font pairs, data_0208ee74 the 0..9 digit glyph indices. Pure data (no
    # relocs in the range); zeroed storage would have printed all zeros.
    "data_0208ee64",
    "data_0208ee6c",
    "data_0208ee74",
    "data_0208ee8c",
    "data_0208eeac",
    "data_0208f074",
    "data_0208f174",
    # THE PARTICLE DEFINITIONS, and the whole answer to "where do particle
    # effects come from" -- not a file, not a narc, a blob compiled into the
    # binary. Particle::SysTracker::Initialise memcmps its first 4 bytes
    # against the 8-byte magic at data_0208f668; on a mismatch it treats the
    # blob as LZ16 and decompresses it onto the heap. func_0204a17c then
    # parses it into per-effect records and builds each one's Behavior[]
    # array out of six flag bits.
    #
    # THE SIZE IS PINNED, and it has to be. The blob is an uncompressed SPA
    # archive whose own header says how big it is: " APS10_1" magic, at +0x08 a
    # u16 emitter count (321) and a u16 texture count (46), at +0x10 the size of
    # the emitter block, at +0x14 the size of the texture block and at +0x18
    # where the texture block starts. Emitters begin at 0x20, so +0x18 is just
    # 0x20 + emitters and the archive really ends at +0x18 plus +0x14: 0xc214,
    # which is exactly where the next real symbol (data_02082128) begins.
    #
    # The config splits the span with nine boundary symbols (data_02078000,
    # data_02080000 and friends) that are page-aligned, referenced by nothing
    # and emitted by nothing, so the default "delta to the next symbol" rule
    # handed the parser the first 0x20ec bytes, 17% of the archive.
    #
    # A SHORT BLOB DOES NOT FAIL LOUDLY, which is what makes it worth this
    # comment. The header still parses, so the subsystem still reports its 321
    # effects and allocates a record for every one of them; the ones whose data
    # fell past the cut just come out as records of nothing. An all-zero record
    # has rate 0, and func_0204c584 emits floor((rate + accumulator) / 0x1000)
    # particles with nothing else feeding that sum, so those effects stay
    # permanently silent while their systems live, move and tick exactly as
    # they should. Mario's running and sliding dust is id 0xda and landed in
    # the truncated tail. Cutting inside the TEXTURE block instead is the loud
    # failure: Particle::Texture::AllocTexVram reads a garbage length and
    # Model::GetVramOffset takes its Crash() branch during Stage::InitResources.
    "data_02075f14:0xc214",
    "data_0208f668",
    # One 8-byte constant the particle render family reads (func_0204b028,
    # func_0204b244) while building the camera-facing quad.
    #
    # data_02099fb4 and data_02099fbc USED TO BE HERE and were not constants at
    # all: each is two DS code addresses, indexed by the billboard's axis and
    # flag fields and called through. Emitted as ROM bytes they were a direct
    # jump into unmapped host memory, which is what crashed a play session on
    # the Bob-omb warp. They are hosted with real function pointers in
    # hal/particle_vtable.cpp now; putting either name back here would define
    # the symbol twice and, if it won, restore the crash.
    "data_02099fc4",
    # cstd::atan2's lookup table: atan(i/1024) in binangs, i = 0..0x400, so
    # 0..0x2000 (45 degrees) and the quadrant fixups do the rest. EVERY
    # heading in the game runs through it -- Vec3_HorzAngle, Vec3_VertAngle,
    # the camera's own rotation, the walk's slope math. Zeroed storage in the
    # HAL made atan2 answer 0 for every off-axis direction.
    "data_020994e0",
    # ---- VS wiring lane (ov075, scene 6) ----------------------------------
    # Five OAM/layout records the VS menu's .data table at 0x0211c968 points
    # at (patched to host addresses in hal/scene_vs_menu.cpp), and the five
    # LZ-compressed sub-screen tile blobs data_0208a0e4 indexes by language.
    # All ten are plain arm9 .data with NO relocation inside any span
    # (checked over config/arm9/relocs.txt by span), so ROM bytes are the
    # right storage; the two tables that DO carry relocations (0x0208a0e4
    # itself, and the typeinfo records) are hosted with real host pointers in
    # hal/scene_vs_menu.cpp instead -- the data_02099fb4 lesson. Plus
    # data_0208f274, one relocation-free word func_ov075_021192bc reads.
    "data_02098be8",
    "data_02098c98",
    "data_02098dd8",
    "data_02098ef8",
    "data_020990f8",
    "data_02096f08",
    "data_02097230",
    "data_020975d4",
    "data_02097950",
    "data_02097ca4",
    "data_0208f274",
    # the five per-language VS TEXT BANKS Message::LoadTextVS points
    # data_0209fd14 at (reloc-free spans; the pointer itself is set by the
    # matched TU at run time, so nothing here bakes a DS address)
    "data_020937bc",
    "data_0209325c",
    "data_02093d7c",
    "data_0209289c",
    "data_02092d3c",
    "data_02092808",
    "data_02092810",
    "data_02092824",
    # VS lobby sub-screen: three palette/tile id runs func_0200f13c walks, and
    # the five OamAttr lists data_0208a0f8 points at (that table is hosted
    # with host pointers in hal/scene_vs_menu.cpp). All reloc-free by span.
    "data_0208a120",
    "data_0208a148",
    "data_0208a15c",
    "data_02098b58",
    "data_02098d20",
    "data_02098e60",
    "data_02098fa0",
    "data_02099068",
    "data_02075b14",
    "data_02075d14",
    "data_0209283c",
    "data_0209285c",
    "data_0209287c",
    # the four records data_020927a0 points at (table hosted with host
    # pointers in hal/scene_vs_menu.cpp; records are reloc-free)
    "data_02092780",
    "data_02092788",
    "data_02092790",
    "data_02092798",
    "data_0208a10c",
    "data_0208a134",
    "data_02092818",
    "data_020756b0",   # D-pad direction -> binang table (Stage::CheckInput)
    # CheckInput's three BUTTON REMAP MAPS: 16 u16 rows each, raw pad bit ->
    # Ctrl held/pressed mask (mode 0 is level play: A->1 punch, B->2 jump,
    # R->0x400 crouch, Y->0x800 run, L->0x4000 snap-behind). data_0209214c
    # below is the per-mode POINTER table over these and its words are DS
    # addresses (the relocated-pointer-table class), so the level boot
    # repoints entries 0..2 at these hosted copies rather than dereferencing
    # it -- tests/walk_window.cpp, the input staging block.
    "data_02075650", "data_02075670", "data_02075690",
    # Per-character voice-id offset. Sound::PlayCharVoice is
    # Play(1, baseId + data_02075250[ch], v), and the bytes read 00/40/80/c0
    # -- the VOICE SEQARC is laid out in 0x40-entry blocks, one per
    # character, so zeroed storage would give every character Mario's voice.
    "data_02075250",
    # func_02048fd4's 9 (near, far) distance-limit pairs for 3D sound
    "data_020821c0",
    "data_0209214c",   # per-mode button remap pointer table (CheckInput)
    # tier-2 state wave: fader/level/message tables
    "data_02086f20",
    "data_0208e420",
    "data_0208eeec",
    "data_0208eeee",
    "data_02086f14",
    "data_02086f2c",
    # death states: SetNextLevel's sublevel -> (next level, entrance) table
    "data_02075638",
    "data_02092664",   # Scene::SetSceneToSpawn's pending-scene ID
    # run link60 (lane S1, the scene boot): the three arm9 .data words the
    # STAR SELECT reads that no level path ever did. All three are ROM bytes
    # and none is safe as zeroed storage: data_0208ee00 is the "3D engines are
    # on" latch Enable3dEngines sets, and the two 16-byte records are the
    # OAM/G2 tables func_0205cb68's VRAM map walks.
    "data_0208ee00",
    "data_0208eb8c",
    "data_0208eb9c",
    # gate 31, the level handoff. Two 0xd0-byte tables, 52 entries each --
    # the ROM's whole level count. data_02092208 is the LVL_Overlay address
    # per level (Stage::InitResources indexes it); data_020758c8 the overlay
    # id that carries it (LoadLevelOverlays indexes it, and it holds level+8
    # for every one of the 52). Nintendo's own answer to "what is level N",
    # so the port asks it rather than keeping a list beside it.
    "data_02092208",
    "data_020758c8",
    # SetNextLevel's return-to-level latch. The ROM ships it -1 ("no recorded
    # return point"), and the port had it as zeroed host storage -- so the
    # first ExitLevel() read 0 and asked for level 0 instead of the castle
    # grounds. Its two neighbours 0x02092118/0x02092120 were already here.
    "data_0209211c",
    "data_0208a0e0",   # the controller count SetNumPlayers writes
    "data_020889b0",   # func_0200ee68's demo/cutscene flag block (NoControl)
    "data_020755bc",   # character -> wipe type (StartExitCharacterWipe)
    # gate 13, the real Camera: the constant vectors the follow chain reads.
    # The mode-preset table itself is a CONTIG run, not a NAMED entry.
    "CAM_SPACE_CAM_POS_ASR_3",   # camera-space eye offset, Behavior's weather
    "data_02086efc",   # up vector {0, 0x1000, 0}    -- Render's LookAt_
    "data_02086f08",   # {0, 0, -0x1000}, the degenerate-look fallback
    "data_02086f38",   # per-level XZ bounds, low
    "data_02086f48",   # per-level XZ bounds, high
    "data_02086cc8",
    "data_02086d50",
    "data_02086e84",
    "data_02086f58",
    "data_02086e90", "data_02086e9c", "data_02086ea8", "data_02086eb4",
    "data_020874f4",   # first record past the mode table
    # gate 14, the level boot: the two kuppa scripts
    # ContinueKuppaScriptIfNecessary compares the pending pointer against.
    "data_02087c00", "data_02089608",
    # run linkw wave 2 / lane w2-e: a third kuppa script, the one
    # func_0200eec8 hands straight to RunKuppaScript. Same shape as the two
    # above -- an opcode stream that runs out into zero padding well inside
    # its 0x50 extent, so the next-symbol delta is the whole script and no
    # size pin is needed. Mounted because func_0200eec8 is the last
    # unresolved link on ov002's real St_EndingFly_Main chain
    # (func_ov002_020c3d1c -> func_ov002_020c3bdc -> func_0200eec8).
    "data_02088610",
    # the Camera's SpawnInfo, for the entrance path's actor registry
    "Camera_SpawnInfo",
    # the STAGE's, for the boot spine: the ActorBase constructor reads the
    # record's +4/+6 halfwords back as the two processing-list priorities, so
    # the Stage cannot be built without it (port/hal/stage_bridges.cpp).
    "_ZN5Stage9spawnDataE",
    # sublevel -> level-part table (GetLevelPart, the death-table index)
    "data_02075264",
    # The model-walk constants the old BSS boundary hid. data_02099f88 is the
    # DEFAULT SCALE VECTOR {0x1000, 0x1000, 0x1000} func_02044534 copies when
    # Render is given no scale argument; with it zeroed the billboard part
    # walk builds an all-zero 3x3 and every vertex of every billboard model
    # lands on the model's translation point.
    "data_02099f80", "data_02099f84",   # BCA keyframe weight tables
    "data_02099f88",                    # billboard/part-walk default scale
    "data_02099f94",                    # texture-matrix size table
    # THE STAGE'S OWN RENDER SCALE. Stage::RenderModel ends in
    # Model::Render(&data_020755d4) and Stage::RenderModelTransparent in the
    # same call through slot 5, so this Vector3 is the only scale the level
    # model is ever drawn with. {125.0, 125.0, 125.0} in Fix12i.
    "data_020755d4",
    # skyboxID -> vrbox file HANDLE, u16[11] plus padding to the next symbol.
    # Stage::LoadSkybox indexes it at skyboxID - 1 (GetSkyboxID's 0 means "no
    # skybox" and returns early); entry 0 is 2040 = data/vrbox/vr01.bmd, the
    # castle grounds sky.
    "data_02075620",
    # gate 35, the course loop.
    #
    # Actor::GivePlayerCoins' two parallel tables, indexed by coin KIND
    # (0 yellow, 1 red, 2 blue): the coin VALUE and the id of the little
    # count-popup actor the collect spawns. Both are what turns "an actor was
    # touched" into a number on the HUD, so a zeroed pair is a coin worth
    # nothing.
    # u16[3] popup actor ids
    # u32[3] coin values
    # The per-sublevel SOUND row: three bytes each, {group, bank, bgm}, which
    # config names as three separate one-byte symbols (data_02075768/69/6a)
    # because Stage::InitResources reads the three columns with a stride of 3.
    # The size has to be spelled: the delta-to-next-symbol rule would make the
    # first column FOUR BYTES and cut off every level past the first.
    # 0x02075804 is the next real symbol, so the run is 0x9c -- 0x34 rows.
    "data_02075768:0x9c",
    # SUBLEVEL -> COURSE. Every star and every coin record is filed under the
    # answer, so zeroed storage credits all 52 sublevels to course 0. It was
    # also DECLARED two different ways -- decl_common.h has it as `signed
    # char[]`, which is what SublevelToLevel indexes, while the HAL defined it
    # as `short[64]`, so even a filled table would have been read at half
    # stride. The bytes settle it: [1] = 29, the castle grounds.
    "SUBLEVEL_LEVEL_TABLE",
    # THE 3D SOUND DISTANCE LIMITS, and the reason no positional sound effect
    # has ever played on the port. func_02048a1c is the gate every
    # Sound::Play with a Vector3 goes through:
    #
    #     lim = data_02099fac;  ...  if (lim < distance) return 0;
    #
    # and Sound::Play's caller-side response to that 0 is a silent return, so
    # a zeroed limit culls every 3D sound in the game without a word. The ROM
    # byte is 0x226 (550 units). All three are file-backed arm9 data below
    # bss_start; they were sitting in the HAL as zeroed BSS.
    # run link60 Stage 4 (lane SD0): Stage::InitResources' own ROM data.
    #
    # THE THREE FADER-WIPE FILE-ID TABLES. InitResources picks exactly one of
    # them and hands its seven entries to FaderWipe::LoadAndSetFile:
    #   data_02075600  VS mode with a level-specific archive loaded
    #   data_020755f0  sublevel 5
    #   data_020755e0  everything else, which is every level the port boots
    # Each is 0x10 by the delta-to-next-symbol rule and the loop reads seven
    # u16 out of it, so the extra halfword is padding the ROM also carries.
    # Zero relocations in any of the three spans (checked over
    # config/arm9/relocs.txt): these really are numbers.
    "data_020755e0", "data_020755f0", "data_02075600",
    # The word InitResources zeroes on its way out (its last statement block)
    # and Stage::Behavior reads. 0x4, no relocations.
    "data_02092778",
    # THE OTHER TWO COLUMNS OF THE PER-SUBLEVEL SOUND ROW, and read the
    # data_02075768 comment above this one first -- these three entries
    # deliberately emit THREE OVERLAPPING COPIES of the same ROM run.
    #
    # The run is 0x02075768..0x02075804, 0x34 rows of {group, bank, bgm}, and
    # the config names each COLUMN as its own one-byte symbol. Three different
    # readers index three different columns at stride 3:
    #   data_02075768[level*3]   GetSoundGroupID, already emitted at 0x9c
    #   data_02075769[level*3]   Stage::InitResources, the SEQARC bank
    #   data_0207576a[level*3]   Stage::InitResources, the layer-1 BGM
    # Byte-copied from its own start address, each array reads its own column
    # correctly, so the overlap is not a redundancy to be cleaned up -- it is
    # what makes three symbols that alias one ROM run work as three host
    # arrays. Nothing does pointer arithmetic ACROSS the three names (checked:
    # hal/star_flow.cpp walks all three columns off the single data_02075768
    # array, row[0]/row[1]/row[2], which is inside one object), so no reader
    # can observe that the copies are separate objects on the host.
    #
    # BOTH SIZES ARE PINNED AND THE PIN IS THE POINT. The next symbol after
    # data_02075769 is data_0207576a ONE BYTE later, so delta-to-next-symbol
    # hands it a one-byte array while the ROM code indexes it to level 0x33 --
    # the undersized-host-global class exactly. 0x9a is 0x02075804 minus
    # 0x0207576a, which covers the last index either column can form
    # (0x33 * 3 = 0x99).
    "data_02075769:0x9a", "data_0207576a:0x9a",
    "data_02099fa4", "data_02099fa8", "data_02099fac",
    # ...and the other half of the same silence: data_02099fb0 is the COUNT
    # func_02048720 walks looking for a free type-9 (positional) voice. The
    # ROM byte is 4. At 0 the search loop never runs, `pick` stays -1, and
    # every positional sound effect comes back "no voice free". func_02048e54
    # and func_02048e94 also write 4 into it at runtime, and neither is on any
    # path the port takes.
    "data_02099fb0",
]


# Address RUNS whose symbols must stay adjacent in host memory because the
# ROM code walks ACROSS the symbol boundaries. func_0200cb58 sets the camera's
# mode pointer to `index * 0x28 + &data_02086fcc`, and Camera::Behavior /
# Camera::Render then compare that pointer against &data_0208733c and
# &data_0208738c -- symbols 15 and 18 records further along. Emitted as one
# array per symbol is byte-correct but places them at unrelated addresses, so
# every mode past the first reads garbage and no comparison can ever hold.
#
# MSVC grouped sections put them back together: the linker concatenates
# contributions to `.name$suffix` sorted by the full section name, so one
# $NNN per symbol in address order lays the run out in ROM order. Each run
# carries the alignment its deltas share (a delta that is not a multiple of the
# alignment would pad and break the run), and the HAL asserts the result at
# runtime.
#
# (tag, start, end, align). align defaults to 8.
CONTIG = [
    # the camera-mode preset table: 33 records of 0x28, 0x528 total
    ("cammod", 0x02086FCC, 0x020874F4, 8),
    # THE OBJECT-MESSAGE REMAP TABLE, read as TWO OVERLAPPING VIEWS.
    # ObjectMessageIDToActualMessageID walks one interleaved {key,value} u16
    # table at 0x0208eeec: data_0208eeec[i].v (stride 4) is the key column and
    # data_0208eeee[i].v (= data_0208eeec+2, same stride) is the value column.
    # dsd named both halves, so the delta-to-next-symbol rule sizes data_0208eeec
    # at 2 bytes -- and the walk then indexes a 4-byte host array off its end,
    # remapping the sign/NPC object id to garbage so St_Talk_Main's box opens on
    # an out-of-range text id and never activates. Laid out contiguously the two
    # views alias the same bytes exactly as on the DS. The run runs to
    # data_0208f021 (dsd's next name inside the same table, at an ODD address --
    # dsd split the run mid-halfword); the walk reads keys up to index 0x61, past
    # that name, so it has to be one host array too. align 1: the members pack
    # byte-tight so every symbol lands at its exact ROM-relative offset, which is
    # the whole point -- data_0208eeee must sit at data_0208eeec+2. x86 tolerates
    # the unaligned u16 reads the Pair view then does.
    ("objmsg", 0x0208EEEC, 0x0208F074, 1),
]


def symbol_table(root):
    import re
    syms = []
    for line in (root / "config/arm9/symbols.txt").read_text().splitlines():
        m = re.search(r"^(\S+)\s+kind:\S+\s+addr:0x([0-9a-fA-F]+)", line)
        if m:
            syms.append((int(m.group(2), 16), m.group(1)))
    syms.sort()
    return syms


def contig_entries(syms):
    """[(tag, align, [(name, addr, size), ...])] for every run in CONTIG."""
    runs = []
    for tag, start, end, align in CONTIG:
        members = [(a, n) for a, n in syms if start <= a < end]
        out = []
        for i, (a, n) in enumerate(members):
            nxt = members[i + 1][0] if i + 1 < len(members) else end
            size = nxt - a
            if size % align:
                sys.exit(f"{tag}: {n} is {size:#x} bytes, not a multiple of "
                         f"{align} -- ordered sections would pad and break the "
                         "run")
            out.append((n, a, size))
        if not out or out[0][1] != start:
            sys.exit(f"{tag}: no symbol at the run start {start:#x}")
        runs.append((tag, align, out))
    return runs


def named_entries(root, syms):
    addr_of = {n: a for a, n in syms}
    contig_names = {n for _, _, mem in contig_entries(syms) for n, _, _ in mem}
    out = []
    for entry in NAMED:
        # "name:0xSIZE" pins the extent explicitly. The default -- delta to the
        # next symbol, the ovdata.py convention -- is only correct when the
        # config happens to have no symbol inside the object, and for anything
        # the game reads as one blob that is an assumption, not a fact: the
        # config carries page-aligned boundary symbols (0x02078000, 0x02080000)
        # that are not real objects and silently cut whatever spans them. The
        # other reason for a pin: a neighbour that is a COLUMN of the same
        # table rather than the next table (see data_02075768).
        name, _, override = entry.partition(":")
        if name in contig_names:
            continue   # the run owns it
        a = addr_of[name]
        if override:
            out.append((name, a, int(override, 0)))
            continue
        nxt = next((s for s, _ in syms if s > a), a + 4)
        out.append((name, a, max(4, nxt - a)))
    return out


def main():
    root = pathlib.Path(__file__).resolve().parents[2]
    img = root / "extracted/arm9_dec.bin"
    if not img.exists():
        sys.exit(f"missing {img} -- extract the ROM first")
    anchor = img.read_bytes()
    a = 0x0205A548 - BASE
    if anchor[a:a + 4] != b"\x0c\x10\xb0\xe8":
        sys.exit("arm9_dec.bin failed the Copy36Bytes anchor -- wrong image/base")

    # The particle archive is the one NAMED entry whose true length is written
    # down inside it, so check the pin against the header rather than trusting
    # the comment next to it. Worth checking because getting this wrong is
    # SILENT: the parse still succeeds and the effects that fell off the end
    # simply never emit.
    spa = 0x02075F14 - BASE
    if anchor[spa:spa + 8] != b" APS10_1":
        sys.exit("no SPA magic at 0x02075f14 -- the particle blob moved")
    tex_size, tex_off = struct.unpack_from("<II", anchor, spa + 0x14)
    spa_size = tex_off + tex_size
    pinned = next((int(e.partition(":")[2], 0) for e in NAMED
                   if e.startswith("data_02075f14:")), None)
    if pinned != spa_size:
        sys.exit(f"particle blob is pinned at {pinned:#x} but its own SPA "
                 f"header says {spa_size:#x} -- fix the pin in NAMED")
    for addr, length, _ in TABLES:
        if addr + length > BSS_START:
            sys.exit(f"{addr:#x}+{length:#x} crosses into BSS -- not file-backed")
    data = img.read_bytes()

    # ROM-CLEAN (2026-08-10, --rom-clean): the shipped exe must embed ZERO ROM
    # bytes. Under the flag every array below is emitted ZEROED and a boot-time
    # loader memcpys the real bytes out of build/assets/romdata.bin. Without the
    # flag (dev default) the arrays keep their bytes exactly as before.
    #
    #   collect(name, raw_bytes): records one blob entry AND returns the C brace
    #   body -- "0" (zeroed, bytes go to the sidecar part) under --rom-clean, the
    #   real bytes otherwise. Byte-honesty: the slice for each symbol is the
    #   identical bytearray the baked emitter prints, so post-load bytes are
    #   byte-identical to the baked build -- only WHERE they live changed. The
    #   loader (port/hal/romdata_loader.cpp) verifies each slice size and a
    #   whole-file sha against the manifest and aborts loudly on any mismatch.
    romblob_common.set_rom_clean(sys.argv)
    entries = []            # (symbol_name, raw_bytes) in emission (blob) order

    def collect(name, raw):
        entries.append((name, bytes(raw)))
        return "{ %s }" % romblob_common.init_body(raw)

    out_path = pathlib.Path(sys.argv[1]) if len(sys.argv) > 1 else (
        root / "build/port/host-src/romdata.c")
    out_path.parent.mkdir(parents=True, exist_ok=True)

    lines = ["/* GENERATED by port/tools/romdata.py from extracted/arm9_dec.bin.",
             " * ROM constants stay out of git; regenerate locally. */"]
    if romblob_common.ROM_CLEAN:
        # --rom-clean zeroes the arrays; the bytes load from romdata.bin.
        lines.append("#include <string.h>")   # for the apply memcpy
    lines.append("")
    for addr, length, ctype in TABLES:
        off = addr - BASE
        blob = data[off:off + length]
        if len(blob) != length:
            sys.exit(f"short read at {addr:#x}")
        # element width + unpack format PER TYPE. A missing entry is a hard
        # error rather than a silent fallback: the old `4 if int else 2` read
        # the byte tables as halfwords and emitted half as many, half-wrong
        # elements (see TABLES).
        ELEM = {
            "int": (4, "<i"), "unsigned int": (4, "<I"),
            "short": (2, "<h"), "unsigned short": (2, "<H"),
            "char": (1, "<b"), "unsigned char": (1, "<B"),
        }
        if ctype not in ELEM:
            sys.exit(f"romdata: no element width for C type {ctype!r}")
        width, fmt = ELEM[ctype]
        if length % width:
            sys.exit(f"romdata: {addr:#x} length {length} is not a multiple "
                     f"of {width} ({ctype})")
        # collect() records the raw bytes for the blob (the loader memcpys them,
        # which on host x86-LE == the DS bytes). The C body must be TYPED element
        # values, not raw bytes -- an int[12] cannot take 48 byte initialisers.
        # Under --rom-clean collect() zeroes the body; otherwise emit the real
        # element values here.
        name = f"data_{addr:08x}"
        collect(name, blob)
        if romblob_common.ROM_CLEAN:
            body = "0"
        else:
            vals = [struct.unpack_from(fmt, blob, i)[0]
                    for i in range(0, length, width)]
            body = ", ".join(str(v) for v in vals)
        lines.append(f"{ctype} {name}[{length // width}] = {{ {body} }};")
        lines.append("")
    syms = symbol_table(root)
    for name, addr, size in named_entries(root, syms):
        if addr + size > BSS_START:
            size = BSS_START - addr
        blob = data[addr - BASE:addr - BASE + size]
        lines.append(f"__declspec(align(8)) unsigned char {name}[{size}] = "
                     f"{collect(name, blob)};")
    lines.append("")

    # Grouped-section runs (see CONTIG).
    for tag, align, members in contig_entries(syms):
        lines.append(f"/* run .{tag}: {members[0][1]:#010x} .. "
                     f"{members[-1][1] + members[-1][2]:#010x}, "
                     f"{len(members)} symbols, laid out in ROM order "
                     f"(align {align}) */")
        for i, (name, addr, size) in enumerate(members):
            sec = f".{tag}${i:04d}"
            lines.append(f'#pragma section("{sec}", read, write)')
            blob = data[addr - BASE:addr - BASE + size]
            lines.append(f'__declspec(allocate("{sec}")) '
                         f'__declspec(align({align})) '
                         f"unsigned char {name}[{size}] = {collect(name, blob)};")
        first, last = members[0], members[-1]
        lines.append(f"unsigned {tag}_run_base = (unsigned)&{first[0]}[0];")
        lines.append(f"unsigned {tag}_run_end = (unsigned)&{last[0]}[0] + "
                     f"{last[2]};")
        lines.append(f"unsigned {tag}_run_span = "
                     f"{members[-1][1] + members[-1][2] - members[0][1]};")
    lines.append("")

    # The archive-mount table at data_0208ecf4: 13 entries of
    # {ptr, heap, u16 idBase, u16 idEnd, char *shortName, char *narcPath}.
    # Interior file IDs (>= 0x8000) resolve as narc[id - idBase]; the fs
    # seam consumes this host-shaped copy (see port/hal/fs.cpp).
    #
    # NOT a ROM-content array: it holds host `const char *` string literals (the
    # narc paths) and host-shaped {base,end} ids, not raw ROM bytes, so it stays
    # a real initialiser -- see the syms-pack verdict in out/rom-clean/DESIGN.md.
    lines.append("struct port_arc_entry { unsigned short base, end;"
                 " const char *narc; };")
    ents = []
    off = 0x0208ECF4 - BASE
    for i in range(13):
        blob = data[off + i*0x14: off + (i+1)*0x14]
        base_id, end_id = struct.unpack_from("<HH", blob, 8)
        path_ptr = struct.unpack_from("<I", blob, 0x10)[0]
        p = data[path_ptr - BASE:path_ptr - BASE + 64].split(b"\0")[0]
        ents.append('    { %d, %d, "%s" },' % (base_id, end_id,
                                               p.decode("ascii").lstrip("/")))
    lines.append("struct port_arc_entry port_archive_map[13] = {")
    lines.extend(ents)
    lines.append("};")
    lines.append("")

    # The boot-time loader face (--rom-clean only). romblob.py's
    # port_romdata_load_all() calls this with a pointer to romdata's section of
    # the verified romdata.bin; each memcpy fills one now-zeroed array. Emitting
    # it here (not centrally) keeps it in the one TU that names every symbol. In
    # a baked build emit_apply returns nothing, so the file is byte-identical to
    # the pre-ROM-CLEAN output.
    apply = romblob_common.emit_apply("romdata", entries)
    if apply:
        lines.append("/* Fill the zeroed arrays above from romdata.bin (already"
                     " read + verified). */")
        lines.extend(apply)
        lines.append("")
    out_path.write_text("\n".join(lines), encoding="ascii")

    # The sidecar blob PART. romblob.py concatenates every emitter's part into
    # build/assets/romdata.bin and writes the whole-file manifest + dispatch.
    total = romblob_common.write_part(out_path, "romdata", entries)
    print(f"romdata -> {out_path} ({total} ROM bytes moved to {out_path}.blobpart)")


if __name__ == "__main__":
    main()
