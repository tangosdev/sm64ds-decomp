extern int func_0205f9ac(int a, int b);
extern unsigned char BACKLIGHT_ENABLED;
void TurnBacklightOn(void) {
    if (func_0205f9ac(2, 1) == 0) {
        BACKLIGHT_ENABLED = 1;
    }
}
