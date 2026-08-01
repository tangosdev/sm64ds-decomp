// @symbol _ZN10ModelAnim24CopyERKS_Pcj
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_Animation.h"
#include "decl_ModelAnim.h"
/* recovered: named members + shared header */
#include "ModelAnim2.h"
/* _ZN10ModelAnim24CopyERKS_Pcj at 0x02016254
 * ModelAnim2::Copy(const ModelAnim2& src, char* newFile, u32 newUnk64):
 *   copy ModelAnim base (r0=this, r1=src, r2=newFile passed through),
 *   then copy the otherAnim Animation subobject at +0x68 from src's +0x50,
 *   then set unk64 at +0x64 from newUnk64 (if nonzero) or from src->unk64.
 */

void _ZN10ModelAnim24CopyERKS_Pcj(struct ModelAnim2 *self, const void *src, void *newFile, unsigned int newUnk64) {
    _ZN9ModelAnim4CopyERKS_Pc(((void *)self), src, newFile);
    _ZN9Animation4CopyERKS_((char *)((void *)self) + 0x68, (const char *)src + 0x50);
    if (newUnk64)
        *(unsigned int *)((char *)&self->unk_064) = newUnk64;
    else
        *(unsigned int *)((char *)&self->unk_064) = *(const unsigned int *)((const char *)src + 0x64);
}
