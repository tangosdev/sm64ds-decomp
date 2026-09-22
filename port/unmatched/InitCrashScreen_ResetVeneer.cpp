/* HOST COPY of src/InitCrashScreen.c -- brings up the developer crash screen and
 * begins by calling Scene::ResetHardwareRegisters().
 *
 * THE CALLING-CONVENTION SEAM (and why this one is defensive, not a live crash):
 *
 * Scene::ResetHardwareRegisters is a member, so its host bridge
 * (hal/cxx_aliases.cpp) is `_ZN8dScene_c22ResetHardwareRegistersEv(void *s)` and
 * reads `s` off the stack. InitCrashScreen is a plain C function with no Scene
 * of its own; the matched C calls the method with no argument
 * (`_ZN8dScene_c22ResetHardwareRegistersEv(void)`), so the host reads an unwritten
 * stack slot as `s`.
 *
 * Unlike the other rows in this class, THE CALLEE NEVER DEREFERENCES `this`:
 * Scene::ResetHardwareRegisters (0x0202e7d4) overwrites r0 with a literal load
 * on its first two instructions and only ever touches I/O registers and one
 * global (`data_02092668`). The DS itself calls it with a bare `bl` and no r0
 * setup. So the garbage `s` is never used and cannot fault -- this is a
 * defensive fix, not a repair of a live crash.
 *
 * Still, reading an uninitialised stack slot into a pointer is undefined even
 * when the value is unused, so THE FIX writes that slot deterministically by
 * passing 0. Nothing downstream reads it.
 *
 * src/InitCrashScreen.c is dropped from slice_gate10.txt in favour of this file;
 * the byte-locked source is unchanged.
 */

extern "C" {
extern void _ZN8dScene_c22ResetHardwareRegistersEv(void *s);  /* member: this off stack */
extern void _ZN2GX12SetBankForBGEt(unsigned short b);
extern void _ZN2GX11LoadBG1CharEPKvjj(const void *p, unsigned int a, unsigned int b);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, unsigned int a, unsigned int b);
extern void ResetCrashScreen(void);
extern void ShowCrashScreen(void);
extern void _ZN2GX6DispOnEv(void);
extern char data_0208a178;
extern char data_0208c178;

// PORT_HOST_ABI: implicit-register-arg (ResetHardwareRegisters ignores its receiver; defensive 0 fills the cdecl slot).
void InitCrashScreen(void)
{
    _ZN8dScene_c22ResetHardwareRegistersEv(0);   /* <-- unused by the callee; slot made definite */
    *(volatile short *)0x4000050 = 0;
    *(volatile short *)0x4001050 = 0;
    _ZN2GX12SetBankForBGEt(8);
    *(volatile int *)0x4000000 = (*(volatile int *)0x4000000 & ~0x1f00) | 0x200;
    *(volatile short *)0x400000a = (unsigned short)((*(volatile unsigned short *)0x400000a & 0x43) | 0x1e10);
    _ZN2GX11LoadBG1CharEPKvjj(&data_0208a178, 0, 0x2000);
    *(volatile short *)0x5000000 = 0x7c00;
    _ZN2GX10LoadBGPlttEPKvjj(&data_0208c178, 0x1a0, 0x60);
    ResetCrashScreen();
    ShowCrashScreen();
    _ZN2GX6DispOnEv();
    *(volatile int *)0x4001000 = *(volatile int *)0x4001000 & ~0x10000;
}
}
