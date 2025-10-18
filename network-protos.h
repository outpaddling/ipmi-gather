/* network.c */
ssize_t ipmi_recv_munge(int msg_fd, char **payload, int flags, int timeout, uid_t *uid, gid_t *gid, int (*close_function)(int));
ssize_t ipmi_recv(int msg_fd, char *buff, size_t buff_len, int flags, int timeout);
ssize_t ipmi_send(int msg_fd, int send_flags, const char *format, ...);
int ipmi_connect_to_gather(const char *hostname);
int ipmi_send_munge(int msg_fd, const char *msg, int (*close_function)(int));
