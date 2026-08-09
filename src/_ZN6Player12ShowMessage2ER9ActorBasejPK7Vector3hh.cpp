//cpp
extern "C" {
typedef unsigned char u8;
typedef short s16;

struct State;
struct ActorBase;
struct Vector3 { int x, y, z; };

extern State data_ov002_0211046c;
extern State data_ov002_0211013c;
extern State data_ov002_02110364;
extern State data_ov002_0211043c;
extern State data_ov002_02110154;
extern State data_ov002_02110424;
extern State data_ov002_021104e4;
extern State data_ov002_02110514;
extern State data_ov002_0211022c;

extern int data_ov002_0210e15c;
extern int data_ov002_0210f224[3];
extern int data_ov002_0210f1ac;
extern void func_020072c0(void);
extern void *data_0209f318;

int _ZN6Player7IsStateERNS_5StateE(void *thiz, State *st);
void _ZN6Player11ChangeStateERNS_5StateE(void *thiz, State *st);
void func_020731dc(void *a, void *b, void *node);
void Vec3_RotateYAndTranslate(Vector3 *out, const Vector3 *in, int angle, const Vector3 *src);
void _ZN6Camera9SetFlag_3Ev(void *self);

int _ZN6Player12ShowMessage2ER9ActorBasejPK7Vector3hh(char *thiz, ActorBase *actor, unsigned int msg, const Vector3 *pos, u8 d, u8 e)
{
    int r7;

    if (_ZN6Player7IsStateERNS_5StateE(thiz, &data_ov002_0211046c)) {
        u8 sub = *(u8 *)(thiz + 0x6e3);
        if (sub == 0 || sub == 3)
            goto finish;
        return 0;
    }

    r7 = 0;
    if (_ZN6Player7IsStateERNS_5StateE(thiz, &data_ov002_0211013c) ||
        _ZN6Player7IsStateERNS_5StateE(thiz, &data_ov002_02110364) ||
        _ZN6Player7IsStateERNS_5StateE(thiz, &data_ov002_0211043c) ||
        _ZN6Player7IsStateERNS_5StateE(thiz, &data_ov002_02110154) ||
        _ZN6Player7IsStateERNS_5StateE(thiz, &data_ov002_02110424) ||
        _ZN6Player7IsStateERNS_5StateE(thiz, &data_ov002_021104e4) ||
        _ZN6Player7IsStateERNS_5StateE(thiz, &data_ov002_02110514)) {
        r7 = 1;
    }

    if (_ZN6Player7IsStateERNS_5StateE(thiz, &data_ov002_0211022c)) {
        u8 v = *(u8 *)(thiz + 0x70a);
        if (!(v == 0 || v == 1 || v == 3 || v == 2 || v == 0xd)) {
            if (v != 8)
                r7 = 1;
        }
    }

    if (!r7)
        return 0;

    _ZN6Player11ChangeStateERNS_5StateE(thiz, &data_ov002_0211046c);
    *(ActorBase **)(thiz + 0x368) = actor;

finish:
    *(u8 *)(thiz + 0x6e5) = 0;
    *(u8 *)(thiz + 0x70c) = d & 3;
    *(u8 *)(((long long)(thiz + 0x70c))) |= (e & 3) << 2;

    if (!(data_ov002_0210e15c & 1)) {
        data_ov002_0210f224[0] = 0;
        data_ov002_0210f224[1] = 0x64000;
        data_ov002_0210f224[2] = 0x64000;
        func_020731dc(data_ov002_0210f224, (void *)func_020072c0, &data_ov002_0210f1ac);
        data_ov002_0210e15c |= 1;
    }

    Vec3_RotateYAndTranslate((Vector3 *)(thiz + 0x744), (Vector3 *)(thiz + 0x5c), *(s16 *)(thiz + 0x8e), (Vector3 *)data_ov002_0210f224);

    if (pos) {
        *(Vector3 *)(thiz + 0x744) = *pos;
    }

    *(int *)(thiz + 0x98) = 0;
    *(u8 *)(thiz + 0x6e3) = 1;
    *(unsigned int *)(thiz + 0x688) = msg;

    if (e == 2) {
        _ZN6Camera9SetFlag_3Ev(data_0209f318);
    }

    return 1;
}
}
