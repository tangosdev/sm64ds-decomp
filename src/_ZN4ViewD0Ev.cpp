//cpp
#include "View.h"

/* Not called. Forces the deleting D0 copy of the inline destructor. */
void View_EmitDeletingDestructor(View *view)
{
    delete view;
}
