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

DBDIR = DB

DBINC = -I $(PQINC) -I $(DBDIR)

DBLIB = -L $(DBDIR) -lpq -lDbAccess

DBLIBDBG = -L $(DBDIR) -lpq -lDbAccessDbg

INCLUDE =  -I. -I $(ALEPH)

LIBS = -L $(ALEPH) -lAleph -lgsl -lgslcblas

all: maploader main-caev-gen main-tariffcode-ue-gen main-tariffcode-product-gen main-ue-gen main-product-gen

maploader: DB/libDbAccess.a models.o maploader.C
	$(CXX) $(FAST) $(INCLUDE) $(DBINC) $@.C -o $@ models.o $(DBLIB) $(LIBS)

maploader-dbg: DB/libDbAccessDbg.a models-dbg.o maploader.C
	$(CXX) $(DBG) $(INCLUDE) $(DBINC) maploader.C -o $@ models-dbg.o $(DBLIBDBG) $(LIBS)

main-caev-gen: models.o caev-gen.o main-caev-gen.C
	$(CXX) $(FAST) $(INCLUDE) $@.C -o $@ models.o caev-gen.o $(LIBS)

main-caev-gen-dbg: models-dbg.o caev-gen-dbg.o main-caev-gen.C
	$(CXX) $(DBG) $(INCLUDE) main-caev-gen.C -o $@ models-dbg.o caev-gen-dbg.o $(LIBS)

main-tariffcode-ue-gen: models.o tariffcode-gen.o main-tariffcode-ue-gen.C
	$(CXX) $(FAST) $(INCLUDE) $@.C -o $@ models.o tariffcode-gen.o $(LIBS)

main-tariffcode-ue-gen-dbg: models.o tariffcode-gen-dbg.o main-tariffcode-ue-gen.C
	$(CXX) $(FAST) $(INCLUDE) main-tariffcode-ue-gen.C -o $@ models.o tariffcode-gen-dbg.o $(LIBS)

main-tariffcode-product-gen: models.o tariffcode-gen.o main-tariffcode-product-gen.C
	$(CXX) $(FAST) $(INCLUDE) $@.C -o $@ models.o tariffcode-gen.o $(LIBS)

main-tariffcode-product-gen-dbg: models.o tariffcode-gen-dbg.o main-tariffcode-product-gen.C
	$(CXX) $(FAST) $(INCLUDE) main-tariffcode-product-gen.C -o $@ models.o tariffcode-gen-dbg.o $(LIBS)

main-ue-gen: models.o ue-gen.o main-ue-gen.C
	$(CXX) $(FAST) $(INCLUDE) $@.C -o $@ models.o ue-gen.o $(LIBS)

main-ue-gen-dbg: models-dbg.o ue-gen-dbg.o main-ue-gen.C
	$(CXX) $(DBG) $(INCLUDE) main-ue-gen.C -o $@ models-dbg.o ue-gen-dbg.o $(LIBS)

main-product-gen: models.o product-gen.o main-product-gen.C
	$(CXX) $(FAST) $(INCLUDE) $@.C -o $@ models.o product-gen.o $(LIBS)

main-product-gen-dbg: models-dbg.o product-gen-dbg.o main-product-gen.C
	$(CXX) $(DBG) $(INCLUDE) main-product-gen.C -o $@ models-dbg.o product-gen-dbg.o $(LIBS)

models.o: models.H models.C
	$(CXX) $(FAST) $(INCLUDE) -c models.C

models-dbg.o: models.H models.C
	$(CXX) $(DBG) $(INCLUDE) -c models.C -o models-dbg.o

caev-gen.o: caev-gen.H caev-gen.C
	$(CXX) $(FAST) $(INCLUDE) -c caev-gen.C

caev-gen-dbg.o: caev-gen.H caev-gen.C	
	$(CXX) $(DBG) $(INCLUDE) -c caev-gen.C -o caev-gen-dbg.o

tariffcode-gen.o: tariffcode-gen.H tariffcode-gen.C
	$(CXX) $(FAST) $(INCLUDE) -c tariffcode-gen.C

tariffcode-gen-dbg.o: tariffcode-gen.H tariffcode-gen.C
	$(CXX) $(FAST) $(INCLUDE) -c tariffcode-gen.C -o tariffcode-gen-dbg.o

ue-gen.o: ue-gen.H ue-gen.C
	$(CXX) $(FAST) $(INCLUDE) -c ue-gen.C

ue-gen-dbg.o: ue-gen.H ue-gen.C	
	$(CXX) $(DBG) $(INCLUDE) -c ue-gen.C -o ue-gen-dbg.o

product-gen.o: product-gen.H product-gen.C
	$(CXX) $(FAST) $(INCLUDE) -c product-gen.C

product-gen-dbg.o: product-gen.H product-gen.C	
	$(CXX) $(DBG) $(INCLUDE) -c product-gen.C -o product-gen-dbg.o

DB/libDbAccess.a:
	$(MAKE) -C $(DBDIR) libDbAccess.a

DB/libDbAccessDbg.a:
	$(MAKE) -C $(DBDIR) libDbAccessDbg.a

clean:
	$(MAKE) -C $(DBDIR) clean
	$(RM) *~ *.o maploader main-caev-gen main-tariffcode-ue-gen main-tariffcode-product-gen main-ue-gen main-product-gen *-dbg
