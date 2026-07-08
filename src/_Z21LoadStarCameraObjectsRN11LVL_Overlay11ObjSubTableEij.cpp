//cpp
namespace LVL_Overlay {
struct ObjSubTable { int pad0; int* p; };
}
extern int STAR_CAMERA_SETTINGS;
void LoadStarCameraObjects(LVL_Overlay::ObjSubTable& t, int a, unsigned int b) {
    STAR_CAMERA_SETTINGS = *t.p;
}
