extern void *data_0209f3a4[];

int AddSpikeBomb(void *p) {
    int i;
    for (i = 0; i < 8; i++) {
        if (data_0209f3a4[i] == 0) {
            data_0209f3a4[i] = p;
            return i;
        }
    }
    return -1;
}
