//cpp
// @symbol _ZN6Camera11ChangeStateEPNS_5StateE
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Camera.h"

extern "C" int _ZN6Camera11ChangeStateEPNS_5StateE(struct Camera *self, void *state) {
    if ((*(unsigned int *)((unsigned char *)&self->unk_154) & 0x10) != 0)
        return 0;
    if (state != *(void **)((unsigned char *)&self->unk_138)) {
        if (*(void **)((unsigned char *)&self->unk_138) == (void *)&data_0209b0c8) {
            FUN_02029a68();
            func_020089f8(((void *)self));
        }
        *(void **)((unsigned char *)&self->unk_138) = state;
        *(unsigned char *)((unsigned char *)&self->unk_1a6) = 0;
    }
    return func_0200cae4(((void *)self));
}
