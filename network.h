#ifndef _NETWORK_H_
#define _NETWORK_H_

#ifdef  __cplusplus
extern "C" {
#endif

#define IPMI_IP_TCP_PORT            20000
#define IPMI_CONNECTION_QUEUE_MAX   128
#define IPMI_NO_SELECT_TIMEOUT      NULL

// IPv6 max address size is 39
#define IPMI_TEXT_IP_ADDRESS_MAX    64
#define IPMI_PAYLOAD_MAX            64*1024
// FIXME: 4096 is just a guestimate
#define IPMI_MSG_LEN_MAX            IPMI_PAYLOAD_MAX + 4096

// Must be <= 0, since recv returns number of bytes
#define IPMI_RECV_FAILED    -1  // bytes returned
#define IPMI_RECV_TIMEOUT   -2  // bytes returned
// FIXME: Getting spurious timeouts on dispatch response
// Keep timeouts small so dispatchd doesn't hang waiting for a msg
#define IPMI_CHAPERONE_STATUS_TIMEOUT   500000
#define IPMI_PRINT_RESPONSE_TIMEOUT     500000
#define IPMI_CONNECT_TIMEOUT            500000

#define IPMI_MUNGE_CRED_VERIFIED_MSG    "MCD"
#define IPMI_MSG_SENT       0
#define IPMI_SEND_FAILED    -2
#define IPMI_MUNGE_FAILED   -3

#include "network-protos.h"

#ifdef  __cplusplus
}
#endif

#endif  // #ifndef _NETWORK_H_
