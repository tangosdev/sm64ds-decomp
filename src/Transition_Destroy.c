// Long-branch veneer. #pragma long_calls makes mwccarm emit the pooled
// `ldr ip,[pc]; bx ip` absolute tail-call the ROM uses to reach the target.
#pragma long_calls on
extern void func_ov007_020c3d1c(void *p);

void Transition_Destroy(void *p) {
    func_ov007_020c3d1c(p);
}
