extern int func_0206cee0(void *);
/* Spin until func_0206cee0 takes the cartridge lock; c receives the saved
   IRQ/FIQ and lock flags that func_0206ce90 restores. */
void func_0206cec0(void *c)
{
    while (func_0206cee0(c) == 0)
        ;
}
