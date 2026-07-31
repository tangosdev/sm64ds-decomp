//cpp
// @symbol func_ov098_02137d40
// @emits ArrowSignRight_OnAttacked1
/* recovered: renamed to Class_Method */
/* daObjYajirusi_c::OnAttacked1 - recovered from vtable slot identity */
struct VT{ int (*f[64])(void*); };
struct O{ VT* vt; };
extern "C" void ArrowSignRight_OnAttacked1(O* c, char* o){
    unsigned r = (*(unsigned short*)(o+0xc) == 0xce) ? 1u : 0u;
    if(r == 0) return;
    c->vt->f[0x7c/4](c);
}
