//cpp
#include "MWCPlusLib.h"

void func_02073300(void *block, unsigned int n, unsigned int size, dtor_t dtor);

/* The Itanium C++ ABI's __cxa_vec_delete(array, element_size,
   padding_size, destructor). The name is the compiler's: mwccarm compiles
   delete[] of a class with a destructor to __cxa_vec_delete(p,
   sizeof(T), 8, T's destructor), the call d_a_ply_methods.cpp and
   d_s_stage_methods.cpp spell out. A null array does nothing. With a
   destructor, func_02073300 destroys the element count stored a word
   below the array (*(a - 4)). That is the ABI's __cxa_vec_dtor step, but
   the compiler never emits that name, so func_02073300 keeps its address.
   The block, starting padding bytes before the array, then goes to
   operator delete[]. */
extern "C" void __cxa_vec_delete(void *a, int b, int c, void (*d)(void *)) {
    if (a == 0)
        return;
    if (d != 0)
        func_02073300(a, ((int *)a)[-1], b, d);
    ::operator delete[]((char *)a - c);
}
