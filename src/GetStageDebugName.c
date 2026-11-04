/* The debug name of the current stage, for the crash screen's
 * "StageNo %2d:%s" line (ShowCrashScreen.cpp). STAGE_DEBUG_NAMES holds the
 * 0x34 stage names ("Test Map", "Main Castle Map", "Castle 1F", ...
 * "Vs Main"), and an out-of-range id gets "*NOT STAGE*" (data_02092654).
 * The stage twin of GetActorDebugName. */
extern signed char gCurrentSublevel; /* the current level id */
extern char *STAGE_DEBUG_NAMES[];
extern char data_02092654[];
char *GetStageDebugName(void) {
    signed char i = gCurrentSublevel;
    return (unsigned)i < 0x34 ? STAGE_DEBUG_NAMES[i] : data_02092654;
}
