// @symbol func_ov084_0212e998
// @emits FirePiranhaPlantBig_OnAimedAtWithEgg
/* recovered: renamed to Class_Method */
/* daFPkn_c::OnAimedAtWithEgg - recovered from vtable slot identity */
int FirePiranhaPlantBig_OnAimedAtWithEgg(void* c) {
    int flags = *(int*)((char*)c + 0x18c);
    if (flags & 1) {
        return *(int*)((char*)c + 0x204) * 100;
    }
    int a = *(int*)((char*)c + 0x204);
    int b = *(int*)((char*)c + 0x20c);
    int r0 = a * b >> 1;
    int a100 = a * 100;
    if (r0 <= a100) r0 = a100;
    return r0;
}
