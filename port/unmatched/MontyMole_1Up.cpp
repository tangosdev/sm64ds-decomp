/* HOST COPY RETIRED (run linkfull wave 23, lane HGFRONT1): func_ov080_02124088,
 * MONTY_MOLE's defeat helper (the anim flip, the poof dust, and the mole-hole
 * census that pops the 1-Up when the seventh mole of the group dies), is the
 * matched TU src/func_ov080_02124088.cpp again, on port/slice_w23_hostgen.txt.
 *
 * This file held the body because the TU's own `extern "C" void
 * func_ov080_02124088(char*)` fought include/decl_common.h's `(void*)`
 * declaration (C2733). The synced TU compiles clean under walk_window's own
 * flags today, it is MATCHING at 2004/b56 with --strict-relocs against the
 * cartridge (ov080 0x02124088, 0x180 bytes), and the object MSVC builds from
 * it makes the same stores, loads and calls with the same arguments in the
 * same order as the body that stood here. The slice file has the evidence.
 *
 * The file stays on the three target lists that name it (one CMake line each,
 * outside this lane's region) and compiles to nothing.
 */
