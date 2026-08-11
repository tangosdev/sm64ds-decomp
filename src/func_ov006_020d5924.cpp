//cpp
// @symbol func_ov006_020d5924
// recovered name: dScMgAmida_c_AfterCleanupResources
/* recovered: renamed to Class_Method */
/* dScMgAmida_c::AfterCleanupResources - recovered from vtable slot identity */
extern "C" { void* _ZN11dScMgBase_c21AfterCleanupResourcesEj(void*, int); }
namespace Memory { void Deallocate(void*); }
extern "C" void* func_ov006_020d5924(char* c, int r1){
  if(r1!=2) return c;
  Memory::Deallocate(*(void**)(c+0x4000+0x70c));
  Memory::Deallocate(*(void**)(c+0x4000+0x710));
  return _ZN11dScMgBase_c21AfterCleanupResourcesEj(c, r1);
}
