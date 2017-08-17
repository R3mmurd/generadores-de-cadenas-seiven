/*
  Este archivo contiene una la definición e instrumentación de una clase
  utilitaria que envuelve el llamado a procesos externos.
  
  Copyright (C) 2017 Corporación de Desarrollo de la Región Los Andes.

  Autor: Alejandro J. Mujica (aledrums en gmail punto com)
  
  Este programa es software libre; Usted puede usarlo bajo los términos de la
  licencia de software GPL versión 2.0 de la Free Software Foundation.
 
  Este programa se distribuye con la esperanza de que sea útil, pero SIN
  NINGUNA GARANTÍA; tampoco las implícitas garantías de MERCANTILIDAD o
  ADECUACIÓN A UN PROPÓSITO PARTICULAR.
  Consulte la licencia GPL para más detalles. Usted debe recibir una copia
  de la GPL junto con este programa; si no, escriba a la Free Software
  Foundation Inc. 51 Franklin Street,5 Piso, Boston, MA 02110-1301, USA.
*/

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

