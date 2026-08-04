//cpp
struct BCA_File;
struct ModelAnim { char pad[1]; };
extern "C" int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(ModelAnim*, BCA_File*, int, int, unsigned int);
extern int data_ov006_02141e7c[];
extern double data_ov006_0213c76c;
struct Obj { char pad[0xc]; ModelAnim anim; };
extern "C" void func_ov006_020e7f5c(Obj *o) {
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&o->anim, (BCA_File*)data_ov006_02141e7c[1], 0, 0x800, 0);
    *(double*)((char*)o + 0x210) = data_ov006_0213c76c;
}
