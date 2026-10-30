/* storage 0x023A40DC runtime 0x027D6054 size 0x16C main.
   marionea ApList.c: UpdateApList — scans the four-entry AP list for a
   matching BSSID (or a free/oldest slot), then clears and repopulates the
   selected record from a received beacon frame.
   Relocs: fn_02385334 (WL_ReadByte), fn_02385354 (WL_WriteByte),
   fn_02382C80 (MIi_CpuClear16), fn_023969D8 (WSetMacAdrs1), wlMan @ 0x0380FFF4.
   Literal pool at the tail holds 0x0380FFF4. */

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern unsigned char fn_02385334(void *p);              /* WL_ReadByte */
extern int fn_02385354(void *p, int data);              /* WL_WriteByte */
extern void fn_02382C80(u16 data, void *destp, u32 size); /* MIi_CpuClear16 */
extern unsigned short *fn_023969D8(unsigned short *dst, unsigned short *src1); /* WSetMacAdrs1 */

/* Inlined by the original TU; expands to the three-halfword BSSID compare. */
#define MatchMacAdrs(a, b) ((a)[2] == (b)[2] && (a)[1] == (b)[1] && (a)[0] == (b)[0])

typedef struct {
    u16 rssi;           /* +0x00 */
    u16 channel;        /* +0x02 */
    u16 capaInfo;       /* +0x04 */
    u16 bssid[3];       /* +0x06 */
    u16 ssidLength;     /* +0x0C */
    u8 ssid[32];        /* +0x0E */
    u16 beaconInterval; /* +0x2E */
    u16 lifeTime;       /* +0x30 */
} WlApElement;          /* size 0x32 */

typedef struct {
    char _pad[0x200];
    WlApElement ApList[4]; /* +0x200 */
} WlMan;

#define wlMan (*(WlMan **)0x0380FFF4)

typedef struct {
    u16 _a[4];
} FIRM_HEADER;

typedef union {
    struct {
        u16 _p[5];
        u16 rsv_RSSI;   /* +0x12 */
    } Rx;
} MAC_HEADER;

typedef struct {
    u16 FrameCtrl;      /* +0x00 */
    u16 Duration;       /* +0x02 */
    u16 DA[3];          /* +0x04 */
    u16 SA[3];          /* +0x0A */
    u16 BSSID[3];       /* +0x24 within the frame header layout used below */
    u16 SeqCtrl;
} MAN_HEADER;

typedef union {
    u16 Data;
    u16 Bit;
} CAPA_INFO;

typedef struct {
    u32 TimeStamp[2];   /* +0x00 */
    u16 BeaconInterval; /* +0x34 within the frame */
    CAPA_INFO CapaInfo; /* +0x36 */
    u8 Buf[54];
    u8 pad[2];
} BEACON_BODY;

typedef struct {
    FIRM_HEADER FirmHeader;
    MAC_HEADER MacHeader;
    MAN_HEADER Dot11Header;
    BEACON_BODY Body;
} BEACON_FRAME;

typedef struct {
    u8 ID;      /* +0x00 */
    u8 Length;  /* +0x01 */
    u8 SSID[32];/* +0x02 */
} SSID_ELEMENT;

void fn_023A40DC(u16 channel, BEACON_FRAME *pFrm, SSID_ELEMENT *pSSID)
{
    WlApElement *pApList = wlMan->ApList;
    u32 i, j;
    u32 free0, free1, life1;

    if (fn_02385334(&pSSID->Length) > 0x20)
        return;

    free0 = 4;
    free1 = 4;
    life1 = 1024;

    for (i = 0; i < 4; i++, pApList++)
    {
        if (pApList->rssi)
        {
            if (MatchMacAdrs(pApList->bssid, pFrm->Dot11Header.BSSID))
                goto lbl;
            if (pApList->lifeTime < life1)
            {
                life1 = pApList->lifeTime;
                free1 = i;
            }
        }
        else
        {
            free0 = i;
        }
    }

    if (free0 != 4)
        i = free0;
    else if (free1 != 4)
        i = free1;
    else
        return;

lbl:
    pApList = &wlMan->ApList[i];
    {
        volatile u16 data = 0;
        fn_02382C80(data, pApList, sizeof(*pApList));
    }
    pApList->lifeTime = 1024;
    pApList->rssi = pFrm->MacHeader.Rx.rsv_RSSI & 0xFF;
    pApList->channel = channel;
    fn_023969D8(pApList->bssid, pFrm->Dot11Header.BSSID);

    pApList->ssidLength = fn_02385334(&pSSID->Length);
    for (j = 0; j < pApList->ssidLength; ++j)
    {
        fn_02385354(&pApList->ssid[j], fn_02385334(&pSSID->SSID[j]));
    }

    pApList->beaconInterval = pFrm->Body.BeaconInterval;
    pApList->capaInfo = pFrm->Body.CapaInfo.Data;
}
