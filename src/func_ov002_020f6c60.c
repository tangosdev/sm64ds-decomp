
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef struct 
{
  s32 x;
  s32 y;
  s32 z;
} Vec3;
extern unsigned int RandomIntInternal(int *seed);
extern void AddVec3(Vec3 *a, Vec3 *b, Vec3 *c);
extern void func_0201267c(int a, void *p);
extern void func_020731dc(int a, int b, void **node);
extern void func_ov002_020f6f48(char *c, Vec3 *v, int amt);
extern void MulVec3Mat4x3(void *m, void *v, Vec3 *out);
extern void Vec3_LslInPlace(Vec3 *v, int sh);
extern u16 KS_FRAME_COUNTER;
extern int RNG_STATE;
extern char *CAMERA;
extern int data_ov002_0210b614;
extern int data_ov002_02110b0c;
extern int data_ov002_02110c20[];
extern int func_020072c0;
extern int data_ov002_02110db8;
extern int data_ov002_0210b958;
extern int INV_VIEW_MATRIX_ASR_3;
inline unsigned int inline_fn(unsigned int arg0)
{
  return arg0;
}

int func_ov002_020f6c60(void *arg0, void *arg1, int arg2, int arg3)
{
  char *c = (char *) arg0;
  u8 *flag = (u8 *) arg1;
  Vec3 v;
  if (arg2 == KS_FRAME_COUNTER)
  {
    if ((*flag) != 0)
    {
      int r = (inline_fn(RandomIntInternal(&RNG_STATE)) >> 20) & 0xfff;
      int m = (s32) (((((long long) r) * 0xa000) + 0x800) >> 12);
      *((s32 *) (c + 0xf4)) = m + 0x19000;
    }
    AddVec3((Vec3 *) (c + 0x5c), (Vec3 *) (CAMERA + 0x8c), (Vec3 *) (c + 0x5c));
    if (arg2 > 0x2bc)
    {
      *((s32 *) ((((int) c) + 0x60) & 0xFFFFFFFFFFFFFFFF)) -= 0x96000;
    }
    if (arg2 != data_ov002_0210b614)
    {
      func_0201267c(0x6a, c + 0x74);
      data_ov002_0210b614 = arg2;
    }
  }
  if (arg3 == KS_FRAME_COUNTER)
  {
    *((u8 *) (c + 0x102)) = 0;
    return 1;
  }
  *((u8 *) (c + 0x102)) = 0x1f;
  if ((*flag) == 0)
  {
    if ((data_ov002_02110b0c & 1) == 0)
    {
      data_ov002_02110c20[0] = 0xffdd6000;
      data_ov002_02110c20[1] = 0xbe4000;
      data_ov002_02110c20[2] = 0x2ae000;
      func_020731dc((int) data_ov002_02110c20, (int) (&func_020072c0), (void **) (&data_ov002_02110db8));
      data_ov002_02110b0c |= 1;
    }
    func_ov002_020f6f48(c, (Vec3 *) data_ov002_02110c20, 0x20);
  }
  else
  {
    MulVec3Mat4x3(&data_ov002_0210b958, &INV_VIEW_MATRIX_ASR_3, &v);
    Vec3_LslInPlace(&v, 3);
    func_ov002_020f6f48(c, &v, 8);
  }
  *((s16 *) (c + 0x92)) = *((s16 *) (c + 0x8c));
  *((s16 *) (c + 0x94)) = *((s16 *) (c + 0x8e));
  *((s16 *) (c + 0x96)) = *((s16 *) (c + 0x90));
  return 1;
}
