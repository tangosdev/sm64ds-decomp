extern void func_02017acc();
extern void func_020731dc();
extern void _ZN13SharedFilePtr9ConstructEj();
extern void func_02017ab4();
extern void SharedFilePtr_Destruct_Anim();

typedef struct { int a, b; } S8;

extern S8 data_ov077_02127c88, data_ov077_02127ca0, data_ov077_02127c90, data_ov077_02127c98;
extern S8 data_ov077_02127cc0, data_ov077_02127ccc, data_ov077_02127ca8, data_ov077_02127cb4;

extern S8 data_ov077_02127ce8[2], data_ov077_02127cf8[2], data_ov077_02127d08[2], data_ov077_02127d18[2], data_ov077_02127cd8[2];
extern S8 data_ov077_02127a20, data_ov077_02127a28, data_ov077_02127a38, data_ov077_02127a40, data_ov077_02127a18, data_ov077_02127a08, data_ov077_02127a10, data_ov077_02127a48, data_ov077_02127a30, data_ov077_02127a00;

void __sinit_ov077_021275fc(void)
{
    func_02017acc(&data_ov077_02127c88, 0x40d);
    func_020731dc(&data_ov077_02127c88, func_02017ab4, &data_ov077_02127cc0);
    _ZN13SharedFilePtr9ConstructEj(&data_ov077_02127ca0, 0x40e);
    func_020731dc(&data_ov077_02127ca0, SharedFilePtr_Destruct_Anim, &data_ov077_02127ccc);
    _ZN13SharedFilePtr9ConstructEj(&data_ov077_02127c90, 0x40f);
    func_020731dc(&data_ov077_02127c90, SharedFilePtr_Destruct_Anim, &data_ov077_02127ca8);
    _ZN13SharedFilePtr9ConstructEj(&data_ov077_02127c98, 0x410);
    func_020731dc(&data_ov077_02127c98, SharedFilePtr_Destruct_Anim, &data_ov077_02127cb4);

    data_ov077_02127ce8[0] = data_ov077_02127a20; data_ov077_02127ce8[1] = data_ov077_02127a28;
    data_ov077_02127cf8[0] = data_ov077_02127a38; data_ov077_02127cf8[1] = data_ov077_02127a40;
    data_ov077_02127d08[0] = data_ov077_02127a18; data_ov077_02127d08[1] = data_ov077_02127a08;
    data_ov077_02127d18[0] = data_ov077_02127a10; data_ov077_02127d18[1] = data_ov077_02127a48;
    data_ov077_02127cd8[0] = data_ov077_02127a30; data_ov077_02127cd8[1] = data_ov077_02127a00;
}
