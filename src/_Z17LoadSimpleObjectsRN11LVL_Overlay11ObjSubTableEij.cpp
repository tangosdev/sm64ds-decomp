//cpp
typedef int Fix12i;
struct Vec { Fix12i x, y, z; };
struct Vector3_16;

struct Entry {
  unsigned short raw;   // 0
  short x;              // 2
  short y;              // 4
  short z;              // 6
};
struct ObjSubTable {
  unsigned char pad0;   // 0
  unsigned char count;  // 1
  unsigned char pad2[2];
  Entry* entries;       // 4
};

extern short NEXT_DEATH_TABLE_ID;
extern unsigned short data_ov002_0210cbf4[];

extern "C" {
void _Z23LoadMinimapChangeObjecti5Fix12IiEh(int a, Fix12i b, signed char h);
void _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int a, unsigned int b, Vec* v, Vector3_16* rot, int e, int f);

void _Z17LoadSimpleObjectsRN11LVL_Overlay11ObjSubTableEij(ObjSubTable& tbl, int p2, int p3)
{
  Entry* e = tbl.entries;
  int i;
  for (i = 0; i < (int)tbl.count; i++, e++) {
    Vec v;
    int raw;
    int param;
    int masked;
    int zz = e->z << 12;
    int yy = e->y << 12;
    int xx = e->x << 12;
    v.x = xx;
    v.y = yy;
    v.z = zz;
    raw = e->raw;
    masked = (unsigned short)(raw & 0x1ff);
    param = raw >> 9;
    if (masked == 0x1ff) {
      _Z23LoadMinimapChangeObjecti5Fix12IiEh(p2, yy, (signed char)(param & 0xf));
    } else {
      short old = NEXT_DEATH_TABLE_ID;
      NEXT_DEATH_TABLE_ID = (short)(old + 1);
      _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
        data_ov002_0210cbf4[masked], param, &v, 0, p2, old);
    }
  }
}
}
