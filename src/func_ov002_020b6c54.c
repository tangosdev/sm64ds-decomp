#pragma opt_strength_reduction off
struct Vec3 { int x, y, z; };
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(char* thiz, void* file, int a, int b);
extern int func_ov002_020b6a80(char* c);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(char* c);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(char* thiz, void* kcl, char* mtx, int fix, short s, char* block);
extern char* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int id, unsigned int p, struct Vec3* pos, void* rot, int a, int b);
int func_ov002_020b6c54(char* sb, char** arg, unsigned int actorID){
  int i;
  void* m;
  m = _ZN5Model8LoadFileER13SharedFilePtr(arg[0]);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(sb+0xd4, m, 1, -1);
  func_ov002_020b6a80(sb);
  _ZN8Platform19UpdateClsnPosAndRotEv(sb);
  m = _ZN12MeshCollider8LoadFileER13SharedFilePtr(arg[1]);
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(sb+0x124, m, sb+0x2ec, 0x199, *(short*)(sb+0x8e), arg[2]);
  for(i = 0; i < 4; i++){
    char* sp;
    *(int*)(sb + (int)((unsigned long long)i & 0xFFFFFFFFFFFFFFFFull)*4 + 0x320) = 0;
    sp = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(actorID, 0, (struct Vec3*)(sb+0x5c), 0, *(signed char*)(sb+0xcc), -1);
    if(sp != 0) *(int*)(sb + i*4 + 0x320) = *(int*)(sp+4);
  }
  return 1;
}
