extern signed char NUM_VS_STARS_OBTAINED_PLAYER[];
void GiveVsStars(int idx, int delta){
  NUM_VS_STARS_OBTAINED_PLAYER[idx] += delta;
}
