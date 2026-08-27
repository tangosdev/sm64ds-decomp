/* func_0205f8b0 at 0x0205f8b0
 * IPC send loop: call IPCSend(8, param, 0) until it returns 0.
 */

typedef int s32;

extern s32 IPCSend(s32 cmd, s32 arg1, s32 arg2);

void func_0205f8b0(s32 param)
{
    s32 r6 = param;
    s32 r5 = 8;
    s32 r4 = 0;
    while (IPCSend(r5, r6, r4) != 0)
        ;
}
