#include "TextureSequence.h"
//cpp
struct BMD_File;
struct BTP_File;
extern "C" {
struct E { int w[2]; };
extern int *data_ov060_021192dc[];
extern E data_ov060_0211ac78;
extern E data_ov060_0211ac40;
extern E data_ov060_0211acb8;
extern E data_ov060_0211ac10;
extern E data_ov060_0211abf0;
extern E data_ov060_0211ac30;
extern E data_ov060_0211ac28;
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *anim, void *file, int a, int d, unsigned e);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *ts, void *file, int a, int d, unsigned e);
extern void _ZN9Animation8SetFlagsEi(void *anim, int flags);

void func_ov060_02111cc0(char *c, int idx)
{
    int a;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (void *)data_ov060_021192dc[idx][1], a, 0x1000, 0);
    switch (idx) {
    case 1:
        TextureSequence::Prepare(*(BMD_File *)data_ov060_0211ac78.w[1], *(BTP_File *)data_ov060_0211ac40.w[1]);
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0x138, (void *)data_ov060_0211ac40.w[1], 0, 0x1000, 0);
        _ZN9Animation8SetFlagsEi(c + 0x138, 0x40000000);
        return;
    case 2:
        TextureSequence::Prepare(*(BMD_File *)data_ov060_0211ac78.w[1], *(BTP_File *)data_ov060_0211acb8.w[1]);
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0x138, (void *)data_ov060_0211acb8.w[1], 0, 0x1000, 0);
        _ZN9Animation8SetFlagsEi(c + 0x138, 0x40000000);
        return;
    case 3:
    case 5:
        TextureSequence::Prepare(*(BMD_File *)data_ov060_0211ac78.w[1], *(BTP_File *)data_ov060_0211ac10.w[1]);
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0x138, (void *)data_ov060_0211ac10.w[1], 0, 0x1000, 0);
        _ZN9Animation8SetFlagsEi(c + 0x138, 0);
        return;
    case 14:
        TextureSequence::Prepare(*(BMD_File *)data_ov060_0211ac78.w[1], *(BTP_File *)data_ov060_0211abf0.w[1]);
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0x138, (void *)data_ov060_0211abf0.w[1], 0, 0x1000, 0);
        _ZN9Animation8SetFlagsEi(c + 0x138, 0);
        return;
    case 10:
        TextureSequence::Prepare(*(BMD_File *)data_ov060_0211ac78.w[1], *(BTP_File *)data_ov060_0211ac30.w[1]);
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0x138, (void *)data_ov060_0211ac30.w[1], 0, 0x1000, 0);
        _ZN9Animation8SetFlagsEi(c + 0x138, 0x40000000);
        return;
    case 0:
    default:
        TextureSequence::Prepare(*(BMD_File *)data_ov060_0211ac78.w[1], *(BTP_File *)data_ov060_0211ac28.w[1]);
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0x138, (void *)data_ov060_0211ac28.w[1], 0, 0x1000, 0);
        _ZN9Animation8SetFlagsEi(c + 0x138, 0);
        return;
    }
}
}
