extern int IsMinigameActorID(unsigned int id);

extern int overlay_2;
extern int overlay_3;
extern int overlay_5;
extern int overlay_6;
extern int overlay_7;

int GetSceneOverlayID(int id)
{
    if (IsMinigameActorID(id))
        return (int)&overlay_6;

    switch (id) {
    case 3:
    case 6:
    case 7:
        return (int)&overlay_2;
    case 5:
        return (int)&overlay_5;
    case 1:
        return (int)&overlay_7;
    case 2:
    case 4:
    case 8:
        return (int)&overlay_3;
    case 0x168:
        break;
    }
    return -1;
}
