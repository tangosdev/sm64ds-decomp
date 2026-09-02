/* HOST COPY of src/func_0200ca50.cpp -- the camera state machine's per-frame
 * main dispatch (Camera::Behavior calls it), with the pointer-to-member call
 * replaced by hal_call_camera_state_fn.
 *
 * Same translation as func_0200cae4_hostcopy.cpp: the State object's SECOND pair
 * (+0x8) is the main function, and the address baked there is DS code.
 * The fov approach and the two flag branches are the matched source.
 */
extern "C" {
void _Z15ApproachLinear2Rsss(short *dst, short a, short b);
int func_0200cbe0(void *c);
int hal_call_camera_state_fn(void *self, unsigned ds_addr);

// PORT_HOST_ABI: the State object's main pair is an mwcc pointer-to-member holding a DS code address MSVC's PMF cannot represent; dispatch routed through hal_call_camera_state_fn
int func_0200ca50(void *self)
{
    char *f = (char *)self;
    int flags;
    int r5;

    {
        char *mode = *(char **)(f + 0x13c);
        _Z15ApproachLinear2Rsss((short *)(f + 0x17a), *(short *)(mode + 0x24),
                                0x80);
    }
    r5 = 1;
    flags = *(int *)(f + 0x154);
    if (flags & 8) {
        ;
    } else if (flags & 0x8000) {
        func_0200cbe0(self);
    } else {
        unsigned *state = *(unsigned **)(f + 0x138);
        if (state && state[2] != 0)
            r5 = hal_call_camera_state_fn(self, state[2]);
    }
    return r5;
}
}  /* extern "C" */
