//cpp

#include "types.h"
#include "FatalSpin.h"

extern "C" int *gSavedRegisterValues;
/* The exception handler main installs (OS_SetUserExceptionHandler), so it
   has the OSExceptionHandler shape; OS_Terminate calls it too, as (0, NULL). Keep
   the context in gSavedRegisterValues, then stop for good in the fatal-error spin
   (func_02013f28). arg is unused. */
extern "C" void func_0201a03c(u32 context, void *arg) {
    gSavedRegisterValues = (int *)context;
    FatalSpin::func_02013f28();
}
