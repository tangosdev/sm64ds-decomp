extern int func_02017acc(void*, int);
extern void func_020731dc(void*, void*, void**);
extern int _ZN13SharedFilePtr9ConstructEj(void*, int);
extern int SharedFilePtr_Construct_TexSeq(void*, int);
extern void func_02017ab4(void);
extern void SharedFilePtr_Destruct_Anim(void);
extern void SharedFilePtr_Destruct_TexSeq(void);
extern void func_020072c0(void);

extern void* data_ov085_0213074c;
extern void* data_ov085_02130744;
extern void* data_ov085_0213073c;
extern void* data_ov085_0213076c;
extern void* data_ov085_02130778;
extern void* data_ov085_02130784;
extern void* data_ov085_02130754;
extern void* data_ov085_02130760;

typedef struct { int a, b; } S8;
typedef struct { S8 x, y; } S16;
typedef struct { int a, b, c; } S12;

extern S16 data_ov085_021307d0;
extern S16 data_ov085_021307e0;
extern S16 data_ov085_02130800;
extern S16 data_ov085_02130810;
extern S16 data_ov085_02130830;
extern S16 data_ov085_02130790;
extern S16 data_ov085_021307b0;
extern S16 data_ov085_021307c0;
extern S16 data_ov085_021307f0;
extern S16 data_ov085_02130820;
extern S16 data_ov085_021307a0;

extern S8 data_ov085_0213025c;
extern S8 data_ov085_021302d4;
extern S8 data_ov085_021302bc;
extern S8 data_ov085_0213026c;
extern S8 data_ov085_02130254;
extern S8 data_ov085_021302dc;
extern S8 data_ov085_021302a4;
extern S8 data_ov085_02130294;
extern S8 data_ov085_02130264;
extern S8 data_ov085_021302f4;
extern S8 data_ov085_021302cc;
extern S8 data_ov085_021302fc;
extern S8 data_ov085_021302ec;
extern S8 data_ov085_0213028c;
extern S8 data_ov085_021302e4;
extern S8 data_ov085_021302b4;
extern S8 data_ov085_021302c4;
extern S8 data_ov085_0213027c;
extern S8 data_ov085_021302ac;
extern S8 data_ov085_02130284;
extern S8 data_ov085_0213029c;
extern S8 data_ov085_02130274;

extern S12 data_ov085_02130840;
extern S12 data_ov085_0213084c;

void __sinit_ov085_0212fa40(void)
{
    func_02017acc(&data_ov085_0213074c, 0x2d7);
    func_020731dc(&data_ov085_0213074c, (void*)&func_02017ab4, (void**)&data_ov085_0213076c);
    _ZN13SharedFilePtr9ConstructEj(&data_ov085_02130744, 0x2d8);
    func_020731dc(&data_ov085_02130744, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov085_02130778);
    SharedFilePtr_Construct_TexSeq(&data_ov085_0213073c, 0x2d9);
    func_020731dc(&data_ov085_0213073c, (void*)&SharedFilePtr_Destruct_TexSeq, (void**)&data_ov085_02130784);

    data_ov085_021307d0.x = data_ov085_0213025c;
    data_ov085_021307d0.y = data_ov085_021302d4;
    data_ov085_021307e0.x = data_ov085_021302bc;
    data_ov085_021307e0.y = data_ov085_0213026c;
    data_ov085_02130800.x = data_ov085_02130254;
    data_ov085_02130800.y = data_ov085_021302dc;
    data_ov085_02130810.x = data_ov085_021302a4;
    data_ov085_02130810.y = data_ov085_02130294;
    data_ov085_02130830.x = data_ov085_02130264;
    data_ov085_02130830.y = data_ov085_021302f4;
    data_ov085_02130790.x = data_ov085_021302cc;
    data_ov085_02130790.y = data_ov085_021302fc;
    data_ov085_021307b0.x = data_ov085_021302ec;
    data_ov085_021307b0.y = data_ov085_0213028c;
    data_ov085_021307c0.x = data_ov085_021302e4;
    data_ov085_021307c0.y = data_ov085_021302b4;
    data_ov085_021307f0.x = data_ov085_021302c4;
    data_ov085_021307f0.y = data_ov085_0213027c;
    data_ov085_02130820.x = data_ov085_021302ac;
    data_ov085_02130820.y = data_ov085_02130284;
    data_ov085_021307a0.x = data_ov085_0213029c;
    data_ov085_021307a0.y = data_ov085_02130274;

    data_ov085_02130840.a = -0x640000;
    data_ov085_02130840.b = 0x378000;
    data_ov085_02130840.c = 0x1670000;
    func_020731dc(&data_ov085_02130840, (void*)&func_020072c0, (void**)&data_ov085_02130754);

    { volatile S12* p = (volatile S12*)&data_ov085_0213084c; p->a = 0; p->b = 0x4d8000; p->c = -0x104000; }
    func_020731dc(&data_ov085_0213084c, (void*)&func_020072c0, (void**)&data_ov085_02130760);
}
