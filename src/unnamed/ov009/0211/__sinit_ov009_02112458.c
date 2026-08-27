extern void func_02017acc(void*, int);
extern void func_020731dc(void*, void*, void*);
extern void _ZN13SharedFilePtr9ConstructEj(void*, unsigned int);
extern int func_02017ab4[];
extern int SharedFilePtr_Destruct_Anim[];
extern int data_ov009_02113c20[];
extern int data_ov009_02113c28[];
extern int data_ov009_02113c30[];
extern int data_ov009_02113c3c[];
struct S8 { int a, b; };
extern struct S8 data_ov009_02113c48;
extern struct S8 data_ov009_02113914;
extern struct S8 data_ov009_0211390c;
extern struct S8 data_ov009_021138fc;
extern struct S8 data_ov009_02113904;

void __sinit_ov009_02112458(void)
{
    struct S8 *d = &data_ov009_02113c48;
    func_02017acc(data_ov009_02113c20, 0x438);
    func_020731dc(data_ov009_02113c20, func_02017ab4, data_ov009_02113c30);
    _ZN13SharedFilePtr9ConstructEj(data_ov009_02113c28, 0x439);
    func_020731dc(data_ov009_02113c28, SharedFilePtr_Destruct_Anim, data_ov009_02113c3c);
    d[0] = data_ov009_02113914;
    d[1] = data_ov009_0211390c;
    d[2] = data_ov009_021138fc;
    d[3] = data_ov009_02113904;
}
