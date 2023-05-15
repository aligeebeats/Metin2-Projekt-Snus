#ifndef _WIN32
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

static volatile sig_atomic_t    sigflag;

static sigset_t                 newmask, oldmask, zeromask;

static void sig_usr(int signo)
{
	sigflag = 1;
	return;
}

void TELL_WAIT()
{
	if (signal(SIGUSR1, sig_usr) == SIG_ERR)
	{
		fprintf(stderr, "signal(SIGINT) error\n");
		exit(1);
	}

	if (signal(SIGUSR2, sig_usr) == SIG_ERR)
	{
		fprintf(stderr, "signal(SIGQUIT) error\n");
		exit(1);
	}

	sigemptyset(&zeromask);

	sigemptyset(&newmask);
	sigaddset(&newmask, SIGUSR1);
	sigaddset(&newmask, SIGUSR2);

	if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
	{
		fprintf(stderr, "SIG_BLOCK error\n");
		exit(1);
	}
}

void TELL_PARENT(pid_t pid)
{
	kill(pid, SIGUSR2);
}

void WAIT_PARENT(void)
{
	while (sigflag == 0)
		sigsuspend(&zeromask);

	sigflag = 0;

	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
	{
		fprintf(stderr, "SIG_SETMASK error\n");
		exit(1);
	}
}

void TELL_CHILD(pid_t pid)
{
	kill(pid, SIGUSR1);
}

void WAIT_CHILD(void)
{
	while (sigflag == 0)
		sigsuspend(&zeromask);

	sigflag = 0;

	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
	{
		fprintf(stderr, "SIG_SETMASK error\n");
		exit(1);
	}
}
#endif
