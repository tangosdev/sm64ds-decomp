extern int ACTOR_DEBUG_NAMES[];
char data_020914a0[8] = "*ERROR*";
int GetActorDebugName(unsigned int i) {
    return (i < 0x188) ? ACTOR_DEBUG_NAMES[i] : (int)&data_020914a0;
}
