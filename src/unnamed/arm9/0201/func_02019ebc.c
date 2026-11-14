/* CARD_PullOut.cpp's CardCallback: nonzero lets the handler terminate
   (func_020610fc); zero returns. */
extern void func_02061128(int (*callback)(void));
extern int func_02019f10(void);

/* Register func_02019f10 as the card pull-out callback (func_02061128),
   during boot (NitroStartUp). func_02019f10 starts the thread that shuts
   the game down. */
void func_02019ebc(void) {
    func_02061128(func_02019f10);
}
