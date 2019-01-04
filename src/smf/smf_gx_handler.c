#define TRACE_MODULE _smf_gx_handler

#include "core_debug.h"

#include "smf_context.h"
#include "fd/gx/gx_message.h"
#include "smf_pfcp_path.h"
#include "smf_ipfw.h"

static status_t bearer_binding(smf_sess_t *sess, gx_message_t *gx_message);

void smf_gx_handle_cca_initial_request(
        smf_sess_t *sess, gx_message_t *gx_message)
{
    status_t rv;
    d_assert(sess, return, "Null param");
    d_assert(gx_message, return, "Null param");

    /* Send Create Session Request with Creating Default Bearer */
    rv = smf_pfcp_send_session_establishment_request(sess);
    d_assert(rv == CORE_OK, return, "Send N4 PFCP Session Establistment Request Failed");

    rv = bearer_binding(sess, gx_message);

}

void smf_gx_handle_cca_termination_request(
        smf_sess_t *sess, gx_message_t *gx_message)
{
    status_t rv;
    d_assert(sess, return, "Null param");
    d_assert(gx_message, return, "Null param");

    rv = smf_pfcp_send_session_deletion_request(sess);
    d_assert(rv == CORE_OK, return, "Send N4 PFCP Session Deletion Request Failed");
    
}

void smf_gx_handle_re_auth_request(
        smf_sess_t *sess, gx_message_t *gx_message)
{
    status_t rv;

    rv = bearer_binding(sess, gx_message);
    d_assert(rv == CORE_OK, return,);
}

static status_t bearer_binding(smf_sess_t *sess, gx_message_t *gx_message)
{
    int i, j;

    d_assert(sess, return CORE_ERROR,);
    d_assert(gx_message, return CORE_ERROR,);

    for (i = 0; i < gx_message->num_of_pcc_rule; i++)
    {
        status_t rv;
        
        smf_bearer_t *bearer = NULL;

        pcc_rule_t *pcc_rule = &gx_message->pcc_rule[i];
        int bearer_created = 0;
        int qos_presence = 0;
        int tft_presence = 0;

        d_assert(pcc_rule, return CORE_ERROR,);
        if (pcc_rule->name == NULL)
        {
            d_error("No PCC Rule Name");
            continue;
        }

        if (pcc_rule->type == PCC_RULE_TYPE_INSTALL)
        {
            bearer = smf_bearer_find_by_qci_arp(sess, 
                        pcc_rule->qos.qci,
                        pcc_rule->qos.arp.priority_level,
                        pcc_rule->qos.arp.pre_emption_capability,
                        pcc_rule->qos.arp.pre_emption_vulnerability);
            if (!bearer)
            {
                bearer = smf_bearer_add(sess);
                d_assert(bearer, return CORE_ERROR, "Null param");

                bearer->name = core_strdup(pcc_rule->name);
                d_assert(bearer->name, return CORE_ERROR,);

                memcpy(&bearer->qos, &pcc_rule->qos, sizeof(qos_t));
                d_assert(pcc_rule->num_of_flow, return CORE_ERROR,
                        "No Flow! [QCI:%d, ARP:%d,%d,%d]",
                        pcc_rule->qos.qci,
                        pcc_rule->qos.arp.priority_level,
                        pcc_rule->qos.arp.pre_emption_capability,
                        pcc_rule->qos.arp.pre_emption_vulnerability);

                bearer_created = 1;
            }
            else
            {
                d_assert(strcmp(bearer->name, pcc_rule->name) == 0,
                        return CORE_ERROR,
                        "PCC Rule Name Mismatched! [%s:%s]",
                        bearer->name, pcc_rule->name);

                if (pcc_rule->num_of_flow)
                {
                    /* Remove all previous flow */
                    smf_pf_remove_all(bearer);

                    /* Update Bearer Request will encode updated TFT */
                    tft_presence = 1;
                }

                if ((pcc_rule->qos.mbr.downlink &&
                    bearer->qos.mbr.downlink != pcc_rule->qos.mbr.downlink) ||
                    (pcc_rule->qos.mbr.uplink &&
                     bearer->qos.mbr.uplink != pcc_rule->qos.mbr.uplink) ||
                    (pcc_rule->qos.gbr.downlink &&
                    bearer->qos.gbr.downlink != pcc_rule->qos.gbr.downlink) ||
                    (pcc_rule->qos.gbr.uplink &&
                    bearer->qos.gbr.uplink != pcc_rule->qos.gbr.uplink))
                {
                    /* Update QoS parameter */
                    memcpy(&bearer->qos, &pcc_rule->qos, sizeof(qos_t));

                    /* Update Bearer Request will encode updated QoS parameter */
                    qos_presence = 1;
                }

                if (tft_presence == 0 && qos_presence == 0)
                {
                    d_warn("[IGNORE] Update Bearer Request : "
                            "Both QoS and TFT is NULL");
                    continue;
                }
            }

            for (j = 0; j < pcc_rule->num_of_flow; j++)
            {
                flow_t *flow = &pcc_rule->flow[j];
                smf_rule_t rule;
                smf_pf_t *pf = NULL;

                d_assert(flow, return CORE_ERROR, "Null param");
                d_assert(flow->description, return CORE_ERROR, "Null param");

                rv = smf_compile_packet_filter(&rule, flow->description);
                d_assert(rv == CORE_OK, return CORE_ERROR,
                        "Failed to compile packet filter");

                pf = smf_pf_add(bearer, pcc_rule->precedence);
                d_assert(pf, return CORE_ERROR, "Null param");

                memcpy(&pf->rule, &rule, sizeof(smf_rule_t));
                pf->direction = flow->direction;
            }

            if (bearer_created == 1)
            {
                //TODO: GTP_CREATE_BEARER_REQUEST_TYPE;
            }
            else
            {
                //TODO: GTP_UPDATE_BEARER_REQUEST_TYPE
            }
            //TODO: commit gtp xact
        }
        else if (pcc_rule->type == PCC_RULE_TYPE_REMOVE)
        {
            bearer = smf_bearer_find_by_name(sess, pcc_rule->name);
            d_assert(bearer, return CORE_ERROR,);
            //TODO: GTP_DELETE_BEARER_REQUEST_TYPE

            return CORE_OK;
        }
        else
            d_assert(0, return CORE_ERROR, "Invalid type(%d)", pcc_rule->type);
    }

    return CORE_OK;
}

