
extern signed char LEVEL_ID;
extern int IsCannonOpenInLevel(signed char level);
void IsCannonOpenInCurLevel(void) {
    (void)IsCannonOpenInLevel(LEVEL_ID);
}