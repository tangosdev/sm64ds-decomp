//cpp
struct BCA_File;
struct WithMeshClsn { int IsOnGround() const; };
struct ModelAnim { void SetAnim(BCA_File *f, int b, int c, unsigned int d); };

extern "C" int _Z14ApproachLinearRiii(int *dst, int target, int rate);

extern "C" void func_ov064_02116460(char *self)
{
    if (_Z14ApproachLinearRiii((int *)(self + 0x98), 0, *(int *)(self + 0x3e8)) != 0) {
        int b = (*(unsigned short *)(self + 0xc) == 0xd8);
        int lim = b ? 0x14 : 0xa;
        if (*(unsigned char *)(self + 0x3f9) < 2) {
            if (((WithMeshClsn *)(self + 0x174))->IsOnGround()) {
                unsigned char *p = (unsigned char *)(((long long)(int)(self + 0x3f9)) & 0xFFFFFFFFFFFFFFFFLL);
                *(int *)(self + 0xa8) = 0xf000;
                *p = *p + 1;
            }
            *(short *)(self + 0x100) = 0;
            return;
        }
        if (*(unsigned short *)(self + 0x100) < (unsigned int)lim)
            return;
        *(int *)(self + 0x398) = 1;
        *(short *)(self + 0x94) = *(short *)(self + 0x8e);
        {
            BCA_File *anim = (BCA_File *)*(int *)(*(int *)(*(int *)(self + 0x330) + 0x10) + 4);
            ((ModelAnim *)(self + 0x110))->SetAnim(anim, 0, 0x2000, 0);
        }
        return;
    }
    *(short *)(self + 0x100) = 0;
}
