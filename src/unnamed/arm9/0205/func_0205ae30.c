/* func_0205ae30 at 0x0205ae30
 * IPC send loop: call IPCSend(7, 0, 0) until it returns >= 0.
 */

typedef int s32;

extern s32 IPCSend(s32 cmd, s32 arg1, s32 arg2);

void func_0205ae30(void)
{
    s32 r5 = 7;
    s32 r4 = 0;
    while (IPCSend(r5, r4, r4) < 0)
        ;
}
