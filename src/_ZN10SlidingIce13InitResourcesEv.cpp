//cpp
extern "C" {
extern int _ZN5Model8LoadFileER13SharedFilePtr(void *);
extern int _ZN12MeshCollider8LoadFileER13SharedFilePtr(void *);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *, int, int, int);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void *);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *, int, void *, int, int, void *);
extern void func_020393d4(int *p, int v);
extern void _ZN16MeshColliderBase6EnableEP5Actor(void *, void *);
extern int func_ov030_02113be8[];
extern char data_ov027_02113be0[];
extern char data_ov027_02113108[];
extern int _ZN16MeshColliderBase21UpdatePosWithVelocityERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_[];
int _ZN10SlidingIce13InitResourcesEv(char *c){
  _ZN5Model8LoadFileER13SharedFilePtr(func_ov030_02113be8);
  _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov027_02113be0);
  int on = (*(unsigned short*)(c+0xc)==0x5d);
  if(on){
    if(_ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, func_ov030_02113be8[1], 1, -1)==0)
      return 0;
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        c+0x124, *(int*)(data_ov027_02113be0+4), c+0x2ec, 0x1000, *(short*)(c+0x8e), data_ov027_02113108);
    func_020393d4((int*)(c+0x124), (int)_ZN16MeshColliderBase21UpdatePosWithVelocityERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
    *(char*)(c+0x170) = 0;
    _ZN16MeshColliderBase6EnableEP5Actor(c+0x124, c);
    *(int*)(c+0x98) = 0x2d000;
    *(short*)(c+0x31e) = 0x64;
    *(short*)(c+0x94) = -0x4000;
    *(int*)(c+0x324) = *(int*)(c+0x60) - 0xc8000;
  } else {
    *(short*)(c+0x31e) = *(unsigned char*)(c+0x320) * 0x14;
    *(char*)(c+0x320) = 5;
  }
  return 1;
}
}
