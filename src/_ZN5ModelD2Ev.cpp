//cpp
// @symbol _ZN5ModelD2Ev
/* The same real destructor definition emits every ABI variant; objisolate
 * retains the base-object destructor for this enrollment.
 */
#include "Model.h"

extern "C" void func_0203cbc0(void *ptr);

Model::~Model()
{
    if (transformsBuf)
        func_0203cbc0(transformsBuf);
}
