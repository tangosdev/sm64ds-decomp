#include "types.h"
// @symbol func_ov002_020bb614
/* recovered: shared common types */
#include "common.h"

/* SignPost's talk routine, ov002 0x020bb614, 0x3dc bytes. Called on the
 * signpost (`c` is a SignPost *, the class that owns 0x020bb23c..0x020bc240;
 * see include/SignPost.h) once a player has started talking to it.
 *
 * It walks the player through a three step approach and then shows the sign's
 * message: step 0 turns the player toward the reading spot (or skips straight
 * to step 1 if the player already stands there), step 1 walks the player onto
 * it, step 2 turns the player around to face the sign and then either plays the
 * message straight away or, for a sign that still has pounds left in it, waits
 * out the pound animations first. A talk state other than 0 or 1 aborts the
 * whole thing through func_ov002_020bbd5c.
 *
 * The reading spot is the sign's own position pushed 0x5a000 (or 0x78000 while
 * the sign still has pounds left) forward along the sign's facing angle, using
 * the shared sine table at data_02082214. The message is anchored 0x50000 above
 * the sign itself.
 *
 * The tail is the hint-sign special case: while data_0209d660 is set and this
 * sign carries message 0x74a, data_0209d6bc selects whether the hint flag
 * data_0209f284 is raised (3) or cleared (9), and the sign plays sound 0x24 on
 * the frame the flag comes up. 0x594 remembers the flag's last value so the
 * sound fires once per transition.
 *
 * Vec3_ApproachHorz is declared here rather than taken from decl_common.h
 * because that header types it void, and this body compares its result against
 * zero -- the ROM does `bl` then `cmp r0, #0`, so the function returns a
 * value. decl_common.h is generated, so it is left alone. */

extern s16 data_02082214[];
extern u8 data_0209d660;
extern u8 data_0209d6bc;
extern u8 data_0209f284;

extern int _ZN6Player12GetTalkStateEv(void *player);
extern s32 Vec3_HorzDist(struct Vector3 *a, struct Vector3 *b);
extern s16 Vec3_HorzAngle(struct Vector3 *a, struct Vector3 *b);
extern int _Z14ApproachLinearRsss(s16 *val, s16 target, s16 step);
extern int Vec3_ApproachHorz(struct Vector3 *pos, struct Vector3 *target, s32 step);
extern int func_ov002_020bec84(void *player, unsigned int i);
extern int func_ov002_020bec9c(void *player, unsigned int a, int b, int d, unsigned short e);
extern int _ZN6Player12FinishedAnimEv(void *player);
extern void _ZN6Player12ShowMessage2ER7fBase_cjPK7Vector3hh(
    void *player, void *actor, unsigned int msg, struct Vector3 *pos, unsigned int a, unsigned int b);
extern void func_ov002_020bbd5c(void *self, int i);
extern void func_02012790(int id);

#define FIXMUL(a, b) ((s32)(((s64)(a) * (b) + 0x800) >> 12))

void func_ov002_020bb614(char *c)
{
    /* C89: all locals at top. */
    struct Vector3 msgPos;
    struct Vector3 tgt;
    struct Vector3 plPos;
    char *player;
    u16 msgId;
    u8 *p58d;
    s32 scale;
    s32 talk;
    u8 st;
    s32 tx, ty, tz;
    s32 my, mz, mx;
    s32 ang;
    s16 sinV, cosV;
    s32 param;

    msgId = 0;
    param = *(s32 *)(c + 8);
    player = *(char **)(c + 0x598);
    if (param != 0xffff) {
        msgId = (u16)param;
    }
    /* The raised message anchor. The horizontal pair is read first and the
       raised height last: that order is what puts the 0x60 load in the gap the
       ROM leaves after `lslne`, which in turn hands the height r1 and the depth
       r2 the way the ROM colours them. Folding the +0x50000 into the temp
       rather than into the store is the other half of it. */
    mx = *(s32 *)(c + 0x5c);
    mz = *(s32 *)(c + 0x64);
    my = *(s32 *)(c + 0x60) + 0x50000;
    msgPos.x = mx;
    msgPos.y = my;
    msgPos.z = mz;

    scale = 0x5a000;
    if (*(u8 *)(c + 0x58e) == 1) {
        scale = 0x78000;
    }
    tx = *(s32 *)(c + 0x5c);
    tgt.x = tx;
    ty = *(s32 *)(c + 0x60);
    tgt.y = ty;
    tz = *(s32 *)(c + 0x64);
    tgt.z = tz;


    ang = (s32) * (u16 *)(c + 0x8e);
    sinV = data_02082214[(ang >> 4) * 2];
    tx = tx + FIXMUL(scale, sinV);
    tgt.x = tx;

    ang = (s32) * (u16 *)(c + 0x8e);
    cosV = data_02082214[(ang >> 4) * 2 + 1];
    tz = tz + FIXMUL(scale, cosV);
    tgt.z = tz;

    {
        char *pPos = player + 0x5c;
        plPos.x = *(s32 *)(pPos);
        plPos.y = *(s32 *)(pPos + 4);
        plPos.z = *(s32 *)(pPos + 8);
    }

    talk = _ZN6Player12GetTalkStateEv(player);
    switch (talk) {
    case 0:
        st = *(u8 *)(c + 0x58d);
        switch (st) {
        case 0:
            if (Vec3_HorzDist(&plPos, &tgt) < 0x32000) {
                p58d = (u8 *)(c + 0x58d);
                *p58d = (u8)(*p58d + 1);
            } else if (_Z14ApproachLinearRsss(
                           (s16 *)(player + 0x8e),
                           Vec3_HorzAngle(&plPos, &tgt),
                           0x800)
                       != 0) {
                p58d = (u8 *)(c + 0x58d);
                *p58d = (u8)(*p58d + 1);
                func_ov002_020bec9c(player, 1, 0, 0x1000, 0);
            }
            break;
        case 1:
            if (Vec3_ApproachHorz((struct Vector3 *)(player + 0x5c), &tgt, 0xa000) != 0) {
                p58d = (u8 *)(c + 0x58d);
                *p58d = (u8)(*p58d + 1);
            }
            break;
        case 2:
            if (_Z14ApproachLinearRsss(
                    (s16 *)(player + 0x8e),
                    (s16)(*(s16 *)(c + 0x8e) + 0x8000),
                    0x800)
                != 0) {
                if (*(u8 *)(c + 0x58e) == 1) {
                    if (func_ov002_020bec84(player, 1) != 0
                        || func_ov002_020bec84(player, 0) != 0) {
                        func_ov002_020bec9c(player, 2, 0x40000000, 0x1000, 0);
                    } else if (func_ov002_020bec84(player, 2) != 0
                               && _ZN6Player12FinishedAnimEv(player) != 0) {
                        func_ov002_020bec9c(player, 3, 0x40000000, 0x1000, 0);
                    } else if (func_ov002_020bec84(player, 3) != 0
                               && _ZN6Player12FinishedAnimEv(player) != 0) {
                        _ZN6Player12ShowMessage2ER7fBase_cjPK7Vector3hh(
                            player, c, (s16)msgId, &msgPos, 0, 1);
                    }
                } else {
                    func_ov002_020bec9c(player, 0, 0, 0x1000, 0);
                    _ZN6Player12ShowMessage2ER7fBase_cjPK7Vector3hh(
                        player, c, (s16)msgId, &msgPos, 0, 1);
                }
            }
            break;
        }
        break;
    case 1:
        break;
    default:
        func_ov002_020bbd5c(c, 0);
        break;
    }

    if (data_0209d660 != 0 && msgId == 0x74a) {
        switch (data_0209d6bc) {
        case 3:
            data_0209f284 = 1;
            break;
        case 9:
            data_0209f284 = 0;
            break;
        }
    }

    if (*(u8 *)(c + 0x594) != data_0209f284 && data_0209f284 != 0) {
        func_02012790(0x24);
    }
    *(u8 *)(c + 0x594) = data_0209f284;
}
