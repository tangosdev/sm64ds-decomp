#include "types.h"

extern int func_0203d974(void);
extern void func_ov075_02115134(void *c);
extern int func_0203da9c(void);
extern int func_ov075_02115098(void *c, int a1);
extern u8 func_02020168(void);
extern void func_02020304(void);
extern void func_020200e0(void);
extern void func_ov075_0211a148(char *c, int a, int b);
extern void func_02020334(void);
extern int func_0203d9b4(void);
extern int func_ov075_02116d40(char *c);
extern void func_02020124(void);

extern int data_ov075_0211d780;
extern int data_0208ee44;

void func_ov075_021190a4(char *c)
{
    if (func_0203d974() != 0) {
        if (*(int *)(c + 0x268) == 0) {
            func_ov075_02115134(*(void **)(c + 0x50));
            if (func_ov075_02115098(*(void **)(c + 0x50), func_0203da9c()) != 0)
                *(int *)(c + 0x268) = 1;
        }
        if (*(int *)(c + 0x268) != 0)
            *(u8 *)(c + 0x282) = 1;
    }

    if (func_02020168() != 0) {
        func_02020304();
        func_020200e0();
        func_ov075_0211a148(c, (int)&data_ov075_0211d780, 5);
        return;
    }

    if (*(int *)(c + 0x264) > 0) {
        int *q = (int *)(c + 0x264);
        *q = *q - data_0208ee44;
        return;
    }

    if (*(u8 *)(c + 0x282) == 0)
        return;
    func_02020334();
    if (func_0203d9b4() == 0)
        return;
    if (func_ov075_02116d40(c) == 0)
        return;
    func_02020124();
}
