typedef struct Vec3 { int x, y, z; } Vec3;

extern void func_020731dc(int a, int b, void** node);
extern void Math_Function_0203b0fc(int* p, int target, int scale, int max);
extern void Vec3_Sub(Vec3* out, Vec3* a, Vec3* b);
extern void Vec3_MulScalar(Vec3* out, const Vec3* in, int scale);
extern void Vec3_Add(Vec3* out, Vec3* a, Vec3* b);
extern void MulVec3Mat4x3(void* a, void* b, void* c);
extern void Vec3_LslInPlace(void* v, int n);
extern int _Z14ApproachLinearRiii(int* dst, int target, int step);

extern unsigned short KS_FRAME_COUNTER;
extern int data_ov002_02110b04;
extern int data_ov002_02110afc;
extern int data_ov002_02110b08;
extern int data_ov002_02110df4[];
extern int data_ov002_02110e0c[];
extern int data_ov002_02110e18[];
extern void func_020072c0(void);
extern int data_ov002_02110de8[];
extern int data_ov002_02110e00[];
extern int data_ov002_02110dac[];
extern int INV_VIEW_MATRIX_ASR_3[];

int func_ov002_020f7538(char* c, unsigned char* arg1, int arg2) {
  int a = arg1[0];
  int b = arg1[1];
  int f1 = a << 0xc;
  int f2 = b << 8;
  int* base;
  int scale;
  Vec3 tmp, diff, scaled, sum;

  if (f1 != 0 && arg2 == KS_FRAME_COUNTER) {
    *(short*)(c + 0x8c) = 0xd00;
    *(short*)(c + 0x8e) = -0x7300;
    *(short*)(c + 0x90) = 0x2f00;
    *(int*)(c + 0xf8) = 0;
  }

  if (!(data_ov002_02110b04 & 1)) {
    data_ov002_02110df4[0] = 0x2800;
    data_ov002_02110df4[1] = 0xfffee900;
    data_ov002_02110df4[2] = -0x4200;
    func_020731dc((int)data_ov002_02110df4, (int)func_020072c0, (void**)data_ov002_02110de8);
    data_ov002_02110b04 |= 1;
  }
  if (!(data_ov002_02110afc & 1)) {
    data_ov002_02110e0c[0] = 0x3300;
    data_ov002_02110e0c[1] = 0xfffee900;
    data_ov002_02110e0c[2] = -0x5e00;
    func_020731dc((int)data_ov002_02110e0c, (int)func_020072c0, (void**)data_ov002_02110e00);
    data_ov002_02110afc |= 1;
  }
  if (!(data_ov002_02110b08 & 1)) {
    data_ov002_02110e18[0] = 0x4c00;
    data_ov002_02110e18[1] = -0xf200;
    data_ov002_02110e18[2] = -0xff00;
    func_020731dc((int)data_ov002_02110e18, (int)func_020072c0, (void**)data_ov002_02110dac);
    data_ov002_02110b08 |= 1;
  }

  if (f1 != 0) {
    base = data_ov002_02110df4;
    scale = 0x1000;
  } else {
    base = data_ov002_02110e18;
    scale = 0;
  }

  Math_Function_0203b0fc((int*)(c + 0xf8), scale, 0x40, 0x100);
  Vec3_Sub(&diff, (Vec3*)data_ov002_02110e0c, (Vec3*)base);
  tmp.x = diff.x;
  tmp.y = diff.y;
  tmp.z = diff.z;
  Vec3_MulScalar(&scaled, &tmp, *(int*)(c + 0xf8));
  Vec3_Add(&sum, (Vec3*)base, &scaled);

  {
    char* dp = (char*)(((long long)(int)(*(char**)(c + 0xe0) + 0x64)) & 0xFFFFFFFFFFFFFFFFLL);
    *(int*)(dp) = sum.x;
    *(int*)(dp + 4) = sum.y;
    *(int*)(dp + 8) = sum.z;
  }
  MulVec3Mat4x3(*(char**)(c + 0xe0) + 0x64, INV_VIEW_MATRIX_ASR_3, c + 0x5c);
  Vec3_LslInPlace(c + 0x5c, 3);
  _Z14ApproachLinearRiii((int*)(c + 0xf4), f1, f2);
  *(unsigned char*)(c + 0x102) = *(int*)(c + 0xf4) >> 0xc;
  return 1;
}
