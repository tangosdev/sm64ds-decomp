// @symbol func_ov091_021338ac
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daObjPile_c.h"
// recovered name: daObjPile_c_InitResources
/* recovered: renamed to Class_Method */
/* daObjPile_c::InitResources - verified 2004/b56 byte-match (ov091), strict-reloc */
typedef int Fix12;
extern int _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*,int,int,int);
extern int _ZN8Platform21UpdateModelPosAndRotYEv(void*);
extern int _ZN8Platform19UpdateClsnPosAndRotEv(void*);
extern int _ZN12MeshCollider8LoadFileER13SharedFilePtr(void*);
extern int _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*,int,void*,Fix12,short,void*);
int func_ov091_021338ac(char* c){
    struct daObjPile_c *self = (struct daObjPile_c *)(void *)c;
  int m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov091_02135654);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, m, 1, -1);
  _ZN8Platform21UpdateModelPosAndRotYEv(c);
  _ZN8Platform19UpdateClsnPosAndRotEv(c);
  int k = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov091_0213564c);
  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(c+0x124, k, c+0x2ec, 0x199, self->unk_08e, (void*)data_ov002_0210d874);
  self->unk_31e = 3;
  return 1;
}
