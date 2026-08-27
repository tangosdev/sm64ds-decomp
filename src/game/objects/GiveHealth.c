extern short data_02092144[];

void GiveHealth(int idx, int amt)
{
    data_02092144[idx] += amt;
    if (data_02092144[idx] > 0x880) { data_02092144[idx] = 0x880; return; }
    if (data_02092144[idx] < 0) { data_02092144[idx] = 0; }
}
