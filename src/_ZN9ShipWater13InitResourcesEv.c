extern int _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*,int,int,int);
extern int _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(int,void*);
extern int _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void*,void*,int,int,unsigned);
extern int _ZN8Platform21UpdateModelPosAndRotYEv(void*);
extern int _ZN8Platform19UpdateClsnPosAndRotEv(void*);
extern int _ZN12MeshCollider8LoadFileER13SharedFilePtr(void*);
extern int _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*,int,void*,int,int,void*);
extern int _ZN16MeshColliderBase6EnableEP5Actor(void*,void*);
extern int data_ov017_02111c88[];
extern int data_ov056_02111a60[];
extern int _ZN16FloatingFloorBfsD0Ev[];
extern int data_ov055_02111a94[];
int _ZN9ShipWater13InitResourcesEv(char* c){
  int m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov017_02111c88);
  _ZN9ModelBase7SetFileEP8BMD_Fileii((char*)c+0xd4, m, 1, -1);
  _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(data_ov017_02111c88[1], data_ov056_02111a60);
  _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj((char*)c+0x320, data_ov056_02111a60, 0, 0x1000, 0);
  _ZN8Platform21UpdateModelPosAndRotYEv(c);
  _ZN8Platform19UpdateClsnPosAndRotEv(c);
  int k = _ZN12MeshCollider8LoadFileER13SharedFilePtr(_ZN16FloatingFloorBfsD0Ev);
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block((char*)c+0x124, k, (char*)c+0x2ec, 0x1000, *(short*)((char*)c+0x8e), (void*)data_ov055_02111a94);
  _ZN16MeshColliderBase6EnableEP5Actor((char*)c+0x124, c);
  *(int*)((char*)c+0x334)=*(int*)((char*)c+0x60);
  return 1;
}
