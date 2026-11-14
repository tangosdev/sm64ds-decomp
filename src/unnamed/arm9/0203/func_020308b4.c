/* Arm the link handshake: clear the passed latch data_020a0f30
   (func_0203d854), then raise flag 0x1000 in this console's input record
   (func_0203d868), which every console sees in its copy of the four
   records. func_020308a8 polls it. */
extern void func_0203d854(void);
extern void func_0203d868(void);

void func_020308b4(void) {
    func_0203d854();
    func_0203d868();
}
