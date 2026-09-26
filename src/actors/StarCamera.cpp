//cpp
/**
 * Factory registered by the STAR_CAMERA actor profile.
 *
 * Allocates 0xd4 bytes through fBase_c::operator new, returns null on
 * allocation failure, and invokes dActor_c's constructor on the allocation.
 * The surviving vptr is dActor_c's. The most-derived identity is unresolved:
 * the allocation is four bytes larger than sizeof(dActor_c), but this factory
 * installs no most-derived vptr and has no identifying RTTI record.
 * StarCamera is the existing profile-derived label, not a recovered class name.
 * This production source contains the factory at ov002 0x020ebe5c..0x020ebe8c;
 * it does not establish an original single-function translation-unit boundary.
 *
 * Partial reconstruction: the allocation and base constructor retain their
 * legacy mangled extern "C" calls. The four extra bytes, most-derived identity,
 * and natural C++ construction form require further evidence and pinned
 * compiler experiments. These are deferred work, not a measured compiler limit.
 * Follow-up: https://github.com/tangosdev/sm64ds-decomp/issues/3198
 * Continuation owner: codex-integrate-open-0926; completion remains partial.
 */

extern "C" {
extern "C" void *_ZN7fBase_cnwEj(unsigned int sz);
extern "C" void *_ZN8dActor_cC1Ev(void *thiz);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- StarCamera_Spawn, 0x020ebe5c, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol StarCamera_Spawn
extern "C" void *StarCamera_Spawn(void)
{
    void *p = _ZN7fBase_cnwEj(0xd4);
    if (p == 0) return p;
    return _ZN8dActor_cC1Ev(p);
}
