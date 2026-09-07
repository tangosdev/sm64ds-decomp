//cpp
#include "View.h"

/* Not called. Forces the out-of-line D1 copy of the inline destructor. */
void View_EmitDestructor(View *view)
{
    view->~View();
}
