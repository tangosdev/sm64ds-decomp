//cpp
extern "C" {
struct V3 { int x, y, z; };
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(void* thiz, void* actor, V3* vec, int a, int b, unsigned int cc, unsigned int d);
extern void func_ov026_021122d4(void* c, void* p);
extern V3 data_ov026_02113f4c;
extern int data_ov026_02113f58;
}

extern "C" int func_ov026_021123c8(char* c){
    V3 vec;
    *(int*)(c + 0x314) = *(int*)(c + 8) & 0xff;
    vec = data_ov026_02113f4c;
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(c + 0x110, c, &vec, 0x64000, 0xa4000, 0x800006, 0);
    func_ov026_021122d4(c, &data_ov026_02113f58);
    return 1;
}
