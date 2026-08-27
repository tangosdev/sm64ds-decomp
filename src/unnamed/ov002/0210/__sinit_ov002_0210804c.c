extern void func_020731dc(void*, void*, void*);
extern int data_ov002_0211116c[];
extern int _ZN7Vector3D1Ev[];
extern int data_ov002_02111160[];

void __sinit_ov002_0210804c(void) {
    *(unsigned int*)data_ov002_0211116c = 0x80000;
    *(unsigned int*)((char*)data_ov002_0211116c + 4) = 0;
    *(unsigned int*)((char*)data_ov002_0211116c + 8) = 0x60000;
    func_020731dc(data_ov002_0211116c, _ZN7Vector3D1Ev, data_ov002_02111160);
}
