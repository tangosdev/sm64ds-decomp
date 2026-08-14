//cpp
#include "common.h"

/*
 * MSL array-destroy helper. The symbol table had carved the catch landing
 * pad off as func_0207335c (0x4c). Combined try/catch is byte-identical
 * over 0x02073300..0x020733a8 on mwccarm 2004/b56, so the pair is merged
 * (notes/mwccarm-codegen.md 9a(2); same as func_02072168 absorbing
 * func_020729e8).
 */
extern "C" void func_020731fc(void);
extern "C" void func_020717c0(void);

typedef void (*dtor_t)(void *);

extern "C" void func_02073300(void *block, unsigned int n, unsigned int size, dtor_t dtor)
{
    if (dtor) {
        char *p = (char *)block + n * size;
        try {
            if (n) {
                do {
                    p -= size;
                    dtor(p);
                } while (--n);
            }
        } catch (...) {
            try {
                if (--n) {
                    do {
                        p -= size;
                        dtor(p);
                    } while (--n);
                }
            } catch (...) {
                func_020731fc();
            }
            func_020717c0();
        }
    }
}
