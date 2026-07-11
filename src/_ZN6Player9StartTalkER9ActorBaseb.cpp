//cpp
extern "C" {
typedef unsigned char u8;
typedef unsigned short u16;
struct State;
struct ActorBase;

extern State data_ov002_0211046c;
extern State data_ov002_0211013c;
extern State data_ov002_02110154;
extern State data_ov002_021104e4;
extern State data_ov002_02110514;
extern State data_ov002_02110364;
extern State data_ov002_02110424;
extern State data_ov002_021105a4;
extern State data_ov002_0211043c;
extern u8 data_020a0e40;
extern u8 data_0209f49e[];

int _ZN6Player7IsStateERNS_5StateE(void *thiz, State *st);
void _ZN6Player11ChangeStateERNS_5StateE(void *thiz, State *st);

int _ZN6Player9StartTalkER9ActorBaseb(char *thiz, ActorBase *actor, int b)
{
    if (_ZN6Player7IsStateERNS_5StateE(thiz, &data_ov002_0211046c)) {
        if (actor == *(ActorBase **)(thiz + 0x368) && *(u8 *)(thiz + 0x6e3) == 0)
            return 1;
        return 0;
    }

    if (!(_ZN6Player7IsStateERNS_5StateE(thiz, &data_ov002_0211013c) ||
          _ZN6Player7IsStateERNS_5StateE(thiz, &data_ov002_02110154) ||
          _ZN6Player7IsStateERNS_5StateE(thiz, &data_ov002_021104e4) ||
          _ZN6Player7IsStateERNS_5StateE(thiz, &data_ov002_02110514) ||
          _ZN6Player7IsStateERNS_5StateE(thiz, &data_ov002_02110364) ||
          _ZN6Player7IsStateERNS_5StateE(thiz, &data_ov002_02110424) ||
          _ZN6Player7IsStateERNS_5StateE(thiz, &data_ov002_021105a4) ||
          _ZN6Player7IsStateERNS_5StateE(thiz, &data_ov002_0211043c)))
        return 0;

    if (b == 0) {
        if (!(*(u16 *)(data_0209f49e + (u8)data_020a0e40 * 0x18) & 3))
            goto ret0;

        if (!((_ZN6Player7IsStateERNS_5StateE(thiz, &data_ov002_0211013c) && *(int *)(thiz + 0x98) < 0x8000) ||
              _ZN6Player7IsStateERNS_5StateE(thiz, &data_ov002_0211043c) ||
              _ZN6Player7IsStateERNS_5StateE(thiz, &data_ov002_02110154)))
            goto ret0;

        *(u8 *)(thiz + 0x6e3) = 0;
        _ZN6Player11ChangeStateERNS_5StateE(thiz, &data_ov002_0211046c);
        *(ActorBase **)(thiz + 0x368) = actor;
        return 1;
    } else {
        if (_ZN6Player7IsStateERNS_5StateE(thiz, &data_ov002_02110424) ||
            _ZN6Player7IsStateERNS_5StateE(thiz, &data_ov002_021105a4)) {
            *(u16 *)(((long long)(thiz + 0x6ce)) & 0xFFFFFFFFFFFFFFFFLL) |= 0x40;
            return 0;
        }

        *(u8 *)(thiz + 0x6e3) = 0;
        _ZN6Player11ChangeStateERNS_5StateE(thiz, &data_ov002_0211046c);
        *(ActorBase **)(thiz + 0x368) = actor;
        return 1;
    }
ret0:
    return 0;
}
}
