extern int BOWSER_SPIKE_BOMBS[];
void ClearSpikeBomb(int idx)
{
    if (idx < 0) return;
    if (idx < 8) BOWSER_SPIKE_BOMBS[idx] = 0;
}
