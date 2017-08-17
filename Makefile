# Makefile para compilar los programas de este paquete.

# Copyright (C) 2017 Corporación de Desarrollo de la Región Los Andes.

# Autor: Alejandro J. Mujica (aledrums en gmail punto com)

# Este programa es software libre; Usted puede usarlo bajo los términos de la
# licencia de software GPL versión 2.0 de la Free Software Foundation.

# Este programa se distribuye con la esperanza de que sea útil, pero SIN
# NINGUNA GARANTÍA; tampoco las implícitas garantías de MERCANTILIDAD o
# ADECUACIÓN A UN PROPÓSITO PARTICULAR.
# Consulte la licencia GPL para más detalles. Usted debe recibir una copia
# de la GPL junto con este programa; si no, escriba a la Free Software
# Foundation Inc. 51 Franklin Street,5 Piso, Boston, MA 02110-1301, USA.

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
