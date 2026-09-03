#include "common.h"
// 6f: keep constant live / flip coloring
#pragma opt_propagation off

extern void Hud_RenderSprite(void *a0, int a1, int a2, int a3, int a4);
extern u16 data_ov006_0213d45c[];
extern void *data_ov006_02142490[];

void _ZN14dScMgMemory2_c9DrawCardsEv(char *a0)
{
    int i;
    int k, flag;
    char *p;

    /* p walks 20 HUD-sprite records at a0+0x51a8, stride 0x18.
       data_ov006_0213d45c is indexed as [id][5]. */
    p = a0;
    i = 0;
    do {
        if (*(u8 *)(p + 0x51ba) != 0) {
            flag = 0;
            if (*(int *)(a0 + 0x53d4) != 2)
                flag = 1;
            k = data_ov006_0213d45c[*(u8 *)(p + 0x51b8) * 5
                                    + *(u8 *)(p + 0x51bd)];
            Hud_RenderSprite(data_ov006_02142490[k],
                             *(int *)(p + 0x51a8) >> 12,
                             *(int *)(p + 0x51ac) >> 12, -1, flag);
        }
        i += 1;
        p += 0x18;
    } while (i < 20);
}
