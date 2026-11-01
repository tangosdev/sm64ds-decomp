extern void func_02073300(int a, unsigned int b, unsigned int c, int dtor);
extern void func_0203cbc0(void *ptr);

/* The Itanium C++ ABI's __cxa_vec_delete(array, element_size,
   padding_size, destructor). The name is the compiler's: mwccarm compiles
   delete[] of a class with a destructor to __cxa_vec_delete(p,
   sizeof(T), 8, T's destructor), the call d_a_ply_methods.cpp and
   d_s_stage_methods.cpp spell out. A null array does nothing. With a
   destructor, func_02073300 destroys the element count stored a word
   below the array (*(a - 4)). That is the ABI's __cxa_vec_dtor step, but
   the compiler never emits that name, so func_02073300 keeps its address.
   The block, starting padding bytes before the array, then goes to
   func_0203cbc0 (operator delete). */
void __cxa_vec_delete(int a, int b, int c, int d) {
    if (a == 0)
        return;
    if (d != 0)
        func_02073300(a, *(int *)(a - 4), b, d);
    func_0203cbc0((void *)(a - c));
}
