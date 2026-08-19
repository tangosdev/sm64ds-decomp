/* dBgW::dBgW() at 0x0203969c
 * Base constructor. Runs the base subobject constructor (func_02039624), then
 * installs the dBgW vtable. Returns this.
 */

extern int _ZTV4dBgW[];   // vtable
extern void func_02039624(void* self);  // base subobject constructor

void* _ZN4dBgWC2Ev(void* self)
{
    *(int*)self = (int)_ZTV4dBgW; // set vptr
    func_02039624(self);
    return self;
}
