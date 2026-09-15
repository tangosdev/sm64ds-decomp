/* func_ov006_020e5b70 @ 0x20e5b70 (ov006) -- tail-call veneer to func_ov006_020e5450 (0x20e5450).
 * ldr ip, [pc]; bx ip; .word 0x20e5450
 *
 * The veneer forwards r0/r1 untouched, so its own parameter list is the target's.
 * func_ov006_020e5450's prologue is `mov sb,r1; mov sl,r0`, which proves two
 * parameters; src/func_ov006_020e5b7c.c spells the same arity.  The old
 * no-parameter spelling here was wrong and merely happened to emit the same bytes.
 *
 * The first parameter is a `this`, and src/func_ov006_020e5450.cpp names its type
 * `dScMgCurling2_c *` -- a C++ class this C shard cannot spell.  `void *` is the
 * honest spelling for it here: it carries the arity, which is the byte-relevant
 * part, and declines to contradict the definition's pointee type rather than
 * asserting a `char *` the definition does not say.
 */
extern void func_ov006_020e5450(void *c, int a);

void func_ov006_020e5b70(void *c, int a) {
    func_ov006_020e5450(c, a);
}
