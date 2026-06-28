//cpp
extern "C" {
extern void func_ov078_02125c48(void* c, void* p);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void* thiz);
extern void func_ov078_02125c24(void* c, int n);
extern void func_0200fa8c(void* t, int a);
extern void func_02012694(int a, void* v);
extern int data_02092138;
extern int data_ov078_021270bc[];

int func_ov078_02124bc4(char* c)
{
    if (*(unsigned short*)(c + 0x100) != 0) return 1;
    if (*(int*)&data_02092138 > *(int*)(c + 0x60)) {
        func_ov078_02125c48(c, data_ov078_021270bc);
        return 1;
    }
    if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x110) != 0) {
        if (*(unsigned char*)(c + 0x499) == 0) {
            func_ov078_02125c24(c, 0x7d0000);
            func_0200fa8c(c, 1);
            func_02012694(0x128, c + 0x74);
            *(short*)(c + 0x100) = 5;
            *(int*)(c + 0xa8) = 0x14000;
            *(int*)(c + 0x98) = 0xa000;
            *(unsigned char*)(c + 0x499) = 1;
        } else {
            func_ov078_02125c48(c, data_ov078_021270bc);
        }
    }
    return 1;
}
}
