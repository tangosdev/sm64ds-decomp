/* func_0205d94c at 0x0205d94c
 *
 * Tail-call thunk: prepends &data_020a8074 and forwards to func_0205c9b8.
 */
extern unsigned int func_0205c9b8(char *thiz, int arg1, unsigned int arg2);

extern int data_020a8074;

int func_0205d94c(int a, int b) {
    return func_0205c9b8((char *)&data_020a8074, a, b);
}
