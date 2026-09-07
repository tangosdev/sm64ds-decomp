extern int func_02017acc(void*, int);
extern void func_020731dc(void*, void*, void**);
extern int _ZN13SharedFilePtr9ConstructEj(void*, int);
extern void func_02017ab4(void);
extern void SharedFilePtr_Destruct_Anim(void);
extern void _ZN7Vector3D1Ev(void);

extern void* data_ov062_0211ddf0;
extern void* data_ov062_0211de10;
extern void* data_ov062_0211dde8;
extern void* data_ov062_0211de1c;
extern void* data_ov062_0211dde0;
extern void* data_ov062_0211de28;
extern void* data_ov062_0211de00;
extern void* data_ov062_0211de34;
extern void* data_ov062_0211de08;
extern void* data_ov062_0211de40;
extern void* data_ov062_0211ddf8;
extern void* data_ov062_0211de4c;

typedef struct { int x, y, z; } V3;
extern V3 data_ov062_0211df10;
extern void* data_ov062_0211de58;
extern V3 data_ov062_0211df1c;
extern void* data_ov062_0211de64;

typedef struct { int a, b; } S8;
struct P2 { S8 a, b; };
extern struct P2 data_ov062_0211de70;
extern struct P2 data_ov062_0211de80;
extern struct P2 data_ov062_0211de90;
extern struct P2 data_ov062_0211dea0;
extern struct P2 data_ov062_0211deb0;
extern struct P2 data_ov062_0211dec0;
extern struct P2 data_ov062_0211ded0;
extern struct P2 data_ov062_0211dee0;
extern struct P2 data_ov062_0211def0;
extern struct P2 data_ov062_0211df00;

extern S8 data_ov062_0211d940, data_ov062_0211d938;
extern S8 data_ov062_0211d998, data_ov062_0211d950;
extern S8 data_ov062_0211d958, data_ov062_0211d960;
extern S8 data_ov062_0211d920, data_ov062_0211d908;
extern S8 data_ov062_0211d900, data_ov062_0211d918;
extern S8 data_ov062_0211d980, data_ov062_0211d990;
extern S8 data_ov062_0211d988, data_ov062_0211d930;
extern S8 data_ov062_0211d978, data_ov062_0211d910;
extern S8 data_ov062_0211d968, data_ov062_0211d928;
extern S8 data_ov062_0211d948, data_ov062_0211d970;

void __sinit_ov062_0211cf30(void)
{
    func_02017acc(&data_ov062_0211ddf0, 0x327);
    func_020731dc(&data_ov062_0211ddf0, (void*)&func_02017ab4, (void**)&data_ov062_0211de10);
    _ZN13SharedFilePtr9ConstructEj(&data_ov062_0211dde8, 0x328);
    func_020731dc(&data_ov062_0211dde8, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov062_0211de1c);
    _ZN13SharedFilePtr9ConstructEj(&data_ov062_0211dde0, 0x329);
    func_020731dc(&data_ov062_0211dde0, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov062_0211de28);
    _ZN13SharedFilePtr9ConstructEj(&data_ov062_0211de00, 0x32a);
    func_020731dc(&data_ov062_0211de00, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov062_0211de34);
    _ZN13SharedFilePtr9ConstructEj(&data_ov062_0211de08, 0x32b);
    func_020731dc(&data_ov062_0211de08, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov062_0211de40);
    _ZN13SharedFilePtr9ConstructEj(&data_ov062_0211ddf8, 0x32c);
    func_020731dc(&data_ov062_0211ddf8, (void*)&SharedFilePtr_Destruct_Anim, (void**)&data_ov062_0211de4c);

    data_ov062_0211df10.x = 0x2c000;
    data_ov062_0211df10.y = -0x14000;
    data_ov062_0211df10.z = -0x78000;
    func_020731dc(&data_ov062_0211df10, (void*)&_ZN7Vector3D1Ev, (void**)&data_ov062_0211de58);

    int* p_df1c = (int*)&data_ov062_0211df1c;
    *p_df1c = 0x44000;
    p_df1c[1] = -0x14000;
    p_df1c[2] = -0x78000;
    func_020731dc(&data_ov062_0211df1c, (void*)&_ZN7Vector3D1Ev, (void**)&data_ov062_0211de64);

    data_ov062_0211ded0.a = data_ov062_0211d940;
    data_ov062_0211ded0.b = data_ov062_0211d938;
    data_ov062_0211dee0.a = data_ov062_0211d998;
    data_ov062_0211dee0.b = data_ov062_0211d950;
    data_ov062_0211df00.a = data_ov062_0211d958;
    data_ov062_0211df00.b = data_ov062_0211d960;
    data_ov062_0211de70.a = data_ov062_0211d920;
    data_ov062_0211de70.b = data_ov062_0211d908;
    data_ov062_0211de90.a = data_ov062_0211d900;
    data_ov062_0211de90.b = data_ov062_0211d918;
    data_ov062_0211dea0.a = data_ov062_0211d980;
    data_ov062_0211dea0.b = data_ov062_0211d990;
    data_ov062_0211dec0.a = data_ov062_0211d988;
    data_ov062_0211dec0.b = data_ov062_0211d930;
    data_ov062_0211def0.a = data_ov062_0211d978;
    data_ov062_0211def0.b = data_ov062_0211d910;
    data_ov062_0211de80.a = data_ov062_0211d968;
    data_ov062_0211de80.b = data_ov062_0211d928;
    data_ov062_0211deb0.a = data_ov062_0211d948;
    data_ov062_0211deb0.b = data_ov062_0211d970;
}
