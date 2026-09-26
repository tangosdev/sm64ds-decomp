extern int func_02017acc();
extern int _ZN13SharedFilePtr9ConstructEj();
extern void *_ZN14BlendModelAnimC1Ev(void *);
extern void *_ZN11ShadowModelC1Ev(void *object);

int func_ov006_020c0a48(char *t)
{
    func_02017acc(t, 0x205);
    _ZN13SharedFilePtr9ConstructEj(t + 8, 0x206);
    _ZN13SharedFilePtr9ConstructEj(t + 0x10, 0x207);
    _ZN14BlendModelAnimC1Ev(t + 0x18);
    _ZN11ShadowModelC1Ev(t + 0x88);
    *(int *)(t + 0xe4) = 0;
    *(short *)(t + 0xf0) = 0;
    *(int *)(t + 0xf4) = 0;
    return (int)t;
}
