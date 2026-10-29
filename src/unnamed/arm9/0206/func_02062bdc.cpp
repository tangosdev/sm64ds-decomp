//cpp
struct IRQ { static unsigned int Disable(); static void Restore(unsigned int); };
extern "C" void func_02062990(void *base, unsigned int a, unsigned int b);
extern "C" void func_020627e8(void *base, int x);

/* NitroSDK data sharing, the parent's port receive callback: WM_StartDataSharing
 * (func_020631dc) registers it with WM_SetPortCallback (func_02061c88) when this
 * station is the parent (status->aid == 0). PORT_RECV (0x15) stores the
 * child's data (func_02062990, WmDataSharingReceiveData) and tries to send the
 * next set (func_020627e8); CONNECTED (7) just tries the send; DISCONNECTED (9)
 * stops waiting for that child in the current set (and the next one in double
 * mode) before sending. A failed receive puts the sharing into state 3, the
 * error state. */

/* NitroSDK WMPortRecvCallback, the leading fields: the offsets agree with every
 * port receive callback in this cluster (func_02062aa4, func_020417d0). */
struct WMPortRecvCallback {
    unsigned short apiid;        /* 0x00 */
    unsigned short errcode;      /* 0x02 */
    unsigned short state;        /* 0x04 -- 7 CONNECTED, 9 DISCONNECTED, 0x15 PORT_RECV */
    unsigned short port;         /* 0x06 */
    void *recvBuf;               /* 0x08 */
    unsigned short *data;        /* 0x0c */
    unsigned short length;       /* 0x10 -- bytes at data */
    unsigned short aid;          /* 0x12 -- the sender */
    unsigned char macAddress[6]; /* 0x14 */
    unsigned short seqNo;        /* 0x1a */
    void *arg;                   /* 0x1c -- WM_SetPortCallback's argument */
};

/* NitroSDK WMDataSet / WMDataSharingInfo, as func_02062aa4 spells them. */
struct WMDataSet {
    unsigned short aidHeader;      /* 0x00 -- members whose data the set holds */
    unsigned short receivedBitmap; /* 0x02 */
    unsigned short data[0x7e];     /* 0x04 */
};

struct WMDataSharingInfo {
    WMDataSet ds[4];              /* 0x000 */
    unsigned short seqNum[4];     /* 0x400 -- each set's MP sequence number */
    unsigned short writeIndex;    /* 0x408 */
    unsigned short sendIndex;     /* 0x40a */
    unsigned short readIndex;     /* 0x40c */
    unsigned short aidBitmap;     /* 0x40e -- the members, this station included */
    unsigned short dataLength;    /* 0x410 -- per member */
    unsigned short stationNumber; /* 0x412 */
    unsigned short dataSetLength; /* 0x414 -- dataLength * stationNumber + 4 */
    unsigned short port;          /* 0x416 */
    unsigned short doubleMode;    /* 0x418 */
    unsigned short currentSeqNum; /* 0x41a */
    unsigned short state;         /* 0x41c -- 1 running, 2 retry the send, 3 error */
    unsigned short reserved[1];   /* 0x41e */
};

extern "C" void func_02062bdc(WMPortRecvCallback *m) {
    WMDataSharingInfo *base = (WMDataSharingInfo *)m->arg;
    if (m->errcode == 0) {
        switch (m->state) {
        case 0x15:
            func_02062990(base, m->aid, (unsigned int)m->data);
            func_020627e8(base, 0);
            return;
        case 7:
            func_020627e8(base, 0);
            return;
        case 9: {
            unsigned short mask = (unsigned short)(1 << m->aid);
            unsigned int saved = IRQ::Disable();
            unsigned short b408 = base->writeIndex;
            base->ds[b408].aidHeader &= ~mask;
            if (base->doubleMode == 1) {
                int n = (base->writeIndex + 1) % 4;
                base->ds[n].aidHeader &= ~mask;
            }
            IRQ::Restore(saved);
            func_020627e8(base, 0);
            if (base->doubleMode == 1)
                func_020627e8(base, 0);
            return;
        }
        default:
            return;
        }
    }
    base->state = 3;
}
