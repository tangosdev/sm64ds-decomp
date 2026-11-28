/* unit020c8498 -- ov007 unit 0x020c8498..0x020c8de0: 12 functions, .text only.
 *
 * The title menu's screen-transition engine. A transition record is created
 * empty, configured by its callers, started forwards or backwards, advanced
 * one frame at a time, and finally freed; the unnamed helpers are its start,
 * stop, clear and per-effect appliers. func_ov007_020c8de0 begins the vector
 * tween unit and is the end edge.
 *
 * Emission order: mwccarm 2004/b56 emits one .text section per function in
 * the reverse of source order, so the highest-address function is written
 * first here and the functions come out in ROM order.
 */
#include "types.h"
#include "Ov007Transition.h"
#include "Ov007MenuMem.h"
#include "nitro/gx/g2.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/hw/registers.h"

void func_ov007_020c86c4(Ov007Transition *tr);
void func_ov007_020c897c(Ov007Transition *tr);
void func_ov007_020c8970(Ov007Transition *tr);
void func_ov007_020c8ac0(Ov007Transition *tr);
void func_ov007_020c8b34(Ov007Transition *tr, int t);
void func_ov007_020c8bc8(Ov007Transition *tr, int t);
void func_ov007_020c8c88(Ov007Transition *tr, int t);

// @symbol func_ov007_020c8c88
void func_ov007_020c8c88(Ov007Transition *tr, int t) {
    unsigned char from = tr->fx.mosaic.from;
    unsigned char to = tr->fx.mosaic.to;
    int loFrom = from & 15;
    unsigned int flags = tr->planes;
    int hiFrom = (from & 0xf0) >> 4;
    int loTo = to & 15;
    int hiTo = (to & 0xf0) >> 4;
    int lo, hi;

    if (t >= 0x1000) {
        lo = loTo;
    } else if (t <= 0) {
        lo = loFrom;
    } else {
        lo = (int)((((s64)(0x1000 - t) * (loFrom << 12) + (s64)t * (loTo << 12)) >> 12)) >> 12;
    }

    if (t >= 0x1000) {
        hi = hiTo;
    } else if (t <= 0) {
        hi = hiFrom;
    } else {
        hi = (int)((((s64)(0x1000 - t) * (hiFrom << 12) + (s64)t * (hiTo << 12)) >> 12)) >> 12;
    }

    if (tr->screens & 1) {
        REG_MOSAIC; /* read and dropped, as the ROM does */
        if ((flags & 1) || (flags & 2) || (flags & 4) || (flags & 8))
            REG_MOSAIC_BG = (unsigned char)(lo | (hi << 4));
        if (flags & 0x10)
            REG_MOSAIC_OBJ = (unsigned char)(lo | (hi << 4));
    }

    if (!(tr->screens & 2))
        return;

    if (flags & 0x10)
        REG_DB_MOSAIC_OBJ = (unsigned char)(lo | (hi << 4));

    if ((flags & 1) || (flags & 2) || (flags & 4) || (flags & 8))
        REG_DB_MOSAIC_BG = (unsigned char)(lo | (hi << 4));
}

// @symbol func_ov007_020c8bc8
void func_ov007_020c8bc8(Ov007Transition *tr, int t) {
    unsigned char c1 = tr->fx.alpha.evaTo;
    unsigned char c0 = tr->fx.alpha.evaFrom;
    int v2 = c1;
    if (t < 0x1000) {
        if (t <= 0)
            v2 = c0;
        else
            v2 = (int)(((s64)(0x1000 - t) * (c0 << 12) + (s64)t * (c1 << 12)) >> 12) >> 12;
    }
    {
        unsigned char d1 = tr->fx.alpha.evbTo;
        unsigned char d0 = tr->fx.alpha.evbFrom;
        int vip = d1;
        if (t < 0x1000) {
            if (t <= 0)
                vip = d0;
            else
                vip = (int)(((s64)(0x1000 - t) * (d0 << 12) + (s64)t * (d1 << 12)) >> 12) >> 12;
        }
        if (tr->effect == 3)
            vip = 0x10 - v2;
        if (tr->screens & 1)
            REG_BLDALPHA = (u16)(v2 | (vip << 8));
        if (tr->screens & 2)
            REG_DB_BLDALPHA = (u16)(v2 | (vip << 8));
    }
}

// @symbol func_ov007_020c8b34
void func_ov007_020c8b34(Ov007Transition *tr, int t) {
    int brightness = tr->fx.brightness.to;
    int from = tr->fx.brightness.from;

    if (t < 0x1000) {
        if (t <= 0) {
            brightness = from;
        } else {
            brightness =
                (int)(((s64)(0x1000 - t) * (from << 12) + (s64)t * (brightness << 12)) >> 12) >> 12;
        }
    }
    if (tr->screens & 1)
        G2_ChangeBlendBrightness(brightness);
    if (tr->screens & 2)
        G2S_ChangeBlendBrightness(brightness);
}

// @symbol Transition_Create
Ov007Transition *Transition_Create(void) {
    Ov007Transition *tr = func_ov007_020c3df4(0, sizeof(Ov007Transition));
    func_ov007_020c8ac0(tr);
    return tr;
}

// @symbol Transition_Destroy
#pragma long_calls on
void Transition_Destroy(void *p) {
    func_ov007_020c3d1c(p);
}
#pragma long_calls off

// @symbol func_ov007_020c8ac0
void func_ov007_020c8ac0(Ov007Transition *tr) {
    tr->screens = 0;
    tr->effect = 0;
    tr->state = 0;
    tr->endAction = 0;
    tr->curve = 0;
    tr->t = 0;
    tr->step = 0;
    tr->reversed = 0;
    tr->onEnd = 0;
    tr->request = 0;
    tr->fx.brightness.to = 0;
    tr->fx.brightness.from = tr->fx.brightness.to;
    tr->planes = tr->fx.brightness.from;
}

// @symbol func_ov007_020c897c
void func_ov007_020c897c(Ov007Transition *tr) {
    tr->state = 0;
    tr->t = 0x1000;
    tr->request = 0;
    switch (tr->effect) {
    case 1:
        if (tr->screens & 1) {
            G2_BG0Mosaic(0);
            G2_BG1Mosaic(0);
            G2_BG2Mosaic(0);
            G2_BG3Mosaic(0);
        }
        if (tr->screens & 2) {
            G2S_BG0Mosaic(0);
            G2S_BG1Mosaic(0);
            G2S_BG2Mosaic(0);
            G2S_BG3Mosaic(0);
        }
        break;
    case 2:
    case 3:
    case 4:
        if (tr->screens & 1) {
            G2_BlendNone();
        }
        if (tr->screens & 2) {
            G2S_BlendNone();
        }
        break;
    }
}

// @symbol func_ov007_020c8970
#pragma long_calls on
void func_ov007_020c8970(Ov007Transition *tr) {
    func_ov007_020c897c(tr);
}
#pragma long_calls off

// @symbol func_ov007_020c86c4
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

// @symbol Transition_StartForward
void Transition_StartForward(Ov007Transition *tr, int v) {
    tr->reversed = 0;
    tr->screens = v;
    tr->t = 0;
    func_ov007_020c86c4(tr);
}

// @symbol Transition_StartBackward
void Transition_StartBackward(Ov007Transition *tr, int v) {
    tr->reversed = 1;
    tr->screens = v;
    tr->t = 0;
    func_ov007_020c86c4(tr);
}

// @symbol Transition_Update
void Transition_Update(Ov007Transition *tr) {
    int eased = 0;
    void (*onEnd)(void);

    if (tr->request == 1) {
        func_ov007_020c86c4(tr);
    } else if (tr->request == 2) {
        func_ov007_020c897c(tr);
    }

    if (tr->state == 0)
        return;

    if (tr->state == 1) {
        int *progress = &tr->t;
        *progress += tr->step;
        if (tr->t > 0x1000) {
            tr->t = 0x1000;
            switch (tr->endAction) {
            case 0:
                func_ov007_020c8970(tr);
                break;
            case 1:
                tr->state = 3;
                break;
            case 2:
                if (tr->reversed == 1)
                    Transition_StartBackward(tr, tr->screens);
                else
                    Transition_StartForward(tr, tr->screens);
                break;
            case 3:
                if (tr->reversed == 1)
                    Transition_StartForward(tr, tr->screens);
                else
                    Transition_StartBackward(tr, tr->screens);
                break;
            }

            onEnd = tr->onEnd;
            if (onEnd != 0) {
                onEnd();
            }
            if (onEnd == tr->onEnd && (u32)tr->endAction <= 1)
                tr->onEnd = 0;
        }
    }

    if (tr->state == 0)
        return;

    switch (tr->curve) {
    case 0:
        eased = tr->t;
        break;
    case 1:
        eased = tr->t << 1;
        if (eased > 0x1000)
            eased = -(eased - 0x2000);
        break;
    case 2:
        eased = (int)(((long long)tr->t * tr->t) >> 12);
        break;
    case 3:
        break;
    }

    if (tr->reversed == 1)
        eased = 0x1000 - eased;

    switch (tr->effect) {
    case 1:
        func_ov007_020c8c88(tr, eased);
        break;
    case 2:
    case 3:
        func_ov007_020c8bc8(tr, eased);
        break;
    case 4:
        func_ov007_020c8b34(tr, eased);
        break;
    default:
        break;
    }
}
