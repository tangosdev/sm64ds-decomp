int WBT_AidbitmapToAid(int x) {
    int i;
    for (i = 0; i < 16; i++)
        if ((x >> i) & 1)
            return i;
    return -1;
}
