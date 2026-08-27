extern void func_02017acc();
extern void func_020731dc();
extern void _ZN13SharedFilePtr9ConstructEj();
extern void func_02017ab4(void);
extern void SharedFilePtr_Destruct_Anim(void);

extern int data_ov027_02113c10[];
extern int data_ov027_02113c30[];
extern int data_ov027_02113c18[];
extern int data_ov027_02113c3c[];
extern int data_ov027_02113c08[];
extern int data_ov027_02113c48[];
extern int data_ov027_02113c20[];
extern int data_ov027_02113c54[];
extern int data_ov027_02113c28[];
extern int data_ov027_02113c60[];

void __sinit_ov027_02112d1c(void)
{
    func_02017acc(data_ov027_02113c10, 0x2e4);
    func_020731dc(data_ov027_02113c10, &func_02017ab4, data_ov027_02113c30);
    _ZN13SharedFilePtr9ConstructEj(data_ov027_02113c18, 0x2e5);
    func_020731dc(data_ov027_02113c18, &SharedFilePtr_Destruct_Anim, data_ov027_02113c3c);
    _ZN13SharedFilePtr9ConstructEj(data_ov027_02113c08, 0x2e6);
    func_020731dc(data_ov027_02113c08, &SharedFilePtr_Destruct_Anim, data_ov027_02113c48);
    _ZN13SharedFilePtr9ConstructEj(data_ov027_02113c20, 0x2e7);
    func_020731dc(data_ov027_02113c20, &SharedFilePtr_Destruct_Anim, data_ov027_02113c54);
    _ZN13SharedFilePtr9ConstructEj(data_ov027_02113c28, 0x2e8);
    func_020731dc(data_ov027_02113c28, &SharedFilePtr_Destruct_Anim, data_ov027_02113c60);
}
