
extern signed char LEVEL_ID;
extern void OpenCannonInLevel(signed char level);
void OpenCannonInCurLevel(void) {
    OpenCannonInLevel(LEVEL_ID);
}