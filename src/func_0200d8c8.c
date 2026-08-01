// @symbol func_0200d8c8
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
/* Camera ground-pound jitter: if the target point is within range, accumulate
 * a jitter offset (unk134) scaled by 0x9000/distance, clamped to 0xc000. */

typedef int Fix12i; /* 20.12 fixed-point */

 /* 0xc */

struct Camera {
	char _pad0[0x8c];
	struct Vector3 pos;   /* 0x8c */
	char _pad1[0x134 - 0x98];
	Fix12i unk134;        /* 0x134 */
};

/* Resolved callees (symbols/verified.tsv):
 *   0x0203cfdc = Vec3_Dist(const Vector3&, const Vector3&) -> Vec3_Dist
 *   0x02053258 = cstd::fdiv(int, int)                      -> _ZN4cstd4fdivEii */
extern Fix12i Vec3_Dist(const struct Vector3 *a, const struct Vector3 *b);

void func_0200d8c8(struct Camera *cam, const struct Vector3 *v, int strength)
{
	Fix12i dist = Vec3_Dist(&cam->pos, v);
	if (strength <= dist)
		return;

	Fix12i offset = (Fix12i)(((long long)_ZN4cstd4fdivEii(strength, dist) * 0x9000 + 0x800) >> 12);

	if (offset <= cam->unk134)
		return;

	if (offset > 0xc000)
		offset = 0xc000;

	cam->unk134 = offset;
}
