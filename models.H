/*
  Este archivo contiene la definición del modelo de datos definido para la
  construcción de cadenas productivas.
  
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

# ifndef MODELS_H
# define MODELS_H

# include <iostream>
# include <string>
# include <sstream>
# include <fstream>
# include <stdexcept>
# include <tuple>
# include <regex>

using namespace std;

# include <htlist.H>
# include <tpl_dynDlist.H>
# include <tpl_dynArray.H>
# include <tpl_dynSetTree.H>
# include <tpl_dynMapTree.H>
# include <tpl_graph.H>

// Declaraciones adelantas de los tipos
struct CAEVSection;
struct CAEVDivision;
struct CAEVGroup;
struct CAEVClass;
struct CAEVBranch;
struct TariffCodeSection;
struct TariffCodeChapter;
struct TariffCodeItem;
struct TariffCodeSubItem;
struct TariffCodeSubSubItem;
struct UE;
struct SubUE;
struct Product;
struct Production;
struct Sale;
struct Input;
struct InputProduction;
struct Purchase;
struct Map;

/// Alias para el tipo de id en base de datos.
using db_id_t = unsigned long long;

/// Alias para representar años.
using year_t = unsigned short;

/// Alias para el tipo de conjunto basado en Árboles Binarios de Búsqueda
template <typename Key, class Cmp = std::less<Key>>
  using TreeSet = DynSetAvlTree<Key, Cmp>;

/// Alias para el tipo de mapa basado en Árboles Binarios de Búsqueda
template <typename Key, typename Data, class Cmp = std::less<Key>>
  using TreeMap = DynMapAvlTree<Key, Data, Cmp>;

/// Alias para el tipo de lista basada en una lista simplemente enlazada.
template <typename T> using List = DynList<T>;

/// Enumerado para representar los niveles de detalle de actividades económicas.
enum class CAEVLevel { SECTION, DIVISION, GROUP, CLASS, BRANCH };

/// Enumerado para representar los niveles de detalle de códigos arancelarios.
enum class TariffCodeLevel { SECTION, CHAPTER, ITEM, SUBITEM, SUBSUBITEM };

/// Enumerado para etiquetar los nodos de los grafos que representan cadenas.
enum class NodePosition { ROOT, UPSTREAM, DOWNSTREAM };

/** Tipo que representa un código de manera general.

    Este tipo se utiliza como generalización para la actividad económica base y
    el código arancelario base.

    @author Alejandro J. Mujica
*/
struct Cod
{
  string cod;
  string description;

  Cod() : cod(""), description("") { /* empty */ }

  Cod(const Cod & c)
    : cod(c.cod), description(c.description)
  {
    // empty
  }

  Cod(Cod && c)
    : Cod()
  {
    std::swap(cod, c.cod);
    std::swap(description, c.description);
  }

  Cod & operator = (const Cod & c)
  {
    if (&c == this)
      return *this;

    cod = c.cod;
    description = c.description;
    return *this;
  }

  Cod & operator = (Cod && c)
  {
    std::swap(cod, c.cod);
    std::swap(description, c.description);
    return *this;
  }

  /// Almacena los valores en un archivo de texto plano cuyo flujo es out.
  virtual void save(ostream & out, TreeMap<Cod *, size_t> &) const
  {
    out << cod << endl << description << endl;    
  }

  /// Carga los valores desde un archivo de texto plano cuyo flujo es in.
  virtual void load(istream & in, DynArray<Cod *> &)
  {
    getline(in, cod);
    getline(in, description);
  }
};

/** Tipo que representa la base para actvidades económicas.

    @author Alejandro J. Mujica
*/
struct CAEV : public Cod
{
  CAEV() : Cod() { /* empty */ }

  CAEV(const CAEV & c) : Cod(c) { /* empty */ }

  CAEV(CAEV && c) : Cod(std::forward<CAEV>(c)) { /* empty */ }

  CAEV & operator = (const CAEV & c)
  {
    if (&c == this)
      return *this;

    (Cod &) *this = c;
    return *this;
  }

  CAEV & operator = (CAEV && c)
  {
    (Cod &) *this = std::move(c);
    return *this;
  }

  /** Función abstracta que retorna todas las sub unidades económicas de
      una actividad económica.
  */
  virtual List<SubUE *> get_sub_ues() const = 0;

  /// Retorna todos los productos asociados a la actividad económica.
  List<Product *> get_products() const
  {
    List<Product *> l;

    get_sub_ues().for_each([&l](auto ue)
			   {
			     ue->products.for_each([&l](auto p)
						   {
						     l.append(p);
						   });
			   });
    return l;
  }
};

/** Tipo que representa actividades económicas en nivel de detalle Sección.

    @author Alejandro J. Mujica
*/
struct CAEVSection : public CAEV
{
  List<CAEVDivision *> divisions;
  
  CAEVSection() : CAEV() { /* empty */ }

  CAEVSection(const CAEVSection & c) : CAEV(c) { /* empty */ }

  CAEVSection(CAEVSection && c)
    : CAEV(std::forward<CAEVSection>(c)) { /* empty */ }

  CAEVSection & operator = (const CAEVSection & c)
  {
    if (&c == this)
      return *this;

    (CAEV &) *this = c;
    return *this;
  }

  CAEVSection & operator = (CAEVSection && c)
  {
    (CAEV &) *this = std::move(c);
    return *this;
  }

  List<SubUE *> get_sub_ues() const override;
};

/** Tipo que representa actividades económicas en nivel de detalle División.

    @author Alejandro J. Mujica
*/
struct CAEVDivision : public CAEV
{
  CAEVSection     * section;
  List<CAEVGroup *> groups;

  CAEVDivision() : CAEV(), section(nullptr), groups() { /* empty */ }

  CAEVDivision(const CAEVDivision & c)
    : CAEV(c), section(c.section), groups(c.groups) { /* empty */ }

  CAEVDivision(CAEVDivision && c)
    : CAEV(std::forward<CAEVDivision>(c)), section(nullptr), groups()
  {
    std::swap(section, c.section);
    groups.swap(c.groups);
  }

  CAEVDivision & operator = (const CAEVDivision & c)
  {
    if (&c == this)
      return *this;

    (CAEV &) *this = c;
    section = c.section;
    groups = c.groups;
    return *this;
  }

  CAEVDivision & operator = (CAEVDivision && c)
  {
    (CAEV &) *this = std::move(c);
    std::swap(section, c.section);
    groups.swap(c.groups);
    return *this;
  }

  List<SubUE *> get_sub_ues() const override;

  void save(ostream & out, TreeMap<Cod *, size_t> & idxs) const override
  {
    CAEV::save(out, idxs);
    out << idxs[section] << endl;
  }

  void load(istream & in, DynArray<Cod *> & sections) override
  {
    string line;
    CAEV::load(in, sections);
    getline(in, line);
    size_t idx = stoul(line);
    section = static_cast<CAEVSection *>(sections.access(idx));
  }
};

/** Tipo que representa actividades económicas en nivel de detalle Grupo.

    @author Alejandro J. Mujica
*/
struct CAEVGroup : public CAEV
{
  CAEVDivision    * division;
  List<CAEVClass *> classes;

  CAEVGroup() : CAEV(), division(nullptr), classes() { /* empty */ }

  CAEVGroup(const CAEVGroup & c)
    : CAEV(c), division(c.division), classes(c.classes) { /* empty */ }

  CAEVGroup(CAEVGroup && c)
    : CAEV(std::forward<CAEVGroup>(c)), division(nullptr), classes()
  {
    std::swap(division, c.division);
    classes.swap(c.classes);
  }

  CAEVGroup & operator = (const CAEVGroup & c)
  {
    if (&c == this)
      return *this;

    (CAEV &) *this = c;
    division = c.division;
    classes = c.classes;
    return *this;
  }

  CAEVGroup & operator = (CAEVGroup && c)
  {
    (CAEV &) *this = std::move(c);
    std::swap(division, c.division);
    classes.swap(c.classes);
    return *this;
  }

  List<SubUE *> get_sub_ues() const override;

  void save(ostream & out, TreeMap<Cod *, size_t> & idxs) const override
  {
    CAEV::save(out, idxs);
    out << idxs[division] << endl;
  }

  void load(istream & in, DynArray<Cod *> & divisions) override
  {
    string line;
    CAEV::load(in, divisions);
    getline(in, line);
    size_t idx = stoul(line);
    division = static_cast<CAEVDivision *>(divisions.access(idx));
  }
};

/** Tipo que representa actividades económicas en nivel de detalle Clase.

    @author Alejandro J. Mujica
*/
struct CAEVClass : public CAEV
{
  CAEVGroup        * group;
  List<CAEVBranch *> branches;

  CAEVClass() : CAEV(), group(nullptr), branches() { /* empty */ }

  CAEVClass(const CAEVClass & c)
    : CAEV(c), group(c.group), branches(c.branches) { /* empty */ }

  CAEVClass(CAEVClass && c)
    : CAEV(std::forward<CAEVClass>(c)), group(nullptr), branches()
  {
    std::swap(group, c.group);
    branches.swap(c.branches);
  }

  CAEVClass & operator = (const CAEVClass & c)
  {
    if (&c == this)
      return *this;

    (CAEV &) *this = c;
    group = c.group;
    branches = c.branches;
    return *this;
  }

  CAEVClass & operator = (CAEVClass && c)
  {
    (CAEV &) *this = std::move(c);
    std::swap(group, c.group);
    branches.swap(c.branches);
    return *this;
  }

  List<SubUE *> get_sub_ues() const override;

  void save(ostream & out, TreeMap<Cod *, size_t> & idxs) const override
  {
    CAEV::save(out, idxs);
    out << idxs[group] << endl;
  }

  void load(istream & in, DynArray<Cod *> & groups) override
  {
    string line;
    CAEV::load(in, groups);
    getline(in, line);
    size_t idx = stoul(line);
    group = static_cast<CAEVGroup *>(groups.access(idx));
  }
};

/** Tipo que representa actividades económicas en nivel de detalle Rama.

    @author Alejandro J. Mujica
*/
struct CAEVBranch : public CAEV
{
  CAEVClass   * clazz;   // class con zz debido a la palabra reservada.
  List<SubUE *> sub_ues;

  CAEVBranch() : CAEV(), clazz(nullptr), sub_ues() { /* empty */ }

  CAEVBranch(const CAEVBranch & c)
    : CAEV(c), clazz(c.clazz), sub_ues(c.sub_ues) { /* empty */ }

  CAEVBranch(CAEVBranch && c)
    : CAEV(std::forward<CAEVBranch>(c)), clazz(nullptr), sub_ues()
  {
    std::swap(clazz, c.clazz);
    sub_ues.swap(c.sub_ues);
  }

  CAEVBranch & operator = (const CAEVBranch & c)
  {
    if (&c == this)
      return *this;

    (CAEV &) *this = c;
    clazz = c.clazz;
    sub_ues = c.sub_ues;
    return *this;
  }

  CAEVBranch & operator = (CAEVBranch && c)
  {
    (CAEV &) *this = std::move(c);
    std::swap(clazz, c.clazz);
    sub_ues.swap(c.sub_ues);
    return *this;
  }

  List<SubUE *> get_sub_ues() const override
  {
    return sub_ues;
  }

  void save(ostream & out, TreeMap<Cod *, size_t> & idxs) const override
  {
    CAEV::save(out, idxs);
    out << idxs[clazz] << endl;
  }
  
  void load(istream & in, DynArray<Cod *> & classes) override
  {
    string line;
    CAEV::load(in, classes);
    getline(in, line);
    size_t idx = stoul(line); 
    clazz = static_cast<CAEVClass *>(classes.access(idx));
  }
};

/** Tipo que representa la base para códigos arancelarios.

    @author Alejandro J. Mujica
*/
struct TariffCode : public Cod
{
  List<TariffCodeChapter *> chapters;
  
  TariffCode() : Cod() { /* empty */ }

  TariffCode(const TariffCode & c) : Cod(c) { /* empty */ }

  TariffCode(TariffCode && c) : Cod(std::forward<TariffCode>(c)) { /* empty */ }

  TariffCode & operator = (const TariffCode & c)
  {
    if (&c == this)
      return *this;

    (Cod &) *this = c;
    return *this;
  }

  TariffCode & operator = (TariffCode && c)
  {
    (Cod &) *this = std::move(c);
   return *this;
  }

  /** Función virtual que devuelve la lista de todos los productos asociados
      al código arancelario.
  */
  virtual List<Product *> get_products() const = 0;
};

/** Tipo que representa códigos arancelaros en nivel de detalle Sección.

    @author Alejandro J. Mujica
*/
struct TariffCodeSection : public TariffCode
{
  TariffCodeSection() : TariffCode() { /* empty */ }

  TariffCodeSection(const TariffCodeSection & c)
    : TariffCode(c) { /* empty */ }

  TariffCodeSection(TariffCodeSection && c)
    : TariffCode(std::forward<TariffCodeSection>(c)) { /* empty */ }

  TariffCodeSection & operator = (const TariffCodeSection & c)
  {
    if (&c == this)
      return *this;

    (TariffCode &) *this = c;
    return *this;
  }

  TariffCodeSection & operator = (TariffCodeSection && c)
  {
    (TariffCode &) *this = std::move(c);
    return *this;
  }

  List<Product *> get_products() const override;
};

/** Tipo que representa códigos arancelaros en nivel de detalle Capítulo.

    @author Alejandro J. Mujica
*/
struct TariffCodeChapter : public TariffCode
{
  TariffCodeSection       * section;
  List<TariffCodeItem *> items;

  TariffCodeChapter() : TariffCode(), section(nullptr), items() { /* empty */ }

  TariffCodeChapter(const TariffCodeChapter & c)
    : TariffCode(c), section(c.section), items(c.items) { /* empty */ }

  TariffCodeChapter(TariffCodeChapter && c)
    : TariffCode(std::forward<TariffCodeChapter>(c)), section(nullptr), items()
  {
    std::swap(section, c.section);
    items.swap(c.items);
  }

  TariffCodeChapter & operator = (const TariffCodeChapter & c)
  {
    if (&c == this)
      return *this;

    (TariffCode &) *this = c;
    section = c.section;
    items = c.items;
    return *this;
  }

  TariffCodeChapter & operator = (TariffCodeChapter && c)
  {
    (TariffCode &) *this = std::move(c);
    std::swap(section, c.section);
    items.swap(c.items);
    return *this;
  }

  List<Product *> get_products() const override;

  void save(ostream & out, TreeMap<Cod *, size_t> & idxs) const override
  {
    TariffCode::save(out, idxs);
    out << idxs[section] << endl;
  }

  void load(istream & in, DynArray<Cod *> & sections) override
  {
    string line;
    TariffCode::load(in, sections);
    getline(in, line);
    size_t idx = stoul(line);
    section = static_cast<TariffCodeSection *>(sections.access(idx));
  }
};

/** Tipo que representa códigos arancelaros en nivel de detalle Partida.

    @author Alejandro J. Mujica
*/
struct TariffCodeItem : public TariffCode
{
  TariffCodeChapter          * chapter;
  List<TariffCodeSubItem *> subitems;

  TariffCodeItem() : TariffCode(), chapter(nullptr), subitems() { /* empty */ }

  TariffCodeItem(const TariffCodeItem & c)
    : TariffCode(c), chapter(c.chapter), subitems(c.subitems) { /* empty */ }

  TariffCodeItem(TariffCodeItem && c)
    : TariffCode(std::forward<TariffCodeItem>(c)), chapter(nullptr), subitems()
  {
    std::swap(chapter, c.chapter);
    subitems.swap(c.subitems);
  }

  TariffCodeItem & operator = (const TariffCodeItem & c)
  {
    if (&c == this)
      return *this;

    (TariffCode &) *this = c;
    chapter = c.chapter;
    subitems = c.subitems;
    return *this;
  }

  TariffCodeItem & operator = (TariffCodeItem && c)
  {
    (TariffCode &) *this = std::move(c);
    std::swap(chapter, c.chapter);
    subitems.swap(c.subitems);
    return *this;
  }
  
  List<Product *> get_products() const override;

  void save(ostream & out, TreeMap<Cod *, size_t> & idxs) const override
  {
    TariffCode::save(out, idxs);
    out << idxs[chapter] << endl;
  }

  void load(istream & in, DynArray<Cod *> & chapters) override
  {
    string line;
    TariffCode::load(in, chapters);
    getline(in, line);
    size_t idx = stoul(line);
    chapter = static_cast<TariffCodeChapter *>(chapters.access(idx));
  }
};

/** Tipo que representa códigos arancelaros en nivel de detalle Sub Partida.

    @author Alejandro J. Mujica
*/
struct TariffCodeSubItem : public TariffCode
{
  TariffCodeItem                * item;
  List<TariffCodeSubSubItem *> subsubitems;
  
  TariffCodeSubItem()
    : TariffCode(), item(nullptr), subsubitems() { /* empty */ }

  TariffCodeSubItem(const TariffCodeSubItem & c)
    : TariffCode(c), item(c.item), subsubitems(c.subsubitems) { /* empty */ }

  TariffCodeSubItem(TariffCodeSubItem && c)
    : TariffCode(std::forward<TariffCodeSubItem>(c)), item(nullptr),
      subsubitems()
  {
    std::swap(item, c.item);
    subsubitems.swap(c.subsubitems);
  }

  TariffCodeSubItem & operator = (const TariffCodeSubItem & c)
  {
    if (&c == this)
      return *this;

    (TariffCode &) *this = c;
    item = c.item;
    subsubitems = c.subsubitems;
    return *this;
  }

  TariffCodeSubItem & operator = (TariffCodeSubItem && c)
  {
    (TariffCode &) *this = std::move(c);
    std::swap(item, c.item);
    subsubitems.swap(c.subsubitems);
    return *this;
  }

  List<Product *> get_products() const override;

  void save(ostream & out, TreeMap<Cod *, size_t> & idxs) const override
  {
    TariffCode::save(out, idxs);
    out << idxs[item] << endl;
  }

  void load(istream & in, DynArray<Cod *> & items) override
  {
    string line;
    TariffCode::load(in, items);
    getline(in, line);
    size_t idx = stoul(line);
    item = static_cast<TariffCodeItem *>(items.access(idx));
  }
};

/** Tipo que representa códigos arancelaros en nivel de detalle Sub Sub Partida.

    @author Alejandro J. Mujica
*/
struct TariffCodeSubSubItem : public TariffCode
{
  TariffCodeSubItem * subitem;
  List<Product *>  products;

  TariffCodeSubSubItem()
    : TariffCode(), subitem(nullptr), products() { /* empty */ }

  TariffCodeSubSubItem(const TariffCodeSubSubItem & c)
    : TariffCode(c), subitem(c.subitem), products(c.products) { /* empty */ }

  TariffCodeSubSubItem(TariffCodeSubSubItem && c)
    : TariffCode(std::forward<TariffCodeSubSubItem>(c)), subitem(nullptr),
      products()
  {
    std::swap(subitem, c.subitem);
    products.swap(c.products);
  }

  TariffCodeSubSubItem & operator = (const TariffCodeSubSubItem & c)
  {
    if (&c == this)
      return *this;

    (TariffCode &) *this = c;
    subitem = c.subitem;
    products = c.products;
    return *this;
  }

  TariffCodeSubSubItem & operator = (TariffCodeSubSubItem && c)
  {
    (TariffCode &) *this = std::move(c);
    std::swap(subitem, c.subitem);
    products.swap(c.products);
    return *this;
  }

  List<Product *> get_products() const override
  {
    return products;
  }

  void save(ostream & out, TreeMap<Cod *, size_t> & idxs) const override
  {
    TariffCode::save(out, idxs);
    out << idxs[subitem] << endl;
  }

  void load(istream & in, DynArray<Cod *> & subitems) override
  {
    string line;
    TariffCode::load(in, subitems);
    getline(in, line);
    size_t idx = stoul(line);
    subitem = static_cast<TariffCodeSubItem *>(subitems.access(idx));
  }
};

/** Tipo que representa una unidad económica.

    @author Alejandro J. Mujica
*/
struct UE
{
  string           rif;
  string           name;
  List<SubUE *> sub_ues;

  UE()
    : rif(""), name(""), sub_ues() { /* empty */ }

  UE(const UE & ue)
    : rif(ue.rif), name(ue.name), sub_ues(ue.sub_ues) { /* empty */ }

  UE(UE && ue)
    : UE()
  {
    std::swap(rif, ue.rif);
    std::swap(name, ue.name);
    sub_ues.swap(ue.sub_ues);
  }

  UE & operator = (const UE & ue)
  {
    if (&ue == this)
      return *this;

    rif = ue.rif;
    name = ue.name;
    sub_ues = ue.sub_ues;
    return *this;
  }

  UE & operator = (UE && ue)
  {
    std::swap(rif, ue.rif);
    std::swap(name, ue.name);
    sub_ues.swap(ue.sub_ues);
    return *this;
  }

  /** Retorna una lista con las actividades económicas asociadas a sus sub
      unidades económicas.
  */
  List<CAEVBranch *> caevs() const
  {
    TreeSet<CAEVBranch *> set;

    sub_ues.for_each([&set](auto p)
		     {
		       set.insert(p->caev);
		     });

    return set.items();
  }

  /** Retorna una lista con los productos de todas sus sub unidades económicas
      filtrados según el criterio filter.

      En esta operación para un producto p, si la operación filter(p) retorna
      true, entonces el p se añade en la lista. Si retorna false, es ignorado.
  */
  template <class Filter>
  List<Product *> filter_products(Filter & filter) const
  {
    List<Product *> l;

    sub_ues.for_each([&l, &filter] (auto sub_ue)
		     {
		       sub_ue->products.for_each([&l, &filter](auto p)
						 {
						   if (filter(p))
						     l.append(p);
						 });
		     });
    return l;
  }

  template <class Filter>
  List<Product *> filter_products(Filter && filter = Filter()) const
  {
    return filter_products<Filter>(filter);
  }

  /** Esta operación es semejante a filter_products pero lo hace con los 
      insumos requeridos por cada uno de los productos.
     
      Nótese que se utiliza filter_products con un criterio que siempre retorna
      true para consultar todos los productos. Los que se filtrarán serán los
      insumos.
  */
  template <class Filter>
  List<Input *> filter_inputs(Filter & filter) const
  {
    List<Input *> l;

    filter_products([&] (auto) { return true; })
      .for_each([&] (auto p) {
	  p->inputs.for_each([&] (auto i) {
	      if (filter(i))
		l.append(i);
	    });
	});
	
    return l;
  }

  template <class Filter>
  List<Input *> filter_inputs(Filter && filter = Filter()) const
  {
    return filter_inputs<Filter>(filter);
  }
  
  void save(ostream & out) const
  {
    out << rif << endl;
    out << name << endl;
  }

  void load(istream & in)
  {
    getline(in, rif);
    getline(in, name);
  }
};

/** Tipo que representa una sub unidad económica.

    @author Alejandro J. Mujica
*/
struct SubUE
{
  db_id_t            db_id;
  string             name;
  string             location;
  UE               * ue;
  CAEVBranch       * caev;
  List<Product *> products;

  SubUE()
    : db_id(0), name(""), location(""), ue(nullptr), caev(nullptr), products()
  {
    // empty
  }

  SubUE(const SubUE & s)
    : db_id(s.db_id), name(s.name), location(s.location), ue(s.ue),
      caev(s.caev), products(s.products)
  {
    // empty
  }

  SubUE(SubUE && s)
    : SubUE()
  {
    std::swap(db_id, s.db_id);
    std::swap(name, s.name);
    std::swap(location, s.location);
    std::swap(ue, s.ue);
    std::swap(caev, s.caev);
    products.swap(s.products);
  }

  SubUE & operator = (const SubUE & s)
  {
    if (&s == this)
      return *this;

    db_id = s.db_id;
    name = s.name;
    location = s.location;
    ue = s.ue;
    caev = s.caev;
    products = s.products;
    return *this;
  }

  SubUE & operator = (SubUE && s)
  {
    std::swap(db_id, s.db_id);
    std::swap(name, s.name);
    std::swap(location, s.location);
    std::swap(ue, s.ue);
    std::swap(caev, s.caev);
    products.swap(s.products);
    return *this;
  }

  /// Retorna la actividad económica según el nivel de detalle dado.
  CAEV * get_caev(CAEVLevel level)
  {
    switch (level)
      {
      case CAEVLevel::SECTION:  return caev->clazz->group->division->section;
      case CAEVLevel::DIVISION: return caev->clazz->group->division;
      case CAEVLevel::GROUP:    return caev->clazz->group;
      case CAEVLevel::CLASS:    return caev->clazz;
      case CAEVLevel::BRANCH:   return caev;
      default: throw domain_error("Error en nivel de código arancelario");
      }
  }
  
  void save(ostream & out, TreeMap<Cod *, size_t> & caevs,
	    TreeMap<UE *, size_t> & ues) const
  {
    out << db_id << endl;
    out << name << endl;
    out << location << endl;
    out << ues[ue] << endl;
    if (caev != nullptr)
      out << caevs[caev] << endl;
    else
      out << -1 << endl;
  }

  void load(istream & in, DynArray<Cod *> & caevs, DynArray<UE *> & ues)
  {
    string line;
    getline(in, line);
    db_id = stoull(line);
    getline(in, name);
    getline(in, location);
    getline(in, line);
    size_t ue_idx = stoul(line);
    ue = ues.access(ue_idx);
    getline(in, line);
    long long caev_idx = stoll(line);
    if (caev_idx == -1)
      caev = nullptr;
    else
      caev = static_cast<CAEVBranch *>(caevs.access(caev_idx));
  }
};

/** Tipo que representa la producción de un producto.

    @author Alejandro J. Mujica
*/
struct Production
{
  Product                   * product;
  double                      quantity;
  string                      meassurement_unit;
  TreeMap<year_t, List<Sale>> sales_by_year;

  Production()
    : product(nullptr), quantity(0.0), meassurement_unit(""), sales_by_year()
  {
    // empty
  }

  Production(const Production & p)
    : product(p.product), quantity(p.quantity),
      meassurement_unit(p.meassurement_unit), sales_by_year(p.sales_by_year)
  {
    // empty
  }

  Production(Production && p)
    : Production()
  {
    std::swap(product, p.product);
    std::swap(quantity, p.quantity);
    std::swap(meassurement_unit, p.meassurement_unit);
    sales_by_year.swap(p.sales_by_year);
  }

  Production & operator = (const Production & p)
  {
    if (&p == this)
      return *this;

    product = p.product;
    quantity = p.quantity;
    meassurement_unit = p.meassurement_unit;
    sales_by_year = p.sales_by_year;
    return *this;
  }

  Production & operator = (Production && p)
  {
    std::swap(product, p.product);
    std::swap(quantity, p.quantity);
    std::swap(meassurement_unit, p.meassurement_unit);
    sales_by_year.swap(p.sales_by_year);
    return *this;
  }

  /// Retorna todas las ventas asociadas a una producción un año dado
  List<Sale> & sales(year_t year)
  {
    return sales_by_year[year];
  }

  /// Como la anterior pero para objetos constantes.
  const List<Sale> & sales(year_t year) const
  {
    return sales_by_year[year];
  }

  void save(ostream & out, TreeMap<UE *, size_t> & ue_idxs) const;

  void load(istream & in, DynArray<UE *> & ues);
};

/** Tipo que representa un producto.

    @author Alejandro J. Mujica
*/
struct Product
{
  db_id_t                     db_id;
  string                      name;
  SubUE                     * sub_ue;
  TariffCodeSubSubItem      * tariffcode;
  List<Input *>               inputs;
  TreeMap<year_t, Production> productions_by_year;

  Product()
    : db_id(0), name(""), sub_ue(nullptr), tariffcode(nullptr), inputs(),
      productions_by_year()
  {
    // empty
  }

  Product(const Product & p)
    : db_id(p.db_id), name(p.name), sub_ue(p.sub_ue), tariffcode(p.tariffcode),
      inputs(p.inputs), productions_by_year(p.productions_by_year)
  {
    // empty
  }

  Product(Product && p)
  {
    std::swap(db_id, p.db_id);
    std::swap(name, p.name);
    std::swap(sub_ue, p.sub_ue);
    std::swap(tariffcode, p.tariffcode);
    inputs.swap(p.inputs);
    productions_by_year.swap(p.productions_by_year);
  }

  Product & operator = (const Product & p)
  {
    if (&p == this)
      return *this;

    db_id = p.db_id;
    name = p.name;
    sub_ue = p.sub_ue;
    tariffcode = p.tariffcode;
    inputs = p.inputs;
    productions_by_year = p.productions_by_year;
    return *this;
  }

  Product & operator = (Product && p)
  {
    std::swap(db_id, p.db_id);
    std::swap(name, p.name);
    std::swap(sub_ue, p.sub_ue);
    std::swap(tariffcode, p.tariffcode);
    inputs.swap(p.inputs);
    productions_by_year.swap(p.productions_by_year);
    return *this;
  }

  /// Retorna la producción de este producto para un año dado.
  Production & production(year_t year)
  {
    return productions_by_year[year];
  }

  /// Como la anterior pero para objetos constantes.
  const Production & production(year_t year) const
  {
    return productions_by_year[year];
  }

  CAEV * get_caev(CAEVLevel level)
  {
    return sub_ue->get_caev(level);
  }

  TariffCode * get_tariffcode(TariffCodeLevel level)
  {
    switch (level)
      {
      case TariffCodeLevel::SECTION:
	return tariffcode->subitem->item->chapter->section;	
      case TariffCodeLevel::CHAPTER:
	return tariffcode->subitem->item->chapter;
      case TariffCodeLevel::ITEM:
	return tariffcode->subitem->item;
      case TariffCodeLevel::SUBITEM:
	return tariffcode->subitem;	
      case TariffCodeLevel::SUBSUBITEM:
	return tariffcode;
      default: throw domain_error("Error en nivel de código arancelario");
      }
  }

  void save(ostream & out, TreeMap<Cod *, size_t> & tc_idxs,
	    TreeMap<UE *, size_t> & ue_idxs,
	    TreeMap<SubUE *, size_t> & sub_ue_idxs) const;

  void load(istream & in, DynArray<Cod *> & tcs, DynArray<UE *> & ues,
	    DynArray<SubUE *> & sub_ues, TreeSet<year_t> & years);
};

/** Tipo que representa una venta para una producción.

    @author Alejandro J. Mujica
*/
struct Sale
{
  UE   * client;
  double quantity;
  string meassurement_unit;
  double price;

  Sale()
    : client(nullptr), quantity(0.0), meassurement_unit(""), price(0.0)
  {
    // empty
  }

  Sale(const Sale & s)
    : client(s.client), quantity(s.quantity),
      meassurement_unit(s.meassurement_unit), price(s.price)
  {
    // empty
  }

  Sale(Sale && s)
    : Sale()
  {
    std::swap(client, s.client);
    std::swap(quantity, s.quantity);
    std::swap(meassurement_unit, s.meassurement_unit);
    std::swap(price, s.price);
  }

  Sale & operator = (const Sale & s)
  {
    if (&s == this)
      return *this;

    client = s.client;
    quantity = s.quantity;
    meassurement_unit = s.meassurement_unit;
    price = s.price;
    return *this;
  }

  Sale & operator = (Sale && s)
  {
    std::swap(client, s.client);
    std::swap(quantity, s.quantity);
    std::swap(meassurement_unit, s.meassurement_unit);
    std::swap(price, s.price);
    return *this;
  }

  void save(ostream & out, TreeMap<UE *, size_t> & ue_idxs) const
  {
    out << ue_idxs[client] << endl;
    out << quantity << endl;
    out << meassurement_unit << endl;
    out << price << endl;
  }

  void load(istream & in, DynArray<UE *> & ues)
  {
    string line;
    getline(in, line);
    size_t ue_idx = stoul(line);
    client = ues.access(ue_idx);
    getline(in, line);
    quantity = stod(line);
    getline(in, meassurement_unit);
    getline(in, line);
    price = stod(line);
  }
};

/** Tipo que representa la producción de un insumo.

    @author Alejandro J. Mujica
*/
struct InputProduction
{
  Input                            * input;
  TreeMap<year_t, List<Purchase>> purchases_by_year;

  InputProduction()
    : input(nullptr), purchases_by_year()
  {
    // empty
  }

  InputProduction(const InputProduction & i)
    : input(i.input), purchases_by_year(i.purchases_by_year)
  {
    // empty
  }

  InputProduction(InputProduction && i)
    : InputProduction()
  {
    std::swap(input, i.input);
    purchases_by_year.swap(i.purchases_by_year);
  }

  InputProduction & operator & (const InputProduction & i)
  {
    if (&i == this)
      return *this;

    input = i.input;
    purchases_by_year = i.purchases_by_year;
    return *this;
  }

  InputProduction & operator = (InputProduction && i)
  {
    std::swap(input, i.input);
    purchases_by_year.swap(i.purchases_by_year);
    return *this;
  }

  /// Retorna las compras asociadas a la producción para un año dado.
  List<Purchase> & purchases(year_t year)
  {
    return purchases_by_year[year];
  }

  /// Como la anterior pero para objetos constantes.
  const List<Purchase> & purchases(year_t year) const
  {
    return purchases_by_year[year];
  }

  void save(ostream & out, TreeMap<UE *, size_t> & ue_idxsm) const;

  void load(istream & in, DynArray<UE *> & ues);
};

/** Tipo que representa un insumo.

    @author Alejandro J. Mujica
*/
struct Input
{
  db_id_t                          db_id;
  string                           name;
  Product                        * product;
  TariffCodeSubSubItem           * tariffcode;
  TreeMap<year_t, InputProduction> productions_by_year;

  Input()
    : db_id(0), name(""), product(nullptr), tariffcode(nullptr),
      productions_by_year()
  {
    // empty
  }

  Input(const Input & i)
    : db_id(i.db_id), name(i.name), product(i.product),
      tariffcode(i.tariffcode),
      productions_by_year(i.productions_by_year)
  {
    // empty
  }

  Input(Input && i)
    : Input()
  {
    std::swap(db_id, i.db_id);
    std::swap(name, i.name);
    std::swap(product, i.product);
    std::swap(tariffcode, i.tariffcode);
    productions_by_year.swap(i.productions_by_year);
  }

  Input & operator = (const Input & i)
  {
    if (&i == this)
      return *this;

    db_id = i.db_id;
    name = i.name;
    product = i.product;
    tariffcode = i.tariffcode;
    productions_by_year = i.productions_by_year;
    return *this;
  }

  Input & operator = (Input && i)
  {
    std::swap(db_id, i.db_id);
    std::swap(name, i.name);
    std::swap(product, i.product);
    std::swap(tariffcode, i.tariffcode);
    productions_by_year.swap(i.productions_by_year);
    return *this;
  }

  /// Retorna la producción del insumo para un año dado.
  InputProduction & production(year_t year)
  {
    return productions_by_year[year];
  }

  /// Como la anterior pero para objetos constantes.
  const InputProduction & production(year_t year) const
  {
    return productions_by_year[year];
  }

  void save(ostream & out, TreeMap<Cod *, size_t> & tc_idxs,
	    TreeMap<Product *, size_t> & products_idx,
	    TreeMap<UE *, size_t> & ue_idxs) const;

  void load(istream & in, DynArray<Cod *> & tcs, DynArray<Product *> & products,
	    DynArray<UE *> & ues, TreeSet<year_t> & years);
};

/** Tipo que representa una compra.

    @author Alejandro J. Mujica
*/
struct Purchase
{
  UE   * provider;
  double quantity;
  string meassurement_unit;
  double price;

  Purchase()
    : provider(nullptr), quantity(0.0), meassurement_unit(""), price(0.0)
  {
    // empty
  }

  Purchase(const Purchase & p)
    :  provider(p.provider),
       quantity(p.quantity), meassurement_unit(p.meassurement_unit),
       price(p.price)
  {
    // empty
  }

  Purchase(Purchase && p)
    : Purchase()
  {
    std::swap(provider, p.provider);
    std::swap(quantity, p.quantity);
    std::swap(meassurement_unit, p.meassurement_unit);
    std::swap(price, p.price);
  }

  Purchase & operator = (const Purchase & p)
  {
    if (&p == this)
      return *this;

    provider = p.provider;
    quantity = p.quantity;
    meassurement_unit = p.meassurement_unit;
    price = p.price;
    return *this;
  }

  Purchase & operator = (Purchase && p)
  {
    std::swap(provider, p.provider);
    std::swap(quantity, p.quantity);
    std::swap(meassurement_unit, p.meassurement_unit);
    std::swap(price, p.price);
    return *this;
  }
  
  void save(ostream & out, TreeMap<UE *, size_t> & ue_idxs) const
  {
    out << ue_idxs[provider] << endl;
    out << quantity << endl;
    out << meassurement_unit << endl;
    out << price << endl;
  }

  void load(istream & in, DynArray<UE *> & ues)
  {
    string line;
    getline(in, line);
    size_t provider_idx = stoul(line);
    provider = ues.access(provider_idx);
    getline(in, line);
    quantity = stod(line);
    getline(in, meassurement_unit);
    getline(in, line);
    price = stod(line);
  }
};

/** Criterio de comparación para códigos (actividades económicas o
    códigos arancelarios en conjuntos basados en Árbol Binario de Búsqueda.

    @author Alejandro J. Mujica
*/
struct CodCmp
{
  bool operator () (const Cod & c1, const Cod & c2) const
  {
    return c1.cod < c2.cod;
  };
};

/** Criterio de comparación para unidades económicas en conjuntos basados en
    Árbol Binario de Búsqueda.

    @author Alejandro J. Mujica
*/
struct UECmp
{
  bool operator () (const UE & ue1, const UE & ue2) const
  {
    return ue1.rif < ue2.rif;
  };
};

/** Criterio de comparación para sub unidades económicas en conjuntos basados en
    Árbol Binario de Búsqueda.

    @author Alejandro J. Mujica
*/
struct SubUECmp
{
  bool operator () (const SubUE & sub1, const SubUE & sub2) const
  {
    return sub1.db_id < sub2.db_id;
  }
};

/** Criterio de comparación para productos en conjuntos basados en
    Árbol Binario de Búsqueda.

    @author Alejandro J. Mujica
*/
struct ProductCmp
{
  bool operator () (const Product & p1, const Product & p2) const
  {
    return p1.db_id < p2.db_id;
  };
};

/** Criterio de comparación para insumos en conjuntos basados en
    Árbol Binario de Búsqueda.

    @author Alejandro J. Mujica
*/
struct InputCmp
{
  bool operator () (const Input & i1, const Input & i2) const
  {
    return i1.db_id < i2.db_id;
  }
};

/** Tipo que representa el mapa de toda la producción y relaciones económicas.

    @author Alejandro J. Mujica
*/
struct Map
{
  TreeSet<CAEVSection, CodCmp>          caev_sections;
  TreeSet<CAEVDivision, CodCmp>         caev_divisions;
  TreeSet<CAEVGroup, CodCmp>            caev_groups;
  TreeSet<CAEVClass, CodCmp>            caev_classes;
  TreeSet<CAEVBranch, CodCmp>           caev_branches;
  TreeSet<TariffCodeSection, CodCmp>    tariffcode_sections;
  TreeSet<TariffCodeChapter, CodCmp>    tariffcode_chapters;
  TreeSet<TariffCodeItem, CodCmp>       tariffcode_items;
  TreeSet<TariffCodeSubItem, CodCmp>    tariffcode_subitems;
  TreeSet<TariffCodeSubSubItem, CodCmp> tariffcode_subsubitems;
  TreeSet<UE, UECmp>                    ues;
  TreeSet<SubUE, SubUECmp>              sub_ues;
  TreeSet<Product, ProductCmp>          products;
  TreeSet<Input, InputCmp>              inputs;
  TreeSet<year_t>                       years;

  template <typename CodType>
  static void save_cod_set(ostream & out,
			   const TreeSet<CodType, CodCmp> & set,
			   TreeMap<Cod *, size_t> & idxs_r,
			   TreeMap<Cod *, size_t> & idxs_w)
  {
    out << set.size() << endl;
    
    size_t counter = 0;
    
    set.for_each([&](const CodType & s)
		 {
		   s.save(out, idxs_r);
		   CodType * ptr = const_cast<CodType *>(&s);
		   idxs_w[ptr] = counter++;
		 });
    
    idxs_r.empty();
    idxs_w.swap(idxs_r);
    assert(idxs_r.size() == set.size());
  }

  void save(ostream & out)
  {
    TreeMap<Cod *, size_t> caev_idxs_r, caev_idxs_w;

    save_cod_set<CAEVSection>(out, caev_sections, caev_idxs_r, caev_idxs_w);
    save_cod_set<CAEVDivision>(out, caev_divisions, caev_idxs_r, caev_idxs_w);
    save_cod_set<CAEVGroup>(out, caev_groups, caev_idxs_r, caev_idxs_w);
    save_cod_set<CAEVClass>(out, caev_classes, caev_idxs_r, caev_idxs_w);
    save_cod_set<CAEVBranch>(out, caev_branches, caev_idxs_r, caev_idxs_w);

    TreeMap<Cod *, size_t> tariffcode_idxs_r, tariffcode_idxs_w;

    save_cod_set<TariffCodeSection>(out, tariffcode_sections,
				    tariffcode_idxs_r, tariffcode_idxs_w);
    save_cod_set<TariffCodeChapter>(out, tariffcode_chapters,
				    tariffcode_idxs_r, tariffcode_idxs_w);
    save_cod_set<TariffCodeItem>(out, tariffcode_items,
				 tariffcode_idxs_r, tariffcode_idxs_w);
    save_cod_set<TariffCodeSubItem>(out, tariffcode_subitems,
				    tariffcode_idxs_r, tariffcode_idxs_w);    
    save_cod_set<TariffCodeSubSubItem>(out, tariffcode_subsubitems,
				       tariffcode_idxs_r, tariffcode_idxs_w);

    TreeMap<UE *, size_t> ue_idxs;
    size_t counter = 0;

    out << ues.size() << endl;

    ues.for_each([&](const UE & ue)
		 {
		   ue.save(out);
		   ue_idxs[&const_cast<UE &>(ue)] = counter++;
		 });

    TreeMap<SubUE *, size_t> sub_ue_idxs;
    counter = 0;

    out << sub_ues.size() << endl;

    sub_ues.for_each([&](const SubUE & sub_ue)
		     {
		       sub_ue.save(out, caev_idxs_r, ue_idxs);
		       sub_ue_idxs[&const_cast<SubUE &>(sub_ue)] = counter++;
		     });

    TreeMap<Product *, size_t> product_idxs;
    counter = 0;

    out << products.size() << endl;
    products.for_each([&](const Product & p)
		      {
			p.save(out, tariffcode_idxs_r, ue_idxs, sub_ue_idxs);
			product_idxs[&const_cast<Product &>(p)] = counter++;
		      });

    out << inputs.size() << endl;
    inputs.for_each([&](const Input & i)
		    {
		      i.save(out, tariffcode_idxs_r, product_idxs, ue_idxs);
		    });
  }

  void load(istream &);
};

/** Función que dada una cadena con el nivel de detalle de las actividades
    económicas retorna el enumerado correspondiente.

    @author Alejandro J. Mujica
*/
CAEVLevel str_to_caevlevel(const string & lvl);

/** Función que dada una constante (del tipo enumerado) con el nivel de
    detalle de las actividades económicas, retorna la cadena con el nombre
    del nivel.

    @author Alejandro J. Mujica
*/
string caevlevel_to_str(CAEVLevel level);

/** Función que busca en el mapa un código de actividad económica en el nivel
    dado.

    @author Alejandro J. Mujica
*/
CAEV * search_caevlevel(const string & cod, CAEVLevel level, const Map & map);

/** Función que dada una cadena con el nivel de detalle del código arancelario
    retorna el enumerado correspondiente.

    @author Alejandro J. Mujica
*/
TariffCodeLevel str_to_tariffcodelevel(const string & lvl);

/** Función que dada una constante (del tipo enumerado) con el nivel de
    detalle del código arancelario, retorna la cadena con el nombre
    del nivel.

    @author Alejandro J. Mujica
*/
string tariffcodelevel_to_str(TariffCodeLevel level);

/** Función que busca en el mapa un código arancelario en el nivel dado.

    @author Alejandro J. Mujica
*/
TariffCode * search_tariffcodelevel(const string & cod, TariffCodeLevel level,
				    const Map & map);

# endif // MODELS_H
