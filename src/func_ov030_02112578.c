typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

void *_ZN5Actor15FindWithActorIDEjPS_(unsigned int id, void *p);
void *_ZN5Actor13ClosestPlayerEv(void *self);
int func_ov030_02111b20(void *self);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *thiz, void *bca, int a, int fx, unsigned int f);
void func_ov030_02111890(void *c);
s16 Vec3_HorzAngle(const void *v0, const void *v1);
int _Z14ApproachLinearRsss(s16 *v, s16 target, s16 step);
s32 Vec3_Dist(const void *a, const void *b);
int _ZN6Player9StartTalkER9ActorBaseb(void *player, void *actor, int b);
void func_ov030_02111908(void *c);
int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(void *player, void *actor, unsigned int id, const void *pos, unsigned int a, unsigned int b);
void _ZN9Animation8SetFlagsEi(void *thiz, int flags);
void func_0201267c(int a, void *b);
int _ZN6Player12GetTalkStateEv(void);
void _ZN6Player18HasFinishedTalkingEv(void *player);
int _ZNK12WithMeshClsn13JustHitGroundEv(const void *thiz);
int _ZN9Animation8FinishedEv(void *thiz);
void _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int a, unsigned int b, unsigned int c, int fx, int e);
u8 DecIfAbove0_Byte(u8 *p);
void _ZN9ActorBase18MarkForDestructionEv(void *thiz);
void _ZN9Animation7AdvanceEv(void *thiz);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *clsn);
void func_ov030_02111f6c(void *c, void *w);
void _ZN12CylinderClsn5ClearEv(void *thiz);

extern void *data_ov030_02115cd0[];
extern void *data_ov030_02115cf8[];
extern void *data_ov030_02115d08[];
extern void *data_ov030_02115d10[];
extern void *data_ov030_02115d18[];

int func_ov030_02112578(void *arg0)
{
    u8 *c = (u8 *)arg0;
    void *r4 = _ZN5Actor15FindWithActorIDEjPS_(0x67, 0);
    void *r6 = _ZN5Actor13ClosestPlayerEv(arg0);
    s32 v[3];
    *(s32 *)((u8 *)v + 0) = 0x981;
    *(s32 *)((u8 *)v + 4) = 0x77a;
    *(s32 *)((u8 *)v + 8) = 0x501;

    switch (c[0x3c7]) {
    case 0:
        if (func_ov030_02111b20(arg0) != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, data_ov030_02115cd0[1], 0, 0x1000, 0);
            *(s32 *)(c + 0x130) = 0x1000;
            *(s32 *)(c + 0x98) = 0;
            { u8 *p = (u8 *)(((unsigned int)c + 0x3c7) & 0xFFFFFFFFFFFFFFFFULL); *p = *p + 1; }
        }
        func_ov030_02111890(arg0);
        break;
    case 1:
        _Z14ApproachLinearRsss((s16 *)(c + 0x8e), Vec3_HorzAngle(c + 0x5c, (u8 *)r6 + 0x5c), 0x300);
        if (Vec3_Dist(c + 0x5c, (u8 *)r6 + 0x5c) < 0x96000) {
            if (_ZN6Player9StartTalkER9ActorBaseb(r6, arg0, 1) != 0) {
                { u8 *p = (u8 *)(((unsigned int)c + 0x3c7) & 0xFFFFFFFFFFFFFFFFULL); *p = *p + 1; }
            }
        }
        func_ov030_02111908(arg0);
        break;
    case 2: {
        s32 sp[3];
        sp[0] = *(s32 *)(c + 0x5c);
        sp[1] = *(s32 *)(c + 0x60);
        sp[2] = *(s32 *)(c + 0x64);
        sp[1] += 0x50000;
        if (_ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(r6, arg0, 0xbd, sp, 1, 0) != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, data_ov030_02115cf8[1], 0, 0x1000, 0);
            _ZN9Animation8SetFlagsEi(c + 0x124, 0);
            func_0201267c(0xd1, c + 0x74);
            { u8 *p = (u8 *)(((unsigned int)c + 0x3c7) & 0xFFFFFFFFFFFFFFFFULL); *p = *p + 1; }
        }
        break;
    }
    case 3:
        if (_ZN6Player12GetTalkStateEv() == 2) {
            _ZN6Player18HasFinishedTalkingEv(r6);
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, data_ov030_02115d18[1], 0, 0x1000, 0);
            { u8 *p = (u8 *)(((unsigned int)c + 0x3c7) & 0xFFFFFFFFFFFFFFFFULL); *p = *p + 1; }
        }
        break;
    case 4:
        if (_Z14ApproachLinearRsss((s16 *)(c + 0x8e), (s16)0xffffe04e, 0x400) != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, data_ov030_02115d08[1], 0x40000000, 0x1000, 0);
            *(s16 *)(c + 0x94) = *(s16 *)(c + 0x8e);
            *(s32 *)(c + 0x98) = 0xf000;
            *(s32 *)(c + 0xa8) = 0x2f000;
            func_0201267c(0xf1, c + 0x74);
            { u8 *p = (u8 *)(((unsigned int)c + 0x3c7) & 0xFFFFFFFFFFFFFFFFULL); *p = *p + 1; }
        }
        break;
    case 5:
        if (_ZNK12WithMeshClsn13JustHitGroundEv(c + 0x194) != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, data_ov030_02115d10[1], 0x40000000, 0x1000, 0);
            *(s32 *)(c + 0x98) = 0;
            { u8 *p = (u8 *)(((unsigned int)c + 0x3c7) & 0xFFFFFFFFFFFFFFFFULL); *p = *p + 1; }
        }
        break;
    case 6:
        if (_ZN9Animation8FinishedEv(c + 0x124) != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, data_ov030_02115cd0[1], 0, 0x1000, 0);
            *(s32 *)(c + 0x130) = 0x1000;
            *(s32 *)((u8 *)r4 + 0x98) = 0x400;
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x20, 0x14, 0x7f, 0x15666, 0);
            c[0x3c6] = 0x78;
            { u8 *p = (u8 *)(((unsigned int)c + 0x3c7) & 0xFFFFFFFFFFFFFFFFULL); *p = *p + 1; }
        }
        /* fallthrough */
    case 7: {
        s32 *pp = (s32 *)(((unsigned int)c + 0x3bc) & 0xFFFFFFFFFFFFFFFFULL);
        *pp = *pp + 0x400;
        if (*(s32 *)(c + 0x3bc) > 0x17ffd) {
            { u8 *p = (u8 *)(((unsigned int)c + 0x3c7) & 0xFFFFFFFFFFFFFFFFULL); *p = *p + 1; }
        }
    }
        /* fallthrough */
    case 8:
        if (DecIfAbove0_Byte((u8 *)(((unsigned int)c + 0x3c6) & 0xFFFFFFFFFFFFFFFFULL)) == 0) {
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x20, 0x7f, 0, 0x15666, 0);
            { u8 *p = (u8 *)(((unsigned int)c + 0x3c7) & 0xFFFFFFFFFFFFFFFFULL); *p = *p + 1; }
        }
        break;
    case 9:
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, data_ov030_02115d08[1], 0x40000000, 0x1000, 0);
        *(s32 *)((u8 *)r4 + 0x9c) = -0x2000;
        *(s32 *)((u8 *)r4 + 0xa0) = -0x3c000;
        { u8 *p = (u8 *)(((unsigned int)c + 0x3c7) & 0xFFFFFFFFFFFFFFFFULL); *p = *p + 1; }
        break;
    case 10:
        if (_ZN5Actor15FindWithActorIDEjPS_(0x67, 0) == 0) {
            _ZN9ActorBase18MarkForDestructionEv(arg0);
        }
        break;
    default:
        break;
    }

    _ZN9Animation7AdvanceEv(c + 0x124);
    _ZN5Actor9UpdatePosEP12CylinderClsn(arg0, c + 0x160);
    func_ov030_02111f6c(arg0, c + 0x194);
    _ZN12CylinderClsn5ClearEv(c + 0x160);
    return 1;
}
