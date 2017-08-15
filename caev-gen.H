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
