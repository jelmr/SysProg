#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>

static volatile pid_t request_pid = 0;
static volatile int   ready_to_send = 1;

static void request_handler(int v, siginfo_t *si, void *unused);
static void wait_handler(int v, siginfo_t *si, void *unused);
static void transmit_file(char *argv[]);
static void wait_for_request();

static void wait_handler(int v, siginfo_t *si, void *unused)
{
    (void)(v);
    (void)(unused);

    if (si->si_pid == request_pid)
        ready_to_send = 1;
}

static void request_handler(int v, siginfo_t *si, void *unused)
{
    (void)(v);
    (void)(unused);
    if (request_pid == 0)
        request_pid = si->si_pid;
}

int main(int argc, char *argv[])
{
    struct sigaction wait_act;

    assert(argc > 1);
    printf("%d\n", getpid());

    wait_for_request();

    sigaction(SIGINT, 0, &wait_act);
    wait_act.sa_sigaction = wait_handler;
    wait_act.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &wait_act, 0);

    transmit_file(argv);

    return 0;
}

/**
* Waits for a request from a client (sigrecv). A client will send a SIGINT
* signal to indicate it wants to receive the data.
*
* After this the PID of the client is stored in request_pid
*/
static void wait_for_request()
{
    struct sigaction request_act;

    sigaction(SIGINT, 0, &request_act);
    request_act.sa_sigaction = request_handler;
    request_act.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &request_act, 0);

    while (!request_pid);
}

/**
* Goes through the input file character by character, sending each single
* character to the client as individual bits.
*
* SIGUSR1 signals are considered as 0-bits.
* SIGUSR2 signals are considered as 1-bits.
* SIGINT is used to indicate the end of the data.
*
* After the first bit, subsequent bits will only be send after an
* acknowledgement in the form of a SIGINT signal has been received.
*/
static void transmit_file(char *argv[])
{
    FILE *file;
    int  c;
    int  i;
    int  bit;

    file = fopen(argv[1], "rb");
    assert(file != NULL);

    while(EOF != (c = fgetc(file)))
    {
        for (i = 0; i < CHAR_BIT; ++i)
        {
            while (!ready_to_send);

            bit = (c >> (CHAR_BIT - i - 1)) & 1;
            ready_to_send = 0;
            if (!bit)
                kill(request_pid, SIGUSR1);
            else
                kill(request_pid, SIGUSR2);

        }
    }
    while (!ready_to_send);
    kill(request_pid, SIGINT);
    fclose(file);
}
