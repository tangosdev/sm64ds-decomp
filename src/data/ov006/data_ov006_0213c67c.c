/*
 * Minigame-overlay data at 0x0213c67c..0x0213c6c4.
 *
 * Sprite templates are OamAttr lists (terminated by 0xffff in
 * attr3); the tables that index them hold OamAttr pointers. A
 * word the module relocates is written as its target symbol so
 * the link re-creates the relocation; the rest are the
 * cartridge words.
 */
#include "types.h"

extern void _ZN11dScMgBase_c10OnDealDoneEv(void *);
extern void _ZN11dScMgBase_c11OnNewRecordEv(void *);
extern void _ZN11dScMgBase_c11PaintSquareEiiii(void *, int, int, int, int);
extern void _ZN11dScMgBase_c16InitMainScreenBgEv(void *);
extern int _ZN11dScMgBase_c16OnGraphCallback0Ev(void *);
extern int _ZN11dScMgBase_c16OnGraphCallback1Ev(void *);
extern int _ZN12dEnemyBase_c19func_ov002_020ad660EPvS0_i(void *, void *, void *, int);
extern void _ZN14dScMgD3DBase_c11InitEnginesEv(void *);
extern int _ZN14dScMgD3DBase_c14GetDisplayModeEv(void *);
extern int _ZN14dScMgD3DBase_c15InitSubScreenBgEv(void *);
extern int _ZN14dScMgD3DBase_c16OnGraphCallback2Ev(void *);
extern int _ZN14dScMgD3DBase_c16OnGraphCallback3Ev(void *);
extern int _ZN14dScMgD3DBase_c17SaveScreenForMenuEv(void *);
extern void _ZN14dScMgD3DBase_c21RestoreScreenFromMenuEv(void *);
extern void _ZN14dScMgD3DBase_c8OpenMenuEv(void *);
extern int _ZN14dScMgD3DBase_c9CloseMenuEv(void *);

#pragma define_section obj ".data.w13b814" ".data.w13b814" ".data.w13b814" far_data
#pragma section obj begin
void * data_ov006_0213c67c[16] = {
    (void *)&_ZN11dScMgBase_c11OnNewRecordEv,
    (void *)&_ZN11dScMgBase_c10OnDealDoneEv,
    (void *)&_ZN11dScMgBase_c16OnGraphCallback1Ev,
    (void *)&_ZN11dScMgBase_c16OnGraphCallback0Ev,
    (void *)&_ZN14dScMgD3DBase_c16OnGraphCallback2Ev,
    (void *)&_ZN14dScMgD3DBase_c16OnGraphCallback3Ev,
    (void *)&_ZN14dScMgD3DBase_c14GetDisplayModeEv,
    (void *)&_ZN14dScMgD3DBase_c8OpenMenuEv,
    (void *)&_ZN14dScMgD3DBase_c9CloseMenuEv,
    (void *)&_ZN14dScMgD3DBase_c17SaveScreenForMenuEv,
    (void *)&_ZN14dScMgD3DBase_c21RestoreScreenFromMenuEv,
    (void *)&_ZN14dScMgD3DBase_c15InitSubScreenBgEv,
    (void *)&_ZN11dScMgBase_c16InitMainScreenBgEv,
    (void *)&_ZN14dScMgD3DBase_c11InitEnginesEv,
    (void *)&_ZN11dScMgBase_c11PaintSquareEiiii,
    (void *)&_ZN12dEnemyBase_c19func_ov002_020ad660EPvS0_i,
};
#pragma section obj end
