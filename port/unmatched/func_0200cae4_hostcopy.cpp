/* HOST COPY of src/func_0200cae4.cpp -- the camera state machine's onEnter
 * dispatch, with the pointer-to-member call replaced by
 * hal_call_camera_state_fn.
 *
 * The State objects at 0x0209b008.. are mwcc member-function pointers: a
 * {code address, this-delta} pair the DS builds in __sinit_02073a24. MSVC's
 * PMF representation is a different shape entirely, and the addresses in the
 * pair are DS code addresses in any case, so the dispatch is translated
 * through the address table in port/hal/camera_states.inc instead. Same hop
 * as Player_ChangeState.cpp. The flag handling is the matched source.
 *
 * This replaces src/func_0200cae4.cpp in the walk_window slice only; the
 * gate-10 smoke keeps the matched file (it never arms data_0209f318, so the
 * dispatch there is dead).
 */
extern "C" int hal_call_camera_state_fn(void *self, unsigned ds_addr);

// PORT_HOST_ABI: State objects are mwcc member-function pointers (code address plus this-delta) holding DS code addresses that MSVC's PMF cannot represent; dispatch routed through the host address table
extern "C" int func_0200cae4(void *c)
{
    char *f = (char *)c;
    if (*(unsigned *)(f + 0x154) & 0x4000) {
        volatile unsigned *flags = (volatile unsigned *)(f + 0x154);
        *flags &= ~0x4000u;
        *flags |= 0x8000u;
    }
    {
        unsigned *state = *(unsigned **)(f + 0x138);
        if (!state || state[0] == 0)
            return 1;
        return hal_call_camera_state_fn(c, state[0]);
    }
}
