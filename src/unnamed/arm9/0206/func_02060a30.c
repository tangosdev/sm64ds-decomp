#include "types.h"
extern int func_02060ebc(void *obj);
extern void func_02060e38(void);

typedef struct {
    void (*vtable)(void *);
} SomeObj;

extern SomeObj data_020a8780;  /* 0x020a8780 */

void func_02060a30(void) {
    SomeObj *obj = &data_020a8780;
    if (func_02060ebc(obj)) {
        obj->vtable(obj);
    }
    func_02060e38();
}
