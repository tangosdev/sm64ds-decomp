/* ==========================================================================
 * RETIRED -- THIS FILE IS NOT IN ANY BUILD. Run link100 wave 15, lane SEAT15D,
 * LINK15 BATCH 3. The one token is in src/ now: src/func_ov006_020d3ba0.cpp:34
 * reads `extern int data_0209d4b8;`, so the arm9 RNG seed has exactly one
 * definition again and it is hal/scene_mg_faces.cpp's. main went further than
 * the token while it was there: it dropped the NONMATCHING banner this copy
 * still carried, put `extern` on the five data_ov006_0212e1xx arrays this copy
 * spells as bare definitions, and deleted the u64 launder masks. The matched
 * TU is taken through port/slice_l15mg.txt and the SEAT15D block in
 * port/CMakeLists.txt, and this file's entry in SLICE_S371_SOURCES there is
 * commented out. The body is deleted so the two copies cannot both be taken;
 * the banner below is kept because it is the derivation, and because its
 * "hostgen --extern-data IS THE WRONG TOOL FOR IT" paragraph is the reusable
 * finding.
 *
 * The rest of the file, from here down, is the note as it was written.
 * ==========================================================================
 */

/* HOST COPY of src/func_ov006_020d3ba0.cpp, dScMgAmida_c (actor id 0x173,
 * scene 371). Run mg9, lane S371.
 *
 * ONE TOKEN CHANGED, and it is the word `extern`. Line 37 of the src TU reads
 *
 *     int data_0209d4b8;
 *
 * inside its `extern "C"` block. mwccarm accepts that as a TENTATIVE
 * DEFINITION and merges it with the real one; C++ has no tentative
 * definitions, so under MSVC it is a strong definition of an arm9 global the
 * HAL already owns:
 *
 *     func_ov006_020d3ba0.c.obj : error LNK2005: _data_0209d4b8 already
 *                                 defined in scene_mg_faces.cpp.obj
 *
 * AND THE HAL HAS TO WIN. hal/scene_mg_faces.cpp defines data_0209d4b8 inside
 * DSSTATE_BEGIN, sized by ROM SPAN rather than by field width, so it is one of
 * the hosted DS globals the save-state layout and the selftest BMP gate both
 * depend on the position of. A copy of that word living in an ov006 object
 * file would be a SECOND RNG seed: src/_ZN12dScMgAmida_c8BehaviorEv.cpp (this class's own
 * Behavior) declares the same symbol `extern int` and calls
 * RandomIntInternal(&data_0209d4b8) against it, so the two TUs of one class
 * would have advanced different seeds and nothing would have said so.
 *
 * THE FILE CANNOT SIMPLY BE COMPILED AS C, which is the cheaper fix everywhere
 * it works. This one is //cpp-marked and needs C++ for real: it declares a
 * THIRTY-SEVEN-virtual `struct Obj` and dispatches `m_90()` -- vtable slot 36,
 * the 37th virtual that makes this the one true width-37 class of the minigame
 * family -- through it, and it opens `namespace G2S`.
 *
 * AND hostgen --extern-data IS THE WRONG TOOL FOR IT, which was checked rather
 * than assumed. That is the transform port/CMakeLists.txt uses on
 * Actor::BeforeBehavior for this exact defect, and it produces the right
 * `extern` line here too -- but it wraps every .c source in `extern "C" { }`
 * unconditionally (tools/hostgen.py emit()), and this file's
 * `namespace G2S { char *GetBG0CharPtr(void); }` inside that wrap would take C
 * linkage and stop naming the arm9 body its two sibling TUs reach. That is the
 * hazard port/CMakeLists.txt's LANGUAGE CXX block already states in words:
 * "the wrap is wrong for them".
 *
 * SO THIS IS THE MgPachinko_OamRender TREATMENT, named in port/slice_pch.txt
 * wave 4 for the identical shape -- a //cpp-marked TU whose extern "C" block
 * writes a bare `void* data_ov006_0213ac24;` and collides with the mount --
 * and its rule is the one followed here: "the host copy is the src body
 * verbatim with `extern` added". THE DECOMP-SIDE ONE-TOKEN FIX IS ROUTED, NOT
 * TAKEN: src/func_ov006_020d3ba0.cpp should read `extern int data_0209d4b8;`,
 * and whether that still builds under mwccarm is a byte-gated-tree question.
 * The TU is bannered NONMATCHING already, so nothing here is at risk of
 * silently un-matching a matched body.
 *
 * NO HAND TRANSCRIPTION. This file was produced mechanically and a reviewer
 * can reproduce it byte for byte:
 *
 *     sed -e '1d' -e 's/^int data_0209d4b8;$/extern int data_0209d4b8;/' \
 *         src/func_ov006_020d3ba0.cpp
 *
 * (the `1d` drops the `//cpp` marker line). Everything below this comment is
 * the src TU unchanged, banner included.
 */

/* THE BODY LIVES IN src/func_ov006_020d3ba0.cpp, on port/slice_l15mg.txt. The
   sed recipe the banner above gives no longer reproduces this file from it,
   and that is the point: src/ moved past the one token. */
