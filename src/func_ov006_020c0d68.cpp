//cpp
typedef int Fix12;
struct BCA_File;
struct BlendModelAnim { int pad; void SetAnim(BCA_File &f, int a, int b, Fix12 d, unsigned short e); };
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *, BCA_File &f, int a, int b, Fix12 d, unsigned short e);

struct Sound { static void PlayBank2_2D(unsigned int); };
extern "C" { extern double data_ov006_0213ac98; }

extern "C" void func_ov006_020c0d68(char *c)
{
    if (*(short *)(c + 0x1a) == 1) {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((BlendModelAnim *)(c + 0x1c), *(BCA_File *)*(int *)(c + 0x224), 0, 0x40000000, 0x800, 0);
    } else {
        Sound::PlayBank2_2D(0x13c);
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((BlendModelAnim *)(c + 0x1c), *(BCA_File *)*(int *)(c + 0x25c), 0, 0x40000000, 0x800, 0);
    }
    *(double *)c = data_ov006_0213ac98;
}
