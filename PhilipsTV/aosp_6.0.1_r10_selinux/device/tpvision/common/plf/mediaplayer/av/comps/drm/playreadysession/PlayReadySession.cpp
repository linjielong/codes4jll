/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//#define LOG_NDEBUG 0
#define LOG_TAG "PlayReadySession"
#include <utils/Log.h>
#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/foundation/hexdump.h>

#include "PlayReadySession.h"
//#include "amp_client.h"
//#include "amp_client_support.h"

#include <curl/curl.h>
//#include <PlayReady.h>
#include <drmmanager.h>
//#include <drmmanagertypes.h>
#include <drmsecureclockstatus.h>
#include <drmutf.h>
#include <drmxmlparser.h>
#include <utils/String16.h>

#if defined(DPK_1_2_BERLIN)
//#include <OSAL_api.h>
#endif

#if defined(DPK_2_0)
#define PK_DRM_APP_CONTEXT DRM_APP_CONTEXT
#define PK_DRM_LICENSE_RESPONSE DRM_LICENSE_RESPONSE
#define PK_DRM_DECRYPT_CONTEXT DRM_DECRYPT_CONTEXT
#define PK_DRM_AES_COUNTER_MODE_CONTEXT DRM_AES_COUNTER_MODE_CONTEXT
#elif defined(DPK_1_2)
#define PK_DRM_APP_CONTEXT DRM_APP_CONTEXT
#define PK_DRM_LICENSE_RESPONSE DRM_LICENSE_RESPONSE
#define PK_DRM_DECRYPT_CONTEXT DRM_DECRYPT_CONTEXT
#define PK_DRM_AES_COUNTER_MODE_CONTEXT DRM_AES_COUNTER_MODE_CONTEXT
#endif

#define TRUST_ZONE

namespace android {

// All DRM_XXX structures in PR are defined by typedef and no tag name, so it's
// impossible to forward-declare them directly.  So we declare the twin classes
// and name them as DrmXxx.
struct DrmAppContext {
    PK_DRM_APP_CONTEXT i;
    DrmAppContext() { memset(&i, 0, sizeof(i)); }
};

struct DrmLicenseResponse {
    PK_DRM_LICENSE_RESPONSE i;
    DrmLicenseResponse() { memset(&i, 0, sizeof(i)); }
};

struct DrmDecryptContext {
    PK_DRM_DECRYPT_CONTEXT i;
    DrmDecryptContext() { memset(&i, 0, sizeof(i)); }
};

struct DrmAesCtsContext {
    PK_DRM_AES_COUNTER_MODE_CONTEXT i;
    DrmAesCtsContext() { memset(&i, 0, sizeof(i)); }
};

static const DRM_CONST_STRING *sRights[1] = {
    &g_dstrWMDRM_RIGHT_PLAYBACK
};

int PlayReadySession::pub_DigCompVideo=0;
int PlayReadySession::pub_DigUnCompVideo=0;
int PlayReadySession::pub_DigCompAudio=0;
int PlayReadySession::pub_DigUnCompAudio=0;
int PlayReadySession::pub_AnalogVideo=0;
#ifdef _BNO_OPL_
char PlayReadySession::pub_uuid[16] = "0000000000000000";
#endif
void PlayReadySession::Set_Opl()
{
mDigCompVideo=PlayReadySession::pub_DigCompVideo;
mDigUnCompVideo=PlayReadySession::pub_DigUnCompVideo;
mDigCompAudio=PlayReadySession::pub_DigCompAudio;
mDigUnCompAudio=PlayReadySession::pub_DigUnCompAudio;
mAnalogVideo=PlayReadySession::pub_AnalogVideo;
#ifdef _BNO_OPL_
memcpy(muuid,PlayReadySession::pub_uuid ,16);
#endif
	    ALOGD("PRsession set_opl mDigCompVideo = %d mDigUnCompVideo = %d mDigCompAudio = %d  mDigUnCompAudio = %d \n",mDigCompVideo,mDigUnCompVideo,mDigCompAudio,mDigUnCompAudio);	

}
#ifdef _BNO_OPL_
void PlayReadySession::Get_Opl(int &DigCompVid,int& DigUnCompVid,int &DigCompAud,int& DigUncompAud,int& AnalogVid, String8& AudEnabler)
#else
void PlayReadySession::Get_Opl(int &DigCompVid,int& DigUnCompVid,int &DigCompAud,int& DigUncompAud,int& AnalogVid)
#endif
{

DigCompVid=mDigCompVideo;
DigUnCompVid=mDigUnCompVideo;
DigCompAud=mDigCompAudio;
DigUncompAud=mDigUnCompAudio;
AnalogVid=mAnalogVideo;
#ifdef _BNO_OPL_
AudEnabler.setTo(muuid);
#endif

#ifdef _BNO_OPL_
ALOGD("PRsession get_opl DigCompVid = %d DigUnCompVid = %d DigCompAud = %d  mDigUnCompAudio = %d muuid = %s \n",DigCompVid,DigUnCompVid,DigCompAud,DigUncompAud, muuid);	
#else
	    ALOGD("PRsession get_opl DigCompVid = %d DigUnCompVid = %d DigCompAud = %d  mDigUnCompAudio = %d \n",DigCompVid,DigUnCompVid,DigCompAud,DigUncompAud);	
#endif

}


/*** NOTICE ***********************************************************
 * OEM needs to define the following function properly according to   *
 * Compliance Rules for PlayReady Final Products.                     *
 **********************************************************************/
static DRM_RESULT PlayReadySession_PolicyCallback(const DRM_VOID *callbackData,
                                                  DRM_POLICY_CALLBACK_TYPE callbackType,
                                                  const DRM_VOID *customData ) {
    const DRM_PLAY_OPL_EX *oplPlay;
#ifdef _BNO_OPL_
	const DRM_EXTENDED_RESTRICTION_CALLBACK_STRUCT *extrights;
#endif
	ALOGI("OPL CALLBACK IN SESSION Enter \n");

    switch (callbackType) {
        case DRM_PLAY_OPL_CALLBACK:
		ALOGI("OPL CALLBACK IN SESSION\n");
            oplPlay = static_cast<const DRM_PLAY_OPL_EX*>(callbackData);
            if (oplPlay == NULL) break;  // No OPL data		
		PlayReadySession::pub_DigCompVideo=oplPlay->minOPL.wCompressedDigitalVideo;
		PlayReadySession::pub_DigUnCompVideo=oplPlay->minOPL.wUncompressedDigitalVideo;
		PlayReadySession::pub_DigCompAudio=oplPlay->minOPL.wCompressedDigitalAudio;
		PlayReadySession::pub_DigUnCompAudio=oplPlay->minOPL.wUncompressedDigitalAudio;
		PlayReadySession::pub_AnalogVideo=oplPlay->minOPL.wAnalogVideo;
		//DrmPlayReadyPlugin::Get_opl(1,2,3,4,5);

            ALOGD("=== DRM_PLAY_OPL_CALLBACK ===");
            ALOGD("    wCompressedDigitalVideo   = %d", oplPlay->minOPL.wCompressedDigitalVideo);
            ALOGD("    wUncompressedDigitalVideo = %d", oplPlay->minOPL.wUncompressedDigitalVideo);
            ALOGD("    wAnalogVideo              = %d", oplPlay->minOPL.wAnalogVideo);
            ALOGD("    wCompressedDigitalAudio   = %d", oplPlay->minOPL.wCompressedDigitalAudio);
            ALOGD("    wUncompressedDigitalAudio = %d", oplPlay->minOPL.wUncompressedDigitalAudio);
            // Secure Video Path: no need to check minOPL.wCompressedDigitalVideo.
            if (oplPlay->minOPL.wUncompressedDigitalVideo >= 301) {  // HDCP: 300
                return DRM_E_TEST_OPL_MISMATCH;
            }
            // No Analog Video Output: no need to check minOPL.wAnalogVideo.
            // Secure Audio Path: no need to check minOPL.wCompressedDigitalAudio.
            if (oplPlay->minOPL.wUncompressedDigitalAudio >= 301) {  // HDCP: 300
                return DRM_E_TEST_OPL_MISMATCH;
            }
            break;
#ifdef _BNO_OPL_
        case DRM_COPY_OPL_CALLBACK:
		ALOGI("OPL CALLBACK IN SESSION DRM_COPY_OPL_CALLBACK\n");
            // Do not support copy operation.
            return DRM_E_TEST_OPL_MISMATCH;

		case DRM_EXTENDED_RESTRICTION_CONDITION_CALLBACK:
			ALOGI("DRM_EXTENDED_RESTRICTION_CONDITION_CALLBACK");
			break;
		case DRM_EXTENDED_RESTRICTION_ACTION_CALLBACK:
			ALOGI("DRM_EXTENDED_RESTRICTION_CONDITION_CALLBACK");
			extrights = static_cast<const DRM_EXTENDED_RESTRICTION_CALLBACK_STRUCT*>(callbackData);
			if (extrights == NULL) break;  // No Extinsible rights
			memcpy(PlayReadySession::pub_uuid, extrights->pRestriction->pbBuffer, 16);
			break;
		case DRM_EXTENDED_RESTRICTION_QUERY_CALLBACK:
			ALOGI("DRM_EXTENDED_RESTRICTION_QUERY_CALLBACK");
			break;
#endif
        default:
		ALOGI("OPL CALLBACK IN SESSION DEFAULT:::\n");
            break;
    }
    return DRM_SUCCESS;
}

static int64_t filetimeToMs(DRMFILETIME* filetime) {
    int64_t ms;
    ms = filetime->dwLowDateTime + (static_cast<int64_t>(filetime->dwHighDateTime) << 32);
    // convert to milliseconds.
    ms /= C_TICS_PER_SECOND / 1000;
    // change base to ANSI time's.
    ms -= C_SECONDS_FROM_1601_TO_1970 * 1000;

    return ms;
}

// Convert UTF-16 string to null-terminated ASCII string.
// This function supposes the architecture is little-endian.
static void convertWStr2AStr(char *str, uint32_t size) {
    CHECK_NE(static_cast<char*>(NULL), str);
    for (uint32_t i = 2, j = 1; i < size; i += 2, ++j) {
        CHECK_EQ(0, str[i+1]);
        str[j] = str[i];
    }
    str[size / 2] = 0;
}

// TODO: OEM need to customize the following string.
// Don't use '/' as a separator.
const char PlayReadySession::kVersion[] = "berlin12";

String16 PlayReadySession::sDrmPath;

PlayReadySession::PlayReadySession()
    : mAppContext(NULL),
      mOpaqueBuffer(NULL),
      mRevocationBuffer(NULL),
      mLicenseResponse(NULL),
      mDecryptContext(NULL),
      mAesCtsContext(NULL) {

      //MV_OSAL_Init();
      ALOGD(" playready:PlayReadySession.cpp inital");
}

PlayReadySession::~PlayReadySession() {
	ALOGI("destructor function = %s line %d\n",__FUNCTION__,__LINE__);
	terminate();
	//MV_OSAL_Exit();
}

/* static */
void PlayReadySession::setDrmPath(const char* drmPath) {
    sDrmPath = String16(drmPath);
    // g_dstrDrmPath is the base directory where all certificates are stored.
    g_dstrDrmPath.pwszString = (DRM_WCHAR*)sDrmPath.string();
    g_dstrDrmPath.cchString = sDrmPath.size();
    ALOGD("the certificates dir: %s", g_dstrDrmPath.pwszString);
    setenv("KEYSTORE", "/data/mediadrm/playready", 1);
#if defined(DPK_1_2_BERLIN)
    setenv("KEYSTORE", "/factory_setting", 1);
    //MV_OSAL_Init();
#endif
}

bool PlayReadySession::initialize(const char* deviceStore) {
    DRM_RESULT dr = DRM_SUCCESS;


    if (deviceStore != NULL) {
	ALOGI("device store is %s\n",deviceStore);
}

    if (deviceStore != NULL) {
        mDeviceStore = String16(deviceStore);
    } else if (mDeviceStore.size() == 0) {
        ALOGE("%s: deviceStore is NULL.", __func__);
        return false;
    }
    DRM_CONST_STRING deviceStoreString = {
        (DRM_WCHAR*)mDeviceStore.string(),
        mDeviceStore.size()
    };

    mAppContext = new DrmAppContext;

#if defined(DPK_2_0)
    mOpaqueBuffer = new DRM_BYTE[MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE];
    memset(mOpaqueBuffer, 0, MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE);

    ChkDR(Drm_Initialize(&mAppContext->i,
                         NULL,
                         mOpaqueBuffer,
                         MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE,
                         &deviceStoreString));
	ALOGI("PR_2 %s function passed in %d line\n",__FUNCTION__,__LINE__);
#elif defined(DPK_1_2)
    ChkDR(Drm_Initialize(&mAppContext->i,
                         NULL,
                         &deviceStoreString));
#endif

    mRevocationBuffer = new DRM_BYTE[REVOCATION_BUFFER_SIZE];
    memset(mRevocationBuffer, 0, REVOCATION_BUFFER_SIZE);

    ChkDR(Drm_Revocation_SetBuffer(&mAppContext->i, mRevocationBuffer, REVOCATION_BUFFER_SIZE));
	ALOGI("PR_2 %s Revocatio set buffer passed in %d line\n",__FUNCTION__,__LINE__);

    return true;

ErrorExit:
    ALOGD("%s: DRM_RESULT %08x", __func__, dr);
    terminate();
    return false;
}

bool PlayReadySession::setDrmHeader(const uint8_t* drmHeader, size_t drmHeaderSize) {
    DRM_RESULT dr;
    ChkDR(Drm_Content_SetProperty(&mAppContext->i,
                                  DRM_CSP_AUTODETECT_HEADER,
                                  drmHeader,
                                  drmHeaderSize));
	ALOGI("PR_2 %s function passed in %d line\n",__FUNCTION__,__LINE__);
    return true;

ErrorExit:
    ALOGD("%s: DRM_RESULT %08x", __func__, dr);
    return false;
}
bool PlayReadySession::setDrmHeader_LeafLicense(const char* els, size_t elsSize, const char* kid, size_t kidSize)
{
	DRM_RESULT dr; 

	// Fill in the Data structures
	DRM_CSP_PLAYREADY_OBJ_WITH_KID_DATA obj_kid;
	//	DRM_BYTE **ppbOpaqueBuffer;

	// DRM_CSP_PLAYREADY_OBJ_WITH_KID_DATA is only available in 2.0
	obj_kid.pbHeaderData = (DRM_BYTE*)els;
	obj_kid.cbHeaderData = elsSize;
	obj_kid.pbKeyID = (DRM_BYTE*)kid;
	obj_kid.cbKeyID = kidSize;

	/*	dr = Drm_Content_SetProperty(&mAppContext->i,
		DRM_CSP_AUTODETECT_HEADER,
		reinterpret_cast<uint8_t*>(&obj_kid),
		sizeof(obj_kid));*/

	dr = Drm_Content_SetProperty(&mAppContext->i,
		DRM_CSP_PLAYREADY_OBJ_WITH_KID,
		reinterpret_cast<uint8_t*>(&obj_kid),
		sizeof(obj_kid));
	/*dr = Drm_Content_SetProperty(&mAppContext->i,
			DRM_CSP_AUTODETECT_HEADER,
			reinterpret_cast<const uint8_t*>(els),
			elsSize);*/

	/*
	   if (dr == DRM_E_BUFFERTOOSMALL)
	   {
	   DRM_RESULT dr1;

	   dr1 = Drm_GetOpaqueBuffer(&mAppContext->i, ppbOpaqueBuffer);

	   if (dr1 == 	DRM_SUCCESS)
	   {
	   DRM_RESULT dr2;
	// Allocate the new buffer
	mNewOpaqueBuffer = new DRM_BYTE[MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE];
	memset(mNewOpaqueBuffer, 0, MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE);

	dr2 = Drm_ResizeOpaqueBuffer(&mAppContext->i,mNewOpaqueBuffer,MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE );
	if(dr2 == DRM_SUCCESS)
	{
	delete mOpaqueBuffer;
	dr = Drm_Content_SetProperty(&mAppContext->i,
	DRM_CSP_AUTODETECT_HEADER,
	reinterpret_cast<uint8_t*>(&obj_kid),
	sizeof(obj_kid));
	if(dr1 != DRM_SUCCESS)
	{
	goto ErrorExit;
	}
	}
	}		
	}*/
	ChkDR(dr);
	return true;

ErrorExit:
	ALOGD("%s: DRM_RESULT %08x", __func__, dr);
	return false;
}
bool PlayReadySession::setKeyId(const char* kid, size_t kidSize) {
    DRM_RESULT dr;
    String16 kid16(kid, kidSize);
    ChkDR(Drm_Content_SetProperty(&mAppContext->i,
                                  DRM_CSP_KID,
                                  reinterpret_cast<const DRM_BYTE*>(kid16.string()),
                                  kid16.size() * sizeof(char16_t)));
    ALOGV("playready function %s, at line: %d", __FUNCTION__, __LINE__);
	ALOGI("PR_2 %s function passed in %d line\n",__FUNCTION__,__LINE__);
    return true;

ErrorExit:
    ALOGD("%s: DRM_RESULT %08x", __func__, dr);
    return false;
}

bool PlayReadySession::reinitialize() {
    terminate();
    // we remove initialize first to enable switch from Netflix and VUDU
    // until google's fix will be checked in.
    // initialize(NULL);
    return true;
}

bool PlayReadySession::generateLicenseChallenge(const char* customData, uint32_t customDataSize,
        char** urlOutput, char** challengeOutput, uint32_t* challengeSizeOutput) {
    ALOGV("playready function %s, at line: %d", __FUNCTION__, __LINE__);
    DRM_RESULT dr;
    DRM_CHAR* url = NULL;
    DRM_DWORD urlSize = 0;
    DRM_CHAR* challenge = NULL;
    DRM_DWORD challengeSize = 0;
    dr = Drm_LicenseAcq_GenerateChallenge(&mAppContext->i,
                                          sRights,
                                          NO_OF(sRights),
                                          NULL,
                                          const_cast<char*>(customData),
                                          customDataSize,
                                          NULL,
                                          &urlSize,
                                          NULL,
                                          NULL,
                                          NULL,
                                          &challengeSize);
    if (dr == DRM_E_NO_URL) {
        dr = Drm_LicenseAcq_GenerateChallenge(&mAppContext->i,
                                             sRights,
                                             NO_OF(sRights),
                                             NULL,
                                             const_cast<char*>(customData),
                                             customDataSize,
                                             NULL,
                                             NULL,
                                             NULL,
                                             NULL,
                                             NULL,
                                             &challengeSize);
    }
    if (dr != DRM_E_BUFFERTOOSMALL) {
        goto ErrorExit;
    }
    ALOGV("Drm_LicenseAcq_GenerateChallenge1: urlSize %d, challengeSize %d",
        urlSize, challengeSize);

    if (urlSize > 0) {
        url = new DRM_CHAR[urlSize];
    }
    challenge = new DRM_CHAR[challengeSize];
    dr = Drm_LicenseAcq_GenerateChallenge(&mAppContext->i,
                                           sRights,
                                           NO_OF(sRights),
                                           NULL,
                                           const_cast<char*>(customData),
                                           customDataSize,
                                           url,
                                           urlSize > 0 ? &urlSize : NULL,
                                           NULL,
                                           NULL,
                                           reinterpret_cast<DRM_BYTE*>(challenge),
                                           &challengeSize);
    ChkDR(dr);
    *urlOutput = url;
    *challengeOutput = challenge;
    *challengeSizeOutput = challengeSize;
	ALOGI("PR_2 %s function passed in %d line\n",__FUNCTION__,__LINE__);
    return true;

ErrorExit:
    ALOGD("%s: DRM_RESULT %08x", __func__, dr);
    delete[] challenge;
    delete[] url;
    return false;
}

bool PlayReadySession::processResponse(const char* responseBuffer, uint32_t responseBufferSize,
                                       bool* needAck) {
    ALOGV("playready function %s, at line: %d", __FUNCTION__, __LINE__);
    DRM_RESULT dr;
    if (mLicenseResponse == NULL) {
        mLicenseResponse = new DrmLicenseResponse;
    }
    // const_cast is for the wrongly declared header file of PlayReady.
    ChkDR(Drm_LicenseAcq_ProcessResponse(&mAppContext->i,
#if defined(DPK_2_0)
                                         DRM_PROCESS_LIC_RESPONSE_SIGNATURE_NOT_REQUIRED,
#endif
                                         NULL,
                                         NULL,
                                         reinterpret_cast<DRM_BYTE*>(
                                                 const_cast<char*>(responseBuffer)),
                                         responseBufferSize,
                                         &mLicenseResponse->i));
    ALOGV("Drm_LicenseAcq_ProcessResponse: type %d, status %d",
         mLicenseResponse->i.m_eType, (int)mLicenseResponse->i.m_dwResult);
    if (!DRM_SUCCEEDED(mLicenseResponse->i.m_dwResult)) {
        // TODO: need to handle this.
        ALOGE("Drm_LicenseAcq_ProcessResponse: %08x", (uint32_t)mLicenseResponse->i.m_dwResult);
        goto ErrorExit;
    }

    *needAck = (mLicenseResponse->i.m_eType == eV3Protocol) &&
            (mLicenseResponse->i.m_cbTransactionID > 0);
	ALOGI("PR_2 %s function passed in %d line\n",__FUNCTION__,__LINE__);
    return true;

ErrorExit:
    ALOGD("%s: DRM_RESULT %08x", __func__, dr);
    return false;
}

bool PlayReadySession::generateLicenseAck(char** ackOutput, uint32_t* ackSizeOutput) {
    ALOGV("playready function %s, at line: %d", __FUNCTION__, __LINE__);
    DRM_RESULT dr;
    DRM_DWORD ackSize = 0;
    DRM_CHAR *ack = NULL;
    if (mLicenseResponse == NULL) {
        ALOGE("Should call PlayReadySession::processResponse() first.");
        return false;
    }
    dr = Drm_LicenseAcq_GenerateAck(&mAppContext->i,
                                    &mLicenseResponse->i,
                                    NULL,
                                    &ackSize);
    if (dr != DRM_E_BUFFERTOOSMALL) {
        goto ErrorExit;
    }
    ack = new DRM_CHAR[ackSize];
    ChkDR(Drm_LicenseAcq_GenerateAck(&mAppContext->i,
                                     &mLicenseResponse->i,
                                     reinterpret_cast<DRM_BYTE*>(ack),
                                     &ackSize));
    *ackOutput = ack;
    *ackSizeOutput = ackSize;
	ALOGI("PR_2 %s function passed in %d line\n",__FUNCTION__,__LINE__);
    return true;

ErrorExit:
    ALOGD("%s: DRM_RESULT %08x", __func__, dr);
    delete[] ack;
    return false;
}

bool PlayReadySession::processAckResponse(const char* responseBuffer, uint32_t responseBufferSize) {
    ALOGV("playready function %s, at line: %d", __FUNCTION__, __LINE__);
    DRM_RESULT dr, dr2 = DRM_SUCCESS;
    // const_cast is for the wrongly declared header file of PlayReady.
    ChkDR(Drm_LicenseAcq_ProcessAckResponse(&mAppContext->i,
                                            reinterpret_cast<DRM_BYTE*>(
                                                    const_cast<char*>(responseBuffer)),
                                            responseBufferSize,
                                            &dr2));
    ALOGV("Drm_LicenseAcq_ProcessAckResponse: status %d", (int)dr2);
	ALOGI("PR_2 %s function passed in %d line\n",__FUNCTION__,__LINE__);
    return true;

ErrorExit:
    ALOGD("%s: DRM_RESULT %08x", __func__, dr);
    return false;
}

/* static */
bool PlayReadySession::supportSecureClock() {
    // TODO: OEM need to implement this properly.
#ifdef DPK_SUPPORT_SECURE_CLOCK
    return true;
#else
    return false;
#endif
}

bool PlayReadySession::generateSecureClockChallenge(
        char** urlOutput, char** challengeOutput, uint32_t* challengeSizeOutput) {
#ifdef DPK_SUPPORT_SECURE_CLOCK
    DRM_RESULT dr;
    DRM_DWORD secTimeSize = 0;
    DRM_DWORD flag = 0;
    char* url = NULL;
    char* challenge = NULL;
    dr = Drm_SecureClock_GetValue(&mAppContext->i,
                                  NULL,
                                  &secTimeSize,
                                  &flag,
                                  NULL,
                                  NULL);
    if (dr != DRM_E_BUFFERTOOSMALL) {
        goto ErrorExit;
    }
    DRM_WCHAR secTime[secTimeSize];
    ChkDR(Drm_SecureClock_GetValue(&mAppContext->i,
                                   secTime,
                                   &secTimeSize,
                                   &flag,
                                   NULL,
                                   NULL));
    if (DRM_CLK_NOT_SET == flag || DRM_CLK_NEEDS_REFRESH == flag) {
        DRM_DWORD urlSize = 0;
        DRM_DWORD challengeSize = 0;
        dr = Drm_SecureClock_GenerateChallenge(&mAppContext->i,
                                               NULL,
                                               &urlSize,
                                               NULL,
                                               &challengeSize);
        if (dr != DRM_E_BUFFERTOOSMALL) {
            goto ErrorExit;
        }
        url = new char[urlSize * SIZEOF(DRM_WCHAR)];
        challenge = new char[challengeSize];
        ChkDR(Drm_SecureClock_GenerateChallenge(&mAppContext->i,
                                                reinterpret_cast<DRM_WCHAR*>(url),
                                                &urlSize,
                                                reinterpret_cast<DRM_BYTE*>(challenge),
                                                &challengeSize));
        convertWStr2AStr(url, urlSize * SIZEOF(DRM_WCHAR));
        *urlOutput = url;
        *challengeOutput = challenge;
        *challengeSizeOutput = challengeSize;
    } else {
        *urlOutput = NULL;
    }
    ALOGV("playready function %s, at line: %d", __FUNCTION__, __LINE__);
    return true;

ErrorExit:
    ALOGD("%s: DRM_RESULT %08x", __func__, dr);
    delete[] challenge;
    delete[] url;
#endif
    return false;
}

bool PlayReadySession::processSecureClockResponse(const char* responseBuffer,
                                                  uint32_t responseBufferSize) {
#ifdef DPK_SUPPORT_SECURE_CLOCK
    DRM_RESULT dr, dr2 = DRM_SUCCESS;
    // const_cast is for the wrongly declared header file of PlayReady.
    ChkDR(Drm_SecureClock_ProcessResponse(&mAppContext->i,
                                          reinterpret_cast<DRM_BYTE*>(
                                                  const_cast<char*>(responseBuffer)),
                                          responseBufferSize,
                                          &dr2));
    ALOGV("Drm_SecureClock_ProcessResponse: status %d", (int)dr2);
    if (dr2 != DRM_SUCCESS) {
        dr = dr2;
        goto ErrorExit;
    }
    ALOGV("playready function %s, at line: %d", __FUNCTION__, __LINE__);
    return true;

ErrorExit:
    ALOGD("%s: DRM_RESULT %08x", __func__, dr);
#endif
    return false;
}

bool PlayReadySession::getKeyId(char** kidOutput) {
    DRM_RESULT dr;
    const int kDefaultKeyIdSize = 48;  // KID: 16 -> 24 (Base64) -> 48 (UTF-16)
    char* kid = new char[kDefaultKeyIdSize];
    DRM_DWORD kidSize = kDefaultKeyIdSize;
    dr = Drm_Content_GetProperty(&mAppContext->i,
                                 DRM_CGP_HEADER_KID,
                                 reinterpret_cast<DRM_BYTE*>(kid),
                                 &kidSize);
    if (dr == DRM_E_BUFFERTOOSMALL) {
        delete[] kid;
        kid = new char[kidSize];
        ChkDR(Drm_Content_GetProperty(&mAppContext->i,
                                      DRM_CGP_HEADER_KID,
                                      reinterpret_cast<DRM_BYTE*>(kid),
                                      &kidSize));
    }
    convertWStr2AStr(kid, kidSize);
    *kidOutput = kid;
    ALOGV("Drm_Content_GetProperty: DRM_CGP_HEADER_KID %s", kid);
    return true;

ErrorExit:
    ALOGD("%s: DRM_RESULT %08x", __func__, dr);
    delete[] kid;
    return false;
}

bool PlayReadySession::getLicenseState(
        uint32_t* state, uint32_t* count, int64_t* from, int64_t* until, uint32_t* vague) {
    DRM_RESULT dr;
    DRM_LICENSE_STATE_DATA licenseState;
    ChkDR(Drm_LicenseQuery_GetState(&mAppContext->i,
                                    sRights,
                                    NO_OF(sRights),
                                    &licenseState,
                                    NULL,
                                    NULL));
    *state =  licenseState.dwCategory;
    switch (licenseState.dwCategory) {
        case DRM_LICENSE_STATE_COUNT:
        case DRM_LICENSE_STATE_COUNT_FROM:
        case DRM_LICENSE_STATE_COUNT_UNTIL:
        case DRM_LICENSE_STATE_COUNT_FROM_UNTIL:
        case DRM_LICENSE_STATE_EXPIRATION_AFTER_FIRSTUSE:
            *count = licenseState.dwCount[0];
            break;
        default:
            *count = 0;
            break;
    }
    switch (licenseState.dwCategory) {
        case DRM_LICENSE_STATE_FROM:
        case DRM_LICENSE_STATE_FROM_UNTIL:
        case DRM_LICENSE_STATE_COUNT_FROM:
        case DRM_LICENSE_STATE_COUNT_FROM_UNTIL:
            *from = filetimeToMs(&licenseState.datetime[0]);
            break;
        default:
            *from = 0;
            break;
    }
    switch (licenseState.dwCategory) {
        case DRM_LICENSE_STATE_UNTIL:
        case DRM_LICENSE_STATE_COUNT_UNTIL:
            *until = filetimeToMs(&licenseState.datetime[0]);
            break;
        case DRM_LICENSE_STATE_FROM_UNTIL:
        case DRM_LICENSE_STATE_COUNT_FROM_UNTIL:
            *until = filetimeToMs(&licenseState.datetime[1]);
            break;
        default:
            *until = 0;
            break;
    }
    *vague = licenseState.dwVague;
    return true;

ErrorExit:
    ALOGD("%s: DRM_RESULT %08x", __func__, dr);
    return false;
}

bool PlayReadySession::checkRight() {
    DRM_RESULT dr;
    mDecryptContext = new DrmDecryptContext;
    ChkDR(Drm_Reader_Bind(&mAppContext->i,
                          sRights,
                          NO_OF(sRights),
                          PlayReadySession_PolicyCallback,
                          NULL,
                          &mDecryptContext->i));
    ChkDR(Drm_Reader_Commit(&mAppContext->i,
                            PlayReadySession_PolicyCallback,
                            NULL));
	Set_Opl();
    ALOGD("%s: DRM_RESULT , at line %d", __func__, __LINE__);
    mAesCtsContext = new DrmAesCtsContext;
    return true;

ErrorExit:
    ALOGD("%s: DRM_RESULT %08x", __func__, dr);
    delete mAesCtsContext;
    mAesCtsContext = NULL;
    delete mDecryptContext;
    mDecryptContext = NULL;
    return false;
}
bool PlayReadySession::leafLicenseCheckRight()
{
    DRM_RESULT dr;
    if(mDecryptContext!=NULL)
	    delete mDecryptContext;
    mDecryptContext = new DrmDecryptContext;//also allocated in check rights for root license
	
	ChkDR(Drm_Reader_Bind(&mAppContext->i,
                          sRights,
                          NO_OF(sRights),
                          PlayReadySession_PolicyCallback,
                          NULL,
                          &mDecryptContext->i));
	ALOGD("%s: DRM_RESULT , at line %d", __func__, __LINE__);
	return true;

ErrorExit:
    ALOGD("%s: DRM_RESULT %08x", __func__, dr);
    delete mDecryptContext;
    mDecryptContext = NULL;
    return false;
}
bool PlayReadySession::removeRight(uint32_t* numDeletedLicenses) {
    DRM_RESULT dr;
    ChkDR(Drm_StoreMgmt_DeleteLicenses(&mAppContext->i, NULL, numDeletedLicenses));
    ALOGV("Drm_StoreMgmt_DeleteLicenses: %u", *numDeletedLicenses);
    return true;

ErrorExit:
    ALOGD("%s: DRM_RESULT %08x", __func__, dr);
    return false;
}

bool PlayReadySession::removeRight(const char* kid, uint32_t* numDeletedLicenses) {
    DRM_RESULT dr;
    String16 kid16(kid);
    DRM_CONST_STRING kidString = {
        (DRM_WCHAR*)kid16.string(),
        kid16.size()
    };
    ChkDR(Drm_StoreMgmt_DeleteLicenses(&mAppContext->i, &kidString, numDeletedLicenses));
    ALOGV("Drm_StoreMgmt_DeleteLicenses: %u", *numDeletedLicenses);
    return true;

ErrorExit:
    ALOGD("%s: DRM_RESULT %08x", __func__, dr);
    return false;
}

// TODO: the following send and recevie functions will be removed,
// when this class is integrated to android DRM plugin.
static char sReceiveBuffer[0x8000];
static char* sReceivePtr;
static size_t receiveCallback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    ALOGV("receiveCallback: ptr %p, size %d, nmemb %d, userdata %p", ptr, size, nmemb, userdata);
    if (sReceivePtr + nmemb > sReceiveBuffer + sizeof(sReceiveBuffer)) {
        nmemb = sReceiveBuffer + sizeof(sReceiveBuffer) - sReceivePtr;
    }
    if (size > 1) {
        ALOGE("Will not handle multiple size");
    }
    memcpy(sReceivePtr, ptr, nmemb);
    sReceivePtr += nmemb;
    return nmemb;
}

static CURLcode sendSoapAction(const char* url, const char* action,
                               const char* data, const uint32_t size) {
    sReceivePtr = sReceiveBuffer;
    memset(sReceiveBuffer, 0, sizeof(sReceiveBuffer));
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: text/xml;");
    headers = curl_slist_append(headers, "charset=utf-8");
    headers = curl_slist_append(headers, action);

    CURL *curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, size);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, receiveCallback);
    CURLcode code = curl_easy_perform(curl_handle);

    curl_easy_cleanup(curl_handle);
    curl_slist_free_all(headers);
    ALOGV("curl_easy_perform1: %d\n", code);
    //android::hexdump(sReceiveBuffer, sReceivePtr - sReceiveBuffer);
    return code;
}

bool PlayReadySession::acquireRight(const char* urlOption) {
    bool hasRight = checkRight();
    if (hasRight) {
        ALOGV("playready function %s, at line: %d", __FUNCTION__, __LINE__);
        return true;
    }

    char* url = NULL;
    char* challenge = NULL;
    uint32_t challengeSize = 0;
    if (!generateLicenseChallenge(NULL, 0, &url, &challenge, &challengeSize) || url == NULL) {
        ALOGV("playready function %s, at line: %d", __FUNCTION__, __LINE__);
        return false;
    }
    if (urlOption != NULL) {
        char* newUrl = new char [strlen(url) + strlen(urlOption) + 1];
        strcpy(newUrl, url);
        strcat(newUrl, urlOption);
        delete[] url;
        url = newUrl;
    }
    if (CURLE_OK != sendSoapAction(url,
                                   "SOAPAction: \"http://schemas.microsoft.com/DRM/2007/03/"
                                   "protocols/AcquireLicense\"",
                                   challenge, challengeSize)) {
        delete[] challenge;
        ALOGV("playready function %s, at line: %d", __FUNCTION__, __LINE__);
        return false;
    }
    delete[] challenge;

    bool needAck = false;
    if (!processResponse(sReceiveBuffer,
                         sReceivePtr - sReceiveBuffer, &needAck)) {
        ALOGV("playready function %s, at line: %d", __FUNCTION__, __LINE__);
        return false;
    }

    if (needAck) {
        char* ack = NULL;
        uint32_t ackSize = 0;
        if (!generateLicenseAck(&ack, &ackSize) ||
                CURLE_OK != sendSoapAction(url,
                                           "SOAPAction: \"http://schemas.microsoft.com/DRM/2007/03/"
                                           "protocols/AcknowledgeLicense\"",
                                           ack, ackSize) ||
                !processAckResponse(sReceiveBuffer,
                                    sReceivePtr - sReceiveBuffer)) {
            delete[] ack;
            // Should we consider the failure of sending ACK as the failure of
            // acquiring the right?
            ALOGV("playready function %s, at line: %d", __FUNCTION__, __LINE__);
            return false;
        }
        ALOGV("Success at sending ACK.");
        delete[] ack;
    }
    delete[] url;
    return checkRight();
}

void PlayReadySession::setAesCtsContext(uint64_t iv, uint64_t blockOffset, uint8_t byteOffset) {
    mAesCtsContext->i.qwInitializationVector = iv;
    mAesCtsContext->i.qwBlockOffset = blockOffset;
    mAesCtsContext->i.bByteOffset = byteOffset;
}

bool PlayReadySession::decrypt(uint8_t* buffer, uint32_t size) {
    DRM_RESULT dr;
    ALOGV("playready function %s, at line: %d", __FUNCTION__, __LINE__);
    ChkDR(Drm_Reader_Decrypt(&mDecryptContext->i,
                             &mAesCtsContext->i,
                             buffer,
                             size));
	ALOGV("PR_2 %s function passed in %d line\n",__FUNCTION__,__LINE__);
    return true;
ErrorExit:
    ALOGD("%s: DRM_RESULT %08x", __func__, dr);
    return false;
}


/*** JLL.S20161013: Track for PR:TF516PHIEUMTK06-4512 ***/
#include <fcntl.h>
#include <ctype.h>
#include <android/log.h>
#include <cutils/properties.h>

#define JLLCONF_DRM_PR_VideoFile "/data/DRM_PR_Video.txt" 
#define JLLCONF_DRM_PR_AudioFile "/data/DRM_PR_Audio.txt"
static char  acDumpDrmCryptoInfo[2048];
static void JLL_Dump_DrmCryptoInfo(const char      *pcTag,
                                   DRM_CRYPTO_INFO *f_pcrypto,
                                   char            *pcBuffer)
{
    static int vfd = -1;
    static int afd = -1;
    int        fd = -1;
    uint32_t   u4BufferSize = 0;
    int propId = 0;
    int i4DumpDrmCryptoInfo = 0;
 
    propId = property_get_int32("persist.sys.drm", 0);
    if (f_pcrypto->dwSecureFlags  && (propId & 0x1)) { /* Video And need to be dump */
        if (vfd < 0) {
            vfd = open(JLLCONF_DRM_PR_VideoFile, O_CREAT | O_RDWR | O_APPEND, 0777);
            if (vfd < 0) {
                ALOGE("JLL@%s| Failed to open(%s)", __FUNCTION__, JLLCONF_DRM_PR_VideoFile);
                property_set("persist.sys.drm", "0");
                if (afd >= 0) {
                    close(afd);
                }
                return;
            }
        }
        fd = vfd;
        i4DumpDrmCryptoInfo = snprintf(acDumpDrmCryptoInfo, 2048,
            "\r\n%s\r\n" \
            "├── [Video] Drm Crypto Info\r\n", pcTag);
        write(fd, acDumpDrmCryptoInfo, i4DumpDrmCryptoInfo);
    }
    if (!f_pcrypto->dwSecureFlags && (propId & 0x2)) { /* Audio And need to be dump */
        if (afd < 0) {
            afd = open(JLLCONF_DRM_PR_AudioFile, O_CREAT | O_RDWR | O_APPEND, 0777);
            if (afd < 0) {
                ALOGE("JLL@%s| Failed to open(%s)", __FUNCTION__, JLLCONF_DRM_PR_VideoFile);
                property_set("persist.sys.drm", "0");
                if (vfd >= 0) {
                    close(vfd);
                }
                return;
            }
        }
        fd = afd;
        i4DumpDrmCryptoInfo = snprintf(acDumpDrmCryptoInfo, 2048,
            "\r\n%s\r\n" \
            "├── [Audio] Drm Crypto Info\r\n", pcTag);
        write(fd, acDumpDrmCryptoInfo, i4DumpDrmCryptoInfo);
    }

    if (fd < 0) {
        if (vfd >= 0) close(vfd);
        if (afd >= 0) close(afd);
        return;
    }

    i4DumpDrmCryptoInfo = snprintf(acDumpDrmCryptoInfo, 2048,
        "│   ├── dwSecureFlags=0x%X\r\n" \
        "│   ├── qwInitializationVector=%llu\r\n" \
        "│   ├── qwBlockOffset=%llu\r\n" \
        "│   └── cbNumSubSamples=%d\r\n",
        f_pcrypto->dwSecureFlags,
        f_pcrypto->qwInitializationVector,
        f_pcrypto->qwBlockOffset,
        f_pcrypto->cbNumSubSamples);
    write(fd, acDumpDrmCryptoInfo, i4DumpDrmCryptoInfo); 
 
    /* The below implement referenced to AOMXComponent::onInputBufferFilled(...) L1703 */ 
    for (uint32_t i = 0; i < f_pcrypto->cbNumSubSamples; ++i) {
        u4BufferSize += f_pcrypto->sub_samples[i].cbNumBytesOfClearData;
        u4BufferSize += f_pcrypto->sub_samples[i].cbNumBytesOfEncryptedData;
        if (i+1 < f_pcrypto->cbNumSubSamples) {
            i4DumpDrmCryptoInfo = snprintf(acDumpDrmCryptoInfo, 2048,
                "│       ├── [%d]{.cbNumBytesOfClearData=%d; .cbNumBytesOfEncryptedData=%d}\r\n",
                i,
                f_pcrypto->sub_samples[i].cbNumBytesOfClearData,
                f_pcrypto->sub_samples[i].cbNumBytesOfEncryptedData);
        } else {
            i4DumpDrmCryptoInfo = snprintf(acDumpDrmCryptoInfo, 2048,
                "│       └── [%d]{.cbNumBytesOfClearData=%d; .cbNumBytesOfEncryptedData=%d}\r\n",
                i,
                f_pcrypto->sub_samples[i].cbNumBytesOfClearData,
                f_pcrypto->sub_samples[i].cbNumBytesOfEncryptedData);
        }
        write(fd, acDumpDrmCryptoInfo, i4DumpDrmCryptoInfo);
    }

    /* Dump the content from buffer
     *
     * Demo As follows: 
     * 00000000: 68 65 6c 6c 6f 20 77 6f 72 6c 64 2c 20 78 78 78 | hello world, xxx
     * 00000016: 78 20 79 79 79 79 20 7a 7a 7a 7a 20 61 61 61 61 | x yyyy zzzz aaaa
     * 00000032: 20 62 62 62 62 62 62 62 62 62 62 20 2b 2d 2a 2a |  bbbbbbbbbb +-**
     * 00000048: 20 30 31 32 33 34 35 36 37 38 39 00 00 00 00 00 |  0123456789.....
     * 00000064: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................
     * 00000080: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................
     * 00000096: 00 00 00 00                                     | ....
     */
    write(fd, "└── Hex Content Of Buffer\r\n", 29);
    {
        uint32_t row, col, sz;
        unsigned char ch;

        sz = u4BufferSize / 16;
        for (row = 0; row < sz; row++) {
            /* Offset address */
            i4DumpDrmCryptoInfo = snprintf(acDumpDrmCryptoInfo, 2048, "    %08u: ", row * 16);
            /* Hexadecimal Notation Table */
            for (col = 0; col < 16; col++) {
                i4DumpDrmCryptoInfo += snprintf(acDumpDrmCryptoInfo + i4DumpDrmCryptoInfo, 2048,
                    "%02x ", pcBuffer[row * 16 + col]);
            }
            i4DumpDrmCryptoInfo += snprintf(acDumpDrmCryptoInfo + i4DumpDrmCryptoInfo, 2048, "| ");
            /* Visiable Character Table */
            for (col = 0; col < 16; col++) {
                ch = pcBuffer[row * 16 + col];
                if (isprint(ch)) {
                    i4DumpDrmCryptoInfo += snprintf(acDumpDrmCryptoInfo + i4DumpDrmCryptoInfo,
                        2048, "%c", ch);
                } else {
                    i4DumpDrmCryptoInfo += snprintf(acDumpDrmCryptoInfo + i4DumpDrmCryptoInfo,
                        2048, ".");
                }
            }
            i4DumpDrmCryptoInfo += snprintf(acDumpDrmCryptoInfo + i4DumpDrmCryptoInfo, 
                2048, "\r\n");
            write(fd, acDumpDrmCryptoInfo, i4DumpDrmCryptoInfo);
        }
        sz = u4BufferSize % 16;
        if (sz != 0) {
            /* Offset address */
            i4DumpDrmCryptoInfo = snprintf(acDumpDrmCryptoInfo, 2048, "    %08u: ", row * 16);
            /* Hexadecimal Notation Table */
            for (col = 0; col < 16; col++) {
                if (col < sz) {
                    i4DumpDrmCryptoInfo += snprintf(acDumpDrmCryptoInfo + i4DumpDrmCryptoInfo, 
                        2048, "%02x ", pcBuffer[row * 16 + col]);
                } else {
                    i4DumpDrmCryptoInfo += snprintf(acDumpDrmCryptoInfo + i4DumpDrmCryptoInfo, 
                        2048, "   ");
                }
            }
            /* Visiable Character Table */
            for (col = 0; col < 16; col++) {
                if (col >= sz) {
                    break;
                }
                ch = pcBuffer[row * 16 + col];
                if (isprint(ch)) {
                    i4DumpDrmCryptoInfo += snprintf(acDumpDrmCryptoInfo + i4DumpDrmCryptoInfo,
                        2048, "%c", ch);
                } else {
                    i4DumpDrmCryptoInfo += snprintf(acDumpDrmCryptoInfo + i4DumpDrmCryptoInfo,
                        2048, ".");
                }
            }
            i4DumpDrmCryptoInfo += snprintf(acDumpDrmCryptoInfo + i4DumpDrmCryptoInfo,
                2048, "\r\n");
            write(fd, acDumpDrmCryptoInfo, i4DumpDrmCryptoInfo);
        }
    }

    if (vfd >= 0) close(vfd);
    if (afd >= 0) close(afd);
}
/*** JLL.E20161013: Track for PR:TF516PHIEUMTK06-4512 ***/

bool PlayReadySession::TZ_decrypt(DRM_CRYPTO_INFO *f_pcrypto,char* EncBuffer,char* DecBuffer)
{
#ifdef TRUST_ZONE 

    DRM_RESULT dr;
    ALOGV("playready function %s, at line: %d", __FUNCTION__, __LINE__);
    //ALOGI("in %s %d line DecBuffer ptr is %p", __FUNCTION__, __LINE__,DecBuffer);

    { /*** JLL.S20161013: Track for PR:TF516PHIEUMTK06-4512 ***/
        JLL_Dump_DrmCryptoInfo("Before Drm_Reader_DecryptCENCSample(...)", f_pcrypto, EncBuffer);
    } /*** JLL.E20161013: Track for PR:TF516PHIEUMTK06-4512 ***/

    ChkDR(Drm_Reader_DecryptCENCSample(&mDecryptContext->i,
					f_pcrypto,
					(unsigned char*)EncBuffer,
					(unsigned char*)DecBuffer));		

    { /*** JLL.S20161013: Track for PR:TF516PHIEUMTK06-4512 ***/
        JLL_Dump_DrmCryptoInfo("After Drm_Reader_DecryptCENCSample(...)", f_pcrypto, DecBuffer);
    } /*** JLL.E20161013: Track for PR:TF516PHIEUMTK06-4512 ***/

	ALOGV("PR_2 %s function passed in %d line\n",__FUNCTION__,__LINE__);
    return true;
ErrorExit:
    ALOGD("%s: DRM_RESULT %08x", __func__, dr);
    return false;
#endif
    return false;

}

bool PlayReadySession::cleanupStore() {
    DRM_RESULT dr;
    ALOGV("playready function %s, at line: %d", __FUNCTION__, __LINE__);
#if defined(DPK_2_0)
    ChkDR(Drm_StoreMgmt_CleanupStore(&mAppContext->i,
                                     DRM_STORE_CLEANUP_DELETE_EXPIRED_LICENSES |
                                     DRM_STORE_CLEANUP_DELETE_REMOVAL_DATE_LICENSES,
                                     NULL,
                                     0,
                                     NULL));
#elif defined(DPK_1_2)
    ChkDR(Drm_StoreMgmt_CleanupStore(&mAppContext->i,
                                     NULL,
                                     0,
                                     NULL));
#endif
    return true;
ErrorExit:
    ALOGD("%s: DRM_RESULT %08x", __func__, dr);
    return false;
}

void PlayReadySession::terminate() {
    if (mAppContext != NULL) {
        Drm_Uninitialize(&mAppContext->i);
    }

		PlayReadySession::pub_DigCompVideo=0;
	PlayReadySession::pub_DigUnCompVideo=0;
	PlayReadySession::pub_DigCompAudio=0;
	PlayReadySession::pub_DigUnCompAudio=0;
	PlayReadySession::pub_AnalogVideo=0;

ALOGI("terminate::DigCompVideo is %d\n",PlayReadySession::pub_DigCompVideo);
ALOGI("terminate::DigUnCompVideo is %d\n",PlayReadySession::pub_DigUnCompVideo);
ALOGI("terminate::DigCompAudio is %d\n",PlayReadySession::pub_DigCompAudio);
ALOGI("terminate::DigUnCompAudio is %d\n",PlayReadySession::pub_DigUnCompAudio);
ALOGI("terminate::AnalogVideo is %d\n",PlayReadySession::pub_AnalogVideo);


    delete mAesCtsContext;
    mAesCtsContext = NULL;
    delete mDecryptContext;
    mDecryptContext = NULL;

    delete mLicenseResponse;
    mLicenseResponse = NULL;
    delete mRevocationBuffer;
    mRevocationBuffer = NULL;
    delete mOpaqueBuffer;
    mOpaqueBuffer = NULL;
    delete mAppContext;
    mAppContext = NULL;
    ALOGV("playready function %s, at line: %d", __FUNCTION__, __LINE__);
}
#if 1
bool PlayReadySession::joinDomain_GenerateChallenge(const char* customData,
								uint32_t customDataSize, 
								const uint8_t* serviceId,
								const uint8_t* accntId,
								char** challengeOutput,uint32_t* challengeSizeOutput)
{
    ALOGV("playready function %s, at line: %d", __FUNCTION__, __LINE__);
    DRM_RESULT dr;
    DRM_CHAR* challenge = NULL;
    DRM_DWORD challengeSize = 0; 
//	DRM_DWORD dwFlags = WMDRM_REGISTER_CUSTOM_DATA; // It can also be WMDRM_REGISTER_INDIRECT_QUERY_DATA
	DRM_DOMAIN_ID Domain_ID;
	//DRM_CHAR *f_pchData = customData; 

	// From HBBTV Playready Integration document, we see the APP gets the data individually.
	// If the falg is WMDRM_REGISTER_CUSTOM_DATA, then we have to decode the data and pass it to PK
	// If the flag is WMDRM_REGISTER_INDIRECT_QUERY_DATA, then we have to pass the f_pchData data pointing to below format.
	/*
    <JoinDomain type = \"challenge\">

        <Data>

          <ServiceID>Optional Base 64 encoded Service Account ID string</ServiceID>

          <AccountID>Optional Base 64 encoded Domain Account ID string</AccountID>

          <Revision>Optional Domain Revision</Revision>

          <FriendlyName>Optional Friendly Name</FriendlyName>

          <CustomData>Optional Custom Data</CustomData>

        </Data>

    </JoinDomain>*/


//	Domain_ID.m_oServiceID = *serviceId;
//	Domain_ID.m_oAccountID = *accntId;
//	Domain_ID.m_dwRevision = NULL;

	memcpy((DRM_GUID*)(&(Domain_ID.m_oServiceID)),(DRM_ID*)serviceId, 16);
	memcpy((DRM_GUID*)(&(Domain_ID.m_oAccountID)),(DRM_ID*)accntId, 16);
	Domain_ID.m_dwRevision = 0;

	dr = Drm_JoinDomain_GenerateChallenge(		
			&mAppContext->i,
			DRM_REGISTER_CUSTOM_DATA,
			&Domain_ID,		
			NULL,
			0,
			const_cast<char*>(customData),
			customDataSize,
			NULL,
			&challengeSize
		);

	if (dr != DRM_E_BUFFERTOOSMALL) {
        goto ErrorExit;
    }

	ALOGV("Drm_JoinDomain_GenerateChallenge: challengeSize %d",challengeSize);

    challenge = new DRM_CHAR[challengeSize];

	dr = Drm_JoinDomain_GenerateChallenge(		
			&mAppContext->i,
			DRM_REGISTER_CUSTOM_DATA,
			&Domain_ID,		
			NULL,
			0,
			const_cast<char*>(customData),
			customDataSize,
			reinterpret_cast<DRM_BYTE*>(challenge),
			&challengeSize
		);
	
	
	ChkDR(dr);
	*challengeOutput = challenge;
	*challengeSizeOutput = challengeSize;
	
	ALOGV("PASSED: Playready function %s, at line: %d", __FUNCTION__, __LINE__);
	return true;
	
	ErrorExit:
    delete[] challenge;
	ALOGD("%s: DRM_RESULT %08x", __func__, dr);
	return false;
}

bool PlayReadySession::leaveDomain_GenerateChallenge(const char* customData,
								uint32_t customDataSize, const uint8_t* serviceId,
								const uint8_t* accntId,
								char** challengeOutput,
								uint32_t* challengeSizeOutput)
{
    ALOGV("playready function %s, at line: %d", __FUNCTION__, __LINE__);
    DRM_RESULT dr;
    DRM_CHAR* challenge = NULL;
    DRM_DWORD challengeSize = 0; 
	//DRM_DWORD dwFlags = WMDRM_REGISTER_CUSTOM_DATA;
	DRM_DOMAIN_ID Domain_ID;
	//DRM_CHAR *f_pchData = customData; 

// If the falg is WMDRM_REGISTER_CUSTOM_DATA, then we have to decode the data and pass it to PK
//	Domain_ID.m_oServiceID = *serviceId;
//	Domain_ID.m_oAccountID = *accntId;
//	Domain_ID.m_dwRevision = NULL;  

	memcpy((DRM_GUID*)(&(Domain_ID.m_oServiceID)),(DRM_ID*)serviceId, 16);
	memcpy((DRM_GUID*)(&(Domain_ID.m_oAccountID)),(DRM_ID*)accntId, 16);
	Domain_ID.m_dwRevision = 0;	

	dr = Drm_LeaveDomain_GenerateChallenge(
			&mAppContext->i,
			DRM_REGISTER_CUSTOM_DATA,
			&Domain_ID,		
			const_cast<char*>(customData),
			customDataSize,
			NULL,
			&challengeSize
		);

	if (dr != DRM_E_BUFFERTOOSMALL) {
			goto ErrorExit;
		}
	
		ALOGV("leaveDomain_GenerateChallenge: challengeSize %d",challengeSize);

		challenge = new DRM_CHAR[challengeSize];
	
		dr = Drm_LeaveDomain_GenerateChallenge(		
				&mAppContext->i,
				DRM_REGISTER_CUSTOM_DATA,
				&Domain_ID, 	
				const_cast<char*>(customData),
				customDataSize,
				reinterpret_cast<DRM_BYTE*>(challenge),
				&challengeSize
			);

	
	ChkDR(dr);
	*challengeOutput = challenge;
	*challengeSizeOutput = challengeSize;
	
	ALOGV("PASSED: Playready function %s, at line: %d", __FUNCTION__, __LINE__);
	return true;
	
	ErrorExit:
		delete[] challenge;
		ALOGD("%s: DRM_RESULT %08x", __func__, dr);
		return false;
}


bool PlayReadySession::joinDomain_processResponse(const char* responseBuffer, uint32_t responseBufferSize)
{

	//DRM_PROCESS_DJ_RESPONSE_FLAG f_dwFlags = DRM_PROCESS_DJ_RESPONSE_NO_FLAGS;
    DRM_RESULT dr;
	DRM_RESULT result;
	DRM_DOMAIN_ID Domain_ID;

	dr = Drm_JoinDomain_ProcessResponse(&mAppContext->i,
			DRM_PROCESS_DJ_RESPONSE_NO_FLAGS,
			reinterpret_cast<DRM_BYTE*>(const_cast<char*>(responseBuffer)),
			responseBufferSize,
			&result,
			&Domain_ID
			);

	if(dr != DRM_SUCCESS)
		goto ErrorExit;
	
	ALOGV("playready function %s, at line: %d", __FUNCTION__, __LINE__);
	return true;
	
	ErrorExit:
	ALOGD("%s: DRM_RESULT %08x", __func__, dr);
	return false;
}

bool PlayReadySession::leaveDomain_processResponse(const char* responseBuffer, uint32_t responseBufferSize)
{
	//DRM_PROCESS_DJ_RESPONSE_FLAG f_dwFlags = DRM_PROCESS_DJ_RESPONSE_NO_FLAGS;
    DRM_RESULT dr;
	DRM_RESULT result;

	dr = Drm_LeaveDomain_ProcessResponse(&mAppContext->i,
									//	0x00, //DRM_PROCESS_DJ_RESPONSE_NO_FLAGS
										reinterpret_cast<DRM_BYTE*>(const_cast<char*>(responseBuffer)),
										responseBufferSize,
										&result
										);

	if(dr != DRM_SUCCESS)
		goto ErrorExit;

	ALOGV("Playready function %s, at line: %d", __FUNCTION__, __LINE__);
    return true;

	ErrorExit:
    ALOGD("%s: DRM_RESULT %08x", __func__, dr);
    return false;

}
#endif
}  // namespace android
