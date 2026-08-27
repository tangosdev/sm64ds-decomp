//cpp
// @symbol _ZN6CameraD0Ev
/* D0, the deleting destructor. Same shadow as the D1 file beside this one; one
 * destructor definition emits D0/D1/D2 and objisolate keeps the variant this
 * file's delinks entry names. */
#include "dBase_c.h"

struct View : dBase_c {
    /* inline on purpose: the cartridge's ~Camera stores View's vtable itself
     * rather than calling a _ZN4ViewD2Ev, so View's destructor was inline in
     * its class body and every subclass inlined it. */
    virtual ~View() {}
};

struct Camera : View {
    virtual ~Camera();
};

Camera::~Camera()
{
}
