/***********************************************************/
/* Copyright (C) 1995 AT&T Bell Labs.  All rights reserved.*/
/***********************************************************/
/* abstract syntax: drvr_asn_tbl */
/* last module in input: Driver-OAM-services */
/* created: Thu Jan 26 11:39:07 1995 */
/* associated control file: drvrdefs.c */

#ifndef DRVRDEFS_H
#define DRVRDEFS_H

#include "tsplatfm.h"


#define          TSRV_DRIVEROAM_REQ 1
#define          TSRV_DRIVEROAM_CONF 2
#define          TSRV_DRIVEROAM 3

typedef struct TSRVDriverOAMReq_t {
    _Int             length;
    unsigned char   FAR *data;
} TSRVDriverOAMReq_t;

typedef struct TSRVDriverOAMConfEvent_t {
    _Int             length;
    unsigned char   FAR *data;
} TSRVDriverOAMConfEvent_t;

typedef struct TSRVDriverOAMEvent_t {
    _Int             length;
    unsigned char   FAR *data;
} TSRVDriverOAMEvent_t;


extern void FAR *drvr_asn_tbl;    /* encoder-decoder control table */

#endif
