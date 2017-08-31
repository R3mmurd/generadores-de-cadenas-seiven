/*
  Este archivo contiene la instrumentación de los algoritmos para construir
  cadenas por producto.

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

# include <product-gen.H>
# include <levenshtein.H>
# include <process.H>

void plot(const Net & net, const ClusterizedNodes & nodes,
	  const string & output_name, year_t year)
{
  ofstream output("temp.dot");
  
  if (not output)
    {
      stringstream s;
      s << "No se pudo crear el archivo " << output_name;
      throw runtime_error(s.str());
    }
  
  output << "digraph g" << endl << "{" << endl
	 << "  rankdir = LR;\n"
	 << "  compound = true;\n";

  TreeMap<UE *, size_t> ue_map;
  size_t ue_counter = 0;

  size_t sub_ue_counter = 0;

  TreeMap<Net::Node *, size_t> nodes_map;
  size_t node_counter = 0;

  nodes.for_each([&] (auto & usn) {

      UE * ue = usn.first;

      string node_color;
      
      switch (get<1>(usn.second.get_root().second.get_first()->get_info()))
	{
	case NodePosition::ROOT: node_color = "seagreen"; break;
	case NodePosition::UPSTREAM: node_color = "orange"; break;
	case NodePosition::DOWNSTREAM: node_color = "cyan"; break;
	default: node_color = "white";
	}

      stringstream ue_title;
      ue_title << ue->name << "\\nRIF: " << ue->rif << "\\n\\n";

      output << "  subgraph cluster_" << ue_counter << endl
	     << "  {\n"
	     << "    style = filled;\n"
	     << "    fontcolor = white;\n"
	     << "    color = " << node_color << ";\n"
	     << "    label = \"" << ue_title.str() << "\";\n";


      usn.second.for_each([&] (auto & sn) {

	  SubUE * sub_ue = sn.first;
	  
	  stringstream sub_ue_title;
	  sub_ue_title << sub_ue->name << "\\n: "
		       << sub_ue->location << "\\n\\n";

	  output << "    subgraph cluster_" << ue_counter
		 << "_" << sub_ue_counter << endl
		 << "    {\n"
		 << "      style = filled;\n"
		 << "      fontcolor = black;\n"
		 << "      color = lightgray;\n"
		 << "      label = \"" << sub_ue_title.str() << "\";\n";

	  sn.second.for_each([&] (auto node) {

	      Product * product = get<0>(node->get_info());

	      const Production & production = product->production(year);

	      stringstream label;

	      label << "\\nCod. Arancelario: " << product->tariffcode->cod
		    << "\\n" << product->name << "\\n"
		    << "Producción: " << production.quantity
		    << " " << production.meassurement_unit << "\\n";

	      output << "      " << node_counter << "[shape=box label=\""
		     << label.str() << "\" style = filled color = white];"
		     << endl;
	      
	      nodes_map.insert(node, node_counter);
	      ++node_counter;
	    });
	  
	  output << "    }\n";

	  ++sub_ue_counter;
	  
	});
      
      output << "  }\n";

      ue_map.insert(ue, ue_counter);
      ++ue_counter;
      
    });

  for (Net::Arc_Iterator it(net); it.has_curr(); it.next())
    {
      Net::Arc  * a = it.get_curr();
      Net::Node * s = net.get_src_node(a);
      Net::Node * t = net.get_tgt_node(a);
      size_t s_idx = nodes_map[s];
      size_t t_idx = nodes_map[t];
      size_t cs_idx = ue_map[get<0>(s->get_info())->sub_ue->ue];
      size_t ct_idx = ue_map[get<0>(t->get_info())->sub_ue->ue];

      stringstream label;
      label << "Precio: " << a->get_info().first
	    << " Demanda: " << a->get_info().second;

      output << "  "   << s_idx << " -> " << t_idx
	     << "[lhead = cluster_" << ct_idx
	     << " ltail = cluster_" << cs_idx
	     << " label = \"" << label.str() << "\"];";
    }
}

bool exists_arc(Net::Node * s, Net::Node * t)
{
  for (Net::Node_Arc_Iterator it(s); it.has_curr(); it.next())
    if (it.get_tgt_node() == t)
      return true;

  return false;
}

void build_upstream(Net & net, Net::Node * root, year_t year,
		    unsigned short num_levels, unsigned short names_distance,
		    ClusterizedNodes & nodes,
		    TreeMap<Product *, Net::Node *> & nodes_map)
{
  if (num_levels == get<2>(root->get_info()))
    return;
  
  Product * product = get<0>(root->get_info());

# ifdef DEBUG
  cout << "Building upstream\n"
       << "Processing product: " << product->name << endl
       << "There are " << product->inputs.size() << " inputs\n";
# endif

  TreeSet<ProductRel, ProductRelCmp> product_set;

  product->inputs.for_each([&] (auto i) {

      assert(i->tariffcode != nullptr);
      
      const auto & purchases = i->production(year).purchases(year);
      
      purchases.for_each([&] (auto & purchase) {

	  Product * min_product = nullptr;
	  size_t min_dist = names_distance + 1;
	  
	  purchase.provider->filter_products
	    ([&](auto fp) {
	      
	      if (fp->tariffcode != i->tariffcode)
		return false;

	      if (not fp->production(year).sales(year)
		  .exists([&] (auto & sale) {
		      return sale.client == product->sub_ue->ue;
		    }))
		return false;

	      size_t dist = levenshtein(fp->name, i->name);

	      if (dist > names_distance)
		return false;
	      
	      if (dist < min_dist)
		{
		  min_product = fp;
		  min_dist = dist;
		}
	      
	      return false;
	      
	    });
	  
	  if (min_product != nullptr)
	    product_set.insert(make_pair(min_product,
					 make_pair(purchase.price,
						   purchase.quantity)));
	});
    });

  product_set.for_each([&] (auto p_rel) {

      auto product_ptr = p_rel.first;
      auto p = nodes_map.search(product_ptr);

      if (p == nullptr)
	{
	  Net::Node * s =
	    net.insert_node(make_tuple(product_ptr, NodePosition::UPSTREAM,
				       get<2>(root->get_info()) + 1));
	  net.insert_arc(s, root, p_rel.second);
	  nodes_map.insert(product_ptr, s);
	  nodes[product_ptr->sub_ue->ue][product_ptr->sub_ue].append(s);
	  build_upstream(net, s, year, num_levels, names_distance, nodes,
			 nodes_map);
	}
      else
	{
	  Net::Node * s = p->second;

	  if (not exists_arc(s, root))
	    net.insert_arc(s, root, p_rel.second);
	}
      
    });
}

void build_downstream(Net & net, Net::Node * root, year_t year,
		      unsigned short num_levels, unsigned short names_distance,
		      ClusterizedNodes & nodes,
		      TreeMap<Product *, Net::Node *> & nodes_map)
{
  if (num_levels == get<2>(root->get_info()))
    return;
  
  Product * product = get<0>(root->get_info());
  
# ifdef DEBUG
  cout << "Building downstream\n"
       << "Processing product: " << product->name << endl;
# endif

  TreeSet<ProductRel, ProductRelCmp> product_set;

  auto & sales = product->production(year).sales(year);
      
  sales.for_each([&] (auto & sale) {
      
      Input * min_input = nullptr;
      size_t min_dist = names_distance + 1;
      
      sale.client->filter_inputs([&] (auto input) {
	  
	  if (product->tariffcode != input->tariffcode)
	    return false;
	  
	  if (not input->production(year).purchases(year)
	      .exists([&] (auto & purchase) {
		  return purchase.provider == product->sub_ue->ue;
		}))
	    return false;
	  
	  size_t dist = levenshtein(product->name, input->name);
	  
	  if (dist > names_distance)
	    return false;
	  
	  if (dist < min_dist)
	    {
	      min_input = input;
	      min_dist = dist;
	    }
	  
	  return false;
	  
	});

      if (min_input != nullptr)
	product_set.insert(make_pair(min_input->product,
				     make_pair(sale.price, sale.quantity)));
    });
  
  product_set.for_each([&] (auto p_rel) {

      auto product_ptr = p_rel.first;
      auto p = nodes_map.search(product_ptr);

      if (p == nullptr)
	{
	  Net::Node * t =
	    net.insert_node(make_tuple(product_ptr, NodePosition::DOWNSTREAM,
				       get<2>(root->get_info()) + 1));
	  net.insert_arc(root, t, p_rel.second);
	  nodes_map.insert(product_ptr, t);
	  nodes[product_ptr->sub_ue->ue][product_ptr->sub_ue].append(t);
	  build_downstream(net, t, year, num_levels, names_distance, nodes,
			   nodes_map);
	}
      else
	{
	  Net::Node * t = p->second;

	  if (not exists_arc(root, t))
	    net.insert_arc(root, t, p_rel.second);
	}
      
    });
}

void generate_product_chain(db_id_t product_id, year_t year,
			    unsigned short num_levels_up,
			    unsigned short num_levels_down,
			    const string & input_name,
			    const string & output_name,
			    unsigned short names_distance)
{
# ifdef DEBUG
  cout << "Params:\n";
  cout << "Product id:       " << product_id << endl
       << "Year:             " << year << endl
       << "Num levels up:    " << num_levels_up << endl
       << "Num levels down:  " << num_levels_down << endl
       << "Input file name:  " << input_name << endl
       << "Output file name: " << output_name << endl
       << "Names distance:   " << names_distance << endl;
# endif

  Map map;

  ifstream in(input_name);

  if (not in)
    {
      stringstream s;
      s << "Archivo " << input_name << " no existe";
      throw logic_error(s.str());
    }

  map.load(in);
  in.close();

  if (map.years.search(year) == nullptr)
    {
      stringstream s;
      s << "No existen registros para el año " << year;
      throw domain_error(s.str());
    }

  Product p;
  p.db_id = product_id;
  Product * product = map.products.search(p);

  if (product == nullptr)
    {
      stringstream s;
      s << "No existe registrado el producto con id " << product_id;
      throw domain_error(s.str());
    }

# ifdef DEBUG
  cout << "Found product name: " << product->name << endl;
# endif

  Net net;
  Net::Node * root = net.insert_node(make_tuple(product, NodePosition::ROOT, 0));
  
  TreeMap<Product *, Net::Node *> nodes_map;
  nodes_map[product] = root;

  ClusterizedNodes nodes;
  nodes[product->sub_ue->ue][product->sub_ue].append(root);
  
  build_upstream(net, root, year, num_levels_up, names_distance, nodes,
		 nodes_map);
  build_downstream(net, root, year, num_levels_down, names_distance, nodes,
		   nodes_map);

  plot(net, nodes, output_name, year);
}
