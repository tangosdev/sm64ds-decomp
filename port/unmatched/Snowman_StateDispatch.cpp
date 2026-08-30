/* HOST COPIES of ov072's four SnowmanBody/SnowmanHead pointer-to-member
 * state dispatchers -- the BabyPenguin_StateEnter treatment applied to the
 * two classes gate 193 left unregistered and lane w3-d seats.
 *
 *   src/func_ov072_0211fc78.cpp   SnowmanBody enter (pp + 0)   pad 0x38c
 *   src/func_ov072_0211fc3c.cpp   SnowmanBody tick  (pp + 1)   pad 0x38c
 *   src/func_ov072_0212059c.cpp   SnowmanHead enter (pp + 0)   pad 0x328
 *   src/func_ov072_02120560.cpp   SnowmanHead tick  (pp + 1)   pad 0x328
 *
 * WHY HOST COPIES. All four matched TUs are one line each and identical in
 * shape:
 *
 *     struct C; typedef void (C::*PMF)();
 *     struct C { char pad[N]; PMF *pp; };
 *     extern "C" void f(C *c) { PMF *p = c->pp + k; (c->**p)(); }
 *
 * -- a REAL C++ pointer-to-member call on a DELIBERATELY INCOMPLETE class
 * (no base, no virtuals declared). mwcc's PMF for that class is the ROM's
 * own 8-byte {function, delta} pair, which is exactly the stride the two
 * sinits copy in (__sinit_ov072_02122018 moves twelve such pairs into the
 * six cells at data_ov072_02122b64; __sinit_ov072_021221f8 moves eight into
 * the four cells at data_ov072_02122c00). MSVC's representation for an
 * incomplete class is the "unknown inheritance" form, which is not a plain
 * function pointer, and calling through it does not reproduce the ROM's
 * {function, delta} semantics on the host.
 *
 * This is not a precaution, it is the same call on the same overlay that
 * port/unmatched/BabyPenguin_StateEnter.cpp already documents -- see that
 * file's header, and port/ov072_syms.txt's for the cell derivation.
 *
 * The `+ k` is the half: k = 0 is the "enter" pair, k = 1 the "tick" pair,
 * and since a PMF is the 8-byte pair, `pp + 1` steps 8 bytes to the second
 * pair of a 0x10-byte cell. The ChangeState bodies stay matched src and are
 * unchanged (func_ov072_0211fcb0 and func_ov072_021205d4, each writing
 * `pp = &table + state * 0x10` before calling its own enter dispatcher).
 *
 * hal/actor_classes_ov072.cpp's port_snowman_body_states_seat() and
 * port_snowman_head_states_seat() verify each cell still holds the ROM's own
 * {address, 0} pairs after the sinit and then overwrite the function words
 * with the host bodies' addresses, so the pointers read here are host code.
 * All four matched TUs stay byte-locked in src/ as the proof, dropped from
 * slice_gate193.txt.
 */
extern "C" {

/* One pair, the unit both sinits copy and both seats rewrite. Layout is
   shared with hal/actor_classes_ov072.cpp's PortSnowmanCell. */
struct PortSnowmanPair { unsigned fn, delta; };
typedef void (*PortSnowmanFn)(void *);

/* No null guard on `pp` in any of the four, matching the matched src
   exactly: each class's ChangeState seats the cell pointer before its
   dispatcher can be reached, and the ROM bodies are unconditional. */

/* PORT_HOST_ABI: mwcc pointer-to-member dispatch on a deliberately
   incomplete class; MSVC's PMF representation there does not reproduce the
   ROM's {function,delta} pair. */
void func_ov072_0211fc78(char *c)          /* SnowmanBody, enter */
{
    PortSnowmanPair *p = *(PortSnowmanPair **)(c + 0x38c);
    ((PortSnowmanFn)(size_t)p[0].fn)(c);
}

/* PORT_HOST_ABI: as above. */
void func_ov072_0211fc3c(char *c)          /* SnowmanBody, tick */
{
    PortSnowmanPair *p = *(PortSnowmanPair **)(c + 0x38c);
    ((PortSnowmanFn)(size_t)p[1].fn)(c);
}

/* PORT_HOST_ABI: as above. */
void func_ov072_0212059c(char *c)          /* SnowmanHead, enter */
{
    PortSnowmanPair *p = *(PortSnowmanPair **)(c + 0x328);
    ((PortSnowmanFn)(size_t)p[0].fn)(c);
}

/* PORT_HOST_ABI: as above. */
void func_ov072_02120560(char *c)          /* SnowmanHead, tick */
{
    PortSnowmanPair *p = *(PortSnowmanPair **)(c + 0x328);
    ((PortSnowmanFn)(size_t)p[1].fn)(c);
}

}
