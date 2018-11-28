#include <cJSON/cJSON.h>

#include "smf_context.h"

char* create_Bearer() {
    smf_self();
    char *string = NULL;
    cJSON *bearer = cJSON_CreateObject();


    cJSON *PLMNId = cJSON_AddObjectToObject(bearer, "PLMNId");
    cJSON_AddStringToObject(PLMNId, "mnc", "208");
    cJSON_AddStringToObject(PLMNId, "mcc", "93");

    cJSON_AddNumberToObject(bearer, "ebi", 5);
    cJSON_AddNumberToObject(bearer, "SGW_TEID", 2);
    cJSON_AddNumberToObject(bearer, "eNB_TEID", 1);

    string = cJSON_Print(bearer);

    cJSON_Delete(bearer);
    return string;
}
