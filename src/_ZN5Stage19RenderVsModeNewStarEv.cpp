//cpp
#include "types.h"
extern "C" {
extern u16 data_0209f308;
extern u8 data_0209f204;
extern int data_0208ee44;
extern u8 data_0209f1f4;
extern int data_ov002_0210cfd0[];
extern int data_ov002_0210d050[];
extern int data_ov002_0210d0e8[];
extern int data_ov002_0210d188[];
extern int _ZN3OAM20VS_NEW_STAR_APPEAREDE[];
extern int GetOwnerLanguage(void);
extern void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int b, void* attr, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9);
extern unsigned int func_02012790(unsigned int a);
}

extern "C" void _ZN5Stage19RenderVsModeNewStarEv(void)
{
    u16 timer = data_0209f308;
    if (timer == 0)
        return;
    if (data_0209f204 != 0)
        return;
    data_0209f308 = timer - data_0208ee44;
    if (data_0209f308 % 60 >= 10) {
        if (GetOwnerLanguage() == 5)
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, data_ov002_0210cfd0, 0x80, 0x30, -1, -1, 0x1000, 0x1000, 0, -1);
        else if (GetOwnerLanguage() == 4)
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, data_ov002_0210d050, 0x80, 0x30, -1, -1, 0x1000, 0x1000, 0, -1);
        else if (GetOwnerLanguage() == 3)
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, data_ov002_0210d0e8, 0x80, 0x30, -1, -1, 0x1000, 0x1000, 0, -1);
        else if (GetOwnerLanguage() == 2)
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, data_ov002_0210d188, 0x80, 0x30, -1, -1, 0x1000, 0x1000, 0, -1);
        else
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, _ZN3OAM20VS_NEW_STAR_APPEAREDE, 0x80, 0x30, -1, -1, 0x1000, 0x1000, 0, -1);
        if (data_0209f1f4 != 0)
            return;
        func_02012790(0x2c);
        data_0209f1f4 = 1;
        return;
    }
    data_0209f1f4 = 0;
}
