/* Color::~Color() at 0x02017574
 * Sets self->vtable to the Color vtable, delegates to its base-subobject
 * destructor (func_020177c4), and returns self (ARM C++ dtor ABI).
 */

extern int data_0208eb2c[];           /* vtable for Color */
extern int func_020177c4(int *self); /* base-subobject destructor */

int _ZN5ColorD1Ev(int *self)
{
    self[0] = (int)data_0208eb2c;  /* +0x00 vptr */
    func_020177c4(self);
    return (int)self;
}
