//cpp
// @symbol func_ov006_020d5924
// @emits dScMgAmida_c_AfterCleanupResources
/* recovered: renamed to Class_Method */
/* dScMgAmida_c::AfterCleanupResources - recovered from vtable slot identity */
extern "C" { void* func_ov004_020b0840(void*, int); }
namespace Memory { void Deallocate(void*); }
extern "C" void* dScMgAmida_c_AfterCleanupResources(char* c, int r1){
  if(r1!=2) return c;
  Memory::Deallocate(*(void**)(c+0x4000+0x70c));
  Memory::Deallocate(*(void**)(c+0x4000+0x710));
  return func_ov004_020b0840(c, r1);
}
