#ifndef NITRO_SPI_TP_H
#define NITRO_SPI_TP_H

/*
 * The touch panel API: the Nitro SDK's nitro/spi/ARM9/tp.h, its types and
 * the prototypes of the functions this ROM keeps (src/engine/spi/TP.c), and
 * the synchronous wrappers the SDK header defines inline over them. The
 * SDK's BOOL return is spelled int: no shared header here defines BOOL.
 */

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TP_SAMPLING_FREQUENCY_MAX 16

#define TP_TOUCH_OFF 0
#define TP_TOUCH_ON 1

#define TP_VALIDITY_VALID 0
#define TP_VALIDITY_INVALID_X 1
#define TP_VALIDITY_INVALID_Y 2
#define TP_VALIDITY_INVALID_XY (TP_VALIDITY_INVALID_X | TP_VALIDITY_INVALID_Y)

typedef enum {
    TP_REQUEST_COMMAND_SAMPLING = 0x0,
    TP_REQUEST_COMMAND_AUTO_ON = 0x1,
    TP_REQUEST_COMMAND_AUTO_OFF = 0x2,
    TP_REQUEST_COMMAND_SET_STABILITY = 0x3,
    TP_REQUEST_COMMAND_AUTO_SAMPLING = 0x10
} TPRequestCommand;

typedef enum {
    TP_REQUEST_COMMAND_FLAG_SAMPLING = 1 << TP_REQUEST_COMMAND_SAMPLING,
    TP_REQUEST_COMMAND_FLAG_AUTO_ON = 1 << TP_REQUEST_COMMAND_AUTO_ON,
    TP_REQUEST_COMMAND_FLAG_AUTO_OFF = 1 << TP_REQUEST_COMMAND_AUTO_OFF,
    TP_REQUEST_COMMAND_FLAG_SET_STABILITY = 1 << TP_REQUEST_COMMAND_SET_STABILITY
} TPRequestCommandFlag;

typedef enum {
    TP_RESULT_SUCCESS = 0,
    TP_RESULT_INVALID_PARAMETER,
    TP_RESULT_ILLEGAL_STATUS,
    TP_RESULT_EXCLUSIVE,
    TP_RESULT_PXI_BUSY
} TPRequestResult;

typedef enum {
    TP_STATE_READY = 0,
    TP_STATE_SAMPLING,
    TP_STATE_AUTO_SAMPLING,
    TP_STATE_AUTO_WAIT_END
} TPState;

/* One panel sample: the axes, then TP_TOUCH_* and TP_VALIDITY_*. */
typedef struct TPData {
    u16 x;        /* 0x0 */
    u16 y;        /* 0x2 */
    u16 touch;    /* 0x4 */
    u16 validity; /* 0x6 */
} TPData;

#define TP_CALIBRATE_DOT_SCALE_SHIFT 8
#define TP_CALIBRATE_ORIGIN_SCALE_SHIFT 2

/* The screen origin and dot size, in raw panel units, that
   TP_CalcCalibrateParam derives from two point pairs. */
typedef struct NvTpData {
    s16 x0;       /* 0x0 */
    s16 y0;       /* 0x2 */
    s16 xDotSize; /* 0x4 */
    s16 yDotSize; /* 0x6 */
} TPCalibrateParam;

typedef void (*TPRecvCallback)(TPRequestCommand command, TPRequestResult result, u16 index);

void TP_Init(void);
void TP_SetCalibrateParam(const TPCalibrateParam *param);
void TP_RequestSamplingAsync(void);
u32 TP_WaitCalibratedResult(TPData *result);
u32 TP_GetCalibratedResult(TPData *result);
void TP_RequestAutoSamplingStartAsync(u16 vcount, u16 frequence, TPData samplingBufs[],
                                      u16 bufSize);
void TP_RequestSetStabilityAsync(u8 retry, u16 range);
void TP_WaitBusy(TPRequestCommandFlag command_flgs);
u32 TP_CheckError(TPRequestCommandFlag command);
u16 TP_GetLatestIndexInAuto(void);
u32 TP_CalcCalibrateParam(TPCalibrateParam *calibrate, u16 raw_x1, u16 raw_y1, u16 dx1, u16 dy1,
                          u16 raw_x2, u16 raw_y2, u16 dx2, u16 dy2);
int TP_GetUserInfo(TPCalibrateParam *calibrate);
void TP_GetCalibratedPoint(TPData *disp, const TPData *raw);

static inline u32 TP_RequestSetStability(u8 retry, u16 range) {
    TP_RequestSetStabilityAsync(retry, range);
    TP_WaitBusy(TP_REQUEST_COMMAND_FLAG_SET_STABILITY);
    return TP_CheckError(TP_REQUEST_COMMAND_FLAG_SET_STABILITY);
}

static inline u32 TP_RequestAutoSamplingStart(u16 vcount, u16 frequence, TPData samplingBufs[],
                                              u16 bufSize) {
    TP_RequestAutoSamplingStartAsync(vcount, frequence, samplingBufs, bufSize);
    TP_WaitBusy(TP_REQUEST_COMMAND_FLAG_AUTO_ON);
    return TP_CheckError(TP_REQUEST_COMMAND_FLAG_AUTO_ON);
}

static inline u32 TP_RequestCalibratedSampling(TPData *result) {
    TP_RequestSamplingAsync();
    return TP_WaitCalibratedResult(result);
}

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char TPData_size_must_be_0x8[sizeof(struct TPData) == 0x8 ? 1 : -1];
#endif

#ifdef __cplusplus
}
#endif

#endif /* NITRO_SPI_TP_H */
