extern short HEALTH_ARR[];

void GiveHealth(int idx, int amt)
{
    HEALTH_ARR[idx] += amt;
    if (HEALTH_ARR[idx] > 0x880) { HEALTH_ARR[idx] = 0x880; return; }
    if (HEALTH_ARR[idx] < 0) { HEALTH_ARR[idx] = 0; }
}
