//cpp
namespace LVL_Overlay {
struct ObjSubTable { int pad0; int* p; };
}
extern int data_02092134;
void LoadStarCameraObjects(LVL_Overlay::ObjSubTable& t, int a, unsigned int b) {
    data_02092134 = *t.p;
}
