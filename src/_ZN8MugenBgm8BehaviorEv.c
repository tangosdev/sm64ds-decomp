
typedef int s32;
typedef short s16;
typedef unsigned char u8;
typedef unsigned int u32;
typedef struct 
{
  s32 x;
  s32 y;
  s32 z;
} Vec3;
typedef struct 
{
  s32 w[12];
} Mtx43;
typedef struct 
{
  char pad[0x50];
} RaycastGround;
extern int func_ov002_020f63d4(void *c);
extern int func_ov002_020f23f0(void *c);
extern void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(void *c, void *cyl);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void *c, void *cyl);
extern void _ZN13RaycastGroundC1Ev(RaycastGround *rc);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(RaycastGround *rc, const Vec3 *v, void *actor);
extern s32 _ZN13RaycastGround10DetectClsnEv(RaycastGround *rc);
extern void _ZN13RaycastGroundD1Ev(RaycastGround *rc);
extern void Vec3_Asr(Vec3 *d, Vec3 *s, int sh);
extern void Matrix4x3_FromTranslation(Mtx43 *m, s32 x, s32 y, s32 z);
extern void Matrix4x3_ApplyInPlaceToRotationZXYExt(void *m, s32 x, s32 y, s32 z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, s32 x, s32 y, s32 z);
extern void func_ov002_020f64ac(void *c, void *p);
extern int func_ov002_020f65ec(void *c);
extern Mtx43 data_020a0e68;
extern int data_ov002_0210bc88;
int _ZN8MugenBgm8BehaviorEv(void *arg0)
{
  char *c = (char *) arg0;
  s32 *new_var;
  RaycastGround rc;
  Vec3 v;
  Vec3 asr;
  int new_var2;
  s32 t;
  t = *((s32 *) (c + 8));
  if (t == 0x2e)
  {
    return func_ov002_020f63d4(c);
  }
  if (t == 0x2f)
  {
    return func_ov002_020f23f0(c);
  }
  if ((((u32) t) >= 0x1a) && (((u32) t) <= 0x2d))
  {
    _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(c, 0);
  }
  else
  {
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
  }
  {
    char *g = *((char **) (c + 0xe0));
    if ((g != 0) && ((*((u8 *) (g + 0x83))) != 0))
    {
      {
        s32 xx = *((s32 *) (c + 0x5c));
        s32 yy = *((s32 *) (c + 0x60));
        s32 zz = *((s32 *) (c + 0x64));
        new_var2 = yy + 0x96000;
        v.x = xx;
        v.y = new_var2;
        v.z = zz;
      }
      _ZN13RaycastGroundC1Ev(&rc);
      _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rc, &v, 0);
      if (_ZN13RaycastGround10DetectClsnEv(&rc) != 0)
      {
        s32 h = *(new_var = (s32 *) (((char *) (&rc)) + 0x44));
        if ((*((s32 *) (c + 0x60))) < h)
        {
          *((s32 *) (c + 0x60)) = h;
          *((u8 *) ((((int) c) + 0x103) & 0xFFFFFFFFFFFFFFFF)) |= 1;
        }
      }
      _ZN13RaycastGroundD1Ev(&rc);
    }
  }
  Vec3_Asr(&asr, (Vec3 *) (c + 0x5c), 3);
  Matrix4x3_FromTranslation(&data_020a0e68, asr.x, asr.y, asr.z);
  t = *((s32 *) (c + 8));
  if ((((u32) t) >= 0x1a) && (((u32) t) <= 0x2d))
  {
    Matrix4x3_ApplyInPlaceToRotationZXYExt(&data_020a0e68, *((s16 *) (c + 0x8c)), *((s16 *) (c + 0x8e)), *((s16 *) (c + 0x90)));
  }
  else
  {
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, *((s16 *) (c + 0x8c)), *((s16 *) (c + 0x8e)), *((s16 *) (c + 0x90)));
  }
  if ((*((char **) (c + 0xdc))) != 0)
  {
    *((Mtx43 *) ((*((char **) (c + 0xdc))) + 0x1c)) = data_020a0e68;
  }
  if ((*((char **) (c + 0xe0))) != 0)
  {
    *((Mtx43 *) ((*((char **) (c + 0xe0))) + 0x1c)) = data_020a0e68;
    func_ov002_020f64ac(*((char **) (c + 0xe0)), &data_ov002_0210bc88);
    func_ov002_020f65ec(*((char **) (c + 0xe0)));
  }
  return 1;
}
