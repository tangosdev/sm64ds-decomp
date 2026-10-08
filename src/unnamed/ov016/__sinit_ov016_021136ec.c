extern void func_02017acc();
extern void func_020731dc();
extern void _ZN13SharedFilePtr9ConstructEj();
extern void func_02017ab4();
extern void SharedFilePtr_Destruct_Anim();

typedef struct { int a, b; } S8;

extern S8 data_ov016_02114d38, data_ov016_02114d20, data_ov016_02114d30, data_ov016_02114d28;
extern S8 data_ov016_02114d58, data_ov016_02114d64, data_ov016_02114d70, data_ov016_02114d40;

extern S8 data_ov016_02114d8c[2], data_ov016_02114d9c[2], data_ov016_02114dac[2], data_ov016_02114dbc[2], data_ov016_02114d7c[2];
extern S8 data_ov016_021148a0, data_ov016_021148a8, data_ov016_021148b8, data_ov016_021148c0, data_ov016_02114888, data_ov016_02114880, data_ov016_021148b0, data_ov016_02114890, BookSwitch_SpawnInfo, data_ov016_02114878;

void __sinit_ov016_021136ec(void)
{
    func_02017acc(&data_ov016_02114d38, 0x3b5);
    func_020731dc(&data_ov016_02114d38, func_02017ab4, &data_ov016_02114d58);
    _ZN13SharedFilePtr9ConstructEj(&data_ov016_02114d20, 0x3b6);
    func_020731dc(&data_ov016_02114d20, SharedFilePtr_Destruct_Anim, &data_ov016_02114d64);
    _ZN13SharedFilePtr9ConstructEj(&data_ov016_02114d30, 0x3b7);
    func_020731dc(&data_ov016_02114d30, SharedFilePtr_Destruct_Anim, &data_ov016_02114d70);
    _ZN13SharedFilePtr9ConstructEj(&data_ov016_02114d28, 0x3b8);
    func_020731dc(&data_ov016_02114d28, SharedFilePtr_Destruct_Anim, &data_ov016_02114d40);

    data_ov016_02114d8c[0] = data_ov016_021148a0; data_ov016_02114d8c[1] = data_ov016_021148a8;
    data_ov016_02114d9c[0] = data_ov016_021148b8; data_ov016_02114d9c[1] = data_ov016_021148c0;
    data_ov016_02114dac[0] = data_ov016_02114888; data_ov016_02114dac[1] = data_ov016_02114880;
    data_ov016_02114dbc[0] = data_ov016_021148b0; data_ov016_02114dbc[1] = data_ov016_02114890;
    data_ov016_02114d7c[0] = BookSwitch_SpawnInfo; data_ov016_02114d7c[1] = data_ov016_02114878;
}
