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

// Project headers
#include "ipmi-send.h"
#include "network.h"

FILE *Log_stream;
bool Debug = true;

int     main (int argc, char *argv[])

{
    int     msg_fd;
    char    outgoing_msg[IPMI_MSG_LEN_MAX + 3],
            *hostname;

    // Shared functions may use ipmi_log
    Log_stream = stderr;
    
    if ( argc != 2 )
    {
        fprintf (stderr, "Usage: %s hostname\n", argv[0]);
        return EX_USAGE;
    }
    hostname = argv[1];
    
    if ( (msg_fd = ipmi_connect_to_gather(hostname)) == -1 )
    {
        perror("ipmi-send: Failed to connect to dispatch");
        return EX_IOERR;
    }
    
    // snprintf(outgoing_msg, IPMI_MSG_LEN_MAX + 3, "%s\n%s", job_string, script_text);

    // FIXME: Exiting here causes dispatchd to crash

    if ( ipmi_send_munge(msg_fd, outgoing_msg, close) != IPMI_MSG_SENT )
    {
        perror("ipmi-send: Failed to send submit request to dispatch");
        close(msg_fd);
        return EX_IOERR;
    }
    
    // ipmi_print_response(msg_fd, "ipmi send");
    
    return EX_OK;
}
