/*
 * setsid.c -- execute a command in a new session
 * Rick Sladkey <jrs@world.std.com>
 * In the public domain.
 *
 * 1999-02-22 Arkadiusz Miśkiewicz <misiek@pld.ORG.PL>
 * - added Native Language Support
 *
 * 2001-01-18 John Fremlin <vii@penguinpowered.com>
 * - fork in case we are process group leader
 *
 * 2008-08-20 Daniel Kahn Gillmor <dkg@fifthhorseman.net>
 * - if forked,wait on child process and emit its return code.
 *
 * This version hacked by K.D.Hedger keithdhedger@gmail.com
 */

#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <err.h>

int main(int argc,char **argv)
{
	pid_t pid;
	int status=1;

	if (getpgrp()==getpid())
		{
			pid=fork();
			switch (pid)
				{
				case -1:
					err(EXIT_FAILURE,("fork"));
				case 0:
					break;
				default:
					if (wait(&status) != pid)
						err(EXIT_FAILURE,"wait");
					if (WIFEXITED(status))
						return WEXITSTATUS(status);
					err(status,("child %d did not exit normally"),pid);
				}
		}
	if (setsid() < 0)
		err(EXIT_FAILURE,("setsid failed"));

	execvp(argv[1],argv+1);
	err(EXIT_FAILURE,("failed to execute %s"),argv[0]);
}

