#ifndef CSTAHEADER_H
#define CSTAHEADER_H


#include <list>
#include <string>
#include <fstream>

using namespace std;

#define CSTA_API_VERSION        "TS2"	/* Current version number of CSTA API */

typedef char            DeviceID_t[64];
typedef long            CSTAMonitorCrossRefID_t;

#define _Int int
#define TSAPI  RetCode_t pascal
typedef short	RetCode_t;
typedef unsigned long	ACSHandle_t;
typedef unsigned char Boolean;

typedef char            ServerID_t[49];
typedef char            LoginID_t[49];
typedef char            Passwd_t[49];
typedef char            AppName_t[21];
typedef char            Version_t[21];

typedef enum {
    APP_GEN_ID,		/* application will provide invokeIDs; any 4-byte value is legal */
    LIB_GEN_ID		/* library will generate invokeIDs in the range 1-32767 */
} InvokeIDType_t;

typedef enum Level_t {
    ACS_LEVEL1 = 1,
    ACS_LEVEL2 = 2,
    ACS_LEVEL3 = 3,
    ACS_LEVEL4 = 4
} Level_t;

typedef enum DeviceIDType_t {
    DEVICE_IDENTIFIER = 0,
    IMPLICIT_PUBLIC = 20,
    EXPLICIT_PUBLIC_UNKNOWN = 30,
    EXPLICIT_PUBLIC_INTERNATIONAL = 31,
    EXPLICIT_PUBLIC_NATIONAL = 32,
    EXPLICIT_PUBLIC_NETWORK_SPECIFIC = 33,
    EXPLICIT_PUBLIC_SUBSCRIBER = 34,
    EXPLICIT_PUBLIC_ABBREVIATED = 35,
    IMPLICIT_PRIVATE = 40,
    EXPLICIT_PRIVATE_UNKNOWN = 50,
    EXPLICIT_PRIVATE_LEVEL3_REGIONAL_NUMBER = 51,
    EXPLICIT_PRIVATE_LEVEL2_REGIONAL_NUMBER = 52,
    EXPLICIT_PRIVATE_LEVEL1_REGIONAL_NUMBER = 53,
    EXPLICIT_PRIVATE_PTN_SPECIFIC_NUMBER = 54,
    EXPLICIT_PRIVATE_LOCAL_NUMBER = 55,
    EXPLICIT_PRIVATE_ABBREVIATED = 56,
    OTHER_PLAN = 60,
    TRUNK_IDENTIFIER = 70,
    TRUNK_GROUP_IDENTIFIER = 71
} DeviceIDType_t;

typedef enum DeviceIDStatus_t {
    ID_PROVIDED = 0,
    ID_NOT_KNOWN = 1,
    ID_NOT_REQUIRED = 2
} DeviceIDStatus_t;

typedef struct ExtendedDeviceID_t {
    DeviceID_t      deviceID;
    DeviceIDType_t  deviceIDType;
    DeviceIDStatus_t deviceIDStatus;
} ExtendedDeviceID_t;

typedef ExtendedDeviceID_t CallingDeviceID_t;

typedef ExtendedDeviceID_t CalledDeviceID_t;

typedef ExtendedDeviceID_t SubjectDeviceID_t;

typedef ExtendedDeviceID_t RedirectionDeviceID_t;

typedef enum ConnectionID_Device_t {
    STATIC_ID = 0,
    DYNAMIC_ID = 1
} ConnectionID_Device_t;

typedef struct ConnectionID_t {
    long            callID;
    DeviceID_t      deviceID;
    ConnectionID_Device_t devIDType;
} ConnectionID_t;

typedef struct Connection_t {
    ConnectionID_t  party;
    SubjectDeviceID_t staticDevice;
} Connection_t;

typedef struct ConnectionList_t {
    _Int             count;
    Connection_t     *connection;
} ConnectionList_t;

typedef enum SystemStatus_t {
    SS_INITIALIZING = 0,
    SS_ENABLED = 1,
    SS_NORMAL = 2,
    SS_MESSAGES_LOST = 3,
    SS_DISABLED = 4,
    SS_OVERLOAD_IMMINENT = 5,
    SS_OVERLOAD_REACHED = 6,
    SS_OVERLOAD_RELIEVED = 7
} SystemStatus_t;

typedef enum StreamType_t {
    ST_CSTA = 1,
    ST_OAM = 2,
    ST_DIRECTORY = 3,
    ST_NMSRV = 4
} StreamType_t;

#define PRIVATE_VENDOR_SIZE 32
typedef struct PrivateData_t {
    char			vendor[PRIVATE_VENDOR_SIZE];
    unsigned short	length;
    char			data[1];	/* actual length determined by application */
} PrivateData_t;


#endif // CSTAHEADER_H

