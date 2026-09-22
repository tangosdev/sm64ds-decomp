// @symbol func_020610fc
// recovered: IPC send-and-hang stub, arm9 0x020610fc (44 bytes).
/* func_020610fc -- arm9 0x020610fc, 0x2c bytes.
 *
 * Retries IPCSend(0xd, 2, 0) until the ARM7 accepts the command, then hangs
 * forever. The same send loop as src/func_02019ff4.c with the same three
 * constants; that one returns through func_0201a028, this one never returns.
 * Its only caller (0x020610e4) treats it as a terminal path.
 *
 * The three arguments are hoisted into r6/r5/r4 outside the loop, which is
 * what naming them as locals in that declaration order produces (the same
 * shape src/func_02059dd4.c needed). The trailing `for (;;) ;` compiles to
 * the bare `b .` with no epilogue, so the previous file's claim that mwccarm
 * will not emit a self-branch without an epilogue was wrong.
 */

typedef int s32;

extern s32 IPCSend(s32 cmd, s32 arg1, s32 arg2);

void func_020610fc(void)
{
    s32 r6 = 0xd;
    s32 r5 = 2;
    s32 r4 = 0;
    while (IPCSend(r6, r5, r4) != 0)
        ;
    for (;;)
        ;
}
