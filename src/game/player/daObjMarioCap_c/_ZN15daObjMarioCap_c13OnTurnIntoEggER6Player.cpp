//cpp
// @symbol _ZN15daObjMarioCap_c13OnTurnIntoEggER6Player
#include "daObjMarioCap_c.h"
#include "Player.h"
/* daObjMarioCap_c::OnTurnIntoEgg -- vtable slot 19, ov002 0x020b81e0.
 *
 * Named from the table: 0x020b81e0 is the word slot 19 of
 * _ZTV15daObjMarioCap_c holds where _ZTV12dEnemyBase_c holds something else, so it is
 * this class's own override of the virtual dActor_c declares.
 *
 * THE OLD `recovered name:` COMMENT SAID WaterfallMist_OnTurnIntoEgg, WHICH IS
 * A DIFFERENT CLASS ENTIRELY -- the same header's own banner already records
 * that this overlay's tables are off by one entry in places, and the file's
 * second comment line already contradicted its first by naming
 * daObjMarioCap_c. The vtable settles it: this address is slot 19 of
 * daObjMarioCap_c's table, and 0x3f4 (mModelIndex) and 0x300 (mModelAnim) --
 * both read below -- are this class's own fields at this class's own offsets.
 *
 * The ROM returns nothing, but dActor_c declares slot 19 as `int` and the
 * cartridge simply leaves r0 alone on the way out; the declared return type
 * is the ancestor's, and no value is produced here for the same reason.
 *
 * 0x3fe falls inside the header's pad_3f8[0x7] and 0x3c0 is unk_3c0 -- the
 * player that just ate the cap is stashed there for the follow-up state --
 * so both stay reached the way the layout describes them rather than by
 * inventing names for them. */

extern "C" {
/* SetAnim carries Fix12<int> by value, so its definition stays a mangled free
   function -- see include/ShadowModel.h. A call is unaffected. */
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(ModelAnim *self, void *animFile,
                                                 int flags, int speed, unsigned int startFrame);
void func_ov002_020b7f2c(daObjMarioCap_c *self, void *p);

extern void *data_ov002_0210de30[];
extern int data_ov002_0210df54;
}

int daObjMarioCap_c::OnTurnIntoEgg(Player &player)
{
    if (player.SetNoControlState(8, -1, 0) == 1) {
        player.SetNewHatCharacter(mModelIndex & 0xff, 1, 0);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov002_0210de30[1],
                                                    0x40000000, 0x1000, 0);
        ((char *)this)[0x3fe] = 1;
    }
    *(Player **)&unk_3c0 = &player;
    func_ov002_020b7f2c(this, &data_ov002_0210df54);
}
