//cpp
/* Stage::SetVramBanks() at 0x0202e73c. Static: sets the VRAM bank assignment for
 * a level and never touches an object. */
#include "Stage.h"

extern "C" {
extern void _ZN2GX15DisableAllBanksEv(void);
extern void _ZN2GX13SetBankForTexEt(u16);
extern void _ZN2GX17SetBankForTexPlttEt(u16);
extern void _ZN2GX12SetBankForBGEt(u16);
extern void _ZN2GX13SetBankForOBJEt(u16);
extern void _ZN2GX15SetBankForSubBGEt(u16);
extern void _ZN2GX22SetBankForSubBGExtPlttEt(u16);
extern void _ZN2GX16SetBankForSubOBJEt(u16);
}

void Stage::SetVramBanks() {
    _ZN2GX15DisableAllBanksEv();
    _ZN2GX13SetBankForTexEt(3);
    _ZN2GX17SetBankForTexPlttEt(0x30);
    _ZN2GX12SetBankForBGEt(8);
    _ZN2GX13SetBankForOBJEt(0x40);
    _ZN2GX15SetBankForSubBGEt(4);
    _ZN2GX22SetBankForSubBGExtPlttEt(0x80);
    _ZN2GX16SetBankForSubOBJEt(0x100);
}
