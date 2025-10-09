/***********************************************************/
/* Copyright (C) 1997 Alcatel.  All rights reserved.*/
/***********************************************************/

#ifndef ALC_PRIV_V2_H
#define ALC_PRIV_V2_H

#define ACD_DATA	0xCACD
#define ACD_IVR		0xCACE
#define PC_DATA		0xCACF
#define VIOLA_DATA	0xCA5A
#define PRIV_LIB_BUSY 0xCA1C

//#define SIZE_OF_ALCATEL_PRIVATE_DATA		600
#define SIZE_OF_ALCATEL_PRIVATE_DATA		3000
#define MAX_NUMBER_OF_PRIVATE_EXTENSIONS	20	/*RA CDHba15105*/
#define MAX_LENGTH_GENERALIZED_TIME			20
#define MAX_LENGTH_PRIVATE_IDENTIFIER		30
#define MAX_LENGTH_PRIVATE_DATA				60
#define MAX_LENGTH_PRIVATE_EVENT			60
#define MAX_LENGTH_SERVICE_REGISTER			30
#define MAX_LENGTH_AGENT_NAME				30
#define MAX_LENGTH_CORRELATOR_DATA			40
#define MAX_LENGTH_OCTET_STRING				60
#define MAX_LENGTH_HOST_IDENTIFICATION		32
#define MAX_LENGTH_OTHER_OPERATION			96
#define MAX_NUMBER_OF_ELEMENT				20
#define MAX_NUMBER_OF_SECRETDEVICEID		5
#define MAX_NUMBER_OF_GLOBALCALLID			3	/*YB 270801*/
/*RoutingService+*/
#define MAX_LENGTH_FLUSH_CHAR				3
#define MAX_LENGTH_DIGITS_TO_DISPLAY		20
#define MAX_NUMBER_OF_DIGITS				1
#define MAX_NUMBER_OF_PROMPTS				10
#define MAX_LENGTH_DIGITS_KEY				3
#define MAX_LENGTH_DIGITS_DETECTED			30
/*RoutingService-*/

typedef char   GeneralizedTime_t[MAX_LENGTH_GENERALIZED_TIME];

//Alcatel private operation ECMA V2
// encapsulated in Escape Service
#define   ALCATEL_DATE_AND_TIME_SETTING    1
#define   ALCATEL_DATE_AND_TIME_SETTING_CONF  2001
#define   ALCATEL_IVR_INFORMATION 2
#define   ALCATEL_IVR_INFORMATION_CONF 2002
#define	  ALCATEL_INTERRUPT_TRANSACTION 3
#define   ALCATEL_INTERRUPT_TRANSACTION_CONF 2003


#define   ALCATEL_OTHER_OPERATION 499
#define   ALCATEL_OTHER_OPERATION_CONF 2499

typedef unsigned char PrivBool;

typedef enum AlcatelCstaPrivDataId_t{
	associateData_Id	= 0,
	dateAndTimeSetting_Id = 1,
	hostInformation_Id = 2,
	interruptTransaction_Id = 3,
	setDeviceInService_Id = 4,
	supervisorAssistRequest_Id = 6,
	supervisorAssistCancel_Id = 7,
	serviceRegisterRequest_Id = 8,
	serviceRegisterCancel_Id = 9,
	aggentLoggingInformation_Id = 10,
	startListeningMessage_Id = 11,
	stopListeningMessage_Id = 12,
	bipToneGeneration_Id = 13,
	permanentListening_Id = 15,
	treatmentRequest_Id = 16,
	setMessageWaitingIndicator_Id = 17,	/*YB 160102*/
/*RoutingService+*/
	startDataCollection_Id = 18,
	stopDataCollection_Id = 19,
	dataCollected_Id = 20,
	generateTelTones_Id = 21,
	cancelTelTones_Id = 22,
/*RoutingService-*/
	otherOperation_Id = 499,
	correlatorDataV2_Id = 500,
	privateErrors_Id = 501,
	acdWaitingTime_Id = 502,
	cstaServiceOption_Id = 503,
	networkTimeSlot_Id = 504,
	notReadyContext_Id = 505,
	queryAgentInformation_Id = 506,
	pilotTransfertInformation_Id = 507,
	acdDistributionInformation_Id = 508,
	ivrInteractiveQueuing_Id = 509,
	ivrNextLevel_Id = 510,
	notReadyActivation_Id = 511,
	partyName_Id = 512,
	requestingDevice_Id = 513,
	acdNetworkRerouted_Id = 514,
	hostIdentification_Id = 515,
	supervisedTransfer_Id = 516,
	acrAttributeList_Id = 518,
	cCTreatmentType_Id = 519,
    secretIdentiteList_Id = 520,
    globalCallID_Id = 521,
    oldglobalCallID_Id = 522,
    nationalIndication_Id = 523,
/*RoutingService+*/
	rerouteAuthorisation_Id = 524,
	digitsReportingCriteriaAdd_Id = 525,
	announcementOrMusic_Id = 526,
/*RoutingService-*/
	globalCallIDList_id = 527,		/*YB 270801*/

	otherPrivateData_Id = 999
} AlcatelCstaPrivDataId_t;

typedef enum AlcatelCstaPrivEventId_t {
	headOfQueueEvent_Id = 1000,
	supervisorAssistRequestEvent_Id = 1001,
	trunkGroupSupervisionEvent_Id = 1002,
	telephonyTonesGeneratedEvent_Id = 1004,	/*RoutingService*/
	busyEvent_Id = 1005,				/*YB 270801*/
	remoteRecordFailedEvent_Id = 1006,	/*YB 270801*/
	otherPrivateEvent_Id = 1499
}AlcatelCstaPrivEventId_t;

// sequence of private data
typedef struct AlcatelCstaPrivData_t {
	unsigned long				numberOfPrivateData;
	unsigned long				AlcCstaPrivPtOffsetArray[MAX_NUMBER_OF_PRIVATE_EXTENSIONS];
	char*                        privateHeap[1];
} AlcatelCstaPrivData_t;

typedef  struct CstaSecurityData_t {
	long				messageSequenceNumber;
	GeneralizedTime_t	timeStamp;
} CstaSecurityData_t;

typedef struct CstaCommonArguments_t {
	CstaSecurityData_t		security;
	AlcatelCstaPrivData_t	privateData;
}CstaCommonArguments_t;


typedef enum CCTreatmentTypeID_t {
	cceapplication = 0,
    ccoapplication = 1,
    ccwapplication = 2,
    other = 255
}CCTreatmentTypeID_t;

typedef	enum IvrStatus_t {
	connected = 0,
	disconnected = 1
} IvrStatus_t;

typedef enum PilotStatus_t {
	opened = 0,
	blocked = 1,
	blocked_on_rule = 2,
	blocked_on_blocked_rule = 3,
	general_forwarding = 4,
	general_forwarding_on_rule = 5,
	blocked_on_general_forwarding_rule = 6,

	otherPilotStatus = 255
}PilotStatus_t;

typedef enum AgentAction_t {
	aaloggedOff =	0,
	aaloggedOn  = 1
}AgentAction_t;

typedef enum  PrivateOperations_t {
	invalidCstaApplicationCorrelator = 20,
	invalidAccountCode = 21,
	invalidAuthorizationCode = 22
}PrivateOperations_t;

typedef enum PrivateErrorsType_t {
	OperationalErrorsType = 1,
	UnspecifiedErrorsType = 7,
	otherErrorsType = 8
} PrivateErrorsType_t;

typedef  struct PrivateErrors_t{
		PrivateErrorsType_t privateErrorsType;
		union{
			PrivateOperations_t operationalErrors;
			Nulltype            unspecifiedErrors;
			char				otherErrors[40];
		}u;
}PrivateErrors_t;

typedef struct AcdWaitingTime_t	{
	unsigned long	waitingTime;
	PrivBool			saturation;
}AcdWaitingTime_t;
 
typedef unsigned short 	CstaServiceOption_t;
typedef unsigned short 	RerouteAuthorisation_t;	/*RoutingService*/

/*YB 021202+*/
#define SO_CALL_PROGRESS_TONE_INHIBITION	0x01
#define SO_HOLD_TONE_INHIBITION				0x02
#define SO_PRIORITARY_TRANSFER				0x04
#define SO_AUTO_ORIGINATE					0x08
#define SO_SUPERVISOR_CALL					0x10
#define SO_SUPERVISOR_STEP_IN				0x20
#define SO_SUPERVISOR_TRANSFER				0x40
#define SO_SUPERVISOR_HEAD_SET_MODE			0x80
#define SO_BACKUP_ROUTING					0x0100
/*YB 021202-*/

typedef unsigned long  NetworkTimeSlot_t;

typedef enum NotReadyContext_t {
	  NRC_FORCED_PAUSE = 0,
	  NRC_OTHER = 255
} NotReadyContext_t;

typedef struct QueryAgentInformation_t {
	PrivBool				agentPreAssigned;
	AgentGroup_t			agentGroup;
	NotReadyContext_t		notReadyContext;
	PrivBool				agentPreAssignedInfoProvided;
	PrivBool				notReadyContextInfoProvided;
}QueryAgentInformation_t;

typedef struct PilotTransferInformation_t {
	PrivBool			possibleTransfer;
	PilotStatus_t	pilotStatus;
}PilotTransferInformation_t;

typedef struct AcdDistributionInformation_t{
	unsigned long	waitingTime;
	unsigned long   globalWaitingTime;
	AgentGroup_t	agentGroup;
}AcdDistributionInformation_t ;

typedef struct IvrInteractiveQueing_t{
	SubjectDeviceID_t pilotNumber;
	long     		  guideCurrentLevel;
}IvrInteractiveQueing_t;

typedef unsigned long  IvrNextLevel_t;
typedef unsigned long  NotReadyActivation_t;

typedef struct OtherPrivateData_t{
	char    privateIdentifier[MAX_LENGTH_PRIVATE_IDENTIFIER];
	char    privateData[MAX_LENGTH_PRIVATE_DATA];
	unsigned long lengthPrivateIdentifier;
	unsigned long lengthPrivateData;
}OtherPrivateData_t;

typedef struct AssociateData_t {
	ConnectionID_t	connectionID;
	AccountInfo_t	accountCode;
	AuthCode_t		authCode;
	char			correlator[40];
}AssociateData_t;

typedef struct InterruptTransaction_t {
	ConnectionID_t	connectionID;
	PrivBool		interruptible;
}InterruptTransaction_t;

typedef struct SetDeviceInService_t {
	DeviceID_t	deviceID;
	PrivBool	setInService;
}SetDeviceInService_t;

typedef struct CorrelatorDataV2_t {
	unsigned long	length;
	unsigned char	correlatorData[MAX_LENGTH_CORRELATOR_DATA];
}CorrelatorDataV2_t;

typedef struct SupervisorAssistRequest_t{
	DeviceID_t device;
}SupervisorAssistRequest_t;

typedef struct SupervisorAssistCancel_t{
	DeviceID_t requestingDevice;
	DeviceID_t otherDevice;
}SupervisorAssistCancel_t;

typedef enum serviceRegisterRequestChoiceId{
	serviceRegisterRequestArgumentId = 0,
	serviceRegisterRequestResultId = 1
}serviceRegisterRequestChoiceId;

typedef struct ServiceRegisterId_t{
	char serviceRegisterId[MAX_LENGTH_SERVICE_REGISTER];
	unsigned long length;
}ServiceRegisterId_t;

typedef struct ServiceRegisterRequestArgument_t{
	unsigned long serviceVersionId;
}ServiceRegisterRequestArgument_t;

typedef struct ServiceRegisterRequestResult_t{
	ServiceRegisterId_t serviceRegisterId;
}ServiceRegisterRequestResult_t;

typedef struct ServiceRegisterRequest_t{
	unsigned long	ChoiceId;
	union{
		ServiceRegisterRequestArgument_t		serviceRegisterRequestArgument;
		ServiceRegisterRequestResult_t			serviceRegisterRequestResult;
	}u;
}ServiceRegisterRequest_t;

typedef struct ServiceRegisterCancel_t{
	ServiceRegisterId_t serviceRegisterId;
}ServiceRegisterCancel_t;

typedef struct AgentName_t{
	char agentName[MAX_LENGTH_AGENT_NAME];
}AgentName_t;

typedef struct AgentLoggingInformation_t{
	ServiceRegisterId_t serviceRegister;
	AgentAction_t		agentAction;
	DeviceID_t			agentNumber;
	DeviceID_t			proAcdNumber;
	AgentName_t			agentName;
}AgentLoggingInformation_t;

typedef struct PartyName_t{
	AgentName_t	callingPartyName;
	AgentName_t	alertingPartyName;
}PartyName_t;

typedef struct RequestingDevice_t{
	DeviceID_t requestingDevice;
}RequestingDevice_t;

typedef PrivBool AcdNetworkRerouted_t;

typedef struct HostIdentification_t{
	char hostIdentification[MAX_LENGTH_HOST_IDENTIFICATION];
}HostIdentification_t;

typedef enum HostType_t{
	HOST_TYPE_VIOLA = 0,
	HOST_TYPE_GENERIC_IVR = 1,
	HOST_TYPE_OTHER_IVR = 255
}HostType_t;

typedef enum HostStatus_t{
	HOST_STATUS_CONNECTED = 0,
	HOST_STATUS_DECONNECTED = 1,
	HOST_STATUS_DISABLED = 2,
	HOST_STATUS_ENABLED = 3,
	HOST_STATUS_INITIALIZING = 4,
	HOST_STATUS_MESSAGES_LOST = 5,
	HOST_STATUS_NORMAL = 6,
	HOST_STATUS_OVERLOAD_IMMINENT = 7,
	HOST_STATUS_OVERLOAD_REACHED = 8,
	HOST_STATUS_OVERLOAD_RELIEVED = 9
}HostStatus_t;

typedef struct CSTAIvrStatus_t {
	HostStatus_t	ivrStatus;
	PrivateData_t	extensions;
}CSTAIvrStatus_t;

typedef struct OctetString_t{
	unsigned long	length;
	unsigned char	octetString[MAX_LENGTH_OCTET_STRING];
}OctetString_t;

typedef OctetString_t GlobalCallID_t;

typedef struct HostInformation_t{
	HostIdentification_t	hostIdentification;
	HostType_t				hostType;
	HostStatus_t			hostStatus;
}HostInformation_t;

typedef struct StartListeningMessage_t{
	DeviceID_t		listenedDevice;
	unsigned long	pcmCrystalNumber;
	unsigned long	pcmCouplerNumber;
	unsigned long	pcmTimeSlot;
	PrivateData_t	extensions;
}StartListeningMessage_t;

typedef struct StopListeningMessage_t{
	DeviceID_t		listenedDevice;
	PrivateData_t	extensions;
}StopListeningMessage_t;

typedef struct BipToneGeneration_t{
	DeviceID_t		bippedDevice;
	unsigned long	tonality;
	unsigned long	presenceTimer;
	unsigned long	silenceTimer;
	PrivateData_t	extensions;
}BipToneGeneration_t;

typedef PrivBool SupervisedTransfer_t;

typedef struct Element_t{
		unsigned short			skillNumber;
        PrivBool				acrStatus;
        unsigned short			expertEvaluationLevel;
}Element_t;

typedef struct ACRAttributeList_t{
	DeviceID_t		acrAgentNumber;
	unsigned short	count;
	Element_t		elementArray[MAX_NUMBER_OF_ELEMENT];
}ACRAttributeList_t;

typedef struct SecretIdentiteList_t{
	unsigned short	count;
	SecretDeviceID_t	deviceIDArray[MAX_NUMBER_OF_SECRETDEVICEID];
}SecretIdentiteList_t;

/*YB 270801+*/
typedef struct GlobalCallIDList_t{
	unsigned short	count;
	GlobalCallID_t globalCallIDArray[MAX_NUMBER_OF_GLOBALCALLID];
}GlobalCallIDList_t;
/*YB 270801-*/

typedef struct PermanentListening_t {
	DeviceID_t requestingDevice;
	DeviceID_t listenAgentDevice;
}PermanentListening_t;

/*YB 160102+*/
typedef struct MessageWaitingIndicator_t {
	DeviceID_t device;
	PrivBool   messageWaitingOn;
}MessageWaitingIndicator_t;
/*YB 160102-*/

typedef enum ApplicationType_t{ 
	CCE_APPLICATION = 0,
	CCO_APPLICATION = 1,
	CCW_APPLICATION = 2,
	CC_OTHER = 255
}ApplicationType_t;

typedef struct TreatmentRequestArgument_t {
	DeviceID_t agentNumber;
	PrivBool	actionType;
	ApplicationType_t applicationType;
}TreatmentRequestArgument_t;

typedef struct TreatmentRequestResult_t{
	GlobalCallID_t	globalCallID;
}TreatmentRequestResult_t;

typedef enum treatmentRequestChoiceId{
	treatmentRequestArgumentId = 0,
	treatmentRequestResultId = 1
}treatmentRequestChoiceId;

typedef struct TreatmentRequest_t{
	unsigned long	ChoiceId;
	union{
	TreatmentRequestArgument_t	treatmentRequestArgument;
	TreatmentRequestResult_t	treatmentRequestResult;
	}u;
}TreatmentRequest_t;

typedef struct OtherOperation_t {
	unsigned long	length;
	unsigned char	otherOperation[96];
}OtherOperation_t;

typedef ExtendedDeviceID_t NationalIndication_t;

/*RoutingService+*/
typedef enum ConnectionID3_type_t {
	CID3_CALLID,
	CID3_DEVICEID,
	CID3_BOTH
} ConnectionID3_type_t;

typedef enum CallObjectType_t {
	DEVICETYPE = 0,
	CONNECTIONTYPE = 1
} CallObjectType_t;

#define	MEDIACLASSE_UNKNOWN			0x00
#define	MEDIACLASSE_VOICE			0x01
#define	MEDIACLASSE_VOICE_DATA		0x02
#define MEDIACLASSE_VOICE_IMAGE		0x04
#define	MEDIACLASSE_VOICE_OTHER		0x08
#define	MEDIACLASSE_VOICE_AUDIO		0x10
#define	MEDIACLASSE_VOICE_NOTKNOWN	0x20

typedef unsigned short 	MediaClass_t;

typedef struct DeviceId3_t {
	ExtendedDeviceID_t extendedDevice;
	MediaClass_t mediaClass;
}DeviceId3_t;

typedef struct LocalDeviceId3_t {
	ConnectionID_Device_t devIDType;
	DeviceId3_t staticID;
	DeviceID_t	dynamicID;
}LocalDeviceId3_t;

typedef struct ConnectionID3_t {
	ConnectionID3_type_t type;
	long				callID;
	LocalDeviceId3_t   deviceID;
} ConnectionID3_t;


typedef struct CallObject_t {
	CallObjectType_t objectType;
	DeviceId3_t device;
	ConnectionID3_t connection;
} CallObject_t;

typedef char FlushChar_t[MAX_LENGTH_FLUSH_CHAR];

typedef struct DigitsReportingCriteria_t {
    long numChars;
    FlushChar_t flushChar;
    long timeout;
} DigitsReportingCriteria_t;

typedef enum DataCollType_t {
	DC_DIGITS = 0,
	DC_TELTONES = 1
} DataCollType_t;

typedef long     DataCollCrid_t;

typedef char DigitsDetected_t[MAX_LENGTH_DIGITS_DETECTED + 1];

typedef struct DDDurationList_t{
	unsigned short	count;
	long longTable[MAX_LENGTH_DIGITS_DETECTED];
}DDDurationList_t;

typedef struct DigitsData_t {
    DigitsDetected_t digitsDetected;
} DigitsData_t;

typedef enum DcollCause_t {
	DCC_FLUSHCHARRECEIVED = 0,
	DCC_CHARCOUNTREACHED = 1,
	DCC_TIMEOUT = 2,
	DCC_SFTERMINATED = 3,
	DCC_NOCAUSE = 255
} DcollCause_t;

typedef struct DataCollected_t {
	DataCollCrid_t dataCollCrid;
	DigitsData_t digitsData;
	DcollCause_t dcollCause;
} DataCollected_t;

typedef enum ToneToSend_t{
//	TONE_BEEP = 0,
//	TONE_BILLING = 1,
	TONE_BUSY = 2,
//	TONE_CARRIER = 3,
//	TONE_CONFIRMATION = 4,
//	TONE_DIAL = 5,
//	TONE_FAXCNG = 6,
//	TONE_HOLD = 7,
//	TONE_HOWLER = 8,
//	TONE_INTRUSION = 9,
//	TONE_MODEMCNG = 10,
//	TONE_PARK = 11,
//	TONE_RECORDWARNING = 12,
//	TONE_REORDER = 13,
	TONE_RINGBACK = 14,
	TONE_SILENCE = 15,
//	TONE_SITVC = 16,
//	TONE_SITIC = 17,
//	TONE_SITRO = 18,
//	TONE_SITNC = 19,
	TONE_SWITCHSPEC0 = 20,
	TONE_SWITCHSPEC1 = 21
}ToneToSend_t ;

typedef enum SpellingOptionType_t {
	SO_NUMBERBYNUMBER = 0,
	SO_DATE = 1,
	SO_HOUR = 2,
	SO_PHONENUMBER = 3,
	SO_CURRENCY = 4,
} SpellingOptionType_t;

typedef struct Prompt_t {
	Boolean interruptable;
	long id;
}Prompt_t;

typedef char Digits_t[MAX_LENGTH_DIGITS_TO_DISPLAY];

typedef struct DigitsToPlay_t {
	SpellingOptionType_t spellingOption;
	Digits_t digits;
}DigitsToPlay_t;

typedef char DigitsKey_t[MAX_LENGTH_DIGITS_KEY];

typedef struct DigitsReportingCriteriaAdd_t {
    DigitsKey_t abortDigits;
    DigitsKey_t ignoreDigits;
    DigitsKey_t backspaceDigits;
    DigitsKey_t termDigits;
    DigitsKey_t resetDigits;
    long startTimeout;
	long digitTimeout;
} DigitsReportingCriteriaAdd_t;

typedef struct StartDataCollectionArgument_t {
    CallObject_t callObject;
    DataCollType_t dataCollType;
    DigitsReportingCriteria_t digitsReportingCriteria;
	DigitsReportingCriteriaAdd_t digitsReportingCriteriaAdd;
} StartDataCollectionArgument_t;

typedef struct StartDataCollectionResult_t {
	DataCollCrid_t dataCollCrid;
} StartDataCollectionResult_t;

typedef enum serviceStartDataCollectionChoiceId{
	serviceStartDataCollectionArgumentId = 0,
	serviceStartDataCollectionResultId = 1
}serviceStartDataCollectionChoiceId;

typedef struct StartDataCollection_t {
	unsigned long	ChoiceId;
	union{
		StartDataCollectionArgument_t startDataCollectionArgument;
		StartDataCollectionResult_t startDataCollectionResult;
	}u;
} StartDataCollection_t;


typedef struct StopDataCollection_t {
	DataCollCrid_t dataCollCrid;
} StopDataCollection_t;

typedef enum AnnouncementOrMusicType_t {
	ANNOUNCEMENT = 0,
	MUSICID = 1
} AnnouncementOrMusicType_t;

typedef struct DigitsOrPromptList_t{
	unsigned short	digitsCount;
	DigitsToPlay_t digitsToPlayArray[MAX_NUMBER_OF_DIGITS];
	unsigned short	promptCount;
	Prompt_t promptArray[MAX_NUMBER_OF_PROMPTS];
}DigitsOrPromptList_t;

typedef struct AnnouncementToGenerate_t {
    long language;
    long expectedWaitingTime;
    long positionInQueue;
	DigitsOrPromptList_t digitsOrPromptList;
} AnnouncementToGenerate_t;

typedef struct AnnouncementOrMusic_t {
	AnnouncementOrMusicType_t type;
	AnnouncementToGenerate_t announcement;
	long musicId;
} AnnouncementOrMusic_t;

typedef struct GenerateTelTones_t {
	ConnectionID3_t connection;
	ToneToSend_t toneToSend;
	long toneDuration;
	AnnouncementOrMusic_t announcementOrMusic;
} GenerateTelTones_t;

typedef struct CancelTelTones_t {
	ConnectionID3_t connection;
} CancelTelTones_t;
/*RoutingService-*/

typedef struct 	AlcatelCstaPrivObject_t {
	unsigned long		objectId;   
	union {
		AssociateData_t					associateData;				//0
		GeneralizedTime_t				dateAndTimeSetting;			//1
		HostInformation_t				hostInformation;			//2
		InterruptTransaction_t			interruptTransaction;		//3
		SetDeviceInService_t			setDeviceInService;			//4
		SupervisorAssistRequest_t		supervisorAssistRequest;	//6
		SupervisorAssistCancel_t		supervisorAssistCancel;		//7
		ServiceRegisterRequest_t		serviceRegisterRequest;		//8
		ServiceRegisterCancel_t			serviceRegisterCancel;		//9
		AgentLoggingInformation_t		agentLoggingInformation;	//10
		StartListeningMessage_t			startListeningMessage;		//11
		StopListeningMessage_t			stopListeningMessage;		//12
		BipToneGeneration_t				bipToneGeneration;			//13
		PermanentListening_t			permanentListening;			//15
		TreatmentRequest_t				treatmentRequest;			//16
		MessageWaitingIndicator_t		messageWaitingIndicator;	//17	/*YB 160102*/
		StartDataCollection_t			startDataCollection;		//18	/*RoutingService*/
		StopDataCollection_t			stopDataCollection;			//19	/*RoutingService*/
		DataCollected_t					dataCollected;				//20	/*RoutingService*/
		GenerateTelTones_t				generateTelTones;			//21	/*RoutingService*/
		CancelTelTones_t				cancelTelTones;				//22	/*RoutingService*/
		OtherOperation_t				otherOperation;				//499
		CorrelatorDataV2_t				correlatorDataV2;			//500 correlator data field for events
		PrivateErrors_t					privateErrors;				//501
		AcdWaitingTime_t				acdWaitingTime;				//502
		CstaServiceOption_t				cstaServiceOption;			//503
		NetworkTimeSlot_t				networkTimeSlot;			//504
		NotReadyContext_t				notReadyContext;			//505
		QueryAgentInformation_t			queryAgentInformation;		//506
		PilotTransferInformation_t		pilotTransferInformation;	//507
		AcdDistributionInformation_t	acdDistributionInformation;	//508
		IvrInteractiveQueing_t			ivrInteractiveQueuing;		//509
		IvrNextLevel_t					ivrNextLevel;				//510
		NotReadyActivation_t			notReadyActivation;			//511
		PartyName_t						partyName;					//512
		RequestingDevice_t				requestingDevice;			//513
		AcdNetworkRerouted_t			acdNetworkRerouted;			//514
		HostIdentification_t			hostIdentification;			//515
		SupervisedTransfer_t			supervisedTransfer;			//516
		ACRAttributeList_t				aCRAttributeList;			//518
		CCTreatmentTypeID_t				cCTreatmentType;			//519
		SecretIdentiteList_t			secretIdentiteList;			//520
		GlobalCallID_t					globalCallID;				//521
		GlobalCallID_t					oldGlobalCallID;			//522
		NationalIndication_t			nationalIndication;			//523
		RerouteAuthorisation_t			rerouteAuthorisation;		//524	/*RoutingService*/
		DigitsReportingCriteriaAdd_t	digitsReportingCriteriaAdd;	//525	/*RoutingService*/
		AnnouncementOrMusic_t			announcementOrMusic;		//526	/*RoutingService*/
		GlobalCallIDList_t				globalCallIDList;			//527	/*YB 270801*/

		OtherPrivateData_t				otherPrivateData;			//999

	}u;
} AlcatelCstaPrivObject_t;


// Private Event
typedef ConnectionID_t 	HeadOfQueueEvent_t;

typedef struct SupervisorAssistRequestEvent_t{
	DeviceID_t			agentDevice;
	ConnectionID_t		agentConnection;
	DeviceID_t			supervisorDevice;
	PrivBool			requestStatus;
	CorrelatorDataV2_t	correlatorData;
}SupervisorAssistRequestEvent_t;

typedef struct TrunkGroupSupervisionEvent_t{
	OctetString_t		trunkGroupNumberList;
	OctetString_t		outOfServiceTrunkNumberList;
	OctetString_t		busyTrunkNumberList;
	OctetString_t		freeTrunkNumberList;
}TrunkGroupSupervisionEvent_t;

/*RoutingService+*/
typedef struct TelephonyTonesGeneratedEvent_t {
	CSTAMonitorCrossRefID_t monitorCrossRefID;
	ConnectionID3_t connection;
	long toneGenerated;
}TelephonyTonesGeneratedEvent_t;
/*RoutingService-*/

/*YB 270801+*/
typedef struct BusyEvent_t{
	SubjectDeviceID_t failingDevice;
	CallingDeviceID_t callingDevice;
	CalledDeviceID_t calledDevice;
	CSTAEventCause_t		cause;
}BusyEvent_t;

typedef struct RemoteRecordFailedEvent_t{
	SubjectDeviceID_t recordedDevice;
	ConnectionID_t  connection;
	GlobalCallID_t	globalCallID;
	CSTAEventCause_t		cause;
}RemoteRecordFailedEvent_t;
/*YB 270801-*/

typedef struct OtherPrivateEvent_t{
	char    privateIdentifier[MAX_LENGTH_PRIVATE_IDENTIFIER];
	char    privateEvent[MAX_LENGTH_PRIVATE_EVENT];
	unsigned long lengthPrivateIdentifier;
	unsigned long lengthPrivateEvent;
}OtherPrivateEvent_t;

typedef struct AlcatelCstaPrivEvent_t {
	AlcatelCstaPrivEventId_t	alcatelCstaPrivEventId;
	union{
		 HeadOfQueueEvent_t 			headOfQueueEvent;				//1000
		 SupervisorAssistRequestEvent_t	supervisorAssistRequestEvent;	//1001
		 TrunkGroupSupervisionEvent_t	trunkGroupSupervisionEvent;		//1002
		TelephonyTonesGeneratedEvent_t telephonyTonesGeneratedEvent;	//1004 /*RoutingService*/
/*YB 270801+*/
		 BusyEvent_t					busyEvent;						//1005
		 RemoteRecordFailedEvent_t		remoteRecordFailedEvent;		//1006
/*YB 270801-*/
		 OtherPrivateEvent_t			otherPrivateEvent;				//1499
	}u;
} AlcatelCstaPrivEvent_t;

typedef CstaCommonArguments_t CSTAEscapeSvcReqEvent_t2;

#endif
