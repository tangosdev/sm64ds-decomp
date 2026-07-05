//cpp
typedef int Fix12;
struct BCA_File;
struct WithMeshClsn { int IsOnGround() const; };
struct ModelAnim { void SetAnim(BCA_File *f, int a, Fix12 b, unsigned int c); };

void ApproachLinear(short &v, short t, short step);

extern "C" void func_ov064_021163c0(char *c)
{
    if (*(unsigned short *)(c + 0x100) == 0) {
        if (((WithMeshClsn *)(c + 0x174))->IsOnGround()) {
            *(short *)(((int)c + 0x94) & 0xFFFFFFFFFFFFFFFF) = (short)((*(short *)(((int)c + 0x94) & 0xFFFFFFFFFFFFFFFF)) + 0x8000);
        }
    }
    *(int *)(c + 0x98) = 0x5000;
    ApproachLinear(*(short *)(c + 0x8e), *(short *)(c + 0x94), 0x200);
    if (*(unsigned short *)(c + 0x100) < 0xf)
        return;
    *(short *)(c + 0x94) = *(short *)(c + 0x8e);
    *(int *)(c + 0x398) = 0;
    BCA_File *f = *(BCA_File **)(*(char **)(*(char **)(c + 0x330) + 0x10) + 4);
    ((ModelAnim *)(c + 0x110))->SetAnim(f, 0, 0x1000, 0);
    return;
}
