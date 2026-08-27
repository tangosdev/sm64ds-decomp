extern void _ZN13SharedFilePtr9ConstructEj();
extern void func_020731dc(void*, void*, void*);
extern void SharedFilePtr_Destruct_Anim();
extern int _ZN7Vector3D1Ev[];
extern int data_ov102_0214e9c0[];
extern int data_ov102_0214e9c8[];
extern int data_ov102_0214e9d0[];
extern int data_ov102_0214e9dc[];
extern int data_ov102_0214ea0c[];
extern int data_ov102_0214e9f4[];
extern int data_ov102_0214ea00[];
extern int data_ov102_0214e9e8[];

typedef struct { int x, y, z; } V3;
extern V3 data_ov102_0214ea18;
extern V3 data_ov102_0214ea24;
extern V3 data_ov102_0214ea30;
extern V3 data_ov102_0214ea3c;

void __sinit_ov102_0214de70(void)
{
    _ZN13SharedFilePtr9ConstructEj(data_ov102_0214e9c0, 0x2b8);
    func_020731dc(data_ov102_0214e9c0, SharedFilePtr_Destruct_Anim, data_ov102_0214e9d0);
    _ZN13SharedFilePtr9ConstructEj(data_ov102_0214e9c8, 0x2b7);
    func_020731dc(data_ov102_0214e9c8, SharedFilePtr_Destruct_Anim, data_ov102_0214e9dc);

    data_ov102_0214ea18.x = 0x1c000;
    data_ov102_0214ea18.y = -0x14000;
    data_ov102_0214ea18.z = 0x28000;
    func_020731dc(&data_ov102_0214ea18, _ZN7Vector3D1Ev, data_ov102_0214ea0c);

    { volatile V3* p = (volatile V3*)&data_ov102_0214ea24; p->x = 0x1c000; p->y = -0x14000; p->z = 0x3c000; }
    func_020731dc(&data_ov102_0214ea24, _ZN7Vector3D1Ev, data_ov102_0214e9f4);

    { volatile V3* p = (volatile V3*)&data_ov102_0214ea30; p->x = 0x2b000; p->y = -0xa000; p->z = -0x14000; }
    func_020731dc(&data_ov102_0214ea30, _ZN7Vector3D1Ev, data_ov102_0214ea00);

    { volatile V3* p = (volatile V3*)&data_ov102_0214ea3c; p->x = 0x30000; p->y = -0x1e000; p->z = 0x3c000; }
    func_020731dc(&data_ov102_0214ea3c, _ZN7Vector3D1Ev, data_ov102_0214e9e8);
}
