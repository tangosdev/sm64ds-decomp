#include "types.h"
extern s32 RENDER_DMA_CHANNEL;    /* 0x02099fd0 */
extern u32 GX_texBank;            /* 0x020a60b8 */
extern u32 GX_texSlotA;           /* 0x020a60ac */
extern u32 GX_texSlotB;           /* 0x020a60bc */
extern u32 GX_texSlotC;           /* 0x020a60c0 */

extern void func_02059fa8(s32 channel);
extern void _ZN2GX13SetBankForTexEt(u32 tex);

void _ZN2GX10EndLoadTexEv(void)
{
    if (RENDER_DMA_CHANNEL != -1) {
        func_02059fa8(RENDER_DMA_CHANNEL);
    }
    _ZN2GX13SetBankForTexEt(GX_texBank);
    GX_texSlotC = 0;
    GX_texSlotB = 0;
    GX_texSlotA = 0;
    GX_texBank  = 0;
}
