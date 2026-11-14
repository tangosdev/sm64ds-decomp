/* CARD_PullOut.cpp's CardCallback: nonzero lets the handler terminate
   (func_020610fc); zero returns. */
extern void _Z13func_02061128PFivE(int (*callback)(void));
extern int _Z13func_02019f10v(void);

/* Register func_02019f10 as the card pull-out callback (func_02061128),
   during boot (NitroStartUp). func_02019f10 starts the thread that shuts
   the game down. */
void func_02019ebc(void) {
    _Z13func_02061128PFivE(_Z13func_02019f10v);
}
