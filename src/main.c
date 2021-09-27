#ifdef REDIRECT

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

/**
 * Executes the command "grep Villanova < scores > out".
 *
 * @author Jim Glenn
 * @version 0.1 9/23/2004
 */

int main()
{
  int in, out;
  char *grep_args[] = {"grep", "Villanova", NULL};

  // open input and output files

  in = open("scores", O_RDONLY);
  out = open("out", O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);

  // replace standard input with input file

  dup2(in, 0);

  // replace standard output with output file

  dup2(out, 1);

  // close unused file descriptors

  close(in);
  close(out);

  // execute grep

  execvp("grep", grep_args);
}

#else //#elif defined PIPE

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

/**
 * Executes the command "cat scores | grep Villanova".  In this quick-and-dirty
 * implementation the parent doesn't wait for the child to finish and
 * so the command prompt may reappear before the child terminates.
 *
 * @author Jim Glenn
 * @version 0.1 9/23/2004
 */

#define READ_PIPE 0
#define WRITE_PIPE 1

int main()
{
    int pipefd[2];
    int pid;

    char *cat_args[] = {"cat", "scores", NULL};
    char *grep_args[] = {"grep", "Villanova", NULL};

    // make a pipe (fds go in pipefd[0] and pipefd[1])

    if (!pipe(pipefd)) {
        printf("Error when try create pipe");
    }

    pid = fork();

    if (pid == 0)
    {
        // child gets here and handles "grep Villanova"

        // replace standard input with input part of pipe

        dup2(pipefd[READ_PIPE], STDIN_FILENO);

        // close unused hald of pipe

        close(pipefd[WRITE_PIPE]);

        // execute grep

        execvp("grep", grep_args);
    }
    else
    {
        // parent gets here and handles "cat scores"

        // replace standard output with output part of pipe

        dup2(pipefd[WRITE_PIPE], STDOUT_FILENO);

        // close unused unput half of pipe

        close(pipefd[READ_PIPE]);

        // execute cat

        execvp("cat", cat_args);
    }
}

#endif
