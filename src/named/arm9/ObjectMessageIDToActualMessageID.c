struct Pair { unsigned short v; unsigned short pad; };
extern struct Pair data_0208eeec[];
extern struct Pair data_0208eeee[];

int ObjectMessageIDToActualMessageID(int id) {
    int i;
    for (i = 0x61; i > 0; i--) {
        if (data_0208eeec[i].v > id && data_0208eeec[i - 1].v <= id) {
            return (unsigned short)((id - data_0208eeec[i - 1].v) + data_0208eeee[i - 1].v);
        }
    }
    return id;
}
