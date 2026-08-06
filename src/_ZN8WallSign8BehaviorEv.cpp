//cpp
// @symbol _ZN8WallSign8BehaviorEv
/* recovered: real C++ method; field access via char* this for layout until header grows */
#include "WallSign.h"
#include "types.h"

extern "C" {
extern int _ZN6Player12GetTalkStateEv(void *thisp);
extern int Vec3_HorzDist(Vector3 *a, Vector3 *b);
extern s16 Vec3_HorzAngle(Vector3 *a, Vector3 *b);
extern int _Z14ApproachLinearRsss(s16 *p, s16 target, s16 step);
extern int func_ov002_020bec9c(void *c, unsigned int a, int b, int d, unsigned short e);
extern int Vec3_ApproachHorz(Vector3 *out, Vector3 *a, int maxStep);
extern void _ZN6Player12ShowMessage2ER9ActorBasejPK7Vector3jj(void *actor, void *self, unsigned int a, Vector3 *pos, unsigned int b, unsigned int c);
extern void *_ZN5Actor10FindWithIDEj(unsigned int id);
extern int AngleDiff(int a, int b);
extern int _ZN6Player9StartTalkER9ActorBaseb(void *actor, void *self, int b);
extern void _ZN12CylinderClsn5ClearEv(void *self);
extern void _ZN12CylinderClsn6UpdateEv(void *self);
extern s16 data_02082214[];
}

int WallSign::Behavior()
{
    char *self = (char *)this;
    char *tgt;
    Vector3 mine, pos, tpos;

    tgt = *(char **)(self + 0x360);
    if (tgt != 0) {
        mine.x = *(int *)(self + 0x5c);
        mine.y = *(int *)(self + 0x60);
        mine.z = *(int *)(self + 0x64);

        pos.x = *(int *)(self + 0x5c);
        pos.y = *(int *)(self + 0x60);
        pos.z = *(int *)(self + 0x64);

        pos.x += (int)(((s64)data_02082214[(*(u16 *)(self + 0x8e) >> 4) << 1] * 0x5a000 + 0x800) >> 12);
        pos.z += (int)(((s64)data_02082214[((*(u16 *)(self + 0x8e) >> 4) << 1) + 1] * 0x5a000 + 0x800) >> 12);

        {
            int *p = (int *)(tgt + 0x5c);
            tpos.x = p[0];
            tpos.y = p[1];
            tpos.z = p[2];
        }

        switch (_ZN6Player12GetTalkStateEv(tgt)) {
        case 0:
            switch (*(u8 *)(self + 0x364)) {
            case 0:
                if (Vec3_HorzDist(&tpos, &pos) < 0x32000) {
                    *(u8 *)(self + 0x364) += 1;
                } else if (_Z14ApproachLinearRsss((s16 *)(tgt + 0x8e), Vec3_HorzAngle(&tpos, &pos), 0x800)) {
                    *(u8 *)(self + 0x364) += 1;
                    func_ov002_020bec9c(tgt, 1, 0, 0x1000, 0);
                }
                break;
            case 1:
                if (Vec3_ApproachHorz((Vector3 *)(tgt + 0x5c), &pos, 0xa000)) {
                    func_ov002_020bec9c(tgt, 0, 0, 0x1000, 0);
                    *(u8 *)(self + 0x364) += 1;
                }
                break;
            case 2:
                if (_Z14ApproachLinearRsss((s16 *)(tgt + 0x8e), (s16)(*(s16 *)(self + 0x8e) + 0x8000), 0x800)) {
                    *(s16 *)(self + 0x366) = 0;
                    if (*(int *)(self + 8) != 0xffff)
                        *(s16 *)(self + 0x366) = (s16)*(int *)(self + 8);
                    _ZN6Player12ShowMessage2ER9ActorBasejPK7Vector3jj(
                        tgt, self, (unsigned int)*(s16 *)(self + 0x366), &mine, 0, 1);
                    *(u8 *)(self + 0x364) = 0;
                }
                break;
            default:
                break;
            }
            break;
        case 1:
            break;
        default:
            *(int *)(self + 0x360) = 0;
            break;
        }
    } else {
        void *other;
        Vector3 opos;
        int isPlayer;
        int *pO;

        if ((*(u32 *)(self + 0x340) & 0x8000000) != 0) {
            other = _ZN5Actor10FindWithIDEj(*(unsigned int *)(self + 0x344));
            if (other != 0) {
                isPlayer = (*(u16 *)((char *)other + 0xc) == 0xbf);
                if (isPlayer != 0) {
                    pO = (int *)((char *)other + 0x5c);
                    opos.x = pO[0];
                    opos.y = pO[1];
                    opos.z = pO[2];
                    if (AngleDiff(Vec3_HorzAngle((Vector3 *)(self + 0x5c), &opos), *(s16 *)(self + 0x8e)) < 0x4000) {
                        if (_ZN6Player9StartTalkER9ActorBaseb(other, self, 0))
                            *(void **)(self + 0x360) = other;
                    }
                }
            }
        }
    }
    _ZN12CylinderClsn5ClearEv(self + 0x320);
    _ZN12CylinderClsn6UpdateEv(self + 0x320);
    return 1;
}
