//cpp
// @symbol _ZN7daDgr_c6RenderEv
/* daDgr_c::Render -- vtable slot 9. Real C++ method over the shared header;
   the Model sub-object at +0xd4 is rendered through its own vtable (mwccarm
   does not devirtualise an embedded member's virtual call). */
#include "daDgr_c.h"
s32 daDgr_c::Render() { mModel.Render(0); return 1; }
