// @symbol func_ov002_020b37ec
// @emits BigBrickBlock_OnAttacked1
/* recovered: renamed to Class_Method */
/* daObjBlockL_c::OnAttacked1 - recovered from vtable slot identity */
void BigBrickBlock_OnAttacked1(void *c) {
    unsigned short v = *(unsigned short*)((char*)c + 0xc);
    int b = (int)(v == 0x11);
    if (b) return;
    void (*fn)(void*) = (void(*)(void*))*(int*)((*(int*)c) + 0x7c);
    fn(c);
}
