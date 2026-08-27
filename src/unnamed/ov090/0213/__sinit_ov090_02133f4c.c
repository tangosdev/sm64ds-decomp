extern void func_02017acc();
extern void func_020731dc();
extern void _ZN13SharedFilePtr9ConstructEj();
extern void func_02017ab4(void);
extern void SharedFilePtr_Destruct_Anim(void);

extern int data_ov090_02134564[];
extern int data_ov090_0213456c[];
extern int data_ov090_0213455c[];
extern int data_ov090_02134578[];

typedef struct { int a, b; } S8;
extern S8 data_ov090_021342c0;
extern S8 data_ov090_021342b8;
extern S8 data_ov090_021342d0;
extern S8 data_ov090_021342c8;
typedef struct { S8 x, y; } S16;
extern S16 data_ov090_02134594;
extern S16 data_ov090_02134584;

void __sinit_ov090_02133f4c(void)
{
    func_02017acc(data_ov090_02134564, 0x418);
    func_020731dc(data_ov090_02134564, &func_02017ab4, data_ov090_0213456c);
    _ZN13SharedFilePtr9ConstructEj(data_ov090_0213455c, 0x419);
    func_020731dc(data_ov090_0213455c, &SharedFilePtr_Destruct_Anim, data_ov090_02134578);
    data_ov090_02134594.x = data_ov090_021342c0;
    data_ov090_02134594.y = data_ov090_021342b8;
    data_ov090_02134584.x = data_ov090_021342d0;
    data_ov090_02134584.y = data_ov090_021342c8;
}
