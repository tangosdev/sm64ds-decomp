
#include "types.h"

extern int data_0209cddc;
extern void func_02013f28(void);
/* The exception handler main installs (OS_SetUserExceptionHandler), so it
   has the OSExceptionHandler shape; OS_Terminate calls it too, as (0, NULL). Keep
   the context in data_0209cddc, then stop for good in the fatal-error spin
   (func_02013f28). arg is unused. */
void func_0201a03c(u32 context, void *arg) {
    data_0209cddc = context;
    func_02013f28();
}