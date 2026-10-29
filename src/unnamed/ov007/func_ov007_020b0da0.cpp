//cpp
#include "types.h"
#include "dExtModel_c.h"
#include "Ov007MenuState.h"
#include "Ov007Touch.h"
#include "cstd.h"

extern "C" {

/* Vector3 declares a constructor and destructor under C++, which makes its
 * assignment copy word by word. The ROM copies these with C's ldm/stm block
 * move, which C++ keeps for a struct with an array member. */
typedef struct Vec3Words {
    Fix12i w[3];
} Vec3Words;
// @symbol func_ov007_020b0da0
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
#include "nitro/fx/fx.h"
#include "Ov007Work.h"
extern short data_02082214[];

extern void Matrix4x3_LoadIdentity(void* mat);

/* Reconstructed layouts and names, not recovered class identity; only the
 * slots this file touches.
 *
 * Ov007Model is the 0xa0-byte model record func_ov007_020bc6d4 allocates
 * (names as in func_ov007_020bc47c). */
typedef struct Ov007Model {
    struct ModelComponents *model; /* 0x00 */
    char pad_04[0x90];
    int visible; /* 0x94: func_ov007_020bbff0 renders only while it is nonzero */
} Ov007Model;

/* The 0x1c-byte record func_ov007_020beaa0 builds around an Ov007Model;
 * medium-confidence name. */
typedef struct Ov007Figure {
    char pad_00[4];
    Ov007Model *model; /* 0x04: func_ov007_020bc6d4 */
    char pad_08[0x10];
    /* 0x18: the per-frame hook; func_ov007_020b0a20 installs this function
     * there and waits for it to read 0 again */
    void *update;
} Ov007Figure;

typedef char Ov007Model_size_must_be_0x98[sizeof(Ov007Model) == 0x98 ? 1 : -1];
typedef char Ov007Figure_size_must_be_0x1c[sizeof(Ov007Figure) == 0x1c ? 1 : -1];

/* The extern keeps decl_common.h's void * spelling; the typed view is
 * reached through a cast macro. */
#define WORK ((Ov007Work *)data_ov007_0210342c)

/* stateWork bits, by what this file does with them; medium confidence */
#define OV007_FIGURE_SWAY 1    /* rock the model and bob it (periods 160 and 40 ticks) */
#define OV007_FIGURE_TOUCHED 2 /* a touch landed in the box below; stop at the next rest */
#define OV007_FIGURE_RISE 4    /* lift it 30 units over the 80 ticks after data_ov007_02103344 */

/* The figure's update hook (func_ov007_020b0a20 installs it): on the first
 * tick shows the model and starts it swaying; a touch inside (0x60..0xa0,
 * 0x40..0x80) after tick 4 marks it touched and plays sound 0x34. While it
 * sways it rocks about z and bobs in y, clearing the hook at a rest once
 * touched; while it rises it lifts quadratically to 30 units and clears the
 * hook at the top. The offset lands in bone 0's translation. arg0 keeps the
 * void * spelling of the definition; `figure` is the typed view. */
#pragma opt_common_subs off
void func_ov007_020b0da0(void* arg0) {
    Ov007Figure *figure = (Ov007Figure *)arg0;
    Ov007Model *obj = figure->model;
    Ov007MenuState *st = WORK->states[1];
    int counter = st->stateTicks;
    Matrix4x3 *mat = obj->model->transforms;
    void* ret = func_ov007_020aebac();

    if (counter == 1) {
        obj->visible = 1;
        st->stateWork = OV007_FIGURE_SWAY;
        data_ov007_02103340 = 0;
        data_ov007_02103344 = 0;
        if (ret == 0) {
            WORK->figure->update = 0;
        }
    }

    {
        struct Vector3 out;
        out.y = 0;
        out.x = 0;
        out.z = 0;

        if (!(*(volatile int *)&st->stateWork & OV007_FIGURE_TOUCHED) && counter >= 4) {
            Ov007Touch *rp = WORK->touch;
            if (rp->cur.touch != 0) {
                int a = rp->cur.x;
                if (a >= 0x60 && a <= 0xa0) {
                    int b = rp->cur.y;
                    if (b >= 0x40 && b <= 0x80) {
                        int *pflag = (int *)&st->stateWork;
                        *pflag |= OV007_FIGURE_TOUCHED;
                        data_ov007_02103340 = counter;
                        func_ov007_020bdeb0(0x34);
                        {
                            char *t0 = *(char **)WORK->itemNodes[8];
                            char *t1 = *(char **)(t0 + 4);
                            *(short *)(t1 + 2) = 4;
                        }
                    }
                }
            }
        }

        if (st->stateWork & OV007_FIGURE_SWAY) {
            int var_r4;
            cstd::mod(counter, 0x28);
            {
                int m1 = cstd::mod(counter, 0x28);
                if (m1 <= 0) {
                    var_r4 = 0;
                } else if (m1 >= 0x28) {
                    var_r4 = 0x1000;
                } else {
                    var_r4 = cstd::div(m1 << 12, 0x28);
                }
            }

            {
                int angle1 = (u16)((var_r4 * 0xffff) >> 12);
                out.y = data_02082214[(angle1 >> 4) * 2] >> 2;
            }

            {
                int var_r8;
                int m2 = cstd::mod(counter, 0xa0);
                if (m2 <= 0) {
                    var_r8 = 0;
                } else if (m2 >= 0xa0) {
                    var_r8 = 0x1000;
                } else {
                    var_r8 = cstd::div(m2 << 12, 0xa0);
                }

                Matrix4x3_LoadIdentity(mat);

                {
                    int angleA = (u16)(((var_r8 * 2 + 0x800) * 0xffff) >> 12);
                    int valA = data_02082214[(angleA >> 4) * 2];
                    int angleB = (u16)((var_r8 * 0xffff) >> 12);
                    int valB = data_02082214[(angleB >> 4) * 2];

                    int valBhalf = (valB + 0x2000) / 2;
                    int term = FX_MUL(valA, 0x82);
                    int combined = FX_MUL(valBhalf, term);
                    short rzS = (short)(0x3fff + ((combined * 0x7fff) >> 12));
                    func_ov007_020c39f8(mat, 0, 0, (u16)rzS);
                }

                if ((st->stateWork & OV007_FIGURE_TOUCHED) && (var_r4 == 0 || var_r4 == 0x800)) {
                    WORK->figure->update = 0;
                }
            }
        }

        if (st->stateWork & OV007_FIGURE_RISE) {
            int diff = counter - data_ov007_02103344;
            int ratio;
            int sq;
            if (diff <= 0) {
                ratio = 0;
            } else if (diff >= 0x50) {
                ratio = 0x1000;
            } else {
                ratio = cstd::fdiv(diff, 0x50);
            }
            sq = (int)(((s64)ratio * ratio) >> 12);
            out.y = sq * 30;
            if (ratio >= 0x1000) {
                WORK->figure->update = 0;
            }
        }

        *(Vec3Words *)&mat->t = *(Vec3Words *)&out;
    }
}
}
