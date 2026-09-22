/* HOST COPY of src/_ZN5Sound19LoadGroupAndSetBankEii.cpp -- Sound::LoadGroupAndSetBank.
 *
 * THE CALLING-CONVENTION SEAM:
 *
 * func_0203d7d4 takes ONE argument (its definition is `int func_0203d7d4(int arg)`
 * and its body tests r0: `cmp r0,#0; movne r0,#1; bxne lr`). In the group == 0x2f
 * branch the ROM calls func_020134d8 and then feeds ITS return value -- left live
 * in r0 -- straight into func_0203d7d4:
 *
 *     0201203c: mov  r0, r5              ; r0 = a
 *     02012040: ldr  r1, [r1]            ; r1 = data_0209b498
 *     02012044: bl   func_020134d8       ; returns in r0
 *     02012048: bl   func_0203d7d4       ; r0 still = func_020134d8's return
 *
 * The matched C spells the two calls as separate statements
 * (`func_020134d8(a, data_0209b498); func_0203d7d4();`) and declares
 * func_0203d7d4 with no parameter; it is byte-identical on ARM because r0
 * already holds func_020134d8's return when func_0203d7d4 is entered.
 *
 * On the host cdecl the zero-argument call pushes nothing; func_0203d7d4's
 * object reads `arg` from an unwritten stack slot and branches on garbage. THE
 * FIX passes the argument explicitly: the return of func_020134d8(a, data_0209b498),
 * exactly the value the ROM leaves in r0.
 *
 * src/_ZN5Sound19LoadGroupAndSetBankEii.cpp is dropped from slice_gate14.txt in
 * favour of this file; the byte-locked source is unchanged.
 */

extern "C" {
extern int func_0203d974(void);
extern int func_020134d8(int a, int b);
extern int func_0203d7d4(int arg);            /* the real one-arg shape */
extern int func_0205117c(int self);
extern void func_020510a4(int a, int b);
extern int data_0209b498;
extern unsigned char data_0208e428;
extern unsigned char data_0209b47c;
extern int data_0209b4a8;
extern int data_0209b484;
extern unsigned char data_0209b478;

// PORT_HOST_ABI: implicit-register-arg (func_0203d7d4's arg = the func_020134d8 return, rode r0 on ARM).
void _ZN5Sound19LoadGroupAndSetBankEii(int a, int b) {
    if (func_0203d974() != 0) {
        if (a != 0x2f) return;
        func_0203d7d4(func_020134d8(a, data_0209b498));   /* <-- func_020134d8's return, ROM's r0 */
        data_0208e428 = b;
        data_0209b47c = a;
        return;
    }
    if (a == 0) {
        func_020134d8(a, data_0209b498);
        data_0209b4a8 = func_0205117c(data_0209b498);
    } else if (data_0209b47c != a) {
        func_020510a4(data_0209b498, data_0209b4a8);
        func_020134d8(a, data_0209b498);
        data_0209b484 = func_0205117c(data_0209b498);
    }
    data_0208e428 = b;
    data_0209b47c = a;
    data_0209b478 = 0;
}
}
