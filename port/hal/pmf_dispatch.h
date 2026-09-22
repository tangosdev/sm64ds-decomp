#ifndef PORT_HAL_PMF_DISPATCH_H
#define PORT_HAL_PMF_DISPATCH_H

/* THE POINTER-TO-MEMBER DISPATCH SEAM, run link100 lane HOSTGEN2.
 *
 * WHAT IS ACTUALLY WRONG AT ONE OF THESE SITES, AND IT IS ONE THING.
 *
 * The sixteen PORT_HOST_ABI rulings that name a member-pointer dispatch were
 * all written before port/CMakeLists.txt's `/vmg /vmm` pair went on
 * add_compile_options (run link100, lane PMF, the block at the top of that
 * file). Every one of them says some version of "MSVC's 4-byte member pointer
 * cannot express the ROM's 8-byte {code, this-adjust} pair". THAT HALF OF EVERY
 * ONE OF THOSE RULINGS IS OBSOLETE. Under /vmg /vmm an MSVC pointer to member
 * function is EIGHT bytes in every inheritance shape, complete or
 * forward-declared, word 0 the code address and word 1 the this-adjust, which
 * is the ROM's own record. The stride is right, the field order is right, and
 * a table the ROM's own __sinit fills is read at the width it was written.
 *
 * WHAT IS LEFT IS THE CODE WORD. The word the ROM stores is a DS code address.
 * MSVC's call sequence loads it and calls it, and on the host that address
 * lands inside the mounted overlay's DATA image: the dispatch jumps into mapped
 * data. That is the fault, and the port already owns the answer to it -- the
 * per-class address switches in port/unmatched/Mg*_StateDispatch.cpp, entered
 * through port_mg_<class>_call0/1, which apply the ROM's null-code guard,
 * refuse an adjustment nobody has measured, and report an address nothing
 * hosts instead of jumping to it.
 *
 * So the correction at a dispatch site is not a retype and not a decode. It is:
 * read the two words the pointer to member already holds, and hand them to the
 * class's own seam instead of calling word 0. That is what this header does,
 * and it is the whole of it.
 *
 * THE this-ADJUST IS PASSED, NOT APPLIED. The seams take the raw adjustment
 * word and enforce the rule the ROM's own pairs support (`adj == 0`), falling
 * through to the framework's report for anything else. Applying `this +
 * (adj >> 1)` here would be inventing a decode for a case nothing in the tree
 * has measured; the seam refusing it is the honest shape and it is the shape
 * every existing host copy already ships.
 *
 * WHY A SITE AND NOT A TU. Within ONE translation unit some of a class's tables
 * are SEATED with host addresses at boot (dScMgPanel_c's data_ov006_02142860
 * and data_ov006_021427bc, seated by port_mg_panel_states_seat) and some still
 * hold the ROM's DS words. A seated table's dispatch is already correct and
 * must NOT be routed: the seam's switch is keyed on DS addresses and would
 * report a host address as unknown and swallow the call. So the rewrite is per
 * SITE, driven by tools/hostgen.py's PMF_SEAM table, and hostgen refuses to
 * emit a listed TU that still carries an unlisted `->*` dispatch. That refusal
 * is the repeatability: a new dispatch site in a TU this file covers fails the
 * build instead of shipping a jump into data.
 *
 * THE STATIC ASSERT IS THE NET FOR THE FLAG. If /vmg /vmm ever comes off
 * add_compile_options, every site that includes this header stops compiling
 * with the reason named, rather than silently reading a four-byte pointer out
 * of an eight-byte table.
 */

struct PortPmfPair {
    unsigned code;   /* word 0: the DS code address, or a seated host address */
    int      adj;    /* word 1: the this-adjust; the ROM's own pairs are all 0 */
};

/* Read the two words out of a pointer to member without naming its class.
   A reinterpreting read rather than a union or a cast-through-int, because the
   member-pointer type is the one thing about these sites that differs from
   site to site and the words are all that is wanted. */
template <class Pmf>
inline PortPmfPair port_pmf_words(const Pmf &pmf)
{
    static_assert(sizeof(Pmf) == 8,
                  "a pointer to member is not the ROM's eight-byte "
                  "{code, this-adjust} pair: this build lost /vmg /vmm");
    PortPmfPair out;
    const unsigned char *src = reinterpret_cast<const unsigned char *>(&pmf);
    unsigned char *dst = reinterpret_cast<unsigned char *>(&out);
    for (int i = 0; i < 8; ++i)
        dst[i] = src[i];
    return out;
}

/* The two call shapes every seam in the tree has. `seam` is the class's own
   port_mg_<class>_call0/1 (or any function of that signature), `obj` the src's
   own receiver expression, `pmf` the src's own table element or field. */
template <class Seam, class Obj, class Pmf>
inline void port_pmf_call0(Seam seam, Obj *obj, const Pmf &pmf)
{
    const PortPmfPair p = port_pmf_words(pmf);
    seam(static_cast<void *>(obj), p.code, p.adj);
}

template <class Seam, class Obj, class Pmf, class Arg>
inline void port_pmf_call1(Seam seam, Obj *obj, const Pmf &pmf, Arg a)
{
    const PortPmfPair p = port_pmf_words(pmf);
    seam(static_cast<void *>(obj), p.code, p.adj, static_cast<int>(a));
}

/* The spellings hostgen's PMF_SEAM table emits. Macros rather than bare calls
   so a site reads as a rewrite and greps as one. */
#define PORT_PMF_CALL0(seam, obj, pmf)      ::port_pmf_call0((seam), (obj), (pmf))
#define PORT_PMF_CALL1(seam, obj, pmf, a)   ::port_pmf_call1((seam), (obj), (pmf), (a))

#endif  /* PORT_HAL_PMF_DISPATCH_H */
