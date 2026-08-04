extern int func_02017acc();
extern int _ZN13SharedFilePtr9ConstructEj();
extern int _ZN14BlendModelAnimC1Ev();
extern int g0[], g1[];

int func_ov006_020c221c(char *t)
{
    func_02017acc(t, g0);
    _ZN13SharedFilePtr9ConstructEj(t + 8, g1);
    _ZN14BlendModelAnimC1Ev(t + 0x10);
    *(int *)(t + 0xa8) = 0;
    return (int)t;
}
