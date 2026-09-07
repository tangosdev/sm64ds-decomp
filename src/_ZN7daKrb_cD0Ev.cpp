//cpp
// @symbol _ZN7daKrb_cD0Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * The five typed sub-objects in include/daKrb_c.h (dCcAc_c, dBgCh_Actr, ModelAnim,
 * ShadowModel, MaterialChanger) are torn down in reverse declaration order and then
 * the dCapEnemy_c base chain runs, all of it compiler-generated.
 */
#include "daKrb_c.h"

daKrb_c::~daKrb_c()
{
}
