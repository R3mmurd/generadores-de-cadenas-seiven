# Copyright (C) 2012
# Alejandro Mujica (amujica@cenditel.gob.ve)
# Erwin Paredes (eparedes@cenditel.gob.ve)
# José Ruiz (jruiz@cenditel.gob.ve)
# Rodolfo Rangel (rrangel@cenditel.gob.ve)
# Julie Vera (jvera@cenditel.gob.ve)

# CENDITEL Fundación Centro Nacional de Desarrollo e Investigación en
# Tecnologías Libres

# Este programa es software libre; Usted puede usarlo bajo los términos de la
# licencia de software GPL versión 2.0 de la Free Software Foundation.

# Este programa se distribuye con la esperanza de que sea útil, pero SIN
# NINGUNA GARANTÍA; tampoco las implícitas garantías de MERCANTILIDAD o
# ADECUACIÓN A UN PROPÓSITO PARTICULAR.
# Consulte la licencia GPL para más detalles. Usted debe recibir una copia
# de la GPL junto con este programa; si no, escriba a la Free Software
# Foundation Inc. 51 Franklin Street,5 Piso, Boston, MA 02110-1301, USA.

PQ = /usr/include/postgresql

CXX = g++ -Ofast -g 

AR = ar

LIBNAME = DbAccess

LOCALLIB = lib$(LIBNAME).a

WARN = -Wall -Wextra -Wcast-align -Wno-sign-compare -Wno-write-strings \
       -Wno-parentheses -Wno-unused-function

FLAGS = -std=c++14 -DDEBUG -D_GLIBCXX__PTHREADS $(WARN)

INCLUDE = -I . -I $(PQ)

SOURCES = dbProperties.C dbConnection.C dbQuery.C strQuery.C

OBJECTS = dbProperties.o dbConnection.o dbQuery.o strQuery.o

lib:
	$(RM) $(LOCALLIB)
	$(CXX) $(INCLUDE) -c $(SOURCES) $(FLAGS)
	$(AR) -cvq $(LOCALLIB) $(OBJECTS)

clean:
	$(RM) *~ *.o
clean-all:
	$(RM) *~ *.o $(LOCALLIB)

