#define TRACE_MODULE _upf_pfcp_path
#include "core_debug.h"
#include "core_pkbuf.h"

#include "3gpp_types.h"
#include "pfcp/pfcp_node.h"
#include "pfcp/pfcp_path.h"

#include "app/context.h"
#include "upf_context.h"
#include "upf_event.h"
#include "upf_pfcp_path.h"
#include "upf_n4_build.h"

static int _pfcp_recv_cb(sock_id sock, void *data)
{    
    event_t e;
    status_t rv;
    pkbuf_t *pkbuf = NULL;
    c_sockaddr_t from;
    pfcp_node_t *upf;
    pfcp_header_t *pfcp_h = NULL;

    d_assert(sock, return -1, "Null param");

    rv = pfcp_recvfrom(sock, &pkbuf, &from);
    if (rv != CORE_OK)
    {
        if (errno == EAGAIN)
            return 0;

        return -1;
    }

    d_assert(from.sin.sin_family == AF_INET, return -1,); // IPv4 only now
    
    pfcp_h = (pfcp_header_t*)pkbuf->payload;
    
    // Check version 
    if (pfcp_h->version > PFCP_VERSION)
    {
        unsigned char v_fail[8];
        pfcp_header_t *pfcp_out = (pfcp_header_t *)v_fail;
        
        d_info("Unsupported PFCP version: %d", pfcp_h->version);
        pfcp_out->flags = (PFCP_VERSION << 5); /* set version */
        pfcp_out->type = PFCP_VERSION_NOT_SUPPORTED_RESPONSE_TYPE;
        pfcp_out->length = htons(4); 
        pfcp_out->sqn_only = pfcp_h->sqn_only;
        core_sendto(sock, v_fail, 8, 0,&from);
        pkbuf_free(pkbuf);            
        return 0;    
    }
    
    upf = pfcp_find_node_sockaddr(&upf_self()->upf_n4_list, &from);
    if (!upf)
    {
        pfcp_f_seid_t f_seid;
        memset(&f_seid, 0, sizeof(f_seid));
        f_seid.ipv4 = 1;
        // f_seid.seid = ?
        f_seid.addr = from.sin.sin_addr.s_addr; 
        upf = pfcp_add_node_with_seid(&upf_self()->upf_n4_list, &f_seid,
            upf_self()->pfcp_port,
            context_self()->parameter.no_ipv4,
            context_self()->parameter.no_ipv6,
            context_self()->parameter.prefer_ipv4);
        d_assert(upf, return CORE_ERROR,);
            
        // list_append(&upf_self()->upf_n4_list, upf);//0518 debug
            
        // rv = pfcp_client(upf);
        // d_assert(rv == CORE_OK, return CORE_ERROR,);   
        upf->sock = upf_self()->pfcp_sock;
    }

#if 0    
    {
        char buf[CORE_ADDRSTRLEN];        

        if (pfcp_h->type == PFCP_HEARTBEAT_REQUEST_TYPE)
        {
            pkbuf_t *heartbeat_rsp;
            
            heartbeat_rsp = pfcp_handle_heartbeat_req(pkbuf, upf_self()->recovery_time);
            if (heartbeat_rsp)
            {
                ssize_t sent;
    
                /* Echo reply */
                d_trace(3, "[UPF] SEND Heartbeat Response to [%s]\n",
                        CORE_ADDR(&from, buf));
    
                sent = core_sendto(sock,
                        heartbeat_rsp->payload, heartbeat_rsp->len, 0, &from);
                if (sent < 0 || sent != heartbeat_rsp->len)
                {
                    d_error("core_sendto failed(%d:%s)", errno, strerror(errno));
                }
                pkbuf_free(heartbeat_rsp);                
            }
            // Finish handling heartbeat here
            pkbuf_free(pkbuf);
            return 0;
        }        
    }
#endif        
        
    d_assert(upf, return -1, "pfcp node not found");

    event_set(&e, UPF_EVT_N4_MESSAGE);
    event_set_param1(&e, (c_uintptr_t)pkbuf);
    event_set_param2(&e, (c_uintptr_t)upf);
    rv = upf_event_send(&e);
    if (rv != CORE_OK)
    {
        d_error("upf_event_send error");
        pkbuf_free(pkbuf);
        return 0;
    }

    return 0;
}

status_t upf_pfcp_open()
{
    status_t rv;

    rv = pfcp_server_list(&upf_self()->pfcp_list, _pfcp_recv_cb);
    d_assert(rv == CORE_OK, return CORE_ERROR,);
    rv = pfcp_server_list(&upf_self()->pfcp_list6, _pfcp_recv_cb);
    d_assert(rv == CORE_OK, return CORE_ERROR,);

    upf_self()->pfcp_sock = pfcp_local_sock_first(&upf_self()->pfcp_list);
    upf_self()->pfcp_sock6 = pfcp_local_sock_first(&upf_self()->pfcp_list6);
    upf_self()->pfcp_addr = pfcp_local_addr_first(&upf_self()->pfcp_list);
    upf_self()->pfcp_addr6 = pfcp_local_addr_first(&upf_self()->pfcp_list6);

    d_assert(upf_self()->pfcp_addr || upf_self()->pfcp_addr6,
            return CORE_ERROR, "No PFCP Server");
#if 0
    {
        pfcp_f_seid_t f_seid;
        memset(&f_seid, 0, sizeof(f_seid));
        f_seid.ipv4 = 1;
        //f_seid.seid = ?
        f_seid.addr = inet_addr("127.0.0.8"); //$ fixme: read from configuration file
        upf = pfcp_add_node_with_seid(&upf_self()->upf_n4_list, &f_seid,
            upf_self()->pfcp_port,
            context_self()->parameter.no_ipv4,
            context_self()->parameter.no_ipv6,
            context_self()->parameter.prefer_ipv4);
        d_assert(upf, return CORE_ERROR,);
            
        list_append(&upf_self()->upf_n4_list, upf);
            
        rv = pfcp_client(upf);
        d_assert(rv == CORE_OK, return CORE_ERROR,);        
    }
#endif     

    return CORE_OK;
}

status_t upf_pfcp_close()
{
    sock_delete_list(&upf_self()->pfcp_list);
    sock_delete_list(&upf_self()->pfcp_list6);

    return CORE_OK;
}


