extern int IsMinigameActorID(unsigned int id);

extern int OVERLAY_2_ID;
extern int OVERLAY_3_ID;
extern int OVERLAY_5_ID;
extern int OVERLAY_6_ID;
extern int OVERLAY_7_ID;

int GetSceneOverlayID(int id)
{
    if (IsMinigameActorID(id))
        return (int)&OVERLAY_6_ID;

    switch (id) {
    case 3:
    case 6:
    case 7:
        return (int)&OVERLAY_2_ID;
    case 5:
        return (int)&OVERLAY_5_ID;
    case 1:
        return (int)&OVERLAY_7_ID;
    case 2:
    case 4:
    case 8:
        return (int)&OVERLAY_3_ID;
    case 0x168:
        break;
    }
    return -1;
}
