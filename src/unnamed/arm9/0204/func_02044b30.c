#include "types.h"
extern s16 data_02082214[];
extern u8 data_020a4bbc;
extern u16 data_02099f94[8];
extern s16 data_020a4bc0;
extern s16 data_020a4bc4;
extern void func_020553a4(int *m);
void func_02044b30(char *obj, int idx)
{
  char *m = (*((char **) (obj + 4))) + (idx * 0x30);
  int new_var;
  int texParam;
  *((int *) 0x40004a4) = *((int *) (m + 0x24));
  *((int *) 0x40004c0) = *((int *) (m + 0x28));
  *((int *) 0x40004c4) = *((int *) (m + 0x2c));
  *((int *) 0x40004ac) = *((int *) (m + 0x20));
  texParam = *((int *) (m + 0x1c));
  *((int *) 0x40004a8) = texParam;
  data_020a4bbc = (u8) (((u32) texParam) >> 0x1e);
  switch (data_020a4bbc)
  {
    case 1:
    {
      int mtx[12];
      int A;
      int B;
      int C;
      int D;
      u16 elemFieldE;
      u16 elemFieldC;
      char *elem;
      int m0;
      u16 rawAngle;
      int angle;
      int k;
      s16 sinVal;
      s16 cosVal;
      int D0;
      int D1;
      *((int *) 0x4000440) = 3;
      rawAngle = *((u16 *) (m + 0x10));
      m0 = *((volatile int *) m);
      elem = (*((char **) ((*((void **) obj)) + 0x18))) + (m0 * 0x14);
      angle = ((u16) (rawAngle << 4)) >> 4;
      k = angle << 1;
      sinVal = data_02082214[k];
      cosVal = data_02082214[k + 1];
      C = (int) ((((long long) (*((int *) (m + 0x8)))) * sinVal) >> 12);
      B = (int) ((((long long) (*((int *) (m + 0x8)))) * cosVal) >> 12);
      A = (int) ((((long long) (*((int *) (m + 0xc)))) * sinVal) >> 12);
      D = (int) ((((long long) (*((int *) (m + 0xc)))) * cosVal) >> 12);
      elemFieldE = *((u16 *) (elem + 0xe));
      elemFieldC = *((u16 *) (elem + 0xc));
      D0 = (A * (-((int) elemFieldE))) / elemFieldC;
      new_var = (C * elemFieldC) / elemFieldE;
      mtx[0] = B;
      mtx[1] = D0;
      mtx[2] = 0;
      D1 = new_var;
      mtx[3] = D1;
      mtx[4] = D;
      mtx[5] = 0;
      mtx[6] = 0;
      mtx[7] = 0;
      mtx[8] = 0;
      mtx[9] = ((elemFieldC * ((*((int *) (m + 0x8))) - (C + B))) * 8) - (elemFieldC * ((int) ((((long long) (*((int *) (m + 0x8)))) * (*((int *) (m + 0x14)))) >> 8)));
      mtx[10] = ((elemFieldE * (((A - D) - (*((int *) (m + 0xc)))) + 0x2000)) * 8) + (elemFieldE * ((int) ((((long long) (*((int *) (m + 0xc)))) * (*((int *) (m + 0x18)))) >> 8)));
      mtx[11] = 0;
      func_020553a4(mtx);
    }
      break;

    case 2:
    {
      /* The ROM's template for this array is data_02099f94 = {4,8,16,32,64,128,256,512},
         i.e. 4 << i, a texture-size table. What stood here was {1,2,...,8}, invented and
         never compared to anything: mwccarm puts a local initializer in an anonymous
         .rodata object (`@30`) that objisolate drops, and match.py wildcards the
         relocation to it, so the file byte-matched with the wrong data in it. */
      s16 buf[8] = {4, 8, 16, 32, 64, 128, 256, 512};
      int idx1;
      int idx2;
      s16 sa;
      s16 sb_;
      idx1 = (((u32) texParam) >> 0x17) & 7;
      idx2 = (((u32) texParam) >> 0x14) & 7;
      data_020a4bc0 = ((s16 *) buf)[idx2];
      data_020a4bc4 = (s16) (-((s16 *) buf)[idx1]);
      sa = (s16) ((*((int *) (m + 0x14))) >> 8);
      sb_ = (s16) ((*((int *) (m + 0x18))) >> 8);
      *((int *) 0x4000488) = ((u16) sa) | (((u16) sb_) << 16);
    }
      break;

  }

}
