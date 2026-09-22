// @symbol func_0206e2f8
/* recovered: MSL memset -- the thin wrapper over __fill_mem.
 *
 * The return type is `void *`, not `int`. func_0206e330, already recovered in
 * src/func_0206e330.c as the MSL byte-head / 32-byte-block / word-tail fill
 * loop, is __fill_mem(void *dst, int val, u32 n); this function forwards all
 * three arguments to it unchanged and returns its own first argument, which is
 * exactly memset's contract. The one caller in the tree, _ZN7fBase_cnwEj,
 * calls it as func_0206e2f8(p, 0, size) with p a void *.
 *
 * include/decl_common.h carried `int` here after the bulk correction in
 * 2bcc1b99c ("32 rows declared void over a value"), which established only
 * that r0 holds a value and picked int as the generic value type. That is
 * refined to the pointer the body actually returns. Both spellings are one
 * word in r0, so the row is byte-neutral either way.
 */
extern void func_0206e330(void *dst, int val, unsigned int n);
void *func_0206e2f8(void *dst, int val, unsigned int n) { func_0206e330(dst, val, n); return dst; }
