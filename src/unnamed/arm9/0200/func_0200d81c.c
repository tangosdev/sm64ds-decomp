#include "types.h"
/* func_0200d81c at 0x0200d81c
 * Camera method: if playerID == data_0209f250, call Camera::ChangeState(state).
 */
struct Camera_State;
struct Camera;

extern u8 data_0209f250;  /* 0x0209f250 */
extern struct Camera_State data_0209b008;  /* 0x0209b008 */
extern void _ZN6Camera11ChangeStateEPNS_5StateE(struct Camera *thiz, struct Camera_State *state); /* 0x0200cb70 */

void func_0200d81c(struct Camera *thiz, int playerID)
{
    if (playerID != (int)data_0209f250)
        return;
    _ZN6Camera11ChangeStateEPNS_5StateE(thiz, &data_0209b008);
}
