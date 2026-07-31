//cpp
// @symbol func_ov015_021112a0
// @emits daObjBkBillboard_c_InitResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daObjBkBillboard_c::InitResources - recovered from vtable slot identity */
struct BMD_File;
struct SharedFilePtr;

struct Model {
    static BMD_File *LoadFile(SharedFilePtr &);
};

struct ModelBase {
    void SetFile(BMD_File *, int, int);
};


extern "C" int daObjBkBillboard_c_InitResources(char *c) {
    BMD_File *file = Model::LoadFile(*(SharedFilePtr *)&data_ov015_02114960);
    ((ModelBase *)(c + 0xd4))->SetFile(file, 1, -1);
    func_ov015_02111214(c);
    return 1;
}
