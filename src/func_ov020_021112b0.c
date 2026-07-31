// @symbol func_ov020_021112b0
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
/* func_ov020_021112b0 at 0x021112b0 (ov020), size 0x90
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 * flags: -O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */


extern void Vec3_Sub(struct Vector3 *d, struct Vector3 *a, struct Vector3 *b);
extern int Vec3_HorzLen(struct Vector3 *);

void func_ov020_021112b0(char *c)
{
  char *p = Actor_ClosestPlayer(c);
  if (!p)
    return;
  struct Vector3 *ps = (struct Vector3 *)(((long long)(int)(p + 0x5c)));
  struct Vector3 tmp;
  tmp.x = ps->x;
  tmp.y = ps->y;
  tmp.z = ps->z;
  struct Vector3 d;
  Vec3_Sub(&d, &tmp, (struct Vector3 *)(c + 0x5c));
  *((short *)(c + 0x446)) = cstd_atan2(d.x, d.z);
  *((short *)(c + 0x444)) = cstd_atan2(d.y, Vec3_HorzLen(&d));
  *((short *)(c + 0x448)) = 0x4000;
}
