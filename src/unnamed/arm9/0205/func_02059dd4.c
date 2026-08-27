/* func_02059dd4 at 0x02059dd4
 * IPC send loop: call IPCSend(0xc, param<<8, 0) until it returns 0.
 */

typedef int s32;

extern s32 IPCSend(s32 cmd, s32 arg1, s32 arg2);

void func_02059dd4(s32 param)
{
    s32 r6 = param << 8;
    s32 r5 = 0xc;
    s32 r4 = 0;
    while (IPCSend(r5, r6, r4) != 0)
        ;
}
