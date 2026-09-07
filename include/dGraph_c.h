#ifndef DGRAPH_C_H
#define DGRAPH_C_H

/* dGraph_c::callback_c -- the scene-graph callback base class.
 *
 * The ROM's own RTTI names it: _ZTIN8dGraph_c10callback_cE at 0x0208ee04
 * (a __class_type_info root), _ZTS bytes "N8dGraph_c10callback_cE", and its
 * vtable at 0x0208ee14 (_ZTVN8dGraph_c10callback_cE) with the canonical
 * {offset-to-top 0, _ZTI} header. Five nested classes derive from it, one
 * per scene class: dScStage_c/dScMB_c/dScMgBase_c/dScDSMT_c/dScEntry_c
 * ::graphCallback_c, each _ZTI a __si_class_type_info whose one base edge
 * points here.
 *
 * The four methods below fill that vtable's four slots (slots 0..3, in
 * address-descending order 0x02018eb8..0x02018ea0), and they are VIRTUAL in
 * the ROM: the dispatchers at func_02018ec0/func_02018efc/0x02019144 poll a
 * registered object pointer and call `ldr r1,[r0]; ldr r1,[r1,#slot*4];
 * blx r1` with r0 = the object -- vptr dispatch, r1 never set as an
 * argument, so every slot is nullary. An earlier reading ("a table of plain
 * function addresses, therefore static") is falsified by the RTTI header on
 * that table and by the base-then-derived vptr store pairs in the derived
 * classes' constructors (e.g. 0x02074edc).
 *
 * The declarations are virtual so derived callback types retain all four
 * inherited slots when they override only one hook. Each default remains in
 * its independently isolated source file; strict object isolation discards
 * compiler-emitted vtable/RTTI passengers that the function range does not
 * own.
 *
 * This class has no virtual destructor and no data members of its own
 * beyond the vptr; derived graphCallback_c objects carry their own fields
 * after it (dScStage_c's holds a 2x2 fixed-point matrix at +0x4).
 */
class dGraph_c {
public:
    class callback_c {
    public:
        virtual int GraphCallback0(); /* slot 0 -- 0x02018eb8, `mov r0,#1; bx lr` */
        virtual int GraphCallback1(); /* slot 1 -- 0x02018eb0, same */
        virtual int GraphCallback2(); /* slot 2 -- 0x02018ea8, same */
        virtual int GraphCallback3(); /* slot 3 -- 0x02018ea0, same */
    };
};

#endif
