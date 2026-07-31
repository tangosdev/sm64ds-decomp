// @symbol func_ov006_021291f8
// @emits dScMgSnowball_c_CleanupResources
/* recovered: renamed to Class_Method */
/* dScMgSnowball_c::CleanupResources - recovered from vtable slot identity */
extern void Ov004_Deallocate(void *p);
int dScMgSnowball_c_CleanupResources(void *c) {
    Ov004_Deallocate(*(void **)((char *)c + 0xabf4));
    return 1;
}
