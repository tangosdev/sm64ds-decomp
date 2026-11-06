// Itanium deleting destructors for the host. MSVC folds D0/D1/D2 into the
// one ~Class() it emits, so a Windows link that calls the ROM's flat
// _ZN…D0Ev name needs that symbol spelled out: the D1 body, called
// qualified, then the class-specific operator delete. mwccarm never
// builds this file. The bodies moved here out of src/ unchanged.
#ifdef _MSC_VER

#include "common.h"
#include "dExtFrameCtrl_c.h"
#include "dCamera_c.h"
#include "daBook_c.h"
#include "daBookGen_c.h"
#include "daDoor_c.h"
#include "daIbl_c.h"
#include "daKpa_c.h"
#include "daKpaTail_c.h"
#include "daObjFl_Puzzle_c.h"
#include "daObjFl_Koma_D_c.h"
#include "daObjRotateUpdownLift_c.h"
#include "daObjSwdoor_c.h"
#include "daPropeller_Heyho_Fire_c.h"
#include "daStar_c.h"
#include "daWanwan2_c.h"
#include "daYegg_c.h"
#include "dScMgAmida_c.h"
#include "dScMgBase_c.h"
#include "dScMgCoin_c.h"
#include "dScMgHanachan_c.h"
#include "dActor_c.h"
#include "PathLift.h"

/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~dExtFrameCtrl_c() it
 * emits, so this arm spells out, in terms of the destructor defined
 * below, what the deleting destructor does: the D1 body, called
 * qualified so it is a direct call, then the class-specific operator
 * delete. It sits ahead of the class's definitions so no ROM symbol's
 * region claims it. Nothing here reaches mwccarm. */
extern "C" dExtFrameCtrl_c *_ZN15dExtFrameCtrl_cD0Ev(dExtFrameCtrl_c *thiz) {
    thiz->dExtFrameCtrl_c::~dExtFrameCtrl_c();
    dExtFrameCtrl_c::operator delete(thiz);
    return thiz;
}

/* The host needs the ROM's flat D0 name. MSVC folds the Itanium variants
 * into the one ~dCamera_c() it emits, so the deleting destructor is spelled
 * out here: the D1 body, called qualified, then the class-specific delete.
 * Nothing in this arm reaches mwccarm. */
extern "C" dCamera_c *_ZN9dCamera_cD0Ev(dCamera_c *thiz) {
    thiz->dCamera_c::~dCamera_c();
    dCamera_c::operator delete(thiz);
    return thiz;
}

/* The host uses the flat ROM D0 name, which MSVC never emits: it folds the
 * Itanium destructor variants into the one ~daBook_c() above. This arm
 * spells out what the deleting destructor does -- the D1 body, called
 * qualified so it is a direct call, then the class-specific operator delete.
 * Nothing here reaches mwccarm. */
extern "C" daBook_c *_ZN8daBook_cD0Ev(daBook_c *thiz)
{
    thiz->daBook_c::~daBook_c();
    daBook_c::operator delete(thiz);
    return thiz;
}

/* Host D0, as for daBook_c above. */
extern "C" daBookGen_c *_ZN11daBookGen_cD0Ev(daBookGen_c *thiz)
{
    thiz->daBookGen_c::~daBookGen_c();
    daBookGen_c::operator delete(thiz);
    return thiz;
}

/* The host needs the ROM's flat D0 name, and MSVC never emits it: it folds
 * the Itanium destructor variants into the one ~daDoor_c() above. This arm
 * spells out what the deleting destructor does -- the D1 body, called
 * qualified so it is a direct call, then the class-specific operator
 * delete. Nothing here reaches mwccarm. */
extern "C" daDoor_c *_ZN8daDoor_cD0Ev(daDoor_c *thiz) {
    thiz->daDoor_c::~daDoor_c();
    daDoor_c::operator delete(thiz);
    return thiz;
}

/* The host needs the ROM's flat D0 name, and MSVC never emits it: it folds
 * the Itanium destructor variants into the one ~daIbl_c() above. This arm
 * spells out what the deleting destructor does -- the D1 body, called
 * qualified so it is a direct call, then the class-specific operator
 * delete. Nothing here reaches mwccarm. */
extern "C" daIbl_c *_ZN7daIbl_cD0Ev(daIbl_c *thiz)
{
    thiz->daIbl_c::~daIbl_c();
    daIbl_c::operator delete(thiz);
    return thiz;
}

/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~daKpa_c() it emits,
 * so compiling the definition below as well would define that symbol
 * twice. This arm spells out, in terms of it, what the deleting destructor
 * does: the D1 body, called qualified so it is a direct call even where a
 * header declares the destructor virtual, then the class-specific
 * operator delete. Nothing here reaches mwccarm: it builds the #else arm
 * and emits the ROM bytes it always emitted. */
extern "C" daKpa_c *_ZN7daKpa_cD0Ev(daKpa_c *thiz)
{
    thiz->daKpa_c::~daKpa_c();        /* the D1 body, through the one host symbol */
    daKpa_c::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}

/* The host needs the ROM's flat D0 name; see the note on _ZN7daKpa_cD0Ev. */
extern "C" daKpaTail_c *_ZN11daKpaTail_cD0Ev(daKpaTail_c *thiz)
{
    thiz->daKpaTail_c::~daKpaTail_c();    /* the D1 body, through the one host symbol */
    daKpaTail_c::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}

/* MSVC needs this flat D0 entry. Call the actual class-body destructor
 * qualified so dispatch is direct, then use the class-specific deallocator.
 * The inline body includes member/base teardown; no separate flat D1 provider
 * is supplied by this branch. The mwccarm definition below is unchanged. */
extern "C" daObjFl_Puzzle_c *_ZN16daObjFl_Puzzle_cD0Ev(daObjFl_Puzzle_c *thiz) {
    thiz->daObjFl_Puzzle_c::~daObjFl_Puzzle_c(); /* direct member/base teardown */
    daObjFl_Puzzle_c::operator delete(thiz);     /* the class-specific delete D0 ends with */
    return thiz;
}

/* Host flat names. Not compiled into the cartridge object. */
extern "C" daObjFl_Koma_D_c *_ZN16daObjFl_Koma_D_cD0Ev(daObjFl_Koma_D_c *thiz) {
    thiz->daObjFl_Koma_D_c::~daObjFl_Koma_D_c();
    daObjFl_Koma_D_c::operator delete(thiz);
    return thiz;
}

extern "C" daObjFl_Koma_D_c *_ZN19RotatingPlatformLllD0Ev(daObjFl_Koma_D_c *thiz) {
    delete thiz;
    return thiz;
}

extern "C" daObjRotateUpdownLift_c *_ZN23daObjRotateUpdownLift_cD0Ev(daObjRotateUpdownLift_c *thiz)
{
    thiz->daObjRotateUpdownLift_c::~daObjRotateUpdownLift_c();
    daObjRotateUpdownLift_c::operator delete(thiz);
    return thiz;
}

/* MSVC needs this flat D0 entry. Call the actual class-body destructor
 * qualified so dispatch is direct, then use the class-specific deallocator.
 * The inline body includes member/base teardown; no separate flat D1 provider
 * is supplied by this branch. The mwccarm definition below is unchanged. */
extern "C" daObjSwdoor_c *_ZN13daObjSwdoor_cD0Ev(daObjSwdoor_c *thiz) {
    thiz->daObjSwdoor_c::~daObjSwdoor_c(); /* direct member/base teardown */
    daObjSwdoor_c::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}

/* The host uses the flat ROM D0 name, which MSVC never emits: it folds the
 * Itanium destructor variants into the one ~daPropeller_Heyho_Fire_c()
 * above. This arm spells out what the deleting destructor does -- the D1
 * body, called qualified so it is a direct call, then the class-specific
 * operator delete. Nothing here reaches mwccarm. */
extern "C" daPropeller_Heyho_Fire_c *
_ZN24daPropeller_Heyho_Fire_cD0Ev(daPropeller_Heyho_Fire_c *thiz) {
    thiz->daPropeller_Heyho_Fire_c::~daPropeller_Heyho_Fire_c();
    daPropeller_Heyho_Fire_c::operator delete(thiz);
    return thiz;
}

/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~daStar_c() it emits,
 * so compiling the definition below as well would define that symbol twice.
 * This arm spells out, in terms of it, what the deleting destructor this
 * file is enrolled for does: the D1 body, called qualified so it is a direct
 * call even where a header declares the destructor virtual, then the
 * class-specific operator delete. Nothing here reaches mwccarm: it builds
 * the `#else` arm and emits the ROM bytes it always emitted, and the object
 * is byte-identical either way. */
extern "C" daStar_c *_ZN8daStar_cD0Ev(daStar_c *thiz)
{
    thiz->daStar_c::~daStar_c();     /* the D1 body, through the one host symbol */
    daStar_c::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}

/* The host needs the ROM's flat D0 name, and MSVC never emits it: it folds
 * the Itanium destructor variants into the one ~daWanwan2_c() above. This
 * arm spells out what the deleting destructor does -- the D1 body, called
 * qualified so it is a direct call, then the class-specific operator
 * delete. Nothing here reaches mwccarm. */
extern "C" daWanwan2_c *_ZN11daWanwan2_cD0Ev(daWanwan2_c *thiz)
{
    thiz->daWanwan2_c::~daWanwan2_c();
    daWanwan2_c::operator delete(thiz);
    return thiz;
}

/* The host uses the flat ROM D0 name, which MSVC never emits: it folds the
 * Itanium destructor variants into the one ~daYegg_c() above. This arm
 * spells out what the deleting destructor does -- the D1 body, called
 * qualified so it is a direct call, then the class-specific operator delete.
 * Nothing here reaches mwccarm. */
extern "C" daYegg_c *_ZN8daYegg_cD0Ev(daYegg_c *thiz)
{
    thiz->daYegg_c::~daYegg_c();
    daYegg_c::operator delete(thiz);
    return thiz;
}

/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~dScMgAmida_c() above,
 * so this spells out what the deleting destructor does in terms of it: the
 * D1 body, called qualified so it is a direct call, then the class-specific
 * operator delete. mwccarm never defines _MSC_VER, so nothing here reaches
 * the cartridge object. This definition stays: MSVC will not emit the
 * Itanium D0 from ~dScMgAmida_c(), and the host calls that symbol. */
extern "C" dScMgAmida_c *_ZN12dScMgAmida_cD0Ev(dScMgAmida_c *thiz) {
    thiz->dScMgAmida_c::~dScMgAmida_c();
    dScMgAmida_c::operator delete(thiz);
    return thiz;
}

/* THE HOST NEEDS THE ROM'S FLAT D2 AND D0 NAMES, AND MSVC NEVER EMITS THEM.
 * MSVC folds the Itanium destructor variants into the one ~dScMgBase_c()
 * above. Descendants tearing down a dScMgBase_c base subobject call the
 * base-object variant by its flat name; the deleting variant is the D1 body
 * followed by the class-specific operator delete. Each is spelled here in
 * terms of the one host destructor, called qualified so it is a direct call.
 * mwccarm never defines _MSC_VER, so nothing here reaches the cartridge
 * object. These two definitions stay: MSVC will not emit the Itanium D2
 * and D0 from ~dScMgBase_c(), and the host calls those symbols. The MgFx
 * state tables are already &MgFx::UpdateDrift and &MgFx::UpdateFlyTo;
 * sMgFxUpdateTable is .bss in the ROM, which is where a pointer-to-member
 * initializer lands. */
extern "C" dScMgBase_c *_ZN11dScMgBase_cD2Ev(dScMgBase_c *thiz)
{
    thiz->dScMgBase_c::~dScMgBase_c();
    return thiz;
}

extern "C" dScMgBase_c *_ZN11dScMgBase_cD0Ev(dScMgBase_c *thiz)
{
    thiz->dScMgBase_c::~dScMgBase_c();
    dScMgBase_c::operator delete(thiz);
    return thiz;
}

/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~dScMgCoin_c() above,
 * so this spells out what the deleting destructor does in terms of it: the
 * D1 body, called qualified so it is a direct call, then the class-specific
 * operator delete. mwccarm never defines _MSC_VER, so nothing here reaches
 * the cartridge object. This definition stays: MSVC will not emit the
 * Itanium D0 from ~dScMgCoin_c(), and the host calls that symbol. */
extern "C" dScMgCoin_c *_ZN11dScMgCoin_cD0Ev(dScMgCoin_c *thiz) {
    thiz->dScMgCoin_c::~dScMgCoin_c();
    dScMgCoin_c::operator delete(thiz);
    return thiz;
}

/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~dScMgHanachan_c()
 * above, so this spells out what the deleting destructor does in terms of
 * it: the D1 body, called qualified so it is a direct call, then the
 * class-specific operator delete. mwccarm never defines _MSC_VER, so
 * nothing here reaches the cartridge object. This definition stays: MSVC
 * will not emit the Itanium D0 from ~dScMgHanachan_c(), and the host calls
 * that symbol. */
extern "C" dScMgHanachan_c *_ZN15dScMgHanachan_cD0Ev(dScMgHanachan_c *thiz) {
    thiz->dScMgHanachan_c::~dScMgHanachan_c();
    dScMgHanachan_c::operator delete(thiz);
    return thiz;
}

/* The host has one ~dActor_c(). These are the ROM's flat D2 and D0 names. */
extern "C" dActor_c *_ZN8dActor_cD2Ev(dActor_c *thiz) {
    thiz->dActor_c::~dActor_c();
    return thiz;
}
extern "C" dActor_c *_ZN8dActor_cD0Ev(dActor_c *thiz) {
    thiz->dActor_c::~dActor_c();
    dActor_c::operator delete(thiz);
    return thiz;
}

/* MSVC needs this flat D0 entry. Call the actual class-body destructor
 * qualified so dispatch is direct, then use the class-specific deallocator.
 * The inline body includes member/base teardown; no separate flat D1 provider
 * is supplied by this branch. The mwccarm definition below is unchanged. */
extern "C" dPathLiftActor_c *_ZN16dPathLiftActor_cD0Ev(dPathLiftActor_c *thiz)
{
    thiz->dPathLiftActor_c::~dPathLiftActor_c();          /* direct member/base teardown */
    dPathLiftActor_c::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}

#endif
