extern int _ZN14BlendModelAnimC1Ev();
extern int _ZN5ModelC1Ev();
extern int _ZN15TextureSequenceC1Ev();
extern int _ZN18TextureTransformerC1Ev();
extern int func_02017acc();
extern int _ZN13SharedFilePtr9ConstructEj();
extern int SharedFilePtr_Construct_TexSeq();

int func_ov006_020c33dc(char *t)
{
    _ZN14BlendModelAnimC1Ev(t + 0x8);
    _ZN5ModelC1Ev(t + 0x78);
    _ZN15TextureSequenceC1Ev(t + 0xc8);
    _ZN18TextureTransformerC1Ev(t + 0xdc);
    func_02017acc(t + 0xf0, 0x286);
    func_02017acc(t + 0xf8, 0x200);
    _ZN13SharedFilePtr9ConstructEj(t + 0x100, 0x1fc);
    _ZN13SharedFilePtr9ConstructEj(t + 0x108, 0x1f6);
    _ZN13SharedFilePtr9ConstructEj(t + 0x110, 0x1fe);
    _ZN13SharedFilePtr9ConstructEj(t + 0x118, 0x1f0);
    _ZN13SharedFilePtr9ConstructEj(t + 0x120, 0x1f4);
    _ZN13SharedFilePtr9ConstructEj(t + 0x128, 0x1f2);
    _ZN13SharedFilePtr9ConstructEj(t + 0x130, 0x1f8);
    _ZN13SharedFilePtr9ConstructEj(t + 0x138, 0x1fa);
    SharedFilePtr_Construct_TexSeq(t + 0x140, 0x1fd);
    SharedFilePtr_Construct_TexSeq(t + 0x148, 0x1f7);
    SharedFilePtr_Construct_TexSeq(t + 0x150, 0x1ff);
    SharedFilePtr_Construct_TexSeq(t + 0x158, 0x1f1);
    SharedFilePtr_Construct_TexSeq(t + 0x160, 0x1f5);
    SharedFilePtr_Construct_TexSeq(t + 0x168, 0x1f3);
    SharedFilePtr_Construct_TexSeq(t + 0x170, 0x1f9);
    SharedFilePtr_Construct_TexSeq(t + 0x178, 0x1fb);
    *(int *)(t + 0x1a4) = 0;
    *(int *)(t + 0x1a8) = 0;
    return (int)t;
}
