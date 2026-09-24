/* RETIRED HOST COPY of func_ov006_020fb7e0 (ov006 0x020fb7e0),
 * dScMgPachinko_c's per-item scoring tick. Run mg5 lane HISCORE wrote it; run
 * linkfull wave 27 lane P1 retired it. This file now defines nothing: it stays
 * an empty translation unit because port/CMakeLists.txt's SCENE_MG_SOURCES
 * list, which enrolls it, has another owner.
 *
 * WHY IT EXISTED. The owner saw "HIGH SCORE 999999" with "SCORE 0" on scene
 * 368. The award routine func_ov006_020fbb2c takes the points as its third
 * argument, and the cartridge hands them across in r2 out of the same ldrh
 * that makes the != 0 test (0x020fb8c0 `ldrh r2, [r0, #0x62]`, then
 * `bl 0x20fbb2c` at 0x020fb8d4 with r2 untouched). The matched source of the
 * day declared the callee with two parameters and called it with two, so under
 * MSVC every award added a stack slot nobody wrote, the score climbed to the
 * ROM's own 999999 cap and the high score kept it. This file was the src body
 * with the third argument declared and passed.
 *
 * WHY IT IS GONE. src/func_ov006_020fb7e0.c now declares
 * func_ov006_020fbb2c(char *c, int idx, unsigned short val), which is what
 * src/func_ov006_020fbb2c.c defines, and calls it with
 * *(u16 *)(b + 0x5962), the value the line above it has just tested. That is
 * everything this copy changed; the loop, the counters and the two clears are
 * the same statements. The TU is 2004/b56 strict MATCHING and linkcheck
 * VERIFIED blind 0 (runs/linkfull/out/P1/), it defines its own flat C name, and
 * it is on port/slice_w28_p1.txt.
 */
