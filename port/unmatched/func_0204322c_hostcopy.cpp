/* HOST COPIES of src/func_0204322c.c (the RENDER Process) and
 * src/func_020432e4.c (the CLEANUP Process), with the mwcc
 * pointer-to-member-functions resolved to the vtable slots they name --
 * exactly the treatment the init and behaviour wrappers (func_0204335c,
 * func_02043288) get from hostgen's VIRTUAL_CALL table since lane shadow-A
 * retired their host copies.
 *
 * The twelve PMFs at arm9 0x02099e74..0x02099ecc are STATIC data, each the
 * mwcc virtual form { vtable byte offset, 1 }:
 *
 *     0x02099e74 = { 0x28, 1 } -> slot 10  BeforeRender
 *     0x02099e7c = { 0x10, 1 } -> slot  4  BeforeCleanupResources
 *     0x02099e84 = { 0x14, 1 } -> slot  5  AfterCleanupResources
 *     0x02099e8c = { 0x0c, 1 } -> slot  3  CleanupResources
 *     0x02099e9c = { 0x2c, 1 } -> slot 11  AfterRender
 *     0x02099ecc = { 0x24, 1 } -> slot  9  Render
 *
 * so func_0204322c is Process(self, slot 9, slot 10, slot 11) and
 * func_020432e4 is Process(self, slot 3, slot 4, slot 5) with the actor id
 * read out before the call and handed to func_0204302c on success -- the
 * destructor's own bookkeeping, which needs the id after the object is gone.
 *
 * The control flow is ActorBase::Process's, unchanged:
 *
 *     r = Before(); if (r) { r = Main(); code = r==-1 ? 3 : r==1 ? 2 : 1; }
 *                   else   { code = 0; }
 *     After(code);
 *     return r;
 */
extern "C" {

typedef int(__fastcall *ActorSlot0)(void *self, void *dummy);
typedef void(__fastcall *ActorSlot2)(void *self, void *dummy, unsigned code);

void func_0204302c(void *id);

static int port_actor_process(void *self, int main_, int before, int after)
{
    void **vt = *(void ***)self;
    int r = ((ActorSlot0)vt[before])(self, 0);
    unsigned code;
    if (r != 0) {
        r = ((ActorSlot0)vt[main_])(self, 0);
        code = r == -1 ? 3u : r == 1 ? 2u : 1u;
    } else {
        code = 0;
    }
    ((ActorSlot2)vt[after])(self, 0, code);
    return r;
}

/* PORT_HOST_ABI: mwcc pointer-to-member dispatch: src calls ActorBase::Process
 * with three PMFs MSVC has no representation for. See the header. */
int func_0204322c(void *self)
{
    return port_actor_process(self, 9, 10, 11);
}

/* PORT_HOST_ABI: mwcc pointer-to-member dispatch: src calls ActorBase::Process
 * with three PMFs MSVC has no representation for. See the header. */
int func_020432e4(void *self)
{
    unsigned short id = *(unsigned short *)((char *)self + 0xc);
    int r = port_actor_process(self, 3, 4, 5);
    if (r == 1)
        func_0204302c((void *)(size_t)id);
    return r;
}

}
