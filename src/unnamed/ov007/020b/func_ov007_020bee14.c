// @symbol func_ov007_020bee14
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"

void func_ov007_020bee14(void)
{
    struct Vector3 v = data_ov007_020d7d60;
    v.z += 0x200;
    func_ov007_020ca5f0(data_ov007_02104bd4[0], data_ov007_02104bd4[1], &v, 0x3e8, 1);
    func_ov007_020cb3dc(data_ov007_02104bd4[0]);
    v.z += 0x200;
    func_ov007_020ca5f0(data_ov007_02104bd4[0], data_ov007_02104bd4[2], &v, 0x3e8, 1);
}
