/***************************************************************************
 *  Description:
 *      Submit a job to ipmi-gather
 *      FIXME: submit should print a warning if the script seems too complex.
 *
 *  History: 
 *  Date        Name        Modification
 *  2025-10-18  Jason Bacon Begin
 ***************************************************************************/

// System headers
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sysexits.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>          // open()
#include <errno.h>
#include <limits.h>         // PATH_MAX
#include <stdbool.h>

// Addons
#include <munge.h>
#include <xtend/string.h>
#include <xtend/file.h>     // xt_rmkdir()
#include <xtend/proc.h>     // xt_get_user_name()

// Project headers
#include "ipmi-send.h"
#include "network.h"

FILE *Log_stream;
bool Debug = true;

int     main (int argc, char *argv[])

{
    int     msg_fd;
    char    outgoing_msg[IPMI_MSG_LEN_MAX + 1],
            *payload_ptr,
            *gather_hostname,
            my_hostname[sysconf(_SC_HOST_NAME_MAX) + 1],
            user_name[USER_NAME_MAX + 1],
            group_name[GROUP_NAME_MAX + 1];
    FILE    *ipmi_stream;
    size_t  msg_size;

    gethostname(my_hostname, sysconf(_SC_HOST_NAME_MAX));
    
    // Shared functions may use ipmi_log
    Log_stream = stderr;
    
    if ( argc != 2 )
    {
        fprintf (stderr, "Usage: %s gather-hostname\n", argv[0]);
        return EX_USAGE;
    }
    gather_hostname = argv[1];
    
    if ( (msg_fd = ipmi_connect_to_gather(gather_hostname)) == -1 )
    {
        perror("ipmi-send: Failed to connect to dispatch");
        return EX_IOERR;
    }
    
    snprintf(outgoing_msg, IPMI_MSG_LEN_MAX + 1, "Hello!\n");

    if ( (ipmi_stream = popen("ipmitool chassis status", "r")) == NULL )
    {
        fprintf(stderr, ": Could not open %s for read: %s.\n",
                "ipmitool chassis status", strerror(errno));
        exit(EX_NOINPUT);
    }
    
    snprintf(outgoing_msg, IPMI_MSG_LEN_MAX + 1,
            "Local hostname : %s\n"
            "User name : %s\n"
            "Group name : %s\n",
            my_hostname, xt_get_user_name(user_name, USER_NAME_MAX + 1),
            xt_get_primary_group_name(group_name, GROUP_NAME_MAX + 1));
    
    payload_ptr = outgoing_msg + strlen(outgoing_msg);
    msg_size = fread(payload_ptr, 1, IPMI_MSG_LEN_MAX + 1, ipmi_stream);
    fclose(ipmi_stream);

    if ( msg_size == 0 )
        strlcpy(payload_ptr, "No IPMI data.", IPMI_MSG_LEN_MAX + 1);
    else
        payload_ptr[msg_size] = '\0';
    
    printf("%zu\n", msg_size);
    
    // FIXME: Exiting here causes dispatchd to crash

    if ( ipmi_send_munge(msg_fd, outgoing_msg, close) != IPMI_MSG_SENT )
    {
        perror("ipmi-send: Failed to send submit request to dispatch");
        close(msg_fd);
        return EX_IOERR;
    }
    
    ipmi_print_response(msg_fd, "ipmi-send");
    close(msg_fd);
    
    return EX_OK;
}
