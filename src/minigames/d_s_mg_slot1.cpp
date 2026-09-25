//cpp
/* Slot machine scene factory (MG_SLOT1). The scene's methods live in
 * src/actors/dScMgSlot1_c.cpp.
 *
 * The allocation, the base C2 call and the three vptr stores are written out
 * by hand: the ROM has no dScMgSlot1_c C1, so `new dScMgSlot1_c` cannot link.
 * Still raw: pad_4684, pad_4690 and pad_469c are embedded objects of unknown
 * type, set up by the unnamed func_ov006_0210c2b0 and func_ov006_0210c208.
 */

#include "dScMgSlot1_c.h"

extern "C" {
extern void _ZN11dScMgBase_cC2Ev(void *);
extern void func_ov006_0210c2b0(char *p);
extern void func_ov006_0210c208(char *p);
extern void *_ZTV12dScMgSlot1_c;
extern void *_ZTVN12dScMgSlot1_c9betIcon_cE;
extern void *_ZTV9dThIcon_c;
void *dScMgSlot1_c_classInit(void);
/* The name is reconstructed; the ROM proves the class, not the spelling. */
void *dScMgSlot1_c_classInit(void)
{
    dScMgSlot1_c *scene =
        (dScMgSlot1_c *)_ZN7fBase_cnwEj(sizeof(dScMgSlot1_c));
    if (scene) {
        _ZN11dScMgBase_cC2Ev(scene);
        *(void **)scene = &_ZTV12dScMgSlot1_c;
        dScMgSlot1_c::betIcon_c *icon = &scene->mBetIcon;
        *(void **)icon = &_ZTV9dThIcon_c;
        *(void **)icon = &_ZTVN12dScMgSlot1_c9betIcon_cE;
        func_ov006_0210c2b0((char *)scene->pad_4684);
        func_ov006_0210c2b0((char *)scene->pad_4690);
        func_ov006_0210c208((char *)scene->pad_469c);
    }
    return scene;
}
}
