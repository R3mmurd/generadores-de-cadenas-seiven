/*
  Este archivo contiene la instrumentación de los algoritmos para construir
  cadenas por código arancelario para las vistas de unidades económicas y 
  productos.

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

# include <tariffcode-gen.H>

void plot_by_ue(const Net & net, const string & output_name)
{
  plot(net, output_name, [] (auto tc) {

      stringstream s_label1, s_label2;

      auto sub_ues = tc->get_sub_ues();

      s_label1 << "Sub-unidades: " << sub_ues.size() << "\n";

      TreeSet<UE *> ues;

      sub_ues.for_each([&](auto sub_ue_ptr) {

	  UE * ue_ptr = sub_ue_ptr->ue;
	  ues.insert(ue_ptr);

	  s_label2 << "\\n" << "RIF: " << ue_ptr->rif << "\\n";
	  s_label2 << ue_ptr->name << "\\n";
	  s_label2 << sub_ue_ptr->name << "   -   "
		   << sub_ue_ptr->location << "\\n";
	  
	});

      s_label1 << "Unidades económica: " << ues.size() << "\\n"
	       << "\\n________________________\\n";

      return s_label1.str() + s_label2.str();
    });
}

void plot_by_product(const Net & net, const string & output_name)
{
  plot(net, output_name, [] (auto tc) {

      stringstream s_label1, s_label2;

      auto sub_ues = tc->get_sub_ues();

      size_t num_products = 0;

      sub_ues.for_each([&](auto sub_ue_ptr) {

	  UE * ue_ptr = sub_ue_ptr->ue;

	  s_label2 << "\\n________________________\\n";
	  s_label2 << "\\n" << "RIF: " << ue_ptr->rif << "\\n";
	  s_label2 << ue_ptr->name << "\\n";
	  s_label2 << sub_ue_ptr->name << "   -   "
		   << sub_ue_ptr->location << "\\n\\n";

	  sub_ue_ptr->products.for_each([&] (auto p) {
	      s_label2 << p->name << "\\n";
	      ++num_products;
	    });
	});

      s_label1 << "Productos: " << num_products << "\\n";

      return s_label1.str() + s_label2.str();
      
    });
}

bool exists_arc(Net::Node * s, Net::Node * t)
{
  for (Net::Node_Arc_Iterator it(s); it.has_curr(); it.next())
    if (it.get_tgt_node() == t)
      return true;

  return false;
}

void build_upstream(Net & net, Net::Node * root, year_t year,
		    TariffCodeLevel level,
		    TreeMap<TariffCode *, Net::Node *> & nodes_map)
{
  TariffCode * tc = root->get_info().first;
  
  // Lista de productos pertenecientes al CAEV
  auto products = tc->get_products();

  TreeSet<TariffCode *> tariffcode_set;

# ifdef DEBUG
  cout << "Building upstream\n"
       << "Processing tariffcode: " << caev->cod << endl
       << "There are " << products.size() << " products\n";
# endif

  products.for_each([&] (auto p) {

      // Miro cada insumo de p
      p->inputs.for_each([&] (auto i) {

	  assert(i->tariffcode != nullptr);
	  
	  // Obtengo las compras asociadas al insumo el año dado
	  const auto & purchases = i->production(year).purchases(year);
	  
	  purchases.for_each([&] (auto & purchase)  {
	      /* Filtro los productos del proveedor por código
		 arancelario. */
	      
	      auto filter_products =
		purchase.provider->filter_products
		([&](auto fp) {
		  if (fp->tariffcode != i->tariffcode)
		    return false;

		  return fp->production(year).sales(year)
		  .exists([&] (auto & sale) {
		      return sale.client == p->sub_ue->ue;
		    });
		});

	      filter_products.for_each([&](auto & fp) {
		  tariffcode_set.insert(fp->get_tariffcode(level));
		});
	    });
      	});
    });

   
  // Añadir nodos al grafo y conectarlos al raíz

  tariffcode_set.for_each([&] (TariffCode * c) {

      auto p = nodes_map.search(c);
      
      if (p == nullptr) 
	{
	  /* Si la actividad económica no ha sido añadida previamente al grafo,
	     entonces se inserta, se escribe en el mapa, se conecta al nodo raíz
	     y se recursiona sobre la nueva actividad añadida.
	  */
	  Net::Node * s = net.insert_node(make_pair(c, NodePosition::UPSTREAM));
	  net.insert_arc(s, root);
	  nodes_map.insert(c, s);
	  build_upstream(net, s, year, level, nodes_map);
	}
      else
	{
	  /* Si la actividad económica ha sido añadida previamente, entonces se
	     verifica que el nodo no sea el mismo nodo raíz, si no es el mismo
	     y no existe arco previo entre el nuevo nodo y el raíz, entonces
	     se conectan.
	  */
	  Net::Node * s = p->second;
	  
	  if (not exists_arc(s, root))
	    net.insert_arc(s, root);
	}
      
    });
}

void build_downstream(Net & net, Net::Node * root, year_t year,
		      TariffCodeLevel level,
		      TreeMap<TariffCode *, Net::Node *> & nodes_map)
{
  TariffCode * tc = root->get_info().first;
  
  // Lista de productos pertenecientes al CAEV
  auto products = tc->get_products();

# ifdef DEBUG
  cout << "Building downstream\n"
       << "Processing tariffcode: " << tc->cod << endl
       << "There are " << products.size() << " products\n";
# endif
  
  TreeSet<TariffCode *> tariffcode_set;

  products.for_each([&] (auto p) {
      
      // Para cada producto miro las ventas del año dado
      auto & sales = p->production(year).sales(year);
      
      sales.for_each([&] (auto & sale) {
	  /* Obtengo los insumos que obtiene el cliente, tal que tengan el 
	     mismo código arancelario del producto */      
	  auto filter_inputs = sale.client->filter_inputs([&] (auto input) {
	      if (input->tariffcode != p->tariffcode)
		return false;

	      return input->production(year).purchases(year)
	      .exists([&] (auto & purchase) {
		  return purchase.provider == p->sub_ue->ue;
		});
	    });

	  filter_inputs.for_each([&] (auto input) {
	      tariffcode_set.insert(input->product->get_tariffcode(level));
	    });
	});
    });

  tariffcode_set.for_each([&] (TariffCode * c) {

      auto p = nodes_map.search(c);
      
      if (p == nullptr) 
	{
	  Net::Node * t =
	    net.insert_node(make_pair(c, NodePosition::DOWNSTREAM));
	  net.insert_arc(root, t);
	  nodes_map.insert(c, t);
	  build_downstream(net, t, year, level, nodes_map);
	}
      else
	{
	  Net::Node * t = p->second;

	  if (root == t)
	    {
	      if (not exists_arc(root, t) and not exists_arc(t, root))
		net.insert_arc(root, t);
	    }
	  else
	    {
	      if (not exists_arc(root, t))
		net.insert_arc(root, t);
	    }
	}
    });
}

void generate_tariffcode_chain(const string & lvl, const string & tariffcode,
			       year_t year, const string & input_name,
			       const string & output_name, ViewType view_type)
{
# ifdef DEBUG
  cout << "Params:\n";
  cout << "Tariff code level: " << lvl << endl
       << "Tariff code:       " << tartiffcode_cod << endl
       << "Year:              " << year << endl
       << "Input file name:   " << input_name << endl
       << "Output file name:  " << output_name << endl;
# endif
  
  TariffCodeLevel level = str_to_tariffcodelevel(lvl);

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
  
  TariffCode * tc = search_tariffcodelevel(tariffcode, level, map);

  if (tc == nullptr)
    {
      stringstream s;
      s << "El código arancelario " << tariffcode
	<< " no existe en el nivel " << tariffcodelevel_to_str(level);
      throw domain_error(s.str());
    }

# ifdef DEBUG
  cout << "Found tariff code description: " << tc->description << endl;
# endif

  Net net;
  Net::Node * root = net.insert_node(make_pair(tc, NodePosition::ROOT));

  TreeMap<TariffCode *, Net::Node *> nodes_map;
  nodes_map[tc] = root;

  build_upstream(net, root, year, level, nodes_map);
  build_downstream(net, root, year, level, nodes_map);

  if (view_type == ViewType::UE)
    plot_by_ue(net, output_name);
  else if (view_type == ViewType::PRODUCT)
    plot_by_product(net, output_name);
  else
    throw domain_error("Tipo de vista no válido");
}


