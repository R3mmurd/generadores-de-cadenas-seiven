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

CXX = clang++

WARN = -Wall -Wextra -Wcast-align -Wno-sign-compare -Wno-write-strings \
       -Wno-parentheses 

FLAGS = -D_GLIBCXX__PTHREADS -std=c++14 $(WARN)

DBG = -O0 -g -DDEBUG $(FLAGS)

FAST = -Ofast -DNDEBUG $(FLAGS)

AR = ar

LIBNAME = DbAccess

LOCALLIB = lib$(LIBNAME).a

LOCALLIBDBG = lib$(LIBNAME)Dbg.a

WARN = -Wall -Wextra -Wcast-align -Wno-sign-compare -Wno-write-strings \
       -Wno-parentheses -Wno-unused-function

FLAGS = -std=c++14 -DDEBUG -D_GLIBCXX__PTHREADS $(WARN)

INCLUDE = -I . -I $(PQ)

SOURCES = dbProperties.C dbConnection.C dbQuery.C strQuery.C

OBJECTS = dbProperties.o dbConnection.o dbQuery.o strQuery.o

OBJECTSDBG = dbPropertiesDbg.o dbConnectionDbg.o dbQueryDbg.o strQueryDbg.o

libDbAccess.a: dbProperties.o dbConnection.o dbQuery.o strQuery.o autoConnection.H
	$(AR) -cvq $(LOCALLIB) $(OBJECTS)

libDbAccessDbg.a: dbPropertiesDbg.o dbConnectionDbg.o dbQueryDbg.o strQueryDbg.o autoConnection.H
	$(AR) -cvq $(LOCALLIBDBG) $(OBJECTSDBG)

dbProperties.o: dbProperties.H dbProperties.C
	$(CXX) $(FAST) $(INCLUDE) -c dbProperties.C

dbPropertiesDbg.o: dbProperties.H dbProperties.C
	$(CXX) $(DBG) $(INCLUDE) -c dbProperties.C -o dbPropertiesDbg.o

dbConnection.o: dbConnection.H dbConnection.C
	$(CXX) $(FAST) $(INCLUDE) -c dbConnection.C

dbConnectionDbg.o: dbConnection.H dbConnection.C
	$(CXX) $(FAST) $(INCLUDE) -c dbConnection.C -o dbConnectionDbg.o

dbQuery.o: dbQuery.H dbQuery.C
	$(CXX) $(FAST) $(INCLUDE) -c dbQuery.C

dbQueryDbg.o: dbQuery.H dbQuery.C
	$(CXX) $(FAST) $(INCLUDE) -c dbQuery.C -o dbQueryDbg.o

strQuery.o: strQuery.H strQuery.C
	$(CXX) $(FAST) $(INCLUDE) -c strQuery.C

strQueryDbg.o: strQuery.H strQuery.C
	$(CXX) $(FAST) $(INCLUDE) -c strQuery.C -o strQueryDbg.o

clean:
	$(RM) *~ *.o $(LOCALLIB) $(LOCALLIBDBG)
