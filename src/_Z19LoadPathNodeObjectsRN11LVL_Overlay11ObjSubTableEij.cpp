//cpp
/* LoadPathNodeObjects(LVL_Overlay::ObjSubTable&, int, unsigned) @ 0x20fe6b8 (ov002)
 * -- veneer: ldr r0,[r0,#4]; b func_0203accc. */
extern "C" {
typedef unsigned int u32;
extern void func_0203accc(void*, int, u32);
void _Z19LoadPathNodeObjectsRN11LVL_Overlay11ObjSubTableEij(void* a, int b, u32 c) {
    func_0203accc(*(void**)((char*)a + 4), b, c);
}
}
