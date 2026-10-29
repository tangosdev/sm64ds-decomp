//cpp
#include "dExtModel_c.h"

extern "C" {

/* An ov007 model record. No class names it; this view carries only the fields
 * this function reads. */
struct Ov007ModelRecord {
    ModelComponents *model; /* 0x00 */
    u8 pad04[0x3c];
    Vector3 pos; /* 0x40 */
    u8 pad4c[0x08];
    Matrix4x3 transform; /* 0x54 */
    u8 pad84[0x10];
    int drawFlag; /* 0x94: zero skips the render */
};

void func_ov007_020bbff0(void *c) {
    Ov007ModelRecord *rec = (Ov007ModelRecord *)c;
    if (rec->drawFlag == 0)
        return;
    rec->model->Render(&rec->transform, &rec->pos);
}
}
