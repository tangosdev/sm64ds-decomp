struct Pair { unsigned short v; unsigned short pad; };
extern struct Pair MSG_ID_CHAR_MAP[];
extern struct Pair data_0208eeee[];

int ObjectMessageIDToActualMessageID(int id) {
    int i;
    for (i = 0x61; i > 0; i--) {
        if (MSG_ID_CHAR_MAP[i].v > id && MSG_ID_CHAR_MAP[i - 1].v <= id) {
            return (unsigned short)((id - MSG_ID_CHAR_MAP[i - 1].v) + data_0208eeee[i - 1].v);
        }
    }
    return id;
}
