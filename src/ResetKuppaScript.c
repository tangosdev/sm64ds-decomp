extern int KS_PLAYER_IDS[];
extern unsigned char NUM_PLAYERS[];
extern unsigned char KS_NUM_PLAYERS[];
extern int CUTSCENE_OBJECT_IDS[];
extern int SAVED_KUPPA_SCRIPT[];
extern int RUNNING_KUPPA_SCRIPT[];

void ResetKuppaScript(void)
{
    int i;
    int j;

    for (i = 0; i < 4; i++) KS_PLAYER_IDS[i] = 0;
    KS_NUM_PLAYERS[0] = NUM_PLAYERS[0];
    for (j = 0; j < 0x10; j++) CUTSCENE_OBJECT_IDS[j] = 0;
    RUNNING_KUPPA_SCRIPT[0] = SAVED_KUPPA_SCRIPT[0];
}
