//cpp
// @symbol func_ov006_02122814
/* recovered: shared common types */
#include "common.h"
extern "C" void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern "C" void Matrix4x3_ApplyInPlaceToRotationZ(void *m, short angZ);

extern Matrix4x3 data_020a0e68;
struct C {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5(void *arg);
};
extern "C" void func_ov006_02122814(char *c);
void func_ov006_02122814(char *c) {
    Matrix4x3_FromTranslation(&data_020a0e68, *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64));
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, *(short *)(c + 0x74));
    *(Matrix4x3 *)(c + 0x1c) = data_020a0e68;
    ((C *)c)->v5(c + 0x50);
}
