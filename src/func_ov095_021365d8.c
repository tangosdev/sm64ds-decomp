/* func_ov095_021365d8 at 0x021365d8 (ov095), size 0x18c
 * Compiler mwccarm 1.2/sp2p3, flags:
 * -O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 * natural signed /2 spelling; mwcc synthesizes add/lsr#31/asr#1 itself and
 * self-schedules the byte store into the ROM slot */
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
extern void *_ZN5Model8LoadFileER13SharedFilePtr(int sfp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *m, void *f, int a, int b);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *c);
extern void *_ZN12MeshCollider8LoadFileER13SharedFilePtr(int sfp);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *mc, void *kcl, void *mtx, int fix, short s, int clps);
extern void func_020393d4(void *p, void *v);
extern void func_020393c4(void *p, void *v);
extern int _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_;
extern int func_ov095_02136788;
extern int data_ov095_02136f68[];
extern int data_ov095_02136f74[];
extern int data_ov095_021375a4[];
int func_ov095_021365d8(char *c)
{
  int idx;
  int *new_var;
  u16 t;
  int b;
  *((u8 *) (c + 0x349)) = 0;
  t = *((u16 *) (c + 0xc));
  b = (int) (t == 0x20);
  if (b != 0)
  {
    *((int *) (c + 0x328)) = 0;
    goto load;
  }
  b = (int) (t == 0x21);
  if (b != 0)
  {
    *((int *) (c + 0x328)) = 1;
    goto load;
  }
  b = (int) (t == 0x83);
  if (b != 0)
  {
    *((int *) (c + 0x328)) = 2;
    *((u8 *) (c + 0x349)) = 1;
    goto load;
  }
  return 0;
  load:
  idx = *((int *) (c + 0x328));

  _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, _ZN5Model8LoadFileER13SharedFilePtr(data_ov095_02136f68[idx]), 1, -1);
  new_var = &_ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_;
  _ZN8Platform19UpdateClsnPosAndRotEv(c);
  idx = *((int *) (c + 0x328));
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(c + 0x124, _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov095_02136f74[idx]), c + 0x2ec, 0x199, *((s16 *) (c + 0x8e)), data_ov095_021375a4[idx]);
  func_020393d4(c + 0x124, new_var);
  func_020393c4(c + 0x124, &func_ov095_02136788);
  *((int *) (c + 0x320)) = 0;
  *((int *) (c + 0x32c)) = 0;
  if ((*((u8 *) (c + 0x349))) == 2)
  {
    *((int *) (c + 0x334)) = (*((int *) (c + 0x60))) + ((*((u16 *) (c + 0x96))) << 12);
  }
  else
  {
    *((int *) (c + 0x334)) = *((int *) (c + 0x60));
  }
  *((int *) (c + 0x338)) = (*((int *) (c + 0x334))) - ((*((u16 *) (c + 0x92))) << 12);
  *((int *) (c + 0x33c)) = ((*((int *) (c + 0x334))) + (*((int *) (c + 0x338)))) / 2;
  *((u8 *) (c + 0x346)) = 0;
  *((u8 *) (c + 0x347)) = 1;
  *((u8 *) (c + 0x348)) = 0;
  *((int *) (c + 0x340)) = 0;
  return 1;
}
