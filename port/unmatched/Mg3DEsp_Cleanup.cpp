/* ==========================================================================
 * RETIRED -- THIS FILE IS NOT IN ANY BUILD. Run link100 wave 15, lane ESP3D,
 * off LINK15 BATCH 3 / lane SEAT15D's out/SEAT15D/bugs.md section 2. The
 * THE DECOMP-SIDE FIX IS ROUTED, NOT TAKEN paragraph below asked for exactly
 * this: src/_ZN12dScMg3DEsp_c16CleanupResourcesEv.cpp now names
 * data_ov006_02141e9c and data_ov006_02141e74 directly (declared as the real
 * SharedFilePtr type, not through decl_common.h's G0/G1 placeholders), so the
 * LNK2005 this banner describes no longer applies to the args -- it applies
 * to the NAME instead, because the matched TU is a real C++ member
 * (`s32 dScMg3DEsp_c::CleanupResources()`) and does not itself define the
 * flat Itanium name this file's one caller (hal/scene_mg_psycheout.cpp)
 * calls. The seat is a one-row flip: port/faces_sync.txt's row for this
 * symbol moves F -> R, so facegen generates the REVERSE face -- it defines
 * the flat name and calls the matched member -- instead of a forwarder that
 * would now collide with the src TU. The matched TU is taken through
 * port/slice_l15mg.txt. The body is deleted so the two copies cannot both be
 * taken; the banner below is kept because the disassembly in it is the
 * derivation and the G0/G1 finding stays true and reusable.
 *
 * The rest of the file, from here down, is the note as it was written.
 * ==========================================================================
 */

/* PORT_HOST_ABI. _ZN12dScMg3DEsp_c16CleanupResourcesEv, dScMg3DEsp_c's vtable slot 3
 * (CleanupResources), and the reason it cannot be compiled from src: BOTH OF
 * ITS ARGUMENTS ARE SPELLED AS SHARED PLACEHOLDER GLOBALS THAT EXIST IN NO
 * CONFIG, AND ONE OF THE TWO IS ALREADY BOUND TO A DIFFERENT ADDRESS.
 * Run mg9, lane PSY.
 *
 * The precedent is unmatched/MgMemory2_SubObjCtor.cpp, which carries the same
 * defect for func_ov006_020c0a48's three file ids. This one is worse in one
 * specific way and that difference is the part worth reading.
 *
 * ---- WHAT src SAYS -------------------------------------------------------
 *
 *     extern int G0[];                       // src/_ZN12dScMg3DEsp_c16CleanupResourcesEv.cpp
 *     _ZN13SharedFilePtr7ReleaseEv(G0);
 *     _ZN13SharedFilePtr7ReleaseEv(G1);      // G1 comes from decl_common.h
 *     return 1;
 *
 * G0 and G1 are PLACEHOLDER NAMES a decompilation pass left behind for two
 * literal-pool words. mwccarm never had an opinion, because the byte gate
 * compiles this TU on its own and matches the instruction stream -- a
 * pc-relative load is a pc-relative load whatever the pool holds.
 *
 * ---- WHAT THE ROM PASSES -------------------------------------------------
 *
 * Read out of extracted/overlays/overlay_0006.bin at base 0x020bfec0. The
 * whole body is ten instructions and a two-word pool, 12 words = 0x30:
 *
 *     020e9cec  stmdb sp!,{lr} / sub sp,sp,#4
 *     020e9cf4  ldr r0,[pc,#0x18]     -> pool 020e9d14 = 0x02141e9c
 *     020e9cf8  bl  0x02017b64        SharedFilePtr::Release
 *     020e9cfc  ldr r0,[pc,#0x14]     -> pool 020e9d18 = 0x02141e74
 *     020e9d00  bl  0x02017b64        SharedFilePtr::Release
 *     020e9d04  mov r0,#1
 *     020e9d08  add sp,sp,#4 / ldm sp!,{lr} / bx lr
 *
 * The two pool words are the only two between the last instruction at
 * 020e9d10 and the next symbol, so there is nothing else they could be. And
 * they are not guesses in any case: src/__sinit_ov006_02130a08.c CONSTRUCTS
 * both, as `func_02017a24(&data_ov006_02141e9c, 0x1ef)` and
 * `func_02017a24(&data_ov006_02141e74, 0x202)`, and this class's own
 * InitResources loads the same two words out of ITS pool at 0x020ea1bc and
 * 0x020ea1d0 to hand to func_020179b4 for the two Models at +0x4f38 and
 * +0x4f88. Constructed by the class constructor, loaded by InitResources,
 * released by CleanupResources: three independent sites, same two addresses.
 *
 * ---- WHY A PER-SOURCE -D ROW DOES NOT WORK HERE, AND THIS IS THE FINDING --
 *
 * The tree's standing repair for a bare placeholder is a per-source rename:
 * port/CMakeLists.txt already carries `VT=data_ov006_0213e24c` for
 * src/actors/dScMgPanel_c.cpp and `VT0=_ZTV13daObjWakame_c` for another. That
 * repair works when the placeholder is declared in the TU and the target is
 * not declared anywhere the TU can see.
 *
 * IT CANNOT WORK WHEN BOTH NAMES ARE IN include/decl_common.h, and G1 is:
 *
 *     include/decl_common.h:396    extern int G1[];
 *     include/decl_common.h:3041   extern void*data_ov006_02141e74;
 *
 * so `-DG1=data_ov006_02141e74` rewrites line 396 into
 * `extern int data_ov006_02141e74[];` and line 3041 is then a redefinition
 * with a different type of indirection. Measured, not predicted: that is
 * error C2372 at decl_common.h(3041), and the same for G0 at the TU's own
 * line 8. A lane reaching for the -D repair should CHECK WHETHER THE TARGET IS
 * IN decl_common.h FIRST; if it is, the repair is a host copy, not a define.
 *
 * ---- THE DELTA FROM src, line by line ------------------------------------
 *
 * The body below is src/_ZN12dScMg3DEsp_c16CleanupResourcesEv.cpp verbatim except:
 *
 *   1. the `extern int G0[];` declaration is gone and decl_common.h is not
 *      included, so G1 does not arrive either;
 *   2. the two call sites name the mount symbols the pool words resolve to.
 *
 * Nothing else. Two releases in the ROM's order and `return 1`.
 *
 * src/_ZN12dScMg3DEsp_c16CleanupResourcesEv.cpp is therefore OUT of port/slice_psy.txt: listing it
 * beside this file would be an LNK2005 on _ZN12dScMg3DEsp_c16CleanupResourcesEv.
 *
 * THE DECOMP-SIDE FIX IS ROUTED, NOT TAKEN. The src TU should name
 * data_ov006_02141e9c and data_ov006_02141e74 rather than two shared
 * placeholders, and whether it still builds byte-identically under mwccarm is
 * a byte-gated-tree question this lane does not answer.
 */

/* THE BODY LIVES IN src/_ZN12dScMg3DEsp_c16CleanupResourcesEv.cpp, on
   port/slice_l15mg.txt, and is reached by the GENERATED REVERSE FACE now
   that port/faces_sync.txt's row for this symbol reads R. It declares
   data_ov006_02141e9c and data_ov006_02141e74 as the real SharedFilePtr type
   (include/SharedFilePtr.h) rather than the void* this file's mount-symbol
   declarations used, and calls .Release() on each rather than the bare flat
   function this file forwards through -- the same two addresses, the same
   ROM order, `return 1`. */
