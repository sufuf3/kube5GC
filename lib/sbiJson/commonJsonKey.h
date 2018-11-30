#ifndef __COMMONJSONKEY__H
#define __COMMONJSONKEY__H

#ifdef __cplusplus
extern "C"
{
#endif

#define JSONKEY_4G_IMSI                         "imsi"
#define JSONKEY_4G_EBI                          "ebi"
#define JSONKEY_4G_APN                          "apn"
#define JSONKEY_4G_SERVINGNFID                  "servingNfId"
#define JSONKEY_4G_GUMMEI                       "gummei"
#define JSONKEY_4G_SERVINGNETWORK               "servingNetwork"
    #define JSONKEY_4G_MMC                      "mcc"
    #define JSONKEY_4G_MNC                      "mnc"

#define JSONKEY_4G_USERLOCATIONINFO             "userLocationInfo"
    #define JSONKEY_4G_EUTRAOCATION             "eutraLocation"
        #define JSONKEY_4G_TAI                  "tai"
            #define JSONKEY_4G_PLMNID           "plmnId"
                /** #define JSONKEY_4G_ "mcc"*/
                /** #define JSONKEY_4G_ "mnc"*/
            #define JSONKEY_4G_TAC              "tac"
        #define JSONKEY_4G_ECGI                 "ecgi"
            /** #define JSONKEY_4G_ "plmnId"*/
                /** #define JSONKEY_4G_ "                mcc" */
                /** #define JSONKEY_4G_ "mnc" */
            #define JSONKEY_4G_EUTRACELLID      "eutraCellId"
#define JSONKEY_4G_UETIMEZONE                   "ueTimeZone"

#define JSONKEY_5G_SUPI                         "supi"
#define JSONKEY_5G_UnAuthenticatedSupi          "unauthenticatedSupi"
#define JSONKEY_5G_Pei                          "pei"
#define JSONKEY_5G_GPSI                         "gpsi"
#define JSONKEY_5G_PDUSessionID                 "pduSessionId"
#define JSONKEY_5G_DNN                          "dnn"
#define JSONKEY_5G_sNssai                       "sNssai"
#define JSONKEY_5G_HplmnsNssai                  "hplmnSnssai"
#define JSONKEY_5G_ServingNfID                  "servingNfId"//M
#define JSONKEY_5G_GUAMI                        "guami"
#define JSONKEY_5G_ServiceName                  "serviceName"
#define JSONKEY_5G_RequestType                  "requestType"
#define JSONKEY_5G_N1SmMsg                      "n1SmMsg"
#define JSONKEY_5G_AnType                       "anType"
#define JSONKEY_5G_RatType                      "ratType"
#define JSONKEY_5G_PresenceInLadn               "presenceInLadn"
#define JSONKEY_5G_UeTimeZone                   "ueTimeZone"
#define JSONKEY_5G_SmContextStatusUri           "smContextStatusUri"
#define JSONKEY_5G_HSmfUri                      "hSmfUri"
#define JSONKEY_5G_OldPduSessionId              "oldPduSessionId"
#define JSONKEY_5G_PduSessionsActivateList      "pduSessionsActivateList"
#define JSONKEY_5G_UeEpsPdnConnection           "ueEpsPdnConnection"
#define JSONKEY_5G_HoState                      "hoState"
#define JSONKEY_5G_AdditionalHsmfUri            "additionalHsmfUri"
#define JSONKEY_5G_PcfId                        "pcfId"
#define JSONKEY_5G_NrfUri                       "nrfUri"
#define JSONKEY_5G_SupportedFeatures            "supportedFeatures"
#define JSONKEY_5G_SelMode                      "selMode"
#define JSONKEY_5G_BackupAmfInfo                "backupAmfInfo"
#define JSONKEY_5G_TraceData                    "traceData"
#define JSONKEY_5G_EpsInterworkingInd           "epsInterworkingInd"

#ifdef __cplusplus
}
#endif 

#endif /** __COMMONJSONKEY__H*/