/* storage 0x0238A4E0 runtime 0x03802378 size 0x28 wram ARM.
 * Tail call through pool: b56 turns the trailing return-call into
 * `ldr r12, =fn_02383024; bx r12` (no BL). Tail pool +0x28: 0x037FAEBC. */

extern int fn_02383024(int a1, int a2, int a3);

int fn_0238A4E0(int a1, int a2) {
    return fn_02383024(5, ((a1 << 8) & 0x7F00) | 0x8000 | (a2 & 0xFF), 0);
}
