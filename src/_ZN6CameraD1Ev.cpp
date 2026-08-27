//cpp
// @symbol _ZN6CameraD1Ev
/* D1, the complete-object destructor, generated from an empty body against a
 * shadow struct that names the real base and the real member types. */
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
