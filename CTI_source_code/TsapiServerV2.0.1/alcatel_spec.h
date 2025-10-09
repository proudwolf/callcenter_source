// alcatel_spec.h

#ifndef _alcatel_spec_h_
#define _alcatel_spec_h_



typedef struct {
	UINT callID;
	DeviceID_t deviceID;
	char devIDType;
	char correlator[40];
}alcPriv_t;


#endif // _alcatel_spec_h_

