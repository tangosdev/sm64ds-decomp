/* RETIRED at run linkfull wave 27, lane S1 (the first main -> port file sync).
 * This file carried HOST COPIES of two ov020 book-closure TUs. Both matched
 * sources hold their seats now (port/slice_w28_s1.txt), taken from main by path
 * and byte-gated at 2004/b56 with strict relocations (object identical to the
 * port's previous text, linkcheck VERIFIED), so the file defines nothing. It
 * stays an (empty) translation unit only because port/slice_w5a.txt, which
 * enrolls it, has another owner; deleting the file and its slice line together
 * is the follow-up.
 *
 * What each copy was for, and why neither reason holds any more:
 *
 * 1. func_ov020_02111418 (the book hit-response dispatcher). The copy's reason
 *    was that the source called data_ov048_021115ac AS A FUNCTION -- ov048's
 *    name for 0x021115ac, the shared level-overlay window naming race. The
 *    source has called the callee by its ov020 name, func_ov020_021115ac, for
 *    some time (main #2882 only retyped the Player arguments), and linkcheck
 *    VERIFIES the relocation lands on func_ov020_021115ac.
 * 2. func_ov020_02111fc4 (the book aggro check). The copy's reason was a
 *    TU-local `struct dActor_c { virtual void v0(); }` shadow next to a flat
 *    `_ZN8dActor_c13ClosestPlayerEv(void *)` call. main #2730 and #2882 replaced
 *    the shadow with include/dActor_c.h and the calls with the members
 *    ClosestPlayer() and GetSubtraction(), so on the host the receiver rides in
 *    the member call and nothing needs passing by hand. (Nothing else changed,
 *    including the ROM's own unchecked dereference of the result.)
 */
