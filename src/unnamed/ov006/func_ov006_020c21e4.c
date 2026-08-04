extern int _ZN13SharedFilePtr7ReleaseEv();
extern int _ZN14BlendModelAnimD1Ev();
extern int SharedFilePtr_Destruct_Anim();
extern int func_02017ab4();

int func_ov006_020c21e4(char *t)
{
    _ZN13SharedFilePtr7ReleaseEv(t);
    _ZN13SharedFilePtr7ReleaseEv(t + 8);
    _ZN14BlendModelAnimD1Ev(t + 0x10);
    SharedFilePtr_Destruct_Anim(t + 8);
    func_02017ab4(t);
    return (int)t;
}
