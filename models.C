/*
  Este archivo contiene la instrumentación de algunos de los métodos del modelo
  de datos definido para la construcción de cadenas productivas.
  
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

# include <models.H>

CAEVLevel str_to_caevlevel(const string & lvl)
{
  if (regex_match(lvl, regex{"[sS][eE][cC][cC][iI][oO][nN]"}))
    return CAEVLevel::SECTION;

  if (regex_match(lvl, regex{"[dD][iI][vV][iI][sS][iI][oO][nN]"}))
    return CAEVLevel::DIVISION;

  if (regex_match(lvl, regex{"[gG][rR][uU][pP][oO]"}))
    return CAEVLevel::GROUP;

  if (regex_match(lvl, regex{"[cC][lL][aA][sS][eE]"}))
    return CAEVLevel::CLASS;

  if (regex_match(lvl, regex{"[rR][aA][mM][aA]"}))
    return CAEVLevel::BRANCH;

  throw logic_error("Error en nivel de actividad económica");
}

string caevlevel_to_str(CAEVLevel level)
{
  switch (level)
    {
    case CAEVLevel::SECTION:  return "Sección";
    case CAEVLevel::DIVISION: return "División";
    case CAEVLevel::GROUP:    return "Grupo";
    case CAEVLevel::CLASS:    return "Clase";
    case CAEVLevel::BRANCH:   return "Rama";
    default: throw logic_error("Error en nivel de actividad económica");
    }
}

CAEV * search_caevlevel(const string & cod, CAEVLevel level, const Map & map)
{
  switch (level)
    {
    case CAEVLevel::SECTION:
      {
	CAEVSection sec;
	sec.cod = cod;
	return map.caev_sections.search(sec);
      }
    case CAEVLevel::DIVISION:
      {
	CAEVDivision div;
	div.cod = cod;
	return map.caev_divisions.search(div);
      }
    case CAEVLevel::GROUP:
      {
	CAEVGroup group;
	group.cod = cod;
	return map.caev_groups.search(group);
      }
    case CAEVLevel::CLASS:
      {
	CAEVClass clazz;
	clazz.cod = cod;
	return map.caev_classes.search(clazz);
      }
    case CAEVLevel::BRANCH:
      {
	CAEVBranch branch;
	branch.cod = cod;
	return map.caev_branches.search(branch);
      }
    default:
      throw logic_error("Error en nivel de actividad económica");
    }
}

TariffCodeLevel str_to_tariffcodelevel(const string & lvl)
{
  if (regex_match(lvl, regex{"[sS][eE][cC][cC][iI][oO][nN]"}))
    return TariffCodeLevel::SECTION;

  if (regex_match(lvl, regex{"[cC][aA][pP][iI][tT][uU][lL][oO]"}))
    return TariffCodeLevel::CHAPTER;

  if (regex_match(lvl, regex{"[pP][aA][rR][tT][iI][dD][aA]"}))
    return TariffCodeLevel::ITEM;

  if (regex_match(lvl, regex{"[sS][uU][bB][pP][aA][rR][tT][iI][dD][aA]"}))
    return TariffCodeLevel::SUBITEM;

  if (regex_match(lvl,
		  regex{"[sS][uU][bB][sS][uU][bB][pP][aA][rR][tT][iI][dD][aA]"}))
    return TariffCodeLevel::SUBSUBITEM;

  throw logic_error("Error en nivel de código arancelario");
}

string tariffcodelevel_to_str(TariffCodeLevel level)
{
  switch (level)
    {
    case TariffCodeLevel::SECTION:    return "Seccion";
    case TariffCodeLevel::CHAPTER:    return "Capitulo";
    case TariffCodeLevel::ITEM:       return "Partida";
    case TariffCodeLevel::SUBITEM:    return "Subpartida";
    case TariffCodeLevel::SUBSUBITEM: return "Subsubpartida";
    default: throw logic_error("Error en nivel de código arancelario");
    }
}

TariffCode * search_tariffcodelevel(const string & cod, TariffCodeLevel level,
				    const Map & map)
{
  return nullptr;
}

List<SubUE *> CAEVSection::get_sub_ues() const
{
  TreeSet<SubUE *> set;
  
  divisions.for_each([&set](CAEVDivision * d)
		     {
		       d->get_sub_ues().for_each([&set](SubUE * s)
						 {
						   set.insert(s);
						 });
		     });
  
  return set.items();
}

List<SubUE *> CAEVDivision::get_sub_ues() const
{
  TreeSet<SubUE *> set;
  
  groups.for_each([&set](CAEVGroup * g)
		  {
		    g->get_sub_ues().for_each([&set](SubUE * s)
					      {
						set.insert(s);
					      });
		  });
  
  return set.items();
}

List<SubUE *> CAEVGroup::get_sub_ues() const
{
  TreeSet<SubUE *> set;
  
  classes.for_each([&set](CAEVClass * c)
		   {
		     c->get_sub_ues().for_each([&set](SubUE * s)
					       {
						 set.insert(s);
					       });
		   });
  
  return set.items();
}

List<SubUE *> CAEVClass::get_sub_ues() const
{
  TreeSet<SubUE *> set;
  
  branches.for_each([&set](CAEVBranch * b)
		    {
		      b->get_sub_ues().for_each([&set](SubUE * s)
						{
						  set.insert(s);
						});
		    });
  
  return set.items();
}

List<Product *> TariffCodeSection::get_products() const
{
  TreeSet<Product *> set;
  
  chapters.for_each([&set](TariffCodeChapter * g)
		    {
		      g->get_products().for_each([&set](Product * p)
						 {
						   set.insert(p);
						 });
		    });
  
  return set.items();
}

List<Product *> TariffCodeChapter::get_products() const
{
  TreeSet<Product *> set;
  
  items.for_each([&set](TariffCodeItem * i)
		 {
		   i->get_products().for_each([&set](Product * p)
					      {
						set.insert(p);
					      });
		 });
  
  return set.items();
}

List<Product *> TariffCodeItem::get_products() const
{
  TreeSet<Product *> set;
  
  subitems.for_each([&set](TariffCodeSubItem * si)
		    {
		      si->get_products().for_each([&set](Product * p)
						  {
						    set.insert(p);
						  });
		    });
  
  return set.items();
}

List<Product *> TariffCodeSubItem::get_products() const
{
  TreeSet<Product *> set;
  
  subsubitems.for_each([&set](TariffCodeSubSubItem * ssi)
		       {
			 ssi->get_products().for_each([&set](Product * p)
						      {
							set.insert(p);
						      });
		       });
  
  return set.items();
}

void Product::save(ostream & out, TreeMap<Cod *, size_t> & tc_idxs,
		   TreeMap<UE *, size_t> & ue_idxs,
		   TreeMap<SubUE *, size_t> & sub_ue_idxs) const
{
  out << db_id << endl;
  out << name << endl;
  out << sub_ue_idxs[sub_ue] << endl;
  out << tc_idxs[tariffcode] << endl;
  
  out << productions_by_year.size() << endl;
  productions_by_year.for_each([&](auto & p)
			       {
				 out << p.first << endl;
				 p.second.save(out, ue_idxs);
			       });
}

void Product::load(istream & in, DynArray<Cod *> & tcs, DynArray<UE *> & ues,
		   DynArray<SubUE *> & sub_ues, TreeSet<unsigned short> & years)
{
  string line;
  getline(in, line);
  db_id = stoull(line);
  getline(in, name);
  getline(in, line);
  size_t sub_ue_idx = stoul(line);
  sub_ue = sub_ues.access(sub_ue_idx);
  assert(sub_ue != nullptr);
  getline(in, line);
  size_t tc_idx = stoul(line);
  tariffcode = static_cast<TariffCodeSubSubItem *>(tcs.access(tc_idx));
  assert(tariffcode != nullptr);

  getline(in, line);
  size_t num_productions_by_year = stoul(line);

  for (size_t i = 0; i < num_productions_by_year; ++i)
    {
      getline(in, line);
      unsigned short year = stoi(line);

      years.insert(year);

      Production & p = production(year);
      p.product = this;
      p.load(in, ues);
    }
}

void Production::save(ostream & out, TreeMap<UE *, size_t> & ue_idxs) const
{
  out << quantity << endl;
  out << meassurement_unit << endl;
  out << sales_by_year.size() << endl;
  sales_by_year.for_each([&](auto & p)
			  {
			    out << p.first << endl;
			    out << p.second.size() << endl;
			    p.second.for_each([&](auto & sale)
					      {
						sale.save(out, ue_idxs);
					      });
			  });
}

void Production::load(istream & in, DynArray<UE *> & ues)
{
  string line;
  getline(in, line);
  quantity = stod(line);
  getline(in, meassurement_unit);
  getline(in, line);
  size_t num_sales_by_year = stoul(line);

  for (size_t i = 0; i < num_sales_by_year; ++i)
    {
      getline(in, line);
      unsigned short year = stoi(line);

      auto & ss = sales(year);

      getline(in, line);
      size_t num_s = stoul(line);

      for (size_t j = 0; j < num_s; ++j)
	{
	  Sale & s = ss.append(Sale());
	  s.load(in, ues);
	}
    }
}

void Input::save(ostream & out, TreeMap<Cod *, size_t> & tc_idxs,
		 TreeMap<Product *, size_t> & products_idx,
		 TreeMap<UE *, size_t> & ue_idxs) const
{
  out << db_id << endl;
  out << name << endl;
  out << products_idx[product] << endl;
  out << tc_idxs[tariffcode] << endl;
  
  out << productions_by_year.size() << endl;
  productions_by_year.for_each([&](auto & p)
			       {
				 out << p.first << endl;
				 p.second.save(out, ue_idxs);
			       });
}

void Input::load(istream & in, DynArray<Cod *> & tcs,
		 DynArray<Product *> & products,
		 DynArray<UE *> & ues, TreeSet<unsigned short> & years)
{
  string line;
  getline(in, line);
  db_id = stoull(line);
  getline(in, name);
  getline(in, line);
  size_t product_idx = stoull(line);
  product = products.access(product_idx);
  getline(in, line);
  size_t tc_idx = stoul(line);
  tariffcode = static_cast<TariffCodeSubSubItem *>(tcs.access(tc_idx));
  assert(tariffcode != nullptr);

  getline(in, line);
  size_t num_productions_by_year = stoul(line);

  for (size_t i = 0; i < num_productions_by_year; ++i)
    {
      getline(in, line);
      unsigned short year = stoi(line);

      years.insert(year);

      InputProduction & p = production(year);
      p.input = this;
      p.load(in, ues);
    }
}



void InputProduction::save(ostream & out, TreeMap<UE *, size_t> & ue_idxs) const
{
  out << purchases_by_year.size() << endl;
  purchases_by_year.for_each([&](auto & p)
			     {
			       out << p.first << endl;
			       out << p.second.size() << endl;
			       p.second.for_each([&](auto & purchase)
						 {
						   purchase.save(out, ue_idxs);
						 });
			     });
}

void InputProduction::load(istream & in, DynArray<UE *> & ues)
{
  string line;
  getline(in, line);
  size_t num_purchases_by_year = stoul(line);

  for (size_t i = 0; i < num_purchases_by_year; ++i)
    {
      getline(in, line);
      unsigned short year = stoi(line);

      auto & ps = purchases(year);

      getline(in, line);
      size_t num_p = stoul(line);

      for (size_t j = 0; j < num_p; ++j)
	{
	  Purchase & p = ps.append(Purchase());
	  p.load(in, ues);
	}
    }
}

void Map::load(istream & in)
{
  DynArray<Cod *> caevs_w, caevs_r;

  string line;

  getline(in, line);
  size_t num_items = stoul(line);
  
  caevs_w.reserve(num_items);

  for (size_t i = 0; i < num_items; ++i)
    {
      CAEVSection caev;
      caev.load(in, caevs_r);
      CAEVSection * ptr = caev_sections.insert(caev);
      caevs_w.touch(i) = ptr;
    }

  caevs_r.swap(caevs_w);
  caevs_w.empty();

  getline(in, line);
  num_items = stoul(line);
  
  caevs_w.reserve(num_items);
    
  for (size_t i = 0; i < num_items; ++i)
    {
      CAEVDivision caev;
      caev.load(in, caevs_r);
      CAEVDivision * ptr = caev_divisions.insert(caev);
      ptr->section->divisions.append(ptr);
      caevs_w.touch(i) = ptr;
    }

  caevs_r.swap(caevs_w);
  caevs_w.empty();

  getline(in, line);
  num_items = stoul(line);
  caevs_w.reserve(num_items);
    
  for (size_t i = 0; i < num_items; ++i)
    {
      CAEVGroup caev;
      caev.load(in, caevs_r);
      CAEVGroup * ptr = caev_groups.insert(caev);
      ptr->division->groups.append(ptr);
      caevs_w.touch(i) = ptr;
    }

  caevs_r.swap(caevs_w);
  caevs_w.empty();

  getline(in, line);
  num_items = stoul(line);
  caevs_w.reserve(num_items);
    
  for (size_t i = 0; i < num_items; ++i)
    {
      CAEVClass caev;
      caev.load(in, caevs_r);
      CAEVClass * ptr = caev_classes.insert(caev);
      ptr->group->classes.append(ptr);
      caevs_w.touch(i) = ptr;
    }

  caevs_r.swap(caevs_w);
  caevs_w.empty();

  getline(in, line);
  num_items = stoul(line);
  caevs_w.reserve(num_items);
    
  for (size_t i = 0; i < num_items; ++i)
    {
      CAEVBranch caev;
      caev.load(in, caevs_r);
      CAEVBranch * ptr = caev_branches.insert(caev);
      ptr->clazz->branches.append(ptr);
      caevs_w.touch(i) = ptr;
    }

  caevs_r.swap(caevs_w);
  caevs_w.empty();

  DynArray<Cod *> tariffcodes_w, tariffcodes_r;

  getline(in, line);
  num_items = stoul(line);
  tariffcodes_w.reserve(num_items);
    
  for (size_t i = 0; i < num_items; ++i)
    {
      TariffCodeSection tariffcode;
      tariffcode.load(in, tariffcodes_r);
      TariffCodeSection * ptr = tariffcode_sections.insert(tariffcode);
      tariffcodes_w.touch(i) = ptr;
    }

  tariffcodes_r.swap(tariffcodes_w);
  tariffcodes_w.empty();

  getline(in, line);
  num_items = stoul(line);
  tariffcodes_w.reserve(num_items);
    
  for (size_t i = 0; i < num_items; ++i)
    {
      TariffCodeChapter tariffcode;
      tariffcode.load(in, tariffcodes_r);
      TariffCodeChapter * ptr = tariffcode_chapters.insert(tariffcode);
      ptr->section->chapters.append(ptr);
      tariffcodes_w.touch(i) = ptr;
    }

  tariffcodes_r.swap(tariffcodes_w);
  tariffcodes_w.empty();
    
  getline(in, line);
  num_items = stoul(line);
  tariffcodes_w.reserve(num_items);
    
  for (size_t i = 0; i < num_items; ++i)
    {
      TariffCodeItem tariffcode;
      tariffcode.load(in, tariffcodes_r);
      TariffCodeItem * ptr = tariffcode_items.insert(tariffcode);
      ptr->chapter->items.append(ptr);
      tariffcodes_w.touch(i) = ptr;
    }

  tariffcodes_r.swap(tariffcodes_w);
  tariffcodes_w.empty();
    
  getline(in, line);
  num_items = stoul(line);
  tariffcodes_w.reserve(num_items);
    
  for (size_t i = 0; i < num_items; ++i)
    {
      TariffCodeSubItem tariffcode;
      tariffcode.load(in, tariffcodes_r);
      TariffCodeSubItem * ptr = tariffcode_subitems.insert(tariffcode);
      ptr->item->subitems.append(ptr);
      tariffcodes_w.touch(i) = ptr;
    }

  tariffcodes_r.swap(tariffcodes_w);
  tariffcodes_w.empty();
    
  getline(in, line);
  num_items = stoul(line);
  tariffcodes_w.reserve(num_items);
    
  for (size_t i = 0; i < num_items; ++i)
    {
      TariffCodeSubSubItem tariffcode;
      tariffcode.load(in, tariffcodes_r);
      TariffCodeSubSubItem * ptr = tariffcode_subsubitems.insert(tariffcode);
      ptr->subitem->subsubitems.append(ptr);
      tariffcodes_w.touch(i) = ptr;
    }

  tariffcodes_r.swap(tariffcodes_w);
  tariffcodes_w.empty();

  DynArray<UE *> ues;
  getline(in, line);
  num_items = stoul(line);
  ues.reserve(num_items);

  for (size_t i = 0; i < num_items; ++i)
    {
      UE ue;
      ue.load(in);
      UE * ptr = this->ues.insert(ue);
      assert(ptr != nullptr);
      ues.touch(i) = ptr;
    }

  DynArray<SubUE *> sub_ues;
  getline(in, line);
  num_items = stoul(line);
  sub_ues.reserve(num_items);
  
  for (size_t i = 0; i < num_items; ++i)
    {
      SubUE sub_ue;
      sub_ue.load(in, caevs_r, ues);
      SubUE * ptr = this->sub_ues.insert(sub_ue);
      ptr->ue->sub_ues.append(ptr);
      if (ptr->caev != nullptr)
	ptr->caev->sub_ues.append(ptr);
      sub_ues.touch(i) = ptr;
    }
  
  getline(in, line);
  num_items = stoul(line);

  DynArray<Product *> products;
  
  for (size_t i = 0; i < num_items; ++i)
    {
      Product product;
      product.load(in, tariffcodes_r, ues, sub_ues, years);
      Product * ptr = this->products.insert(product);
      assert(ptr != nullptr);
      assert(ptr->tariffcode != nullptr);
      assert(ptr->sub_ue != nullptr);
      ptr->tariffcode->products.append(ptr);
      ptr->sub_ue->products.append(ptr);
      products.touch(i) = ptr;
    }

  getline(in, line);
  num_items = stoul(line);

  for (size_t i = 0; i < num_items; ++i)
    {
      Input input;
      input.load(in, tariffcodes_r, products, ues, years);
      Input * ptr = this->inputs.insert(input);
      assert(ptr != nullptr);
      ptr->product->inputs.append(ptr);
    }
}
