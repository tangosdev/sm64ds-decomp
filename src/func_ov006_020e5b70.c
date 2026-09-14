/* func_ov006_020e5b70 @ 0x20e5b70 (ov006) -- tail-call veneer to func_ov006_020e5450 (0x20e5450).
 * ldr ip, [pc]; bx ip; .word 0x20e5450
 *
 * The veneer forwards r0/r1 untouched, so its own parameter list is the target's.
 * func_ov006_020e5450's prologue is `mov sb,r1; mov sl,r0`, which proves two
 * parameters; src/func_ov006_020e5b7c.c spells the same (char *, int).  The old
 * no-parameter spelling here was wrong and merely happened to emit the same bytes.
 */
extern void func_ov006_020e5450(char *c, int a);

void func_ov006_020e5b70(char *c, int a) {
    func_ov006_020e5450(c, a);
}
