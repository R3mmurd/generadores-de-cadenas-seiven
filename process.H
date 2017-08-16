/*
  Este archivo contiene una la definición e instrumentación de una clase
  utilitaria que envuelve el llamado a procesos externos.
  Copyright (C) 2017 Alejandro J. Mujica
  
  Este programa es software libre: puede redistribuirlo y/o modificarlo bajo
  los términos de la Licencia General Pública de GNU publicada por la Free
  Software Foundation, ya sea la versión 3 de la Licencia, o (a su elección)
  cualquier versión posterior.
  
  Este programa se distribuye con la esperanza de que sea útil pero SIN
  NINGUNA GARANTÍA; incluso sin la garantía implícita de MERCANTIBILIDAD o
  CALIFICADA PARA UN PROPÓSITO EN PARTICULAR. Vea la Licencia General Pública
  de GNU para más detalles.
  
  Usted ha debido de recibir una copia de la Licencia General Pública
  de GNU junto con este programa. Si no, vea <http://www.gnu.org/licenses/>.

  Cualquier solicitud de parte de los usuarios de este software, escribir a

  Alejandro Mujica

  aledrums@gmail.com
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

