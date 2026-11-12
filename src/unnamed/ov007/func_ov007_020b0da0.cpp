//cpp
#include "types.h"
#include "dExtModel_c.h"
#include "Ov007MenuState.h"
#include "Ov007Touch.h"
#include "Ov007Item.h"
#include "Ov007Model.h"
#include "nitro/fx/fx_cp.h"

extern "C" {

#include "decl_common.h"
#include "common.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "Ov007Work.h"
#include "Ov007Figure.h"
#include "math/ProgressRatio.h"
#include "math/MtxStack.h"

/* Vector3 declares a constructor and destructor under C++, which makes its
 * assignment copy word by word. The ROM copies these with C's ldm/stm block
 * move, which C++ keeps for a struct with an array member. */
typedef struct Vec3Words {
    Fix12i w[3];
} Vec3Words;


/* stateWork bits, by what this file does with them; medium confidence */
#define OV007_FIGURE_SWAY 1    /* rock the model and bob it (periods 160 and 40 ticks) */
#define OV007_FIGURE_TOUCHED 2 /* a touch landed in the box below; stop at the next rest */
#define OV007_FIGURE_RISE 4    /* lift it 30 units over the 80 ticks after data_ov007_02103344 */

/* The figure's update hook (func_ov007_020b0a20 installs it): on the first
 * tick shows the model and starts it swaying; a touch inside (0x60..0xa0,
 * 0x40..0x80) after tick 4 marks it touched and plays sound 0x34. While it
 * sways it rocks about z and bobs in y, clearing the hook at a rest once
 * touched; while it rises it lifts quadratically to 30 units and clears the
 * hook at the top. The offset lands in bone 0's translation. */
void func_ov007_020b0da0(Ov007Figure *figure) {
    Ov007Model *obj = figure->model;
    Ov007MenuState *st = WORK->states[1];
    int ticks = st->stateTicks;
    Matrix4x3 *mat = obj->model->transforms;
    int noSaveFiles = func_ov007_020aebac();

    if (ticks == 1) {
        obj->visible = 1;
        st->stateWork = OV007_FIGURE_SWAY;
        data_ov007_02103340 = 0;
        data_ov007_02103344 = 0;
        if (noSaveFiles == 0) {
            WORK->figure->update = 0;
        }
    }

    {
        struct Vector3 out;
        out.y = 0;
        out.x = 0;
        out.z = 0;

        if (!(st->stateWork & OV007_FIGURE_TOUCHED) && ticks >= 4) {
            Ov007Touch *touch = WORK->touch;
            if (touch->cur.touch != 0) {
                int x = touch->cur.x;
                if (x >= 0x60 && x <= 0xa0) {
                    int y = touch->cur.y;
                    if (y >= 0x40 && y <= 0x80) {
                        st->stateWork |= OV007_FIGURE_TOUCHED;
                        data_ov007_02103340 = ticks;
                        func_ov007_020bdeb0(0x34);
                        WORK->itemNodes[8]->item->state->requestedState = 4;
                    }
                }
            }
        }

        if (st->stateWork & OV007_FIGURE_SWAY) {
            int bob, rock;
            /* The cartridge calls this once more and drops the result. */
            FX_ModS32(ticks, 0x28);
            bob = ProgressRatio(FX_ModS32(ticks, 0x28), 0x28);
            out.y = FX_SinIdx((u16)((bob * 0xffff) >> 12) >> 4) >> 2;

            rock = ProgressRatio(FX_ModS32(ticks, 0xa0), 0xa0);
            MTX_Identity43_(mat);
            {
                int swing = FX_SinIdx((u16)(((rock * 2 + 0x800) * 0xffff) >> 12) >> 4);
                int reach = FX_SinIdx((u16)((rock * 0xffff) >> 12) >> 4);
                int tilt = FX_MUL((reach + 0x2000) / 2, FX_MUL(swing, 0x82));
                func_ov007_020c39f8(mat, 0, 0, (u16)(short)(0x3fff + ((tilt * 0x7fff) >> 12)));
            }

            if ((st->stateWork & OV007_FIGURE_TOUCHED) && (bob == 0 || bob == 0x800)) {
                WORK->figure->update = 0;
            }
        }

        if (st->stateWork & OV007_FIGURE_RISE) {
            int diff = ticks - data_ov007_02103344;
            int ratio, sq;
            /* FX_Div here, not FX_DivS32: the rise calls the SDK's out-of-line divide. */
            if (diff <= 0) {
                ratio = 0;
            } else if (diff >= 0x50) {
                ratio = 0x1000;
            } else {
                ratio = FX_Div(diff, 0x50);
            }
            sq = (fx32)(((fx64)ratio * ratio) >> FX32_SHIFT);
            out.y = sq * 30;
            if (ratio >= 0x1000) {
                WORK->figure->update = 0;
            }
        }

        *(Vec3Words *)&mat->t = *(Vec3Words *)&out;
    }
}
}
