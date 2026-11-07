extern int data_020a0f94;
extern int data_020a0f5c;
extern int Wireless_Reset(void);

/* Shut the link down by its status data_020a0f94: while starting (2),
   clear the target mode data_020a0f5c so the callback chain winds down;
   while running (3 or 4), clear it and reset (Wireless_Reset). Idle or
   error does nothing. func_0203db64 calls it when a handshake times out,
   func_0203df40 when the link mode drops to 0, and the card pull-out
   thread func_02019f64 until the status reads 0 or 1. */
void func_02040724(void)
{
    switch (data_020a0f94) {
    case 0:
        break;
    case 1:
        break;
    case 2:
        data_020a0f5c = 0;
        break;
    case 3:
    case 4:
        data_020a0f5c = 0;
        Wireless_Reset();
        break;
    }
}
