/* Clear the held and edge bytes of the touch record (gTouchHeld, 4 bytes
   a player, PlayerInput.h) of each player present (func_0203daac). The
   soft reset (func_02023498 in func_0202345c.cpp) runs it every frame
   while a reset is pending. */
#include "PlayerInput.h"
extern int _Z13func_0203daacv(void);
void func_0203bb14(void) {
    unsigned char *p = gTouchHeld;
    int i = 0;
    if (_Z13func_0203daacv() <= 0)
        return;
    do {
        p[0] = 0;
        p[1] = 0;
        i++;
        p += 4;
    } while (i < _Z13func_0203daacv());
}
