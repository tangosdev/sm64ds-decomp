//cpp
typedef int Fix12;
struct BCA_File;
struct dBgCh_Actr { int IsOnGround() const; };
struct ModelAnim { void SetAnim(BCA_File *f, int a, Fix12 b, unsigned int c); };
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, BCA_File *f, int a, Fix12 b, unsigned int c);


void ApproachLinear(short &v, short t, short step);

extern "C" void func_ov064_021163c0(char *c)
{
    if (*(unsigned short *)(c + 0x100) == 0) {
        if (((dBgCh_Actr *)(c + 0x174))->IsOnGround()) {
            *(short *)(((int)c + 0x94)) = (short)((*(short *)(((int)c + 0x94))) + 0x8000);
        }
    }
    *(int *)(c + 0x98) = 0x5000;
    ApproachLinear(*(short *)(c + 0x8e), *(short *)(c + 0x94), 0x200);
    if (*(unsigned short *)(c + 0x100) < 0xf)
        return;
    *(short *)(c + 0x94) = *(short *)(c + 0x8e);
    *(int *)(c + 0x398) = 0;
    BCA_File *f = *(BCA_File **)(*(char **)(*(char **)(c + 0x330) + 0x10) + 4);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((ModelAnim *)(c + 0x110), f, 0, 0x1000, 0);
    return;
}
