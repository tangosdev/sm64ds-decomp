//cpp
typedef int Fix12i;
struct Vec { Fix12i x, y, z; };
struct Vector3_16;

struct Entry {
  unsigned short raw;   // 0
  short x;              // 2
  short y;              // 4
  short z;              // 6
  unsigned short rot[3];// 8 (Vector3_16)
  unsigned short param; // 0xe
};
struct ObjSubTable {
  unsigned char pad0;   // 0
  unsigned char count;  // 1
  unsigned char pad2[2];
  Entry* entries;       // 4
};

extern short data_ov002_0211118c;
extern unsigned short data_ov002_0210cbf4[];

extern "C" {
void _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int a, unsigned int b, Vec* v, Vector3_16* rot, int e, int f);

void _Z19LoadStandardObjectsRN11LVL_Overlay11ObjSubTableEij(ObjSubTable& tbl, int p2, unsigned int p3)
{
  Entry* e = tbl.entries;
  int i;
  for (i = 0; i < (int)tbl.count; i++, e++) {
    Vec v;
    unsigned int a = data_ov002_0210cbf4[e->raw];
    int zz = e->z << 12;
    int yy = e->y << 12;
    int xx = e->x << 12;
    v.x = xx;
    v.y = yy;
    v.z = zz;
    short old = data_ov002_0211118c;
    data_ov002_0211118c = (short)(old + 1);
    _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
      a, e->param, &v, (Vector3_16*)((char*)e + 8), p2, old);
  }
}
}
