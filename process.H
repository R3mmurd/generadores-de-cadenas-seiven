# ifndef PROCESS_H
# define PROCESS_H

# include <unistd.h>
# include <sys/types.h>
# include <sys/wait.h>

class Process
{
public:
  static int exec(char const * command, char * args[])
  {    
    int   status = 0;
    pid_t pid;
    
    pid = fork();
    
    if (pid < 0)
      {      
	return -1;
      }
    else if (pid > 0)
      {
	if (waitpid(pid, &status, 0) != pid) {
	  status = -1;
	}
	
      }
    else
      {	
	execvp(command, args);
      }
    return status;
  }
};

# endif // PROCESS_H

