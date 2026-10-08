extern void func_020731dc(void*, void*, void*);
extern int data_ov019_021135d8[];
extern int _ZN7Vector3D1Ev[];
extern int data_ov019_021135cc[];

void __sinit_ov019_02112b14(void) {
    *(unsigned int*)data_ov019_021135d8 = 0xfed60000;
    *(unsigned int*)((char*)data_ov019_021135d8 + 4) = 0x700000;
    *(unsigned int*)((char*)data_ov019_021135d8 + 8) = (unsigned int)-0x200000;
    func_020731dc(data_ov019_021135d8, _ZN7Vector3D1Ev, data_ov019_021135cc);
}
