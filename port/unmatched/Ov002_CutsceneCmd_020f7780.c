/* HOST COPY RETIRED (run linkfull wave 23, lane HGFRONT1): func_ov002_020f7780,
 * cutscene-object command slot 2, is the matched TU src/func_ov002_020f7780.c
 * again, compiled through tools/hostgen.py (PORT_HOSTGEN_TU) and named by
 * port/slice_w23_hostgen.txt.
 *
 * This file held the body for one cast: the src adds 0x8e to a loaded void*
 * (`*(short *)(*(void **)(p + (idx << 2)) + 0x8e)`), the GNU byte arithmetic
 * mwcc accepts and MSVC refuses with C2036. hostgen's VOIDPP_ARITH rewrite
 * makes exactly that cast now -- the loaded pointer becomes char* before the
 * 0x8e, same address, same load -- and the object MSVC builds from the
 * generated copy is instruction for instruction the one this file built. The
 * src is MATCHING at 2004/b56 with --strict-relocs against the cartridge
 * (ov002 0x020f7780, 0x240 bytes).
 *
 * The file stays on SLICEINTRO_SOURCES (outside this lane's CMake region) and
 * compiles to nothing.
 */
