
#ifndef NETWORK_TYPES_H_
#define NETWORK_TYPES_H_

enum NetworkType
{
    NO_SERVICE,
    GSM_MODE,
    GPRS_MODE,
    EDGE_MODE,
    WCDMA_MODE,
    HSDPA_MODE,
    HSUPA_MODE,
    HSDPA_MODE_AND_HSUPA_MODE,
    TD_SCDMA_MODE,
    HSPA_PLUS_MODE,
};


enum OperationMode
{
    LPM = 0,    ///< Low power consumption.
    ONLINE,
    OFFLINE,
    OFFLINE_FTM,
    RESET_MS,
    RF_OFF,
};


#endif
