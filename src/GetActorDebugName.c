extern int ACTOR_DEBUG_NAMES[];
extern int data_020914a0;
int GetActorDebugName(unsigned int i) {
    return (i < 0x188) ? ACTOR_DEBUG_NAMES[i] : (int)&data_020914a0;
}
