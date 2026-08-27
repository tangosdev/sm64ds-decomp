//cpp
// @symbol _ZN11SnowmanBody8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SnowmanBody.h"
/* dCc_c comes from the real dCcAc_c chain now that SnowmanBody.h types
   mdCcAc_c; the ad-hoc redeclaration that used to stand in for it ICEd
   mwccarm (CClass.c:3328) once the real class was visible. Clear and Update
   are non-virtual there, so the direct bl is unchanged. */
int SnowmanBody::Behavior()
{
    CallStateBehavior();
    ((dCc_c*)((char *)&mCylinder))->Clear();
    ((dCc_c*)((char *)&mCylinder))->Update();
    UpdateModel();
    return 1;
}
