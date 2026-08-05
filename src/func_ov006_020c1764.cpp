//cpp
typedef int Fix12;
struct BCA_File;
struct BlendModelAnim {
    void SetAnim(BCA_File &f, int a, int b, Fix12 c, unsigned short d);
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *, BCA_File &f, int a, int b, Fix12 c, unsigned short d);

struct Sound { static void PlayBank2_2D(unsigned int id); };
struct P2 { int w[2]; };

extern P2 data_ov006_0213ac50;

extern "C" void func_ov006_020c1764(char *c)
{
    if (*(short *)(c + 0x1a) == 1) {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((BlendModelAnim *)(c + 0x1c), *(BCA_File *)*(void **)(c + 0x234), 0, 0, 0x800, 0);
        if (*(int *)(c + 0x26c) == 0) {
            Sound::PlayBank2_2D(0x13a);
            *(int *)(c + 0x26c) = 1;
        }
    } else {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((BlendModelAnim *)(c + 0x1c), *(BCA_File *)*(void **)(c + 0x1fc), 0, 0, 0x800, 0);
    }
    *(P2 *)c = data_ov006_0213ac50;
}
