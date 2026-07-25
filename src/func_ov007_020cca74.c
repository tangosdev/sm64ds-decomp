// Long-branch veneer. #pragma long_calls makes mwccarm emit the pooled
// `ldr ip,[pc]; bx ip` absolute tail-call the ROM uses to reach the target.
#pragma long_calls on
extern int func_0201816c(void);

int func_ov007_020cca74(void)
{
    return func_0201816c();
}
