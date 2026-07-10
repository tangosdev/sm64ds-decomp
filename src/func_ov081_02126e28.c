typedef short s16;
typedef unsigned short u16;
typedef long long s64;
typedef struct { int x, y, z; } Vector3;
extern int _ZN5Actor17DetectRaycastClsnER7Vector3S1_b(void* self, Vector3* a, Vector3* b, int cc);
extern void _ZN12WithMeshClsn13SetLimMovFlagEv(void* self);
extern s16 data_02082214[];

int func_ov081_02126e28(char* c) {
  int* pf; Vector3 v; char* player; char* src; int zero; s16 ang; int idx; s16 s;

  pf = (int*)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFFLL);
  *pf = *pf & ~0x80000;
  zero = 0;
  player = *(char**)(c + 0xd0);
  *(int*)(c + 0x98) = *(int*)(player + 0x98) + 0x7000;
  *(int*)(c + 0xa8) = zero;
  player = *(char**)(c + 0xd0);
  ang = *(s16*)(player + 0x8e);
  *(s16*)(c + 0x8e) = ang;
  *(s16*)(c + 0x94) = *(s16*)(c + 0x8e);

  player = *(char**)(c + 0xd0);
  src = (char*)(((long long)(int)(player + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
  *(int*)(c + 0x5c) = *(int*)src;
  *(int*)(c + 0x60) = *(int*)(src+4);
  *(int*)(c + 0x64) = *(int*)(src+8);

  idx = (*(u16*)(c + 0x8e) >> 4);
  s = *(s16*)((char*)data_02082214 + (idx << 2));
  *(int*)(((int)c + 0x5c) & 0xFFFFFFFFFFFFFFFFLL) =
    *(int*)(((int)c + 0x5c) & 0xFFFFFFFFFFFFFFFFLL)
    + (int)(((s64)s * 0x50000 + 0x800) >> 12);
  *(int*)(((int)c + 0x60) & 0xFFFFFFFFFFFFFFFFLL) =
    *(int*)(((int)c + 0x60) & 0xFFFFFFFFFFFFFFFFLL) + 0x50000;
  idx = (*(u16*)(c + 0x8e) >> 4);
  s = *(s16*)((char*)data_02082214 + ((idx * 2 + 1) << 1));
  *(int*)(((int)c + 0x64) & 0xFFFFFFFFFFFFFFFFLL) =
    *(int*)(((int)c + 0x64) & 0xFFFFFFFFFFFFFFFFLL)
    + (int)(((s64)s * 0x50000 + 0x800) >> 12);

  player = *(char**)(c + 0xd0);
  {
    int y = *(int*)(player + 0x60);
    int z = *(int*)(player + 0x64);
    int y2 = y + 0x14000;
    int x = *(int*)(player + 0x5c);
    ((int*)&v)[0]=x; ((int*)&v)[1]=y2; ((int*)&v)[2]=z;
  }

  _ZN5Actor17DetectRaycastClsnER7Vector3S1_b(c, &v, (Vector3*)(c + 0x5c), 1);
  *(int*)(c + 0xd0) = zero;
  _ZN12WithMeshClsn13SetLimMovFlagEv(c + 0x1e4);
  *(int*)(c + 0x3e0) = 7;
  return 1;
}
