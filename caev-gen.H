/*
  Este archivo contiene la definición de los algoritmos para construir cadenas
  por actividad económica.
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

# ifndef CAEVGEN_H
# define CAEVGEN_H

# include <regex>

# include <models.H>
# include <tpl_graph.H>

using NodeInfo = pair<CAEV *, NodePosition>;
using Net      = List_Graph<Graph_Node<NodeInfo>>;

void plot(const Net &, const string &);

void build_upstream(Net &, Net::Node *, year_t, CAEVLevel,
		    TreeMap<CAEV *, Net::Node *> &);

void build_downstream(Net &, Net::Node *, year_t, CAEVLevel,
		      TreeMap<CAEV *, Net::Node *> &);

void generate_caev_chain(const string &, const string &, year_t, const string &,
			 const string &);

# endif // CAEVGEN_H
