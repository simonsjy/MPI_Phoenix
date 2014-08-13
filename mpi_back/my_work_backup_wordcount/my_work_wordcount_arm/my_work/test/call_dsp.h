/*
 * video.h
 *
 * ============================================================================
 * Copyright (c) Texas Instruments Incorporated 2010
 *
 * Use of this software is controlled by the terms and conditions found in the
 * license agreement under which this software has been supplied or provided.
 * ============================================================================
 */

#ifndef _VIDEO_H
#define _VIDEO_H


/* Thread function prototype */
extern  "C" void RFOG_INIT(char *inbuf,char *outbuf);
extern  "C" void RFOG_PROCESS();
extern  "C" void RFOG_DESTROY();

#endif /* _VIDEO_H */
