extern int func_02017acc();
extern int _ZN13SharedFilePtr9ConstructEj();
extern int _ZN14BlendModelAnimC1Ev();
extern int _ZN11ShadowModelC1Ev();
extern int g0[], g1[], g2[];

int func_ov006_020c0a48(char *t)
{
    func_02017acc(t, g0);
    _ZN13SharedFilePtr9ConstructEj(t + 8, g1);
    _ZN13SharedFilePtr9ConstructEj(t + 0x10, g2);
    _ZN14BlendModelAnimC1Ev(t + 0x18);
    _ZN11ShadowModelC1Ev(t + 0x88);
    *(int *)(t + 0xe4) = 0;
    *(short *)(t + 0xf0) = 0;
    *(int *)(t + 0xf4) = 0;
    return (int)t;
}
