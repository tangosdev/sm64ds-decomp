/* HOST COPY of src/ChangeArea.c -- the argless HideArea() call given its
 * argument explicitly.
 *
 * The matched source declares `extern void HideArea(void)` and calls it with
 * no argument, while HideArea itself is `void HideArea(int idx)`. That is
 * legal on the DS: the ARM ABI passes the index in r0, and the value already
 * sitting in r0 at the call site is the area ID the caller was handed -- a
 * ride-through the compiler cannot see and the ROM depends on. Under the
 * MSVC cdecl ABI the callee instead reads an uninitialized stack slot, and
 * HideArea's body is `data_0209f314[idx].flag = 0` -- a wild write at a
 * garbage index into the area table.
 *
 * The host copy passes what the ROM's r0 holds: data_02092120, the currently
 * shown area, which is also what the guard just tested. Seed it to -1 (no
 * area shown) so the first ChangeArea skips the hide entirely.
 */
typedef signed char s8;

extern s8 data_02092120;

extern void HideArea(int idx);
extern void ShowArea(int areaID);

// PORT_HOST_ABI: matched TU calls HideArea() argless relying on the ARM r0 ride-through; MSVC cdecl reads an uninitialized stack slot and writes at a garbage area-table index
void ChangeArea(int areaID)
{
    if (data_02092120 >= 0)
        HideArea(data_02092120);
    data_02092120 = (s8)areaID;
    ShowArea(areaID);
}
