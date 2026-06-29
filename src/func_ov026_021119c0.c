extern int _ZN5Model8LoadFileER13SharedFilePtr(void *);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *, int, int, int);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void *);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *);
extern int _ZN12MeshCollider8LoadFileER13SharedFilePtr(void *);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *, int, void *, int, int, void *);
extern void _ZN16MeshColliderBase6EnableEP5Actor(void *, void *);
extern char data_ov026_02113ee4[];
extern char data_ov026_02113edc[];
extern char data_ov026_02112fd4[];
extern char data_0209caa0[];
int func_ov026_021119c0(char *c){
  void *m = (void*)_ZN5Model8LoadFileER13SharedFilePtr(data_ov026_02113ee4);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4,(int)m,1,-1);
  _ZN8Platform21UpdateModelPosAndRotYEv(c);
  _ZN8Platform19UpdateClsnPosAndRotEv(c);
  {
    void *k = (void*)_ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov026_02113edc);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        c+0x124,(int)k,c+0x2ec,0x199,*(short*)(c+0x8e),(void*)data_ov026_02112fd4);
  }
  if(*(int*)(data_0209caa0+4) & 0x204) return 0;
  _ZN16MeshColliderBase6EnableEP5Actor(c+0x124,c);
  return 1;
}
