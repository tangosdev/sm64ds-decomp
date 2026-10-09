extern int func_0205f9ac(int a, int b);
extern unsigned char data_0208ee3c;
void TurnBacklightOn(void) {
    if (func_0205f9ac(2, 1) == 0) {
        data_0208ee3c = 1;
    }
}
