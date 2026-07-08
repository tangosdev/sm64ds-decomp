extern int func_0205f9ac(int a, int b);
extern unsigned char BACKLIGHT_ENABLED;
void TurnBacklightOff(void) {
    if (func_0205f9ac(2, 0) == 0) {
        BACKLIGHT_ENABLED = 0;
    }
}
