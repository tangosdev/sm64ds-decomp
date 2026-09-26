//cpp
/**
 * Spawn hook for the star-select camera actor.
 *
 * The whole translation unit is this one factory: take 0xd4 bytes from
 * fBase_c's own operator new and construct a plain dActor_c in them, or
 * propagate the null when the heap is exhausted. ov002 0x020ebe5c..0x020ebe8c.
 *
 * deslop
 * Leftover: operator new and the dActor_c constructor are still called
 *   through their mangled spellings as extern "C". Giving them real C++
 *   forms needs a StarCamera class definition this TU does not yet have,
 *   and the call shapes are what reproduce the ROM here. Not yet
 *   reconstructed -- see notes/tu-promotion-conventions.md on source
 *   reconstruction being a separate claim from production packaging.
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
