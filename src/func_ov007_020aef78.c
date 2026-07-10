/* func_ov007_020aef78 @ 0x20aef78 (ov007) -- tail-call veneer to func_ov007_020b9640 (0x20b9640).
 * ldr ip, [pc]; bx ip; .word 0x20b9640
 */
extern void func_ov007_020b9640(void);

void func_ov007_020aef78(void) {
    func_ov007_020b9640();
}
