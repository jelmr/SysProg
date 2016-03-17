#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>

static volatile int done = 0;
static volatile int bit_read = 0;
static volatile int bit_value = 0;
static int          server_pid = 0;


static void bit_zero_handler(int v, siginfo_t* si, void *unused);
static void bit_one_handler(int v, siginfo_t* si, void *unused);
static void done_handler(int v, siginfo_t* si, void *unused);
static void receive_and_print();

static void bit_zero_handler(int v, siginfo_t* si, void *unused)
{
    (void)(v);
    (void)(unused);

    if (si->si_pid == server_pid)
    {
        bit_value = 0;
        bit_read = 1;
    }
}
static void bit_one_handler(int v, siginfo_t* si, void *unused)
{
    (void)(v);
    (void)(unused);

    if (si->si_pid == server_pid)
    {
        bit_value = 1;
        bit_read = 1;
    }
}
static void done_handler(int v, siginfo_t* si, void *unused)
{
    (void)(v);
    (void)(unused);

    if (si->si_pid == server_pid)
        done = 1;
}

int main(int argc, char *argv[])
{
    assert(argc > 1);
    server_pid = (pid_t)strtoul(argv[1], 0, 10);

    kill(server_pid, SIGINT);

    struct sigaction zero_act;
    sigaction(SIGUSR1, 0, &zero_act);
    zero_act.sa_sigaction = bit_zero_handler;
    zero_act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &zero_act, 0);

    struct sigaction one_act;
    sigaction(SIGUSR2, 0, &one_act);
    one_act.sa_sigaction = bit_one_handler;
    one_act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR2, &one_act, 0);

    struct sigaction done_act;
    sigaction(SIGINT, 0, &done_act);
    done_act.sa_sigaction = done_handler;
    done_act.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &done_act, 0);

    receive_and_print();
}

/**
* Prints the data sent from the server (running sigsend).
*
* SIGUSR1 signals are considered as 0-bits.
* SIGUSR2 signals are considered as 1-bits.
* SIGINT is used to indicate the end of the data.
*
* Note that sigsend will not send another byte before it has received an
* acknowledgement in the form of a SIGINT signal.
*/
void receive_and_print()
{
    int character;
    int i;

    i = character = 0;

    while(!done)
    {
        if (bit_read)
        {
            character = character | (bit_value << (CHAR_BIT - i - 1));
            bit_read = 0;
            kill(server_pid, SIGINT);

            if (i == CHAR_BIT - 1)
            {
                printf("%c", character);
                i = 0;
                character = 0;
            }
            else
                i++;
        }
    }
}
