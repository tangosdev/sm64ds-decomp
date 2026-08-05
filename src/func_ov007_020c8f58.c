// Long-branch veneer. #pragma long_calls makes mwccarm emit the pooled
// `ldr ip,[pc]; bx ip` absolute tail-call the ROM uses to reach the target.
#pragma long_calls on
extern int _ZN6Player17St_EndingFly_MainEv(void);

int func_ov007_020c8f58(void)
{
    return _ZN6Player17St_EndingFly_MainEv();
}
