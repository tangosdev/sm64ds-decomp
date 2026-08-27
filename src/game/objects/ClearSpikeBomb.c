extern int data_0209f3a4[];
void ClearSpikeBomb(int idx)
{
    if (idx < 0) return;
    if (idx < 8) data_0209f3a4[idx] = 0;
}
