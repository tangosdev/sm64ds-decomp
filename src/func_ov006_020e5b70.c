/* func_ov006_020e5b70 @ 0x20e5b70 (ov006) -- tail-call veneer to func_ov006_020e5450 (0x20e5450).
 * ldr ip, [pc]; bx ip; .word 0x20e5450
 *
 * IT MUST FORWARD ITS TWO ARGUMENTS. On ARM a tail jump leaves r0 and r1 alone,
 * so the image passes the object and the record index without naming them; this
 * file used to spell the callee `(void)` and call it with nothing, which was
 * harmless only while 0x020e5450 was a port trap that ignored its parameters.
 * 0x020e5450 now has a body that strides an eleven-entry 0x30-byte array by that
 * index, so the drop would hand it garbage on every call through slot 2 of state
 * table data_ov006_021419d8. The signature below is the fix and it costs the byte
 * gate nothing: mwccarm 1.2/sp2p3 emits the same three words either way
 * (arguments already sit in r0/r1 at entry, and the tail call needs no moves).
 */
extern void func_ov006_020e5450(char *c, int i);

void func_ov006_020e5b70(char *c, int i) {
    func_ov006_020e5450(c, i);
}
