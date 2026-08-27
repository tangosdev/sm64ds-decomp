#include "types.h"
/* func_02019780 at 0x02019780 */
extern void func_02058c84(void);
extern void func_0201a490(void);
extern void func_02019ebc(void);
extern void _ZN4Heap18InitializeRootHeapEv(void);
extern void _ZN4Heap9SetNodeIDEj(void *heap, u32 id);
extern void *data_020a0e9c;

void func_02019780(void)
{
    func_02058c84();
    func_0201a490();
    func_02019ebc();
    _ZN4Heap18InitializeRootHeapEv();
    _ZN4Heap9SetNodeIDEj(*(void**)&data_020a0e9c, 1);
}
