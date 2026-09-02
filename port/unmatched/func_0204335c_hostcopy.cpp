/* HOST COPY of src/func_0204335c.c -- the actor init Process with its three
 * member-function pointers resolved to the vtable slots they name.
 *
 * The ROM's spawn spine ends here: func_020433b8 calls this, and this calls
 *
 *     ActorBase::Process(self, data_02099ebc, data_02099ec4, data_02099e94)
 *
 * with three mwcc pointer-to-member-function objects that a static
 * initializer copied out of relocated pairs. They are the vptr-relative form
 * of InitResources, BeforeInitResources and AfterInitResources -- the first
 * three slots of every actor vtable, in the group order the vtable itself
 * uses. MSVC has no representation for an mwcc PMF and the addresses inside
 * them are ARM code anyway, so the port takes the same route the camera's
 * state dispatch took (port/unmatched/func_0200ca50_hostcopy.cpp): read the slots
 * directly.
 *
 * The control flow is Process's, unchanged:
 *
 *     r = Before(); if (r) { r = Main(); code = r==-1 ? 3 : r==1 ? 2 : 1; }
 *                   else   { code = 0; }
 *     After(code);
 *     return r;
 *
 * The slots are __fastcall thunks in the HAL vtables (ecx carries `this`
 * exactly as __thiscall does, the dummy edx absorbs fastcall's second
 * register) -- the calling convention hal/actor_vtables.cpp established.
 */
extern "C" {

typedef int(__fastcall *ActorSlot0)(void *self, void *dummy);
typedef void(__fastcall *ActorSlot2)(void *self, void *dummy, unsigned code);

// PORT_HOST_ABI: ActorBase::Process's three args are mwcc pointer-to-member-functions holding ARM code addresses MSVC cannot represent; resolved to vtable slots 0/1/2 instead
int func_0204335c(void *self)
{
    void **vt = *(void ***)self;
    int r = ((ActorSlot0)vt[1])(self, 0);        /* BeforeInitResources */
    unsigned code;
    if (r != 0) {
        r = ((ActorSlot0)vt[0])(self, 0);        /* InitResources */
        code = r == -1 ? 3u : r == 1 ? 2u : 1u;
    } else {
        code = 0;
    }
    ((ActorSlot2)vt[2])(self, 0, code);          /* AfterInitResources */
    return r;
}

}
