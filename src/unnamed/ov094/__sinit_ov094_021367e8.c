extern void func_02017acc();
extern void func_020731dc();
extern void _ZN13SharedFilePtr9ConstructEj();
extern void func_02017ab4();
extern void SharedFilePtr_Destruct_Anim();

typedef struct { int a, b; } S8;

extern S8 data_ov094_02136ae0;
extern S8 data_ov094_02136af8;
extern S8 data_ov094_02136ae8;
extern S8 data_ov094_02136af0;
extern S8 data_ov094_02136b18, data_ov094_02136b24, data_ov094_02136b00, data_ov094_02136b0c;

extern S8 data_ov094_02136b40[2], data_ov094_02136b50[2], data_ov094_02136b60[2], data_ov094_02136b70[2], data_ov094_02136b30[2];
extern S8 data_ov094_021369e0, data_ov094_021369c8, data_ov094_021369f8, data_ov094_02136a00, data_ov094_021369d8, data_ov094_021369c0, data_ov094_021369d0, data_ov094_02136a08, data_ov094_021369f0, data_ov094_021369e8;

void __sinit_ov094_021367e8(void)
{
    func_02017acc(&data_ov094_02136ae0, 0x3cb);
    func_020731dc(&data_ov094_02136ae0, func_02017ab4, &data_ov094_02136b18);
    _ZN13SharedFilePtr9ConstructEj(&data_ov094_02136af8, 0x3cc);
    func_020731dc(&data_ov094_02136af8, SharedFilePtr_Destruct_Anim, &data_ov094_02136b24);
    _ZN13SharedFilePtr9ConstructEj(&data_ov094_02136ae8, 0x3cd);
    func_020731dc(&data_ov094_02136ae8, SharedFilePtr_Destruct_Anim, &data_ov094_02136b00);
    _ZN13SharedFilePtr9ConstructEj(&data_ov094_02136af0, 0x3ce);
    func_020731dc(&data_ov094_02136af0, SharedFilePtr_Destruct_Anim, &data_ov094_02136b0c);

    data_ov094_02136b40[0] = data_ov094_021369e0; data_ov094_02136b40[1] = data_ov094_021369c8;
    data_ov094_02136b50[0] = data_ov094_021369f8; data_ov094_02136b50[1] = data_ov094_02136a00;
    data_ov094_02136b60[0] = data_ov094_021369d8; data_ov094_02136b60[1] = data_ov094_021369c0;
    data_ov094_02136b70[0] = data_ov094_021369d0; data_ov094_02136b70[1] = data_ov094_02136a08;
    data_ov094_02136b30[0] = data_ov094_021369f0; data_ov094_02136b30[1] = data_ov094_021369e8;
}
