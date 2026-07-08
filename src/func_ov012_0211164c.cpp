//cpp
extern "C" {
int _ZN5Model8LoadFileER13SharedFilePtr(void*);
int _ZN12MeshCollider8LoadFileER13SharedFilePtr(void*);
int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*,int,int,int);
int _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(int,void*);
int _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void*,void*,int,int,unsigned);
int _ZN8Platform21UpdateModelPosAndRotYEv(void*);
int _ZN8Platform19UpdateClsnPosAndRotEv(void*);
int _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*,int,void*,int,int,void*);
int _ZN16MeshColliderBase6EnableEP5Actor(void*,void*);
extern int data_ov012_021124d0[];
extern int data_ov012_021124c8[];
extern int data_ov012_02111c24[];
extern int data_ov012_02111c90[];
extern int SAVE_DATA[];
int func_ov012_0211164c(char* c){
  _ZN5Model8LoadFileER13SharedFilePtr(data_ov012_021124d0);
  _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov012_021124c8);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, data_ov012_021124d0[1], 1, 0x14);
  _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(data_ov012_021124d0[1], data_ov012_02111c24);
  _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(c+0x320, data_ov012_02111c24, 0, 0x1000, 0);
  _ZN8Platform21UpdateModelPosAndRotYEv(c);
  _ZN8Platform19UpdateClsnPosAndRotEv(c);
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(c+0x124, data_ov012_021124c8[1], c+0x2ec, 0x1000, *(short*)(c+0x8e), data_ov012_02111c90);
  _ZN16MeshColliderBase6EnableEP5Actor(c+0x124, c);
  int v = *(int*)(c+0x60) - 0x4b0000;
  *(int*)(c+0x334) = v;
  if(SAVE_DATA[2] & 0x80000){
    *(int*)(c+0x60) = *(int*)(c+0x334);
  }
  return 1;
}
}
