
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

/// 3G Connection state
enum TGConnectionState
{
    TG_INVALID = 0,       ///< Initial state
    TG_CHECKING_NETWORK,  ///< Checking network
    TG_CONNECTING,        ///< In connecting, network found.
    TG_CONNECTED,         ///< Connected
    TG_DISCONNECTED,      ///< Disconnect from network, but modem is still active
    TG_STOP,              ///< Modem power off.
};

#endif
