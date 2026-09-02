/* HOST COPY of src/SetNumPlayers.c -- one more ARM argument ride-through.
 *
 * The matched TU is
 *
 *     extern void func_020308d0(void);
 *     void SetNumPlayers(int n) { data_0208a0e0 = n; func_020308d0(); }
 *
 * and src/func_020308d0.c defines that callee as `void func_020308d0(int n)`,
 * reading the count it was never passed. On the ARM it is passed: n is still
 * in r0 across the store and the branch. On a host cdecl call the callee reads
 * whatever the stack happens to hold, and the count it seats the four player
 * slots from is garbage -- data_0209fc5c[0], the "this slot is live" flag the
 * entrance loader checks before it keeps the Player it just spawned, comes out
 * of that loop.
 *
 * Same body, argument spelled. SetNumPlayers is the only caller in the whole
 * tree, so nothing else depends on the register form.
 */
extern void func_020308d0(int n);
extern unsigned char data_0208a0e0;

// PORT_HOST_ABI: matched src calls func_020308d0() argless while the callee reads n; n rides in r0 on ARM but the host cdecl call leaves it garbage, so the argument is spelled here
void SetNumPlayers(int n)
{
    data_0208a0e0 = (unsigned char)n;
    func_020308d0(n);
}
