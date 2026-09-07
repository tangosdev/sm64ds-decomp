//cpp
#include "types.h"
struct BCA_File;
struct ModelAnim { void SetAnim(BCA_File *f, int a, Fix12i b, unsigned int c); };
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, BCA_File *f, int a, Fix12i b, unsigned int c);


extern void *data_ov062_0211de00[];
extern int data_ov062_0211dec0;
extern "C" void Chuckya_ChangeState(void *c, void *p);

extern "C" int func_ov062_021164e8(char *c)
{
    int flag;
    int t;
    if (*(unsigned char *)(c + 0x3e5) == 0) {
        t = (*(int *)(c + 0xb0) & 0x4000) != 0;
        if (t) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((ModelAnim *)(c + 0x300), (BCA_File *)data_ov062_0211de00[1], 0, 0x1000, 0);
            *(unsigned char *)(c + 0x3e5) = 1;
        }
    }
    flag = *(int *)(c + 0xb0);
    t = (flag & 0x400) != 0;
    if (t) goto do_block;
    t = (flag & 0x2000) != 0;
    if (t) goto do_block;
    t = (flag & 0x100) != 0;
    if (t) goto done;
do_block:
    {
        void *m = *(void **)(c + 0x3f8);
        *(u16 *)(c + 0x94) = *(s16 *)((char *)m + 0x8e);
        *(int *)(c + 0x128) |= 2;
        Chuckya_ChangeState(c, &data_ov062_0211dec0);
        if (*(int *)((char *)*(void **)(c + 0x3f8) + 8) == 2) {
            *(int *)(c + 0xa8) = 0x50000;
            *(int *)(c + 0x98) = 0x14000;
        }
        *(void **)(c + 0x3f8) = 0;
    }
done:
    return 1;
}
