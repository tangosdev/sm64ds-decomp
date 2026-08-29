//cpp
/* func_ov006_0210d6b8 at 0x0210d6b8 (ov006), size 0x88
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 * flags: -O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
#include "dScMgSlot1_c.h"

extern "C" {
extern void *_ZN7fBase_cnwEj(unsigned int sz);
extern void _ZN11dScMgBase_cC2Ev(void *);
extern void func_ov006_0210c2b0(char *p);
extern void func_ov006_0210c208(char *p);
extern void *_ZTV12dScMgSlot1_c;
extern void *_ZTVN12dScMgSlot1_c9betIcon_cE;
extern void *_ZTV9dThIcon_c;
void *func_ov006_0210d6b8(void);
void *func_ov006_0210d6b8(void)
{
    dScMgSlot1_c *scene =
        (dScMgSlot1_c *)_ZN7fBase_cnwEj(sizeof(dScMgSlot1_c));
    if (scene) {
        _ZN11dScMgBase_cC2Ev(scene);
        *(void **)scene = &_ZTV12dScMgSlot1_c;
        /* Taking &scene->mBetIcon folds two address calculations in mwccarm.
           Preserve the original tree while keeping the pointer typed. */
        dScMgSlot1_c::betIcon_c *icon =
            (dScMgSlot1_c::betIcon_c *)((int)scene + 0x4660);
        *(void **)icon = &_ZTV9dThIcon_c;
        *(void **)icon = &_ZTVN12dScMgSlot1_c9betIcon_cE;
        func_ov006_0210c2b0((char *)scene->pad_4684);
        func_ov006_0210c2b0((char *)scene->pad_4690);
        func_ov006_0210c208((char *)scene->pad_469c);
    }
    return scene;
}
}
