/* The touch panel (the Nitro SDK's spi/ARM9/tp.c), arm9 .text
 * 0x0205e9fc..0x0205f5c0, .bss 0x020a80c8..0x020a8104.
 *
 * TP_Init installs TPi_TpCallback on PXI tag 6. Each request sends one or
 * two words to the ARM7, which replies through that callback: it copies the
 * sample the ARM7 left in the system work area into tpState.buf (one-shot)
 * or the next slot of the auto-sampling ring, clears the command's bit in
 * command_flg, sets its bit in err_flg on a failure, and calls the user
 * callback. TP_GetCalibratedPoint maps a raw sample to screen dots with the
 * parameters TP_SetCalibrateParam stored, which TP_GetUserInfo derives from
 * the firmware's calibration points.
 *
 * mwccarm emits the bodies in reverse, so TPi_TpCallback sits highest in the
 * ROM and TP_CheckError lowest. The functions nothing calls (TP_SetCallback,
 * TP_WaitRawResult, the auto-sampling stop, the latest-point readers,
 * TP_GetUnCalibratedPoint, TP_WaitAllBusy, TP_CheckBusy) were dead-stripped.
 *
 * This SDK is older than the ntrtwl/NitroSDK copy: TP_RequestSetStabilityAsync
 * sends the retry count and the range as two words, TP_SetCalibrateParam has
 * no zero-dot-size branch, and TP_GetUserInfo returns whether the calibration
 * was accepted instead of clearing it on a failure.
 */
#include "types.h"
#include "nitro/cp/divider.h"
#include "nitro/os/system.h"
#include "nitro/os/systemWork.h"
#include "nitro/spi/tp.h"
#include "nitro/pxi/fifo.h"
#include "nitro/pxi/init.h"
#include "nitro/spi/userInfo.h"

typedef int BOOL;
#define TRUE 1
#define FALSE 0
#define NULL 0

/* The SPI channel's PXI word (the SDK's nitro/spi/common/type.h). */
#define SPI_PXI_START_BIT 0x02000000
#define SPI_PXI_END_BIT 0x01000000
#define SPI_PXI_INDEX_SHIFT 16
#define SPI_PXI_DATA_MASK 0x0000ffff

#define SPI_PXI_COMMAND_TP_SAMPLING 0x0000
#define SPI_PXI_COMMAND_TP_AUTO_ON 0x0001
#define SPI_PXI_COMMAND_TP_AUTO_OFF 0x0002
#define SPI_PXI_COMMAND_TP_SETUP_STABILITY 0x0003
#define SPI_PXI_COMMAND_TP_AUTO_SAMPLING 0x0010

#define SPI_PXI_RESULT_SUCCESS 0x0000
#define SPI_PXI_RESULT_INVALID_COMMAND 0x0001
#define SPI_PXI_RESULT_INVALID_PARAMETER 0x0002
#define SPI_PXI_RESULT_ILLEGAL_STATUS 0x0003
#define SPI_PXI_RESULT_EXCLUSIVE 0x0004

typedef union {
    struct {
        u32 x : 12;
        u32 y : 12;
        u32 touch : 1;
        u32 validity : 2;
        u32 dummy : 5;
    } e;
    u32 raw;
    u16 halfs[2];
} SPITpData;

#define GX_LCD_SIZE_X 256
#define GX_LCD_SIZE_Y 192

#define TP_RAW_MAX 0x1000
#define TP_CALIBRATE_DOT_SCALE_SHIFT 8
#define TP_CALIBRATE_ORIGIN_SCALE_SHIFT 2
#define TP_CALIBRATE_DOT_INV_SCALE_SHIFT (28 - TP_CALIBRATE_DOT_SCALE_SHIFT)

static void TPi_TpCallback(PXIFifoTag tag, u32 data, BOOL err);

typedef struct {
    s32 x0;
    s32 xDotSize;
    s32 xDotSizeInv;
    s32 y0;
    s32 yDotSize;
    s32 yDotSizeInv;
} TPiCalibrateParam;

static struct {
    TPRecvCallback callback;
    TPData buf;
    u16 index;
    u16 frequence;
    TPData *samplingBufs;
    u16 bufSize;
    TPiCalibrateParam calibrate;
    u16 calibrate_flg;
    vu16 state;
    vu16 err_flg;
    vu16 command_flg;
} tpState;

static inline BOOL TPi_SamplingNow(void) {
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_TOUCHPANEL,
                               SPI_PXI_START_BIT | SPI_PXI_END_BIT | (0 << SPI_PXI_INDEX_SHIFT) |
                                   (SPI_PXI_COMMAND_TP_SAMPLING << 8),
                               0)) {
        return FALSE;
    }

    return TRUE;
}

static inline BOOL TPi_AutoSamplingOn(u16 vCount, u8 frequency) {
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_TOUCHPANEL,
                               SPI_PXI_START_BIT | (0 << SPI_PXI_INDEX_SHIFT) |
                                   (SPI_PXI_COMMAND_TP_AUTO_ON << 8) | frequency,
                               0)) {
        return FALSE;
    }

    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_TOUCHPANEL,
                               SPI_PXI_END_BIT | (1 << SPI_PXI_INDEX_SHIFT) | vCount, 0)) {
        return FALSE;
    }

    return TRUE;
}

static inline BOOL TPi_SetupStability(u8 retry, u16 range) {
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_TOUCHPANEL,
                               SPI_PXI_START_BIT | (0 << SPI_PXI_INDEX_SHIFT) |
                                   (SPI_PXI_COMMAND_TP_SETUP_STABILITY << 8) | retry,
                               0)) {
        return FALSE;
    }

    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_TOUCHPANEL,
                               SPI_PXI_END_BIT | (1 << SPI_PXI_INDEX_SHIFT) | range, 0)) {
        return FALSE;
    }

    return TRUE;
}

static inline void TPi_CopyTpFromSystemWork(TPData *result) {
    SPITpData spi_tp;

    spi_tp.halfs[0] = *(u16 *)(&(OS_GetSystemWork()->touch_panel[0]));
    spi_tp.halfs[1] = *(u16 *)(&(OS_GetSystemWork()->touch_panel[2]));

    result->x = spi_tp.e.x;
    result->y = spi_tp.e.y;
    result->touch = (u8)spi_tp.e.touch;
    result->validity = (u8)spi_tp.e.validity;
}

static inline void TPi_ErrorAtPxi(TPRequestCommand command) {
    tpState.err_flg |= (1 << command);

    if (tpState.callback) {
        (tpState.callback)(command, TP_RESULT_PXI_BUSY, 0);
    }
}

static void TPi_TpCallback(PXIFifoTag tag, u32 data, BOOL err) {
#pragma unused(tag)

    u16 result;
    u16 command;

    result = (data & SPI_PXI_DATA_MASK);
    command = ((result & 0x7f00) >> 8);

    if (err) {
        TPi_ErrorAtPxi(command);
        return;
    }

    if (command == SPI_PXI_COMMAND_TP_AUTO_SAMPLING) {
        tpState.index++;

        if (tpState.index >= tpState.bufSize) {
            tpState.index = 0;
        }

        TPi_CopyTpFromSystemWork(&tpState.samplingBufs[tpState.index]);

        if (tpState.callback) {
            (tpState.callback)(command, TP_RESULT_SUCCESS, (u8)tpState.index);
        }

        return;
    }

    if (!(data & SPI_PXI_END_BIT)) {
        return;
    }

    switch ((result & 0x00ff)) {
    case SPI_PXI_RESULT_SUCCESS:
        switch (command) {
        case SPI_PXI_COMMAND_TP_SAMPLING:
            TPi_CopyTpFromSystemWork(&tpState.buf);
            tpState.state = TP_STATE_READY;
            break;
        case SPI_PXI_COMMAND_TP_AUTO_ON:
            tpState.state = TP_STATE_AUTO_SAMPLING;
            break;
        case SPI_PXI_COMMAND_TP_AUTO_OFF:
            tpState.state = TP_STATE_READY;
            break;
        }

        tpState.command_flg &= ~(1 << command);

        if (tpState.callback) {
            (tpState.callback)(command, TP_RESULT_SUCCESS, 0);
        }

        break;
    case SPI_PXI_RESULT_EXCLUSIVE:
        result = TP_RESULT_EXCLUSIVE;
        goto common;
    case SPI_PXI_RESULT_INVALID_PARAMETER:
        result = TP_RESULT_INVALID_PARAMETER;
        goto common;
    case SPI_PXI_RESULT_ILLEGAL_STATUS:
        result = TP_RESULT_ILLEGAL_STATUS;
    common:
        tpState.err_flg |= (1 << command);
        tpState.command_flg &= ~(1 << command);

        if (tpState.callback) {
            (tpState.callback)(command, (result & 0xff), 0);
        }
        break;

    case SPI_PXI_RESULT_INVALID_COMMAND:
    default:
        OS_Terminate();
        return;
    }
}

void TP_Init(void) {
    static u16 initial = FALSE;

    if (initial) {
        return;
    }

    initial = TRUE;

    PXI_Init();

    tpState.index = 0;
    tpState.callback = NULL;
    tpState.samplingBufs = NULL;
    tpState.state = TP_STATE_READY;
    tpState.calibrate_flg = FALSE;
    tpState.command_flg = 0;
    tpState.err_flg = 0;

    while (!PXI_IsCallbackReady(PXI_FIFO_TAG_TOUCHPANEL, PXI_PROC_ARM7)) {
    }

    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_TOUCHPANEL, TPi_TpCallback);
}

BOOL TP_GetUserInfo(TPCalibrateParam *calibrate) {
    NVRAMConfig *info = (NVRAMConfig *)(OS_GetSystemWork()->nvramUserInfo);

    u16 x1, y1, x2, y2, dx1, dy1, dx2, dy2;

    x1 = info->ncd.tp.raw_x1;
    y1 = info->ncd.tp.raw_y1;
    dx1 = (info->ncd.tp.dx1);
    dy1 = (info->ncd.tp.dy1);
    x2 = info->ncd.tp.raw_x2;
    y2 = info->ncd.tp.raw_y2;
    dx2 = (info->ncd.tp.dx2);
    dy2 = (info->ncd.tp.dy2);

    if (x1 == 0 && x2 == 0 && y1 == 0 && y2 == 0) {
        return FALSE;
    }

    if (TP_CalcCalibrateParam(calibrate, x1, y1, dx1, dy1, x2, y2, dx2, dy2) != 0) {
        return FALSE;
    }

    return TRUE;
}

void TP_SetCalibrateParam(const TPCalibrateParam *param) {
    OSIntrMode enabled;

    if (param == NULL) {
        tpState.calibrate_flg = FALSE;
        return;
    }

    enabled = OS_DisableInterrupts();

    CP_SetDiv32_32(0x10000000, param->xDotSize);

    tpState.calibrate.x0 = param->x0;
    tpState.calibrate.xDotSize = param->xDotSize;
    tpState.calibrate.xDotSizeInv = CP_GetDivResult32();

    CP_SetDiv32_32(0x10000000, param->yDotSize);

    tpState.calibrate.y0 = param->y0;
    tpState.calibrate.yDotSize = param->yDotSize;
    tpState.calibrate.yDotSizeInv = CP_GetDivResult32();

    OS_RestoreInterrupts(enabled);

    tpState.calibrate_flg = TRUE;
}

void TP_RequestSamplingAsync(void) {
    OSIntrMode enabled;

    enabled = OS_DisableInterrupts();

    if (TPi_SamplingNow() == FALSE) {
        OS_RestoreInterrupts(enabled);
        TPi_ErrorAtPxi(TP_REQUEST_COMMAND_SAMPLING);
        return;
    }

    tpState.command_flg |= TP_REQUEST_COMMAND_FLAG_SAMPLING;
    tpState.err_flg &= ~TP_REQUEST_COMMAND_FLAG_SAMPLING;

    OS_RestoreInterrupts(enabled);
}

u32 TP_GetCalibratedResult(TPData *result) {
    if (tpState.err_flg & TP_REQUEST_COMMAND_FLAG_SAMPLING) {
        return 1;
    }

    *result = tpState.buf;
    TP_GetCalibratedPoint(result, result);
    return 0;
}

u32 TP_WaitCalibratedResult(TPData *result) {
    TP_WaitBusy(TP_REQUEST_COMMAND_FLAG_SAMPLING);
    return TP_GetCalibratedResult(result);
}

void TP_RequestAutoSamplingStartAsync(u16 vcount, u16 frequence, TPData samplingBufs[],
                                      u16 bufSize) {
    u32 i;
    OSIntrMode enabled;

    tpState.samplingBufs = samplingBufs;
    tpState.index = 0;
    tpState.frequence = frequence;
    tpState.bufSize = bufSize;

    for (i = 0; i < bufSize; i++) {
        tpState.samplingBufs[i].touch = TP_TOUCH_OFF;
    }

    enabled = OS_DisableInterrupts();

    if ((u8)TPi_AutoSamplingOn(vcount, frequence) == FALSE) {
        OS_RestoreInterrupts(enabled);
        TPi_ErrorAtPxi(TP_REQUEST_COMMAND_AUTO_ON);
        return;
    }

    tpState.command_flg |= TP_REQUEST_COMMAND_FLAG_AUTO_ON;
    tpState.err_flg &= ~TP_REQUEST_COMMAND_FLAG_AUTO_ON;

    OS_RestoreInterrupts(enabled);
}

void TP_RequestSetStabilityAsync(u8 retry, u16 range) {
    OSIntrMode enabled;

    enabled = OS_DisableInterrupts();

    if (TPi_SetupStability(retry, range) == FALSE) {
        OS_RestoreInterrupts(enabled);
        TPi_ErrorAtPxi(TP_REQUEST_COMMAND_SET_STABILITY);
        return;
    }

    tpState.command_flg |= TP_REQUEST_COMMAND_FLAG_SET_STABILITY;
    tpState.err_flg &= ~TP_REQUEST_COMMAND_FLAG_SET_STABILITY;

    OS_RestoreInterrupts(enabled);
}

u16 TP_GetLatestIndexInAuto(void) {
    return tpState.index;
}

#define IN_S16_RANGE(x) ((x) < 0x8000 && (x) >= -0x8000)

u32 TP_CalcCalibrateParam(TPCalibrateParam *calibrate, u16 raw_x1, u16 raw_y1, u16 dx1, u16 dy1,
                          u16 raw_x2, u16 raw_y2, u16 dx2, u16 dy2) {
    s32 rx_width, dx_width, ry_width, dy_width;
    s32 tmp32;
    OSIntrMode enabled;

    if (raw_x1 >= TP_RAW_MAX || raw_y1 >= TP_RAW_MAX || raw_x2 >= TP_RAW_MAX ||
        raw_y2 >= TP_RAW_MAX) {
        return 1;
    }
    if (dx1 >= GX_LCD_SIZE_X || dx2 >= GX_LCD_SIZE_X || dy1 >= GX_LCD_SIZE_Y ||
        dy2 >= GX_LCD_SIZE_Y) {
        return 1;
    }
    if (dx1 == dx2 || dy1 == dy2 || raw_x1 == raw_x2 || raw_y1 == raw_y2) {
        return 1;
    }

    rx_width = raw_x1 - raw_x2;
    dx_width = dx1 - dx2;

    enabled = OS_DisableInterrupts();

    CP_SetDiv32_32((rx_width) << TP_CALIBRATE_DOT_SCALE_SHIFT, dx_width);

    ry_width = raw_y1 - raw_y2;
    dy_width = dy1 - dy2;

    tmp32 = CP_GetDivResult32();
    CP_SetDiv32_32((ry_width) << TP_CALIBRATE_DOT_SCALE_SHIFT, dy_width);

    if (!IN_S16_RANGE(tmp32)) {
        OS_RestoreInterrupts(enabled);
        return 1;
    }
    calibrate->xDotSize = tmp32;
    tmp32 = (s16)((((raw_x1 + raw_x2) << TP_CALIBRATE_DOT_SCALE_SHIFT) -
                   ((dx1 + dx2) * calibrate->xDotSize)) >>
                  (TP_CALIBRATE_DOT_SCALE_SHIFT - TP_CALIBRATE_ORIGIN_SCALE_SHIFT + 1));
    if (!IN_S16_RANGE(tmp32)) {
        OS_RestoreInterrupts(enabled);
        return 1;
    }
    calibrate->x0 = tmp32;

    tmp32 = CP_GetDivResult32();
    OS_RestoreInterrupts(enabled);

    if (!IN_S16_RANGE(tmp32)) {
        return 1;
    }

    calibrate->yDotSize = tmp32;
    tmp32 = (s16)((((raw_y1 + raw_y2) << TP_CALIBRATE_DOT_SCALE_SHIFT) -
                   ((dy1 + dy2) * calibrate->yDotSize)) >>
                  (TP_CALIBRATE_DOT_SCALE_SHIFT - TP_CALIBRATE_ORIGIN_SCALE_SHIFT + 1));
    if (!IN_S16_RANGE(tmp32)) {
        return 1;
    }

    calibrate->y0 = tmp32;

    return 0;
}

void TP_GetCalibratedPoint(TPData *disp, const TPData *raw) {
    TPiCalibrateParam *cal;

    enum { MAX_X = GX_LCD_SIZE_X - 1, MAX_Y = GX_LCD_SIZE_Y - 1 };

    if (!tpState.calibrate_flg) {
        *disp = *raw;
        return;
    }

    cal = &tpState.calibrate;

    disp->touch = raw->touch;
    disp->validity = raw->validity;

    if (raw->touch == 0) {
        disp->x = 0;
        disp->y = 0;
        return;
    }

    disp->x = ((((u64)(raw->x << TP_CALIBRATE_ORIGIN_SCALE_SHIFT) - cal->x0) * cal->xDotSizeInv) >>
               (TP_CALIBRATE_DOT_INV_SCALE_SHIFT + TP_CALIBRATE_ORIGIN_SCALE_SHIFT));

    if ((s16)disp->x < 0) {
        disp->x = 0;
    } else if ((s16)disp->x > MAX_X) {
        disp->x = MAX_X;
    }

    disp->y = ((((u64)(raw->y << TP_CALIBRATE_ORIGIN_SCALE_SHIFT) - cal->y0) * cal->yDotSizeInv) >>
               (TP_CALIBRATE_DOT_INV_SCALE_SHIFT + TP_CALIBRATE_ORIGIN_SCALE_SHIFT));

    if ((s16)disp->y < 0) {
        disp->y = 0;
    } else if ((s16)disp->y > MAX_Y) {
        disp->y = MAX_Y;
    }
}

void TP_WaitBusy(TPRequestCommandFlag command_flgs) {
    while (tpState.command_flg & command_flgs) {
    }
}

u32 TP_CheckError(TPRequestCommandFlag command_flgs) {
    return (tpState.err_flg & command_flgs);
}
