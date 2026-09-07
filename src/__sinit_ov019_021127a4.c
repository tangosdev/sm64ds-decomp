extern int func_02017acc(void*, int);
extern void func_020731dc(void*, void*, void**);
extern int _ZN13SharedFilePtr9ConstructEj(void*, int);
extern int SharedFilePtr_Construct_TexSeq(void*, int);
extern void func_02017ab4(void);
extern void SharedFilePtr_Destruct_Anim(void);
extern void SharedFilePtr_Destruct_TexSeq(void);
extern void _ZN7Vector3D1Ev(void);

extern void* data_ov019_02113498;
extern void* data_ov019_021134dc;
extern void* data_ov019_02113470;
extern void* data_ov019_021134f4;
extern void* data_ov019_021134a0;
extern void* data_ov019_02113500;
extern void* data_ov019_02113478;
extern void* data_ov019_0211350c;
extern void* data_ov019_02113460;
extern void* data_ov019_02113524;
extern void* data_ov019_02113480;
extern void* data_ov019_02113530;
extern void* data_ov019_02113488;
extern void* data_ov019_02113548;
extern void* data_ov019_02113490;
extern void* data_ov019_02113554;
extern void* data_ov019_021134a8;
extern void* data_ov019_02113560;
extern void* data_ov019_021134b0;
extern void* data_ov019_021134b8;
extern void* data_ov019_02113468;
extern void* data_ov019_021134c4;

struct P2 { int a, b; };
extern struct P2 data_ov019_0211356c[12];
extern struct P2 data_ov019_021132b8;
extern struct P2 data_ov019_02113280;
extern struct P2 data_ov019_021132a8;
extern struct P2 data_ov019_021132b0;
extern struct P2 data_ov019_02113278;
extern struct P2 data_ov019_02113290;
extern struct P2 data_ov019_021132c0;
extern struct P2 data_ov019_021132a0;
extern struct P2 data_ov019_02113288;
extern struct P2 data_ov019_02113298;
extern struct P2 data_ov019_02113270;
extern struct P2 data_ov019_021132c8;

typedef struct { int x, y, z; } V3;
extern V3 data_ov019_021134e8;
extern void* data_ov019_021134d0;

extern V3 data_ov019_0211353c;
extern void* data_ov019_02113518;

void __sinit_ov019_021127a4(void)
{
    func_02017acc(&data_ov019_02113498, 0x3fb);
    func_020731dc(&data_ov019_02113498, (void*)&func_02017ab4, (void**)&data_ov019_021134dc);
    _ZN13SharedFilePtr9ConstructEj(&data_ov019_02113470, 0x406);
    func_020731dc(&data_ov019_02113470, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov019_021134f4);
    _ZN13SharedFilePtr9ConstructEj(&data_ov019_021134a0, 0x408);
    func_020731dc(&data_ov019_021134a0, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov019_02113500);
    _ZN13SharedFilePtr9ConstructEj(&data_ov019_02113478, 0x402);
    func_020731dc(&data_ov019_02113478, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov019_0211350c);
    _ZN13SharedFilePtr9ConstructEj(&data_ov019_02113460, 0x401);
    func_020731dc(&data_ov019_02113460, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov019_02113524);
    _ZN13SharedFilePtr9ConstructEj(&data_ov019_02113480, 0x400);
    func_020731dc(&data_ov019_02113480, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov019_02113530);
    _ZN13SharedFilePtr9ConstructEj(&data_ov019_02113488, 0x403);
    func_020731dc(&data_ov019_02113488, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov019_02113548);
    _ZN13SharedFilePtr9ConstructEj(&data_ov019_02113490, 0x405);
    func_020731dc(&data_ov019_02113490, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov019_02113554);
    SharedFilePtr_Construct_TexSeq(&data_ov019_021134a8, 0x3fd);
    func_020731dc(&data_ov019_021134a8, (void*)&SharedFilePtr_Destruct_TexSeq, (void**)&data_ov019_02113560);
    SharedFilePtr_Construct_TexSeq(&data_ov019_021134b0, 0x404);
    func_020731dc(&data_ov019_021134b0, (void*)&SharedFilePtr_Destruct_TexSeq, (void**)&data_ov019_021134b8);
    SharedFilePtr_Construct_TexSeq(&data_ov019_02113468, 0x407);
    func_020731dc(&data_ov019_02113468, (void*)&SharedFilePtr_Destruct_TexSeq, (void**)&data_ov019_021134c4);

    data_ov019_0211356c[0] = data_ov019_021132b8;
    data_ov019_0211356c[1] = data_ov019_02113280;
    data_ov019_0211356c[2] = data_ov019_021132a8;
    data_ov019_0211356c[3] = data_ov019_021132b0;
    data_ov019_0211356c[4] = data_ov019_02113278;
    data_ov019_0211356c[5] = data_ov019_02113290;
    data_ov019_0211356c[6] = data_ov019_021132c0;
    data_ov019_0211356c[7] = data_ov019_021132a0;
    data_ov019_0211356c[8] = data_ov019_02113288;
    data_ov019_0211356c[9] = data_ov019_02113298;
    data_ov019_0211356c[10] = data_ov019_02113270;
    data_ov019_0211356c[11] = data_ov019_021132c8;

    data_ov019_021134e8.x = -25962657;
    data_ov019_021134e8.y = -23381572;
    data_ov019_021134e8.z = -26155537;
    func_020731dc(&data_ov019_021134e8, (void*)&_ZN7Vector3D1Ev, (void**)&data_ov019_021134d0);

    data_ov019_0211353c.x = -19464192;
    data_ov019_0211353c.y = 0x600000;
    data_ov019_0211353c.z = 0;
    func_020731dc(&data_ov019_0211353c, (void*)&_ZN7Vector3D1Ev, (void**)&data_ov019_02113518);
}
