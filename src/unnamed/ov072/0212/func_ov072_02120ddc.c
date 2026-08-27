// @symbol func_ov072_02120ddc
/* recovered: shared common types */
#include "common.h"
typedef int Fix12i;

extern Fix12i Vec3_Dist(const struct Vector3 *a, const struct Vector3 *b);
extern void func_ov072_02121d50(void *c, int n);
void func_ov072_02120ddc(char *c)
{
    char *o = *(char **)(c + 0x364);
    if (o == 0)
        return;
    if (Vec3_Dist((const struct Vector3 *)(c + 0x5c), (const struct Vector3 *)(o + 0x5c)) >= 0x190000)
        return;
    func_ov072_02121d50(c, 3);
}
