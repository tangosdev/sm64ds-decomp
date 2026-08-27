int IsMinigameActorID(unsigned int id)
{
    if (id >= 0x169 && id <= 0x186)
        return 1;
    return 0;
}
