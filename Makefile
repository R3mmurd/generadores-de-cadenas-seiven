# Makefile
# Copyright (C) 2017 Alejandro J. Mujica

# Este programa es software libre: puede redistribuirlo y/o modificarlo bajo
# los términos de la Licencia General Pública de GNU publicada por la Free
# Software Foundation, ya sea la versión 3 de la Licencia, o (a su elección)
# cualquier versión posterior.

# Este programa se distribuye con la esperanza de que sea útil pero SIN
# NINGUNA GARANTÍA; incluso sin la garantía implícita de MERCANTIBILIDAD o
# CALIFICADA PARA UN PROPÓSITO EN PARTICULAR. Vea la Licencia General Pública
# de GNU para más detalles.

# Usted ha debido de recibir una copia de la Licencia General Pública
# de GNU junto con este programa. Si no, vea <http://www.gnu.org/licenses/>.

# Cualquier solicitud de parte de los usuarios de este software, escribir a

# Alejandro Mujica

# aledrums@gmail.com

CXX = clang++

WARN = -Wall -Wextra -Wcast-align -Wno-sign-compare -Wno-write-strings \
       -Wno-parentheses 

FLAGS = -D_GLIBCXX__PTHREADS -std=c++14 $(WARN)

DBG = -O0 -g -DDEBUG $(FLAGS)

FAST = -Ofast -DNDEBUG $(FLAGS)

ALEPH = ~/Aleph-w

PQINC = /usr/include/postgresql

DBINC = -I $(PQINC) -I./DB

DBLIB = -L./DB -lpq -lDbAccess

INCLUDE =  -I. -I $(ALEPH)

LIBS = -L $(ALEPH) -lAleph -lgsl -lgslcblas

all: dbloader main-caev-gen

dbloader: models.o dbloader.C
	$(CXX) $(FAST) $(INCLUDE) $(DBINC) $@.C -o $@ models.o $(DBLIB) $(LIBS)

dbloader-dbg: models-dbg.o dbloader.C
	$(CXX) $(DBG) $(INCLUDE) $(DBINC) dbloader.C -o $@ models-dbg.o $(DBLIB) $(LIBS)

main-caev-gen: models.o caev-gen.o main-caev-gen.C
	$(CXX) $(FAST) $(INCLUDE) $@.C -o $@ models.o caev-gen.o $(LIBS)

main-caev-gen-dbg: models-dbg.o caev-gen-dbg.o main-caev-gen.C
	$(CXX) $(DBG) $(INCLUDE) main-caev-gen.C -o $@ models-dbg.o caev-gen-dbg.o $(LIBS)


models.o: models.H models.C
	$(CXX) $(FAST) $(INCLUDE) -c models.C

models-dbg.o: models.H models.C
	$(CXX) $(DBG) $(INCLUDE) -c models.C -o models-dbg.o

caev-gen.o: caev-gen.H caev-gen.C
	$(CXX) $(FAST) $(INCLUDE) -c caev-gen.C

caev-gen-dbg.o: caev-gen.H caev-gen.C	
	$(CXX) $(DBG) $(INCLUDE) -c caev-gen.C -o caev-gen-dbg.o

clean:
	$(RM) *~ *.o dbloader main-caev-gen *-dbg
