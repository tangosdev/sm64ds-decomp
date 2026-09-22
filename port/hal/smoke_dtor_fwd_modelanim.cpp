// port/hal/smoke_dtor_fwd_modelanim.cpp -- ONE row of
// port/hal/dtor_forwarders_gen_w9c.cpp, for smoke_modelanim.
//
// Lane SMOKELINK3, run link100 wave 10 round 2.
//
// WHAT THE ROW IS.  ModelAnim's base-object destructor at ROM 0x0201689c is
// defined OUT OF LINE by src/_ZN9ModelAnimD1Ev.cpp, so the only translation
// unit that emits it emits it under its MSVC name ??1ModelAnim@@UAE@XZ, while
// port/unmatched/ModelBaseModelBlend_Dtors_HostCopy.c's BlendModelAnim bodies
// call the flat _ZN9ModelAnimD2Ev the ROM's own callers spell.  On the window
// targets port/tools/dtorfwd.py generates that forwarder into
// hal/dtor_forwarders_gen_w9c.cpp; walk_window.map line 26138 shows it there.
//
// WHY NOT THAT FILE ITSELF.  It carries thirty rows for thirty classes,
// including TowerStep, KnockDownPlank and the rest of wave 9c's batch, and a
// model-animation harness hosts none of them.  This is the subset argument
// port/faces_sync_smoke_heap.txt makes for the ledger, applied once more.
//
// THE ROW BELOW IS COPIED CHARACTER FOR CHARACTER from
// hal/dtor_forwarders_gen_w9c.cpp lines 150-152, comment included.  It is a
// generated file, so if dtorfwd.py's output for this class ever changes the
// two texts disagree and the diff is one grep away.  No target carries both
// files, so the two definitions cannot collide.

#include "ModelAnim.h"

/* ROM 0x0201689c _ZN9ModelAnimD2Ev -- batch 5, ~ModelAnim(), defined out of line by src/_ZN9ModelAnimD1Ev.cpp */
extern "C" void _ZN9ModelAnimD2Ev(void *self)
{ ((ModelAnim *)self)->ModelAnim::~ModelAnim(); }
