//cpp
// @symbol func_ov002_020e3e00
/* recovered: shared common types */
#include "common.h"
//

/* decl_common.h is deliberately NOT included. This file declares its callees
   concretely -- `const int *`, `unsigned int`, `Model *` -- while the header
   declares the same symbols with generic `void*`/`int`. Under `extern "C"`
   those are not compatible redeclarations but illegal overloads, so the file
   did not compile AT ALL (errors on the MulMat3x3Mat3x3, func_02016acc and
   func_ov002_020e3e00 lines) for as long as the include was here -- which is
   why no gate ever noticed: an uncompilable file is dropped by all of them.
   Only two declarations were ever needed from the header; they are restated
   below under their real names. */

extern "C" {
    extern signed char data_0209f2f8;
    extern signed char data_02092120;
    extern int data_020a0e68[12];

    /* Were spelled `func_02111b64` / `func_02111b6c` -- invented names that
       exist in no symbols.txt anywhere in config/. ov002's own relocs record
       both sites as `kind:load` to an address claimed by 17 and 13 modules
       respectively (config/arm9/overlays/ov002/relocs.txt:6676-6677). A `load`
       cannot answer a function, which rules out ov012/ov025/ov045/ov052; of
       the data claimants that remain, ov055 alone declares BOTH addresses
       unambiguous `kind:bss`. ov017 and ov033 are clean at 0x02111b6c but
       ambiguous at 0x02111b64; every other candidate is ambiguous at both. */
    extern int data_ov055_02111b64;
    extern int data_ov055_02111b6c;

    void func_0203c178(int *dst, int a, int b, int c);
    void MulMat3x3Mat3x3(int *dst, const int *a, const int *b);
    void func_02016acc(void *obj, unsigned int mask);
    void func_02016b24(void *obj, unsigned int mask);
}


struct Model { };

extern "C" void _ZN5Model6RenderEPK7Vector3(Model *self, const Vector3 *v);
extern "C" void _ZN9ModelBase12ApplyOpacityEj(Model *self, unsigned int a, int b);

typedef struct { int m[12]; } Mat;

extern "C" void func_ov002_020e3e00(Model *obj, Vector3 *vec, unsigned int opacity) {
    int ip = (data_0209f2f8 == 0x2f) ? 1 : 0;

    if (!ip) {
        if (data_0209f2f8 != 5) return;
        if (data_02092120 != 3) return;
    }

    if (ip) {
        int ok;
        if (data_ov055_02111b6c == 0 || (data_ov055_02111b64 & 0x20000) != 0)
            ok = 1;
        else
            ok = 0;
        if (!ok) return;
    }

    Mat tmp;
    Mat *src = (Mat*)((char*)obj + 0x1c);
    tmp = *src;

    int r0;
    if (ip)
        r0 = 0;
    else
        r0 = 0x1086000;
    int t = r0 >> 3;
    t <<= 1;
    int diff = t - ((int*)src)[9];
    ((int*)src)[9] = diff;

    func_0203c178(data_020a0e68, -0x1000, 0x1000, 0x1000);
    MulMat3x3Mat3x3((int*)src, data_020a0e68, (int*)src);

    func_02016acc(obj, 0x80);
    func_02016b24(obj, 0x40);
    _ZN5Model6RenderEPK7Vector3(obj, vec);
    _ZN9ModelBase12ApplyOpacityEj(obj, opacity, 0);
    func_02016b24(obj, 0x80);
    func_02016acc(obj, 0x40);

    *src = tmp;
}
