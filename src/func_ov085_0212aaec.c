// @symbol func_ov085_0212aaec
/* recovered: shared common types, declarations from a shared header */
#include "decl_Message.h"
/* recovered: shared common types */
#include "common.h"
typedef short s16;
typedef int Fix12;


extern s16 Vec3_HorzAngle(const struct Vector3 *v0, const struct Vector3 *v1);
extern int _ZN6Player12GetTalkStateEv(void *p);
extern int _Z14ApproachLinearRsss(s16 *p, s16 a, s16 b);
extern int _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int soundID, unsigned int vol, unsigned int pan, Fix12 dist, int loop);
extern int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(void *self, void *ab, unsigned int id, const struct Vector3 *v, unsigned int a, unsigned int b);
extern void _ZN7Message7EndTalkEv(void);
extern int func_ov085_0212bc78(void *c, void *p);

extern int data_ov085_021306bc[];

int func_ov085_0212aaec(char *self)
{
    void *player;
    int *pq;
    struct Vector3 pos;
    struct Vector3 pp;
    s16 angle;
    unsigned int id;

    player = *(void **)(self + 0x460);
    pq = (int *)(((int)player + 0x5c) & 0xFFFFFFFFFFFFFFFF);
    pos.x = *(int *)(self + 0x5c);
    pos.y = *(int *)(self + 0x60);
    pos.z = *(int *)(self + 0x64);
    pp.x = pq[0];
    pp.y = pq[1];
    pp.z = pq[2];
    angle = Vec3_HorzAngle((struct Vector3 *)(self + 0x5c), &pp);

    id = 0x139;
    switch (_ZN6Player12GetTalkStateEv(player)) {
    case 0:
        pos.y = pos.y + 0x46000;
        if (*(int *)(self + 0x43c) == 7) {
            id = 0x13d;
        }
        if (*(int *)(self + 0x43c) == 6) {
            id = 0x13a;
        }
        if (_Z14ApproachLinearRsss((s16 *)(self + 0x94), angle, 0x800)) {
            if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x26, 0x12, 0x7f, 0x15ccc, 0)) {
                _ZN7Message11PrepareTalkEv();
                _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(player, self, id, &pos, 0, 0);
            }
        }
        break;
    case 1:
        break;
    default:
        if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x26, 0x7f, 0, 0x7444, 0)) {
            _ZN7Message7EndTalkEv();
            func_ov085_0212bc78(self, data_ov085_021306bc);
        }
        break;
    }
    return 1;
}