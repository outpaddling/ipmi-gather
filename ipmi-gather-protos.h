/* ipmi-gather.c */
int main(int argc, char *argv[]);
int check_listen_fd(int listen_fd);
int ipmi_listen(struct sockaddr_in *server_address);
int usage(char *argv[]);
