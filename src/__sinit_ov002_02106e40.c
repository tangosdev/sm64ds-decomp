extern int func_02017acc(void*, int);
extern void func_020731dc(void*, void*, void**);
extern int _ZN13SharedFilePtr9ConstructEj(void*, int);

extern void func_02017ab4(void);
extern void func_02017984(void);

extern void* POWER_STAR_MODEL_PTR;
extern void* data_ov002_0211096c;
extern void* TRANSPARENT_STAR_MODEL_PTR;
extern void* data_ov002_02110978;
extern void* data_ov002_02110954;
extern void* data_ov002_02110984;
extern void* data_ov002_0211093c;
extern void* data_ov002_02110990;
extern void* data_ov002_0211092c;
extern void* data_ov002_0211099c;
extern void* data_ov002_02110944;
extern void* data_ov002_021109a8;
extern void* data_ov002_02110924;
extern void* data_ov002_021109b4;
extern void* data_ov002_02110964;
extern void* data_ov002_021109c0;
extern void* data_ov002_02110934;
extern void* data_ov002_021109cc;

struct P2 { int a, b; };
extern struct P2 data_ov002_021109d8[14];

extern struct P2 data_ov002_0210a9a8;
extern struct P2 data_ov002_0210a9f8;
extern struct P2 data_ov002_0210a9f0;
extern struct P2 data_ov002_0210a9e8;
extern struct P2 data_ov002_0210a9a0;
extern struct P2 data_ov002_0210a9e0;
extern struct P2 data_ov002_0210a998;
extern struct P2 data_ov002_0210a9b8;
extern struct P2 data_ov002_0210a9c8;
extern struct P2 data_ov002_0210a9d8;
extern struct P2 data_ov002_0210a9d0;
extern struct P2 data_ov002_0210a990;
extern struct P2 data_ov002_0210a9b0;
extern struct P2 data_ov002_0210a9c0;

void __sinit_ov002_02106e40(void)
{
    func_02017acc(&POWER_STAR_MODEL_PTR, 0x8015);
    func_020731dc(&POWER_STAR_MODEL_PTR, (void*)&func_02017ab4, (void**)&data_ov002_0211096c);
    func_02017acc(&TRANSPARENT_STAR_MODEL_PTR, 0x8017);
    func_020731dc(&TRANSPARENT_STAR_MODEL_PTR, (void*)&func_02017ab4, (void**)&data_ov002_02110978);
    func_02017acc(&data_ov002_02110954, 0x8019);
    func_020731dc(&data_ov002_02110954, (void*)&func_02017ab4, (void**)&data_ov002_02110984);
    func_02017acc(&data_ov002_0211093c, 0x4a0);
    func_020731dc(&data_ov002_0211093c, (void*)&func_02017ab4, (void**)&data_ov002_02110990);
    func_02017acc(&data_ov002_0211092c, 0x801c);
    func_020731dc(&data_ov002_0211092c, (void*)&func_02017ab4, (void**)&data_ov002_0211099c);
    _ZN13SharedFilePtr9ConstructEj(&data_ov002_02110944, 0x8016);
    func_020731dc(&data_ov002_02110944, (void*)&func_02017984, (void**)&data_ov002_021109a8);
    _ZN13SharedFilePtr9ConstructEj(&data_ov002_02110924, 0x801a);
    func_020731dc(&data_ov002_02110924, (void*)&func_02017984, (void**)&data_ov002_021109b4);
    _ZN13SharedFilePtr9ConstructEj(&data_ov002_02110964, 0x801b);
    func_020731dc(&data_ov002_02110964, (void*)&func_02017984, (void**)&data_ov002_021109c0);
    _ZN13SharedFilePtr9ConstructEj(&data_ov002_02110934, 0x8018);
    func_020731dc(&data_ov002_02110934, (void*)&func_02017984, (void**)&data_ov002_021109cc);

    data_ov002_021109d8[0] = data_ov002_0210a9a8;
    data_ov002_021109d8[1] = data_ov002_0210a9f8;
    data_ov002_021109d8[2] = data_ov002_0210a9f0;
    data_ov002_021109d8[3] = data_ov002_0210a9e8;
    data_ov002_021109d8[4] = data_ov002_0210a9a0;
    data_ov002_021109d8[5] = data_ov002_0210a9e0;
    data_ov002_021109d8[6] = data_ov002_0210a998;
    data_ov002_021109d8[7] = data_ov002_0210a9b8;
    data_ov002_021109d8[8] = data_ov002_0210a9c8;
    data_ov002_021109d8[9] = data_ov002_0210a9d8;
    data_ov002_021109d8[10] = data_ov002_0210a9d0;
    data_ov002_021109d8[11] = data_ov002_0210a990;
    data_ov002_021109d8[12] = data_ov002_0210a9b0;
    data_ov002_021109d8[13] = data_ov002_0210a9c0;
}
