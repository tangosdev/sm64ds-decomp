#include "common.h"

void *func_ov002_020e496c(char *c);
int _ZN6Player14IsFrontSlidingEv(void *self);
int _ZN6Player17LostGrabbedObjectEv(void *self);
void Math_Function_0203b14c(void *out, int a1, int a2, int a3, int a4);
void *_ZN5Actor11UpdateCarryER6PlayerRK7Vector3(void *self, void *player, const void *v);
void Matrix4x3_FromRotationY(void *m, int angle);
void MulMat4x3Mat4x3(void *a, void *b, void *out);
void Vec3_Lsl(Vector3 *d, Vector3 *s, int sh);
void Vec3_LslInPlace(Vector3 *v, int sh);
int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(int sys, unsigned int kind, int fx, int a1, int a2, const void *v, void *cb);
void func_ov102_0214b444(void *c);
extern int data_ov102_0214ea18[];
extern int data_ov102_0214ea1c[];
extern int data_ov102_0214ea20[];
extern Matrix4x3 data_020a0e68;
extern s16 data_02082214[];

void func_ov102_0214b53c(char *c)
{
  volatile u16 tmp[6]; /* angle-home pins */
  Vector3 p;
  volatile Vector3 dead; /* stack-layout placeholder; never a live value */
  int x0;
  Vector3 d;
  int xSaved;
  char *pPos;
  char *src;
  /* chained u32 masks: pin the compare dest; u32 form is invisible to the u64 ratchet */
  if (((((((((*((u8 *) (c + 0x3f5))) == 2) && ((src = *((char **) (c + 0xc8))) != 0)) & 0xFFFFFFFFu) & 0xFFFFFFFFu) & 0xFFFFFFFFu) & 0xFFFFFFFFu) & 0xFFFFFFFFu) & 0xFFFFFFFFu)
  {
    *((Matrix4x3 *) (c + 0x31c)) = *((Matrix4x3 *) src);
  }
  else
  {
    int haveMtx = (int) (((*((u32 *) (c + 0xb0))) & 0x4000) != 0);
    char *player0;
    if (((haveMtx != 0) && ((player0 = *((char **) (c + 0x390))) != 0)) && ((*((int *) (player0 + 0xc8))) != 0))
    {
      char *ret = (char *) func_ov002_020e496c(player0);
      u8 idx = 0;
      if (_ZN6Player14IsFrontSlidingEv(*((char **) (c + 0x390))) != 0)
      {
        idx = 1;
      }
      if ((_ZN6Player17LostGrabbedObjectEv(*((char **) (c + 0x390))) != 0) && (((u32) (((u32) ((*((int *) (ret + 0x58))) << 4)) >> 0x10)) < 0xe))
      {
        idx = 1;
      }
      if ((*((int *) ((*((char **) (c + 0x390))) + 8))) == 2)
      {
        idx = (u8) (idx + 2);
      }
      int off = idx * 0xc;
      Math_Function_0203b14c(c + 0x3d0, *((int *) (((char *) data_ov102_0214ea18) + off)), 0x800, 0x3e8000, 4);
      Math_Function_0203b14c(c + 0x3d4, *((int *) (((char *) data_ov102_0214ea1c) + off)), 0x800, 0x3e8000, 4);
      Math_Function_0203b14c(c + 0x3d8, *((int *) (((char *) data_ov102_0214ea20) + off)), 0x800, 0x3e8000, 4);
      Matrix4x3 *rmtx = (Matrix4x3 *) _ZN5Actor11UpdateCarryER6PlayerRK7Vector3(c, *((char **) (c + 0x390)), c + 0x3d0);
      *((Matrix4x3 *) (c + 0x31c)) = *rmtx;
    }
    else
    {
      *((int *) (c + 0x3d0)) = 0;
      *((int *) (c + 0x3d4)) = 0;
      *((int *) (c + 0x3d8)) = 0;
      Matrix4x3_FromRotationY(c + 0x31c, *((s16 *) (c + 0x8e)));
      *((int *) (c + 0x340)) = (*((int *) (c + 0x5c))) >> 3;
      *((int *) (c + 0x344)) = (*((int *) (c + 0x60))) >> 3;
      *((int *) (c + 0x348)) = (*((int *) (c + 0x64))) >> 3;
    }
  }
  int haveMtx2 = (int) (((*((u32 *) (c + 0xb0))) & 0x4000) != 0);
  if ((haveMtx2 != 0) || ((*((u16 *) (c + 0x3ea))) != 0))
  {
    {
      Matrix4x3 *g = &data_020a0e68;
      char *m = c + 0x31c;
      p.x = 0;
      p.y = 0;
      p.z = 0;
      *g = *((Matrix4x3 *) m);
    }
    MulMat4x3Mat4x3((*((char **) (c + 0x314))) + 0x60, &data_020a0e68, &data_020a0e68);
    {
      int ty = *((int *) (((char *) (&data_020a0e68)) + 0x28));
      int tx = *((int *) (((char *) (&data_020a0e68)) + 0x24));
      p.y = ty;
      int tz = *((int *) (((char *) (&data_020a0e68)) + 0x2c));
      p.z = tz;
      p.x = tx;
      Vec3_Lsl(&d, &p, 3);
    }
    {
      int dy = *((int *) (((char *) (&d)) + 4));
      int dx = *((int *) (((char *) (&d)) + 0));
      x0 = dx;
      p.y = dy;
      int dz = *((int *) (((char *) (&d)) + 8));
      p.x = x0;
      p.z = dz;
    }
    int haveMtx3 = (int) (((*((u32 *) (c + 0xb0))) & 0x4000) != 0);
    if (haveMtx3 != 0)
    {
      *((int *) (c + 0x3f8)) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(*((int *) (c + 0x3f8)), 0x13, p.x, p.y, p.z, 0, 0);
    }
    if ((*((u16 *) (c + 0x3ea))) != 0)
    {
      int haveShadow = (int) (((*((u32 *) (c + 0xb0))) & 0x40000) != 0);
      if (haveShadow != 0)
      {
        char *obj = *((char **) (c + 0xd0));
        if (obj != 0)
        {
          char *pos = obj + 0x5c;
          int dx = *((int *) pos);
          int gbase = (int) &data_020a0e68;
          dead.x = dx;
          int dy = *((int *) (pos + 4));
          int off5ec = 0x5ec;
          dead.y = dy;
          int dz = *((int *) (pos + 8));
          dead.z = dz;
          data_020a0e68 = *((Matrix4x3 *) (((char *) *((int *) (c + 0xd0))) + off5ec));
          {
            int tx = *(volatile int *) ((char *) &data_020a0e68 + 0x24); /* pin mtx translation loads */
            int ty = *(volatile int *) ((char *) &data_020a0e68 + 0x28);
            int tz = *(volatile int *) ((char *) &data_020a0e68 + 0x2c);
            p.x = tx;
            p.z = tz;
            pPos = (char *) &p;
            p.y = ty;
            Vec3_LslInPlace((Vector3 *) pPos, 3);
          }
          char *p2 = *((char **) (c + 0xd0));
          u16 a8c = *((volatile u16 *) (p2 + 0x8c)); /* pin angle loads */
          u16 a8e = *((volatile u16 *) (p2 + 0x8e));
          tmp[1] = a8e;
          tmp[0] = a8c;
          xSaved = p.x;
          x0 = *((volatile u16 *) (p2 + 0x90));
          s16 angY = (s16) tmp[1];
          tmp[2] = (u16) x0;
          u16 v580 = *((volatile u16 *) (p2 + 0x580));
          tmp[4] = *((u16 *) (p2 + 0x582));
          tmp[3] = v580;
          s16 v582s = (s16) tmp[4];
          tmp[5] = *((u16 *) (p2 + 0x584));
          int diff = (angY + 0x1800) - v582s;
          tmp[1] = (u16) diff;
          {
            u16 ang8e = tmp[1];
            u16 v582 = (u16) v582s;
            int py = p.y + 0x14000;
            p.y = py;
            p.y = py + (((int) data_02082214[(v582 >> 4) * 2]) * 0x1e);
            int idxX2 = (ang8e >> 4) * 2;
            p.x = xSaved + (((int) data_02082214[idxX2]) * 0x3c);
            p.z = p.z + (((int) data_02082214[idxX2 + 1]) * 0x3c);
          }
        }
      }
      *((int *) (c + 0x3fc)) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(*((int *) (c + 0x3fc)), 0x19, p.x, p.y, p.z, 0, 0);
    }
  }
  func_ov102_0214b444(c);
}
