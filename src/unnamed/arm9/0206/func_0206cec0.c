extern int _Z13func_0206cee0Pi(void *);
/* Spin until func_0206cee0 takes the cartridge lock; c receives the saved
   IRQ/FIQ and lock flags that func_0206ce90 restores. */
void func_0206cec0(void *c)
{
    while (_Z13func_0206cee0Pi(c) == 0)
        ;
}
