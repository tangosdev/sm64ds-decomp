/* HOST COPY of src/func_ov007_020b4b5c.c -- THE TITLE SCREEN'S CELL SIZE, with
 * the one thing the ROM leaves implicit spelled out: the CAMERA.
 * ov007 0x020b4b5c, 0x70 bytes (112, 28 ARM instructions).
 *
 * ============================ WHAT THIS IS ================================
 *
 * A DISPLACEMENT, NOT AN INTERIM, on the precedent
 * port/unmatched/Ov007_ImplicitR0_020add3c.cpp established and
 * Ov007_RideThrough_020be980.cpp before it. src/func_ov007_020b4b5c.c is
 * MATCHED and stays matched; it is out of the port's ov007 slice only, and this
 * body links in its place. Same defect class as add3c, one register over: that
 * one loses an implicit r0, this one loses an implicit r1.
 *
 * THE ROW IS NOT NEW. port/ov007_seat.txt section 5d's re-derived census lists
 * it by name -- "func_ov007_020c43bc  defined 2, declared 1 by
 * src/func_ov007_020b4b5c.c" -- as one of the sixteen FAULTING-DIRECTION rows,
 * and port/tools/aritycheck_plainfunc_baseline.txt line 159 carries it as an
 * accepted baseline row. port/tools/tailjump_guard.py holds a note on it too
 * ("Three pushes before the call and only one of them is an argument; c43bc
 * reads two"). WHAT IS NEW IS THE SYMPTOM: this is the reason the title
 * screen's own art draws at three and a half times the size it should.
 *
 * ============================ THE SEAM ====================================
 *
 * TWO matched TUs, and both are right on ARM:
 *
 *   src/func_ov007_020b4b5c.c    extern void func_ov007_020c43bc(void *p);
 *                                func_ov007_020c43bc(o);      <- passes one
 *   src/func_ov007_020c43bc.cpp  void func_ov007_020c43bc(A *a, B *b)
 *
 * The ROM's frame, read off extracted/overlays/overlay_0007.bin with capstone
 * at the config-aligned base 0x020ad660 (NOT the dsd export, which is short by
 * 1248 bytes for this overlay):
 *
 *   020B4B5C  push {r4, r5, r6, lr}
 *   020B4B60  mov  r6, r0
 *   020B4B64  ldr  r4, [r6, #4]
 *   020B4B68  mov  r0, #0x1000
 *   020B4B6C  str  r0, [r4, #0x40]
 *   020B4B70  ldr  r2, [r4, #0x40]
 *   020B4B74  mov  r0, r4
 *   020B4B78  mov  r5, r1            <- SAVES its own second argument
 *   020B4B7C  str  r2, [r4, #0x3c]
 *   020B4B80  bl   #0x20c43bc        <- and r1 STILL HOLDS IT
 *   ...
 *   020B4BA0  mov  r1, r5            <- spelled out for the SECOND call
 *   020B4BC4  bl   #0x20c4388
 *
 * r1 is never written between the entry and the branch at 0x020b4b80, so the
 * callee's second argument is this frame's own second argument. The `mov r5,
 * r1` is the save for the later call, and that later call DOES restore r1 --
 * which is the proof that the value is an argument and not an accident: the
 * ROM only bothers to reload r1 where something clobbered it.
 *
 * ALIGNMENT WAS PROVEN BEFORE THE TARGET WAS READ, on the byte-matched
 * neighbour func_ov007_020c41dc (0x020c41dc, the OTHER caller of the same
 * callee): its disassembly reproduces src/func_ov007_020c41dc.c instruction for
 * instruction, including the `mov r1, r2` at 0x020c4200 that passes ITS third
 * parameter as the camera. That caller is correct and is left alone; this one
 * is the only frame in the tree that loses the value.
 *
 * ========================= WHAT IT COSTS, MEASURED ========================
 *
 * func_ov007_020c43bc is the cell system's SIZE routine. Given a record with a
 * texel width at +0x0c, a texel height at +0x0e and a depth at +0x10, and given
 * a camera, it writes the record's world-space quad size at +0x20 and +0x24:
 *
 *     f20 = fdiv( (fc << 12) * ((sin * |depth| + 0x800) >> 12) >> 12,
 *                 cos * 0x60 )
 *         = texels * depth * tan(halfFov) / 96
 *
 * which is the DS's one-texel-one-pixel rule written out: 96 is half the 192
 * scanlines, and depth * tan(halfFov) is the half-height of the view at that
 * depth. It reads the field of view from the camera at +0xd4, through the same
 * sine table (data_02082214) and the same index arithmetic that
 * func_ov007_020b2160 uses to build the projection.
 *
 * ON THE HOST THE CAMERA IS A STACK SLOT NOBODY FILLED. What it read instead,
 * measured on this tree before the fix: an index of 256, i.e. a half-field of
 * 22.5 degrees, where the camera in force on the title screen carries 6.77
 * degrees. The two cotangents are 2.414214 and 8.420166 and their ratio is
 * 3.4877. So every cell came out 3.49 times too large.
 *
 * THE ARITHMETIC REPRODUCES THE OBSERVED VALUE EXACTLY, which is what turns
 * this from a candidate into the cause. A 16-texel cell at the title's own
 * depth (0x20FF, 2.0623 world units):
 *
 *     with the GARBAGE camera (index 256, sin 1567 cos 3784):  f20 = 583
 *     with the REAL camera     (index  77, sin  483 cos 4067):  f20 = 167
 *
 * and 583 is the number the record carried in every pre-fix run
 * (SM64DS_CELLQUAD_PROBE). 583/167 = 3.49.
 *
 * THE OTHER HALF OF THE SCREEN WAS ALWAYS RIGHT AND SAYS SO. The title's
 * backdrop goes through the same submitter (func_ov007_020c4684, the inline
 * arm) and measures EXACTLY 1.000 DS pixels per texel, because its record is
 * sized through func_ov007_020c41dc, the caller that passes the camera. One
 * frame, one function, two arms, and only the arm behind this seam is wrong.
 *
 * ============================ THE SIGNATURE ===============================
 *
 * UNCHANGED FROM THE MATCHED SOURCE. `(char *c, int arg)` is what src/ declares
 * and what its call sites pass; the only edit is that the call to
 * func_ov007_020c43bc forwards `arg`, and the declaration here names the two
 * parameters the callee's own matched definition names. `arg` IS the camera:
 * the very next call in the same body, func_ov007_020c4388(o, arg), hands it on
 * to func_ov007_020bfe4c as the camera it reads +0xd4 from, and the two other
 * call sites of func_ov007_020c43bc pass data_ov007_0210342c + 0x34, which is
 * the same camera func_ov007_020b2160 builds the projection from.
 *
 * WHY THE CALLER AND NOT THE CALLEE, the only real choice here. The value is
 * this frame's own declared parameter: it exists here, it is wanted one frame
 * down, and this is where it stops being passed. func_ov007_020c43bc declares
 * the parameter it needs and is blameless -- and it has a second caller,
 * func_ov007_020c41dc, that passes correctly, so displacing the callee would
 * displace the blameless end. That is section 5d's own rule for this row.
 */
extern "C" {

void func_ov007_020c43bc(void *self, int camera);
void func_ov007_020c4388(char *r0, int r1);

/* PORT_HOST_ABI: implicit r1 argument. The matched src declares its callee with
 * one parameter and calls it with one, because on ARM the caller's own incoming
 * second argument is still in r1 at the branch. See the header. */
void func_ov007_020b4b5c(char *c, int arg)
{
    char *o = *(char **)(c + 4);
    *(int *)(o + 0x40) = 0x1000;
    *(int *)(o + 0x3c) = *(int *)(o + 0x40);
    func_ov007_020c43bc(o, arg);
    *(int *)(o + 0x50) = 0x1f000;
    *(unsigned short *)(o + 0x44) = 0x7fff;
    *(int *)(o + 4) = ((short *)(*(char **)(*(char **)c + 0x24)))[1] << 0xc;
    *(int *)(o + 8) = ((short *)(*(char **)(*(char **)c + 0x24)))[2] << 0xc;
    func_ov007_020c4388(o, arg);
}

}  /* extern "C" */
