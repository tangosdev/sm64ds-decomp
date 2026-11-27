/* The Metrowerks C++ runtime's static-initializer driver and the global
 * destructor chain walk. pret pokediamond carries the same pair plus the
 * chain's .bss as the one RUNTIME_CPLUS object
 * arm9/asm/RUNTIME_CPLUS_StaticInitializers.s, which is the evidence they
 * are one translation unit here. __register_global_object, which pushes
 * the nodes this file pops, is not part of it: the cartridge links it
 * above __throw_catch_compare (0x02072fcc..0x020731dc), away from this
 * pair, and pokediamond's object does not carry it either.
 *
 * Written __call_static_initializers then __destroy_global_chain because
 * mwcc emits compiled C in reverse source order and the cartridge holds
 * the destroy walk below the initializer driver. */
#include "types.h"

struct DestructorChain {
    struct DestructorChain *next;
    void (*fn)(void *object, int v);
    void *object;
};
extern struct DestructorChain *__global_destructor_chain;

typedef void (*fn_t)(void);

/* Run the static-initializer table at 0x02086b60 (the __sinit_* pointers)
   in order until its null entry. _start calls it last, just before it
   jumps to NitroMain, where pret's NitroSDK crt0 calls it by name. */
void __call_static_initializers(void) {
    fn_t *p = (fn_t *)0x2086b60;
    fn_t f;
    while (p != 0 && (f = *p) != 0) {
        f();
        p++;
    }
}

/* Pop each node that __register_global_object pushed, newest first, and
   call its destructor on its object with -1. exit calls it. */
void __destroy_global_chain(void) {
    struct DestructorChain *head = __global_destructor_chain;
    if (head == 0)
        return;
    do {
        __global_destructor_chain = head->next;
        head->fn(head->object, -1);
        head = __global_destructor_chain;
    } while (head != 0);
}
