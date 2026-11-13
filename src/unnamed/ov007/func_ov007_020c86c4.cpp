//cpp
#include "types.h"
#include "nitro/gx/g2.h"
#include "nitro/hw/registers.h"
#include "Ov007Transition.h"



/* Starts the transition: state 1 from t = 0, with the request cleared, and
 * sets up its effect's registers on each of its screens. Mosaic (1) sets or
 * clears the mosaic bit of BG0..3CNT by the planes mask; blend alpha (2/3)
 * and brightness (4) load BLDCNT and BLDALPHA/BLDY with the start values,
 * which are the end values when the curve runs reversed. */
void func_ov007_020c86c4(Ov007Transition *tr) {
    tr->state = 1;
    tr->t = 0;
    tr->request = 0;

    switch (tr->effect) {
    case 0:
        break;

    case 1: {
        s32 mask = tr->planes;
        if (tr->screens & 1) {
            if (mask & 1)
                REG_BG0CNT |= BG_CONTROL_MOSAIC;
            else
                REG_BG0CNT &= ~BG_CONTROL_MOSAIC;
            if (mask & 2)
                REG_BG1CNT |= BG_CONTROL_MOSAIC;
            else
                REG_BG1CNT &= ~BG_CONTROL_MOSAIC;
            if (mask & 4)
                REG_BG2CNT |= BG_CONTROL_MOSAIC;
            else
                REG_BG2CNT &= ~BG_CONTROL_MOSAIC;
            if (mask & 8)
                REG_BG3CNT |= BG_CONTROL_MOSAIC;
            else
                REG_BG3CNT &= ~BG_CONTROL_MOSAIC;
        }
        if (!(tr->screens & 2))
            return;
        if (mask & 1)
            REG_DB_BG0CNT |= BG_CONTROL_MOSAIC;
        else
            REG_DB_BG0CNT &= ~BG_CONTROL_MOSAIC;
        if (mask & 2)
            REG_DB_BG1CNT |= BG_CONTROL_MOSAIC;
        else
            REG_DB_BG1CNT &= ~BG_CONTROL_MOSAIC;
        if (mask & 4)
            REG_DB_BG2CNT |= BG_CONTROL_MOSAIC;
        else
            REG_DB_BG2CNT &= ~BG_CONTROL_MOSAIC;
        if (mask & 8)
            REG_DB_BG3CNT |= BG_CONTROL_MOSAIC;
        else
            REG_DB_BG3CNT &= ~BG_CONTROL_MOSAIC;
        return;
    }

    case 2:
    case 3:
        if (tr->screens & 1) {
            G2_SetBlendAlpha(tr->planes, tr->fx.alpha.planes2,
                             tr->reversed ? tr->fx.alpha.evaTo : tr->fx.alpha.evaFrom,
                             tr->reversed ? tr->fx.alpha.evbTo : tr->fx.alpha.evbFrom);
        }
        if (!(tr->screens & 2))
            return;
        G2S_SetBlendAlpha(tr->planes, tr->fx.alpha.planes2,
                          tr->reversed ? tr->fx.alpha.evaTo : tr->fx.alpha.evaFrom,
                          tr->reversed ? tr->fx.alpha.evbTo : tr->fx.alpha.evbFrom);
        return;

    case 4:
        if (tr->screens & 1) {
            G2_SetBlendBrightness(tr->planes,
                                  tr->reversed ? tr->fx.brightness.to : tr->fx.brightness.from);
        }
        if (!(tr->screens & 2))
            return;
        G2S_SetBlendBrightness(tr->planes,
                               tr->reversed ? tr->fx.brightness.to : tr->fx.brightness.from);
        return;
    }
}

