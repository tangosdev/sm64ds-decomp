/* ==========================================================================
 * RETIRED -- THIS FILE IS NOT IN ANY BUILD. Run linkw wave 6, lane w6-C item 3.
 *
 * The MATCHED source _ZN7fBase_c21AfterCleanupResourcesEj now hosts itself
 * through hostgen's ROLE_DATA map (--extern-data binds the three role-named
 * engine globals by address), driven from HOSTABI_RETIRE_SYMS in
 * port/CMakeLists.txt; the source-list entry there is commented out.
 *
 * Read the wave-6 note next to it: the three actor smokes had been building a
 * gate-9 hostgen copy of the same symbol WITHOUT --extern-data, i.e. running
 * the private-char body this banner's own prose warns about. That is fixed in
 * the same landing.
 *
 * Kept, not deleted, because the prose below is the readable statement of why
 * a delinked TU's file-scope globals cannot be taken literally on the host.
 * Nothing below this banner is compiled.
 * ==========================================================================
 *
 * HOST COPY of src/_ZN7fBase_c21AfterCleanupResourcesEj.cpp -- the actor
 * teardown, with its three engine globals pointed at the engine.
 *
 * The matched source spells them as file-scope DEFINITIONS:
 *
 *     char gGlobalA;                 // 0x020a4b6c, the scene tree
 *     char gGlobalB;                 // 0x020a4ba8, the cleanup list
 *     struct Heap* Memory_gameHeapPtr;   // 0x020a0eac
 *
 * which is a fine way to write a delinked TU -- the addresses come back from
 * the relocation table, and the declared width is whatever that one function
 * needed. On the host they are three real objects one byte or one word wide,
 * so the teardown unlinks the actor from a private char and frees it against
 * a null heap. The port's rule is that engine BSS belongs to the HAL, and
 * hostgen's --extern-data enforces it for `data_<hex>` names; these three are
 * spelled by role rather than by address, so the fix lives here.
 *
 * NOTHING ELSE CHANGES. The body is the matched source's, in order:
 * unlink the scene node, unlink the behaviour node from the cleanup list,
 * destroy the actor's own heap if it has one, run the D1 destructor through
 * vtable slot 16, and hand the object back to the game heap.
 *
 * WHAT REACHES IT, and why it is not dead code the way gate 9 left it: an
 * actor whose InitResources returns 0 gets vfSuccess 1, and
 * ActorDerived::AfterInitResources turns that into MarkForDestruction. That
 * is the castle grounds' TREE by design -- twenty-one Tree actors spawn, each
 * threads a CylinderClsnWithPos onto its variant's list, and only the FIRST
 * of each variant returns 1 and stays alive to own the model and the render
 * walk. The other twenty are meant to die on the next frame's cleanup pass,
 * and with the unlink landing on a private char they never left the list:
 * the second pass walked freed memory.
 */
extern "C" {

void func_0203b3c0(void *list, void *node);
void func_0203b27c(void *list, void *node);
void func_02044334(void *p);
void Heap_Destroy(void *heap);
void Memory_Deallocate(void *p, void *heap);

extern int data_020a4b6c[];        /* the scene tree */
extern int data_020a4ba8[];        /* the cleanup list */
extern void *data_020a0eac;        /* Memory::gameHeapPtr */

typedef void(__fastcall *ActorDtor)(void *self, void *dummy);

void _ZN7fBase_c21AfterCleanupResourcesEj(void *selfv, unsigned vfSuccess)
{
    char *self = (char *)selfv;
    if (vfSuccess != 2)
        return;
    func_0203b3c0(data_020a4b6c, self + 0x14);
    func_0203b27c(data_020a4ba8, self + 0x28);
    if (*(void **)(self + 0x4c))
        Heap_Destroy(*(void **)(self + 0x4c));
    if (*(void **)(self + 0x48))
        func_02044334(*(void **)(self + 0x48));
    ((ActorDtor)(*(void ***)self)[16])(self, 0);   /* ~ActorBase (D1) */
    Memory_Deallocate(self, data_020a0eac);
}

}
