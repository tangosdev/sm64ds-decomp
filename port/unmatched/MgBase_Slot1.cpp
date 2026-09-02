/* HOST COPY of dScMgBase_c slot 1, func_ov004_020b0930. Run link60, lane MG1.
 *
 * ONE OF FOUR. port/unmatched/MgBase_DeclConflict.cpp carries the full
 * derivation, the sweep that found exactly four, and the reason the fix is not
 * in include/decl_common.h. The short version: that header declares this
 * function void* while src/func_ov004_020b0930.cpp defines it char*, and the
 * TU includes the header, so MSVC refuses it with C2733. mwccarm accepts it.
 *
 * WHAT CHANGED: `char* c` became `void *cv`, plus a first line
 * `char *c = (char *)cv;`. Nothing else. The body below is the src file's,
 * statement for statement.
 *
 * WHY IT IS ITS OWN TU rather than living beside the other three: src spells
 * data_0209d460 and data_0209d458 as `unsigned char[]` here and slot 30 spells
 * the same two globals as scalar u8. Both are right for their own body and one
 * TU cannot hold both, so the four copies keep the one-body-per-TU shape src
 * has. Merging them was tried and produced C2040 on exactly those two names.
 *
 * Ruled REAL_DECOMP against the ROM before seating: 0x108, 60 instructions, a
 * six-word literal pool every word of which src names. See
 * port/tools/inferred_stub_adjudicated.txt.
 */
#include "types.h"
#include "decl_Scene.h"
#include "decl_common.h"
#include "dScMgBase_c.h"

/* ---- func_ov004_020b0930, slot 1, BeforeInitResources -------------------- */
namespace mg30 {
struct Obj {
    virtual void v0();  virtual void v1();  virtual void v2();  virtual void v3();
    virtual void v4();  virtual void v5();  virtual void v6();  virtual void v7();
    virtual void v8();  virtual void v9();  virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual int v26();  virtual void v27();
    virtual void v28(); virtual void v29(); virtual void v30(); virtual void v31();
    virtual void v32(); virtual void v33();
};
}  /* namespace mg30 */

extern "C" {
extern void Enable3dEngines(void);
extern char data_0209b308[];
extern void *data_ov004_020beb60;
extern char *data_ov004_020beb68;
extern char data_0209f61c[];
extern unsigned char data_0209d460[];
extern unsigned char data_0209d458[];

// PORT_HOST_ABI: decl_common.h declares this void* while src defines it char*, which MSVC rejects as C2733 where mwccarm accepts; host copy respells the parameter
int func_ov004_020b0930(void *cv)
{
    char *c = (char *)cv;                       /* the one added line */
    struct dScMgBase_c *self = (struct dScMgBase_c *)(void *)c;
    if (_ZN5Scene19BeforeInitResourcesEv(c) == 0) return 0;
    if (((mg30::Obj*)c)->v26() == 0)
        func_02019028();
    else
        Enable3dEngines();
    self->unk_0c8 = *(int*)(data_0209b308 + 0x28);
    if (data_ov004_020beb60 == 0)
        data_ov004_020beb60 = _ZN6Memory13operator_new2Ej(0x4000);
    if (data_ov004_020beb68 != 0)
        *(int*)(data_ov004_020beb68 + 0xb0) = 0;
    self->unk_0b4 = 0;
    self->unk_0b8 = 0;
    self->unk_465c = 0;
    func_ov004_020b8a8c(c + 0x4000);
    ((mg30::Obj*)c)->v33();
    func_ov004_020b2cb8();
    _ZN5Scene9SetFadersEP15FaderBrightness(data_0209f61c);
    func_0202ec9c(data_0209f61c, 0);
    data_0209d460[0] = 0;
    data_0209d458[0] = 0;
    ((mg30::Obj*)c)->v31();
    return 1;
}
}  /* extern "C" */

