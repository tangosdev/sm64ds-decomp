//cpp
// @symbol Sound_PlayBank3_02012694
/* recovered: shared common types */
#include "common.h"
extern "C" {

void _ZN5Sound4PlayEjjRK7Vector3(unsigned int bank, unsigned int id, const Vector3 *v);
void Sound_PlayBank3_02012694(unsigned int id, const Vector3 *v)
{
    _ZN5Sound4PlayEjjRK7Vector3(3, id, v);
}
}