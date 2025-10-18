/***************************************************************************
 *  Description:
 *  
 *  Arguments:
 *
 *  Returns:
 *      0 on success, non-zero error codes otherwise
 *
 *  History: 
 *  Date        Name        Modification
 *  2025-10-06  Jason Bacon Begin
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>     // exit()
#include <sysexits.h>
#include <arpa/inet.h>  // inet_ntoa()
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>     // strerror()
#include <errno.h>
#include <unistd.h>     // sleep()
#include <stdbool.h>

#include "ipmi-gather.h"
#include "network.h"
#include "misc.h"

FILE    *Log_stream;
bool    Debug = true;

int     main(int argc,char *argv[])

{
    int     listen_fd;
    struct sockaddr_in  server_address = { 0 };
    
    switch(argc)
    {
        case 1:
            break;
        
        default:
            return usage(argv);
    }
    
    Log_stream = stderr;
    
    /*
     *  Step 1: Create a socket for listening for new connections.
     */

    listen_fd = ipmi_listen(&server_address);
    
    /*
     *  Step 2: Accept new connections, and create a separate socket
     *  for communication with each new compute node.
     */
    
    while ( true )
        check_listen_fd(listen_fd);
        
    return EX_OK;
}


int     check_listen_fd(int listen_fd)

{
    int             msg_fd;
    ssize_t         bytes;
    char            *munge_payload;
    socklen_t       address_len = sizeof (struct sockaddr_in);
    uid_t           munge_uid;
    gid_t           munge_gid;
    struct sockaddr_in client_address = { 0 };
    
    bytes = 0;
    /* Accept a connection request */
    if ((msg_fd = accept(listen_fd,
            (struct sockaddr *)&client_address, &address_len)) == -1)
    {
        fprintf(stderr, "%s(): Error: accept() failed, even though select indicated listen_fd.\n",
                __FUNCTION__);
        return -1;
    }
    else
    {
        fprintf(stderr, "%s(): Accepted connection. fd = %d  addr = %s  port = %u\n",
                 __FUNCTION__, msg_fd, inet_ntoa(client_address.sin_addr),
                 client_address.sin_port);

        /* Read a message through the socket */
        // FIXME: May need to borrow safe_close from LPJS
        bytes = ipmi_recv_munge(msg_fd,
                     &munge_payload, 0, 0,
                     &munge_uid, &munge_gid,
                     ipmi_gather_safe_close);

        ipmi_debug("%s(): Got %zd byte message.\n", __FUNCTION__, bytes);
        printf("Incoming IP : %s\nuid : %d\ngid : %d\n%s\n",
                    inet_ntoa(client_address.sin_addr),
                    munge_uid, munge_gid, munge_payload);
        
        if ( bytes == IPMI_RECV_TIMEOUT )
        {
            fprintf(stderr, "%s(): Error: ipmi_recv_munge() timed out after %dus: %s, closing %d.\n",
                    __FUNCTION__, IPMI_CONNECT_TIMEOUT, strerror(errno), msg_fd);
            ipmi_gather_safe_close(msg_fd);
            // Nothing to free if munge_decode() failed, since it
            // allocates the buffer
            // free(munge_payload);
            return IPMI_RECV_TIMEOUT;
        }
        else if ( bytes == IPMI_RECV_FAILED )
        {
            fprintf(stderr, "%s(): Error: ipmi_recv_munge() failed (%zd bytes): %s, closing %d.\n",
                    __FUNCTION__, bytes, strerror(errno), msg_fd);
            ipmi_gather_safe_close(msg_fd);
            // Nothing to free if munge_decode() failed, since it
            // allocates the buffer
            // free(munge_payload);
            return IPMI_RECV_FAILED;
        }
        // bytes must be at least 1, or no mem is allocated
        else if ( bytes < 1 )
        {
            fprintf(stderr, "%s(): Bug: Invalid return code from ipmi_recv_munge(): %zu\n",
                     __FUNCTION__, bytes);
            return IPMI_RECV_FAILED;
        }
        
        free(munge_payload);
    }

    ipmi_send_munge(msg_fd, "Got it.\n", ipmi_no_close);
    ipmi_gather_safe_close(msg_fd);
    
    return bytes;
}


/***************************************************************************
 *  Use auto-c2man to generate a man page from this comment
 *
 *  Name:
 *      -
 *
 *  Library:
 *      #include <>
 *      -l
 *
 *  Description:
 *  
 *  Arguments:
 *
 *  Returns:
 *
 *  Examples:
 *
 *  Files:
 *
 *  Environment
 *
 *  See also:
 *
 *  History: 
 *  Date        Name        Modification
 *  2025-10-05  Jason Bacon Begin
 ***************************************************************************/

int     ipmi_listen(struct sockaddr_in *server_address)

{
    int     listen_fd;

    /*
     *  Create a socket endpoint to pair with the endpoint on the client.
     *  This only creates a file descriptor.  It is not yet bound to
     *  any network interface and port.
     *  AF_INET and PF_INET have the same value, but PF_INET is more
     *  correct according to BSD and Linux man pages, which indicate
     *  that a protocol family should be specified.  In theory, a
     *  protocol family can support more than one address family.
     *  SOCK_STREAM indicates a reliable stream oriented protocol,
     *  such as TCP, vs. unreliable unordered datagram protocols like UDP.
     */
    if ((listen_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr, "%s(): Error: Can't create listener socket.\n", __FUNCTION__);
        exit(EX_UNAVAILABLE);
    }

    /*
     *  Port on which to listen for new connections from compute nodes.
     *  Convert 16-bit port number from host byte order to network byte order.
     */
    server_address->sin_port = htons(IPMI_IP_TCP_PORT);
    
    // AF_INET = inet4, AF_INET6 = inet6
    server_address->sin_family = AF_INET;
    
    /*
     *  Listen on all local network interfaces for now (INADDR_ANY).
     *  We may allow the user to specify binding to a specific IP address
     *  in the future, for multihomed servers acting as gateways, etc.
     *  Convert 32-bit host address to network byte order.
     */
    server_address->sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind socket fd and server address
    while ( bind(listen_fd, (struct sockaddr *)server_address,
              sizeof (*server_address)) < 0 )
    {
        fprintf(stderr,"%s(): Error: bind() failed: %s\n", __FUNCTION__, strerror(errno));
        fprintf(stderr,"%s(): Retry in 10 seconds...\n", __FUNCTION__);
        sleep(10);
    }
    fprintf(stderr,"%s(): Bound to port %d...\n", __FUNCTION__, IPMI_IP_TCP_PORT);
    
    /*
     *  Create queue for incoming connection requests
     */
    if (listen(listen_fd, IPMI_CONNECTION_QUEUE_MAX) != 0)
    {
        fprintf(stderr, "%s(): Error: listen() failed.\n", __FUNCTION__);
        exit(EX_UNAVAILABLE);
    }
    return listen_fd;
}


int     usage(char *argv[])

{
    fprintf(stderr, "Usage: %s\n", argv[0]);
    return EX_USAGE;
}
