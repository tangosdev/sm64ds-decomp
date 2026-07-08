extern void *BOWSER_SPIKE_BOMBS[];

int AddSpikeBomb(void *p) {
    int i;
    for (i = 0; i < 8; i++) {
        if (BOWSER_SPIKE_BOMBS[i] == 0) {
            BOWSER_SPIKE_BOMBS[i] = p;
            return i;
        }
    }
    return -1;
}
