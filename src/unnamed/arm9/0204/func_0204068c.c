extern unsigned WM_GetSharedDataAddress(char *dsInfo, void *receiveData, int aid);

extern void *data_020a0f74;
extern void *data_020a0f80;

/* daPly_c aid's packet in the last shared-data step: WM_GetSharedDataAddress
   on the sharing info data_020a0f74 and received set data_020a0f80.
   func_0203ea5c unpacks each player's packet through it. */
int func_0204068c(int aid) {
    return WM_GetSharedDataAddress(data_020a0f74, data_020a0f80, aid);
}
