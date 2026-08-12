// @symbol func_ov091_021335d4
// recovered name: daObjPile_c_OnHitByMegaChar
/* recovered: renamed to Class_Method */
/* daObjPile_c::OnHitByMegaChar - verified 2004/b56 byte-match (ov091), strict-reloc */
extern void _ZN6Player16IncMegaKillCountEv(void*);
extern void _ZN5Actor8PoofDustEv(void*);
extern int func_02012694(int a, void* b);
extern int func_ov091_021334b8(void* self, int b);
void func_ov091_021335d4(char* self, void* arg)
{
    if (*(unsigned char*)(self + 0x320)) return;
    if (arg == 0) return;
    if (*(unsigned char*)(self + 0x31e) == 0) return;
    if (*(unsigned char*)(self + 0x31f)) return;
    _ZN6Player16IncMegaKillCountEv(arg);
    _ZN5Actor8PoofDustEv(self);
    func_02012694(0x62, self + 0x74);
    func_ov091_021334b8(self, 1);
}
