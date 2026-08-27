/* Title/menu per-element stylus hit test.
 *
 * func_ov007_020aed98 calls this once per element per frame, for all 24
 * elements: func_ov007_020b46b0(ctx[0x114 + i*4], ctx[0x50]), where ctx is
 * data_ov007_0210342c.  arg0 is the element (a two-word record: [0] is the
 * menu object, [4] its placement), arg1 is the touch-panel record that
 * func_ov007_020c1db0 maintains.  Returns nothing; a hit acts by calling
 * func_ov007_020b63e4, which parks the chosen item's id in ctx+0x180.
 *
 * Touch-panel fields, named from func_ov007_020c1db0's own struct:
 *   +0x08 u16  stylus x, this frame       +0x0c u16  stylus down, this frame
 *   +0x0a u16  stylus y, this frame       +0x14 u16  stylus down, last frame
 *
 * Two independent paths:
 *
 *   1. PRESS.  Gated on down-now && !down-last-frame, i.e. the frame the
 *      stylus first touches, plus (u32)tp+0x24 >= 1.  Tests the stylus point
 *      against the element's own rectangle (obj+0x28 holds min x/min y/max
 *      x/max y at +4/+8/+0xc/+0x10, offset by the placement's 20.12 position
 *      at +4/+8 >> 12).  A hit selects immediately.  If that misses, three
 *      element types get a second chance against a hardcoded screen rect:
 *      x 8..0x66, y 0x50..0x60 for type 0xc (when ctx[0x28]->[9] is 0) or
 *      type 0xd (when it is not), and x 8..0x48, y 0x20..0x30 for type 9.
 *
 *   2. HOLD.  Only while ctx[4]->[0] == 3, and only for element types 3 and 6.
 *      Requires the stylus merely to be down, not freshly pressed, and counts
 *      frames in the element's state record at +4; past 0x3a it selects.  Any
 *      frame the element is not held resets that counter to 0.
 *
 * Two things about the box shape below are load-bearing, not style.  Each axis is
 * shifted once into its position local rather than at each of the four edges, and
 * the element is re-dereferenced in every edge expression rather than read through
 * a bound pointer.  Neither alone is enough.  Measured at 2004/b56 over all four
 * combinations: pointer locals with per-edge shifts scores 37 divergences,
 * re-dereferencing with per-edge shifts also 37, pointer locals with pre-shifted
 * positions 51, and only the two together 0.
 *
 * Reusing the preamble's already-bound object pointer here, rather than re-reading
 * the element, does not just cost divergences, it breaks the size: the ROM performs
 * that reload, so folding it away compiles to 0x49c instead of 0x4ac.
 */
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

typedef struct { u32 f0; u32 f4; u32 f8; } Tbl;
extern Tbl data_ov007_020d77dc[];
extern char* data_ov007_0210342c;

extern int func_ov007_020b79e4(void);
extern int func_ov007_020c1da0(int i);
extern void func_ov007_020b63e4(char** a);
extern int func_ov007_020b8f78(void);
extern int func_ov007_020b8fa0(void);
extern void func_ov007_020c1d78(int i);

void func_ov007_020b46b0(void* arg0, void* arg1)
{
    char* elem = (char*)arg0;
    char* tp = (char*)arg1;
    int held = 0;

    {
        char* obj_a = *(char**)elem;
        int sel_a = 0;
        int gate2_a = 0;
        int gate1_a = 0;
        if (*(*(s16**)(obj_a + 4)) == 0) {
            if (*(s32*)(obj_a + 0x10) >= 0x1000
                || (data_ov007_020d77dc[**(u16**)elem].f0 & 2))
                gate1_a = 1;
        }
        if (gate1_a) {
            if (!(data_ov007_020d77dc[**(u16**)elem].f0 & 1))
                gate2_a = 1;
        }
        if (gate2_a) {
            int ok_a = 1;
            if (func_ov007_020b79e4()) {
                int sub_a = 0;
                if (func_ov007_020b79e4()) {
                    u16 id2 = **(u16**)elem;
                    int x = ok_a;
                    if (id2 != 0x16 && id2 != 0x17) x = sub_a;
                    if (x) sub_a = 1;
                }
                if (!sub_a) ok_a = 0;
            }
            if (ok_a) sel_a = 1;
        }

        if (sel_a && func_ov007_020c1da0(0) == 0) {
            int armed = 0;
            int fresh = 0;
            int touching = *(u16*)(tp + 0xc);
            if (touching != 0) {
                if (*(u16*)(tp + 0x14) == 0)
                    fresh = 1;
            }
            if (fresh) {
                if ((u32)*(s32*)(tp + 0x24) >= 1U)
                    armed = 1;
            }
            if (armed) {
                int pos_y = *(s32*)(*(char**)(elem + 4) + 8) >> 12;
                int pos_x = *(s32*)(*(char**)(elem + 4) + 4) >> 12;
                int max_y = *(s32*)(*(char**)(*(char**)elem + 0x28) + 0x10) + pos_y;
                int min_y = *(s32*)(*(char**)(*(char**)elem + 0x28) + 8) + pos_y;
                int max_x = *(s32*)(*(char**)(*(char**)elem + 0x28) + 0xc) + pos_x;
                int min_x = *(s32*)(*(char**)(*(char**)elem + 0x28) + 4) + pos_x;
                if (touching != 0
                    && (s32)*(u16*)(tp + 8) >= min_x && (s32)*(u16*)(tp + 8) <= max_x
                    && (s32)*(u16*)(tp + 0xa) >= min_y && (s32)*(u16*)(tp + 0xa) <= max_y) {
                    func_ov007_020b63e4((char**)arg0);
                } else {
                    u16 t = *(u16*)(*(char**)elem);
                    if ((t == 0xc
                         && (*(u8**)(*(char**)(data_ov007_0210342c + 0x28)))[9] == 0)
                        || (t == 0xd
                            && (*(u8**)(*(char**)(data_ov007_0210342c + 0x28)))[9] != 0)) {
                        if (touching != 0) {
                            s32 x = *(u16*)(tp + 8);
                            if (x >= 8 && x <= 0x66) {
                                s32 y = *(u16*)(tp + 0xa);
                                if (y >= 0x50 && y <= 0x60)
                                    func_ov007_020b63e4((char**)arg0);
                            }
                        }
                    } else if (t == 9) {
                        if (touching != 0) {
                            s32 x = *(u16*)(tp + 8);
                            if (x >= 8 && x <= 0x48) {
                                s32 y = *(u16*)(tp + 0xa);
                                if (y >= 0x20 && y <= 0x30)
                                    func_ov007_020b63e4((char**)arg0);
                            }
                        }
                    }
                }
                goto done;
            }
        }
    }

    if (*(s16*)(*(char**)(data_ov007_0210342c + 4)) == 3) {
        u16 t0 = *(u16*)(*(char**)arg0);
        if (t0 == 3 || t0 == 6) {
            char* obj_b = *(char**)elem;
            int sel_b = 0;
            int gate2_b = 0;
            int gate1_b = 0;
            if (*(*(s16**)(obj_b + 4)) == 0) {
                if (*(s32*)(obj_b + 0x10) >= 0x1000
                    || (data_ov007_020d77dc[**(u16**)elem].f0 & 2))
                    gate1_b = 1;
            }
            if (gate1_b) {
                if (!(data_ov007_020d77dc[**(u16**)elem].f0 & 1))
                    gate2_b = 1;
            }
            if (gate2_b) {
                int ok_b = 1;
                if (func_ov007_020b79e4()) {
                    int sub_b = 0;
                    if (func_ov007_020b79e4()) {
                        u16 id2 = **(u16**)elem;
                        int x = ok_b;
                        if (id2 != 0x16 && id2 != 0x17) x = sub_b;
                        if (x) sub_b = 1;
                    }
                    if (!sub_b) ok_b = 0;
                }
                if (ok_b) sel_b = 1;
            }

            if (sel_b && func_ov007_020c1da0(0) == 0
                && *(u16*)(tp + 0xc) != 0
                && *(s16*)func_ov007_020b8f78() == 5
                && func_ov007_020b8fa0() == 0) {
                int pos_y = *(s32*)(*(char**)(elem + 4) + 8) >> 12;
                int pos_x = *(s32*)(*(char**)(elem + 4) + 4) >> 12;
                int max_y = *(s32*)(*(char**)(*(char**)elem + 0x28) + 0x10) + pos_y;
                int min_y = *(s32*)(*(char**)(*(char**)elem + 0x28) + 8) + pos_y;
                int max_x = *(s32*)(*(char**)(*(char**)elem + 0x28) + 0xc) + pos_x;
                int min_x = *(s32*)(*(char**)(*(char**)elem + 0x28) + 4) + pos_x;
                if (*(u16*)(tp + 0xc) != 0
                    && (s32)*(u16*)(tp + 8) >= min_x && (s32)*(u16*)(tp + 8) <= max_x
                    && (s32)*(u16*)(tp + 0xa) >= min_y && (s32)*(u16*)(tp + 0xa) <= max_y) {
                    char* state = *(char**)(*(char**)elem + 4);
                    held = 1;
                    *(s32*)(state + 4) += 1;
                    if (*(s32*)(*(char**)(*(char**)elem + 4) + 4) > 0x3a)
                        func_ov007_020b63e4((char**)arg0);
                }
            }
        }
    }

done:
    if (held == 0)
        *(s32*)(*(char**)(*(char**)elem + 4) + 4) = 0;
    {
        char* st = *(char**)(*(char**)elem + 4);
        if (*(s16*)st != 2)
            return;
        if (*(s16*)(st + 2) == -1)
            return;
    }
    func_ov007_020c1d78(0);
}