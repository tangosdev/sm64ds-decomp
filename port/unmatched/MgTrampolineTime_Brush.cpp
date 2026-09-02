/* PORT_HOST_ABI.  dScMgTrampoline_c's vtable SLOT 34, the BRUSH.
 * Run mg11, lane TTI.  ROM 0x02120da8, 0x170.
 *
 * ---- 1. WHY A HOST COPY -------------------------------------------------
 *
 * src/func_ov006_02120da8.c reads its own mode selector as
 *
 *     int mode = *(int *)(obj + 0x6c);
 *
 * with `obj` declared `void *`.  ARITHMETIC ON A void* IS A GNU/mwcc EXTENSION
 * and MSVC refuses it outright:
 *
 *     src\func_ov006_02120da8.c(37): error C2036: 'void *': unknown size
 *
 * That is a SPELLING the host compiler will not accept, not a semantic defect
 * and not an ABI repair.  The body below is that file verbatim with ONE
 * character sequence changed -- `(char *)obj + 0x6c` -- and nothing else moved:
 * the same eight externs, the same `mask = 0xf` initialiser, the same
 * `if (j < n)` guard around a do/while that re-tests the same condition, the
 * same switch with the same four cases and the same `return` on default, the
 * same address arithmetic, the same three-step read-modify-write through
 * MultiCopy_Int.  The `#pragma opt_loop_invariants off` line is mwcc's and has
 * no MSVC meaning, so it is recorded here rather than carried.
 *
 * IT TAKES THE SAME SYMBOL NAME rather than a port_ one, which is the
 * unmatched/MgBSC_StateDispatch.cpp convention for "the port cannot compile
 * the src" and NOT the unmatched/MgFlower_Slot2.cpp convention for "the src
 * drops an argument".  The distinction is whether the host body differs from
 * the ROM body in behaviour: this one does not.  src/func_ov006_02120da8.c is
 * therefore out of port/slice_tti.txt, because listing both would be an
 * LNK2005.
 *
 * ---- 2. SLOT 34 IS REACHABLE ON THIS SCENE, AND THAT IS NEW --------------
 *
 * Run mg9 lane S371 reported the base's own slot-34 thunk (mb_v34 in
 * hal/scene_mg.cpp) as a LATENT defect -- five parameters against a
 * two-parameter thunk -- and lane LKY's closing note strengthened it to
 * "slot 34's ONLY dispatcher tree-wide is the bodiless floor 020ae5c4, so slot
 * 34 is UNREACHABLE on every scene and mb_v34 has no potential witness".
 *
 * THE SECOND HALF OF THAT IS NO LONGER TRUE ON THIS TREE, and this class is
 * why.  func_ov004_020ae5c4 is not a floor any more: hal/scene_mg_faces.cpp's
 * own header records it as a REAL DECOMPILATION (src/func_ov004_020ae5c4.c, a
 * Bresenham line walk that stamps slot 34 at every lattice point).  And THIS
 * class calls it from two places:
 *
 *     src/func_ov006_0212157c.c line 41   -- reached from vtable slot 6 on
 *                                            EVERY behavior tick
 *     src/func_ov006_02121d64.cpp line 64 -- chain link 0, the intro state
 *
 * so scene 384 is the first seated scene on which slot 34 has a live
 * dispatcher.  The "unreachable" half of the earlier conclusion should be read
 * as scoped to the twenty-one classes seated before this one; the ARITY half
 * of it stands and is what this file's thunk depends on.
 *
 * THE ARITY IS THE SAME FIVE, re-derived rather than carried.  Every slot-34
 * dispatch in src/func_ov004_020ae5c4.c is `ldr Rd,[r0] / ldr Rd,[Rd,#0x88] /
 * blx Rd` with r1, r2, r3 loaded and one word stored at [sp] first -- 0x88/4 =
 * 34 -- so five arguments arrive: the receiver, the two walked coordinates, the
 * caller's `val` and its `n`.  src/func_ov004_020ae3b4.c (the BASE's own brush)
 * declares the same five and hal/scene_mg.cpp's mb_v34 forwards all five.
 * hal/scene_mg_trampoline.cpp's tti_v34 has the same shape for the same reason.
 *
 * ---- 3. WHAT THE BRUSH DOES --------------------------------------------
 *
 * An n-by-n block of the 4-bit colour `val` centred on (x_base, y), clipped to
 * 256x192, written into whichever VRAM surface the object's +0x6c mode selects.
 * The write is read-modify-write through data_ov006_02142f6c because the target
 * is 4bpp: MultiCopy_Int pulls four words down, the nibble for this x is
 * replaced, and the four go back.  The `-1 ^ (mask << ...)` is the source's own
 * spelling of the inverted nibble mask and is kept.
 */

extern "C" {

extern int data_ov006_02142f6c[];
extern void *func_02054efc(void);
extern void *func_02054ea8(void);
extern void *_ZN2G213GetBG2CharPtrEv(void);
extern int func_02054d88(void);
extern void MultiCopy_Int(int *dst, int *src, int len);

// PORT_HOST_ABI: src reads its mode selector via arithmetic on a void* (a GNU/mwcc extension) that MSVC refuses as C2036; the host copy is the src body verbatim with the one read cast to (char *)obj + 0x6c, no behaviour change.
void func_ov006_02120da8(void *obj, int x_base, int y, int val, int n)
{
    int half;
    int x0;
    int j;
    int x;
    int *addr;
    int i;
    int yy;
    int mask = 0xf;

    half = n / 2;
    j = 0;
    if (j < n)
    {
        yy = y - half;
        x0 = x_base - half;
        do
        {
            for (i = 0; i < n; i++)
            {
                x = i + x0;
                if (x >= 0 && x < 0x100)
                {
                    if (yy >= 0 && yy < 0xc0)
                    {
                        void *ret;
                        int mode = *(int *)((char *)obj + 0x6c);

                        switch (mode)
                        {
                            case 0:
                                ret = func_02054efc();
                                break;
                            case 1:
                                ret = func_02054ea8();
                                break;
                            case 2:
                                ret = _ZN2G213GetBG2CharPtrEv();
                                break;
                            case 3:
                                ret = (void *)func_02054d88();
                                break;
                            default:
                                return;
                        }

                        addr = (int *)((char *)ret +
                            ((x / 8) + (yy / 8) * 32) * 32 +
                            (yy & 7) * 4);

                        MultiCopy_Int(addr, data_ov006_02142f6c, 4);
                        data_ov006_02142f6c[0] =
                            (data_ov006_02142f6c[0] &
                             (-1 ^ (mask << ((x & 7) * 4)))) |
                            (val << ((x & 7) * 4));
                        MultiCopy_Int(data_ov006_02142f6c, addr, 4);
                    }
                }
            }
            yy++;
            j++;
        } while (j < n);
    }
}

}  /* extern "C" */
