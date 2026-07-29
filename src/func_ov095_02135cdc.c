typedef int Fix12i;
typedef short s16;
typedef unsigned short u16;
typedef long long s64;

struct Vector3 { Fix12i x, y, z; };
struct SubA { char pad[0x24]; s16 f24; };

extern Fix12i Vec3_Dist(const struct Vector3* a, const struct Vector3* b);
extern s16 Vec3_HorzAngle(const struct Vector3* v0, const struct Vector3* v1);
extern int func_ov095_0213579c(void* self, char* p);
extern int AngleDiff(int a, int b);
extern s16 data_02082214[];

void func_ov095_02135cdc(char* self, char* other)
{
	const struct Vector3* op;
	int dist;
	int ang;
	int r;
	int x;
	s16 d;
	int y;
	int acc;
	s16* pa;
	s16 dv;
	s16 lim;

	op = (const struct Vector3*)(other + 0x5c);
	dist = Vec3_Dist((const struct Vector3*)(self + 0x5c), op);
	ang = Vec3_HorzAngle((const struct Vector3*)(self + 0x5c), op);
	*(unsigned char*)(self + 0x326) = 1;
	r = func_ov095_0213579c(self, other);

	pa = (s16*)(int)(((long long)(int)(self + 0x8c)) & 0xFFFFFFFFFFFFFFFFLL);
	*pa += ((struct SubA*)(self + 0x300))->f24;
	x = (int)(((s64)dist * r + 0x800) >> 12);
	d = (s16)AngleDiff(ang, *(s16*)(self + 0x8e));
	y = (int)(((s64)x * data_02082214[((u16)d >> 4) * 2 + 1] + 0x800) >> 12);

	if (((struct SubA*)(self + 0x300))->f24 * x < 0)
		acc = (int)(((s64)y * 0xa3LL + 0x800) >> 12);
	else
		acc = (int)(((s64)y * 0x51LL + 0x800) >> 12);

	dv = acc / 4096;
	*(s16*)(int)(((long long)(int)(self + 0x324)) & 0xFFFFFFFFFFFFFFFFLL) += dv;
	lim = (int)(((s64)r * 0x32000LL + 0x800) >> 12) / 4096;
	if (((struct SubA*)(self + 0x300))->f24 > lim) ((struct SubA*)(self + 0x300))->f24 = lim;
	if (((struct SubA*)(self + 0x300))->f24 < -lim) ((struct SubA*)(self + 0x300))->f24 = -lim;
}
