#include <iostream>
#include <libssh/libssh.h>


int connect_session(ssh_session session, char *hostname, char *username, char *password) {
    int rc;

    if (session == NULL) {
        fprintf(stderr, "Error creating new ssh session");
        exit(-1);
    }

    // set options
    ssh_options_set(session, SSH_OPTIONS_HOST, hostname);
    ssh_options_set(session, SSH_OPTIONS_USER, username);

    // connect to server
    rc = ssh_connect(session);
    if (rc != SSH_OK) {
        fprintf(stderr, "Error connecting to host: %s\n", ssh_get_error(session));
        ssh_free(session);
        exit(-1);
    }

    // authenticate using password
    rc = ssh_userauth_password(session, NULL, password);
    if (rc != SSH_AUTH_SUCCESS) {
        fprintf(stderr, "Error authenticating with password: %s\n", ssh_get_error(session));
        ssh_disconnect(session);
        ssh_free(session);
        exit(-1);
    }

    return rc;
}

int connect_channel (ssh_channel channel, ssh_session session) {
    int rc;
    if (channel == NULL) {
        fprintf(stderr, "Error creating channel: %s\n", ssh_get_error(channel));
        ssh_channel_free(channel);
        exit(-1);
    }

    // connect shell channel
    rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK) {
        fprintf(stderr, "Error opening session in channel: %s\n", ssh_get_error(session));
        ssh_channel_free(channel);
        exit(-1);
    }
    return rc;
}

int main() {
    int rc;

    // TODO: make hostname as an user input
    // TODO: make password as an user input
    char *hostname = (char*)"192.168.0.136";
    char *username = (char*)"root";
    char *password = (char*)"root";

    // create a new ssh session
    ssh_session red_pitaya_session = ssh_new();
    // connect session to host
    rc = connect_session(red_pitaya_session, hostname, username, password);
    //fprintf(stdout, "SSH Session Return %d\n", rc);

    // create shell channel
    ssh_channel shell_channel = ssh_channel_new(red_pitaya_session);
    // use session to connect channel
    rc = connect_channel(shell_channel, red_pitaya_session);
    //fprintf(stdout, "Channel Return %d\n", rc);

    // we can send commands now
    char buffer[1024];
    unsigned int nbytes;

    // TODO: make command as an user input

    printf("Executing remote command...\n");
    rc = ssh_channel_request_exec(shell_channel, "ls -alh");

    printf("Received:\n");
    nbytes = ssh_channel_read(shell_channel, buffer, sizeof(buffer), 0);
    while (nbytes > 0) {
        fwrite(buffer, 1, nbytes, stdout);
        nbytes = ssh_channel_read(shell_channel, buffer, sizeof(buffer), 0);
    }


    // close and free channel
    ssh_channel_close(shell_channel);
    ssh_channel_send_eof(shell_channel);
    ssh_channel_free(shell_channel);
    // close and free session
    ssh_disconnect(red_pitaya_session);
    ssh_free(red_pitaya_session);
    return 0;
}
