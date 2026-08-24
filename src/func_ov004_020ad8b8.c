extern int* data_ov004_020beb68;
extern int data_0209b308[];
extern int func_ov004_020adc3c(int* c);
extern int func_02013580(int a, int b);
/* THE ARGUMENT IS THE FRAMEWORK OBJECT AND IT USED TO BE DROPPED HERE. The ROM
   at 0x020ad8c0 loads data_ov004_020beb68 into r0, branches away on null, and
   falls straight into `bl 0x20adc3c` with r0 untouched -- so the callee's `c`
   is that pointer, and func_ov004_020adc3c dereferences it (`c[8/4] >> 8`) to
   get this minigame's HUD panel index. This TU declared it `(void)` and called
   it with nothing: on ARM r0 was still live and the byte gate stayed green, on
   the host the callee read an unwritten stack slot and indexed some other
   panel. func_ov004_020ad878, the function immediately above this one in the
   ROM, is the same shape spelled correctly and is the witness for it.

   IT IS ON THE PAYOUT CHAIN TWICE. dScMgRoulette_c slot 0 seeds its +0xa8 from
   this function, and +0xa8 is both the stake basis (slot 18 sets the active
   mushroom count to min(+0xa8, 5)) and the number the round's winnings are
   added to before func_ov004_020ad79c writes the HUD. That same writer also
   early-outs when the new value equals what this function returns, so a wrong
   answer here can also swallow the HUD update outright. */
int func_ov004_020ad8b8(void){
  int* c=data_ov004_020beb68;
  if(c!=0){
    int r=func_02013580(func_ov004_020adc3c(c),0);
    if(r==0) return data_0209b308[0x14/4];
    return r;
  }
  return 0;
}
