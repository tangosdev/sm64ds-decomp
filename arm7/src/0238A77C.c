/* storage 0x0238A77C runtime 0x03802614 size 0x64 wram ARM.
 * Dispatcher: run the pre-hook, map (a1, a2) to a value, override it via
 * (a3, a4) when a1 is 6 or 134, then run the post-hook on the result.
 * Five BL callees, no pool. */

extern void fn_0238AFA8(void); /* runtime 0x03802E40 */
extern int fn_0238B01C(int, int); /* runtime 0x03802EB4 */
extern int fn_0238AFE8(int); /* runtime 0x03802E80 */
extern int fn_0238B08C(int, int); /* runtime 0x03802F24 */
extern int fn_0238B11C(int, int); /* runtime 0x03802FB4 */

int fn_0238A77C(int a1, int a2, int a3, int a4) {
    int v;
    fn_0238AFA8();
    v = fn_0238B01C(a1, a2);
    if (a1 != 6) {
        if (a1 == 134) {
            v = fn_0238B11C(a3, a4);
        }
    } else {
        v = fn_0238B08C(a3, a4);
    }
    return fn_0238AFE8(v);
}
