#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define REQ_PIPE_NAME "REQ_PIPE_54973"
#define RESP_PIPE_NAME "RESP_PIPE_54973"

#define MAX_MESSAGE_SIZE 256

int main() {
    int respPipe = -1, reqPipe = -1;

    // Step 1: Create the response pipe
    if (mkfifo(RESP_PIPE_NAME, 0666) != 0) {
        perror("ERROR\ncannot create the response pipe\n");
        return 1;
    }

    // Step 2: Open the request pipe for reading
    reqPipe = open(REQ_PIPE_NAME, O_RDONLY);
    if (reqPipe == -1) {
        perror("ERROR\ncannot open the request pipe\n");
        unlink(RESP_PIPE_NAME);
        return 1;
    }

    // Step 3: Open the response pipe for writing
    respPipe = open(RESP_PIPE_NAME, O_WRONLY);
    if (respPipe == -1) {
        perror("ERROR\ncannot open the response pipe\n");
        close(reqPipe);
        unlink(RESP_PIPE_NAME);
        return 1;
    }

    // Step 4: Send the connection request message
    if (write(respPipe, "CONNECT!", 8) != 8) {
        perror("ERROR\n");
        close(reqPipe);
        close(respPipe);
        unlink(RESP_PIPE_NAME);
        return 1;
    }

    printf("SUCCESS\n");


    // Close the pipes when done
    close(reqPipe);
    close(respPipe);
    unlink(RESP_PIPE_NAME);

    return 0;
}

