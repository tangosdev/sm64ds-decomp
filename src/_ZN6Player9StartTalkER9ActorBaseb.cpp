//cpp
typedef unsigned char u8;
typedef unsigned short u16;
struct State;
struct ActorBase;
struct Player;

extern "C" int _ZN6Player7IsStateERNS_5StateE(Player* thiz, State* s);
extern "C" int _ZN6Player11ChangeStateERNS_5StateE(Player* thiz, State* s);

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

extern "C" int _ZN6Player9StartTalkER9ActorBaseb(Player* thiz, ActorBase* actor, int bParam)
{
    char* c = (char*)thiz;

    if (_ZN6Player7IsStateERNS_5StateE(thiz, &data_ov002_0211046c)) {
        if (actor != *(ActorBase**)(c + 0x368)) {
            goto retZero;
        }
        if (*(u8*)(c + 0x6e3) == 0) {
            return 1;
        }
    retZero:
        return 0;
    }

    if (_ZN6Player7IsStateERNS_5StateE(thiz, &data_ov002_0211013c)) goto common;
    if (_ZN6Player7IsStateERNS_5StateE(thiz, &data_ov002_02110154)) goto common;
    if (_ZN6Player7IsStateERNS_5StateE(thiz, &data_ov002_021104e4)) goto common;
    if (_ZN6Player7IsStateERNS_5StateE(thiz, &data_ov002_02110514)) goto common;
    if (_ZN6Player7IsStateERNS_5StateE(thiz, &data_ov002_02110364)) goto common;
    if (_ZN6Player7IsStateERNS_5StateE(thiz, &data_ov002_02110424)) goto common;
    if (_ZN6Player7IsStateERNS_5StateE(thiz, &data_ov002_021105a4)) goto common;
    if (!_ZN6Player7IsStateERNS_5StateE(thiz, &data_ov002_0211043c)) {
        return 0;
    }

common:
    if (bParam) {
        goto bparam_true;
    }

    if ((*(u16*)(data_0209f49e + (u8)data_020a0e40 * 0x18) & 3) == 0) {
        goto ret0_tail;
    }

    if (_ZN6Player7IsStateERNS_5StateE(thiz, &data_ov002_0211013c)) {
        if (*(int*)(c + 0x98) < 0x8000) {
            goto success;
        }
    }
    if (_ZN6Player7IsStateERNS_5StateE(thiz, &data_ov002_0211043c)) {
        goto success;
    }
    if (!_ZN6Player7IsStateERNS_5StateE(thiz, &data_ov002_02110154)) {
        goto ret0_tail;
    }

success:
    *(u8*)(c + 0x6e3) = 0;
    _ZN6Player11ChangeStateERNS_5StateE(thiz, &data_ov002_0211046c);
    *(ActorBase**)(c + 0x368) = actor;
    return 1;

bparam_true:
    if (_ZN6Player7IsStateERNS_5StateE(thiz, &data_ov002_02110424)) {
        goto set_flag;
    }
    if (!_ZN6Player7IsStateERNS_5StateE(thiz, &data_ov002_021105a4)) {
        goto success2;
    }
set_flag:
    *(u16*)(((long long)(c + 0x6ce)) & 0xFFFFFFFFFFFFFFFFLL) |= 0x40;
    return 0;

success2:
    *(u8*)(c + 0x6e3) = 0;
    _ZN6Player11ChangeStateERNS_5StateE(thiz, &data_ov002_0211046c);
    *(ActorBase**)(c + 0x368) = actor;
    return 1;

ret0_tail:
    return 0;
}
