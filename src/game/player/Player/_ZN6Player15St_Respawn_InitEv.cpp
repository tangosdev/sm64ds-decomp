//cpp
// @symbol _ZN6Player15St_Respawn_InitEv
/* recovered: named members + shared header, real C++ method
 *
 * Restores the spawn point saved by Player::InitResources, lifts the player
 * 0x3e8000 above it, re-seats the mesh collider at the new position, then
 * resets speed/angle state and starts the respawn animation.
 */
#include "Player.h"
extern "C" {
extern void Player_DisableInteraction(Player*);
extern void func_02035860(void*, void*);
extern void func_0200cf40(char*);
/* SetAnim's ROM name carries by-value class parameters (Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true types
   breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern unsigned int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, int, unsigned int);
}
extern unsigned char data_0209f250;
extern char* data_0209f318;

int Player::St_Respawn_Init()
{
    Player_DisableInteraction(this);
    mPosX = mSpawnPosX;
    mPosY = mSpawnPosY;
    mPosZ = mSpawnPosZ;
    mPosY += 0x3e8000;
    func_02035860(&mMeshClsn, &mPosX);
    mPrevPosX = mPosX;
    mPrevPosY = mPosY;
    mPrevPosZ = mPosZ;
    mPrevAngleY = mSpawnAngleY;
    mAngleY = mPrevAngleY;
    mIsBodyClsnEnabled = 1;
    unk_716 = 0;
    if (mPlayerNo == data_0209f250) {
        func_0200cf40(*(char**)((int)&data_0209f318));
    }
    _ZN6Player7SetAnimEji5Fix12IiEj(this, 0x54, 0x40000000, 0x1000, 0);
    mStateStep = 0;
    mPeakY = mPosY;
    mHorzSpeed = 0;
    mVertSpeed = mHorzSpeed;
    mVertAccel = 0;
    mStateTimer = 0xa;
    return 1;
}
