/*
  Copyright (C) 2012
  Alejandro Mujica (amujica@cenditel.gob.ve)
  Erwin Paredes (eparedes@cenditel.gob.ve)
  José Ruiz (jruiz@cenditel.gob.ve)
  Rodolfo Rangel (rrangel@cenditel.gob.ve)
  Julie Vera (jvera@cenditel.gob.ve)
 
  CENDITEL Fundación Centro Nacional de Desarrollo e Investigación en
  Tecnologías Libres
 
  Este programa es software libre; Usted puede usarlo bajo los términos de la
  licencia de software GPL versión 2.0 de la Free Software Foundation.
 
  Este programa se distribuye con la esperanza de que sea útil, pero SIN
  NINGUNA GARANTÍA; tampoco las implícitas garantías de MERCANTILIDAD o
  ADECUACIÓN A UN PROPÓSITO PARTICULAR.
  Consulte la licencia GPL para más detalles. Usted debe recibir una copia
  de la GPL junto con este programa; si no, escriba a la Free Software
  Foundation Inc. 51 Franklin Street,5 Piso, Boston, MA 02110-1301, USA.
*/

/*
  Autor:             Alejandro J. Mujica
  Fecha de creación: 25/07/2013
  Este archivo contiene la implementación de la clase StrQuery.
*/

# include <strQuery.H>

StrQuery::StrQuery()
    : select(""), from(""), where(""), join(""), orderBy(""), orderByOption("") {

    // Empty
}
  
StrQuery::StrQuery(const StrQuery & strQuery)
    : select(strQuery.select), from(strQuery.from), where(strQuery.where),
      join(strQuery.join), orderBy(strQuery.orderBy),
      orderByOption(strQuery.orderByOption) {

    // Empty
}
  
StrQuery::StrQuery(StrQuery && strQuery)
  : select(), from(), where(), join(), orderBy(), orderByOption() {

    std::swap(select, strQuery.select);
    std::swap(from, strQuery.from);
    std::swap(where, strQuery.where);
    std::swap(join, strQuery.join);
    std::swap(orderBy, strQuery.orderBy);
    std::swap(orderByOption, strQuery.orderByOption);
}

void StrQuery::clear() {

    select.clear();
    from.clear();
    where.clear();
    join.clear();
    orderBy.clear();
    orderByOption.clear();
}

const std::string & StrQuery::getSelect() const {
    return select;
}

void StrQuery::setSelect(const std::string & select) {
    this->select = select;
}

void StrQuery::setSelect(std::string && select) {
    this->select = std::move(select);
}

const std::string & StrQuery::getFrom() const {
    return from;
}

void StrQuery::setFrom(const std::string & from) {
    this->from = from;
}

void StrQuery::setFrom(std::string && from) {
    this->from = std::move(from);
}

const std::string & StrQuery::getWhere() const {
    return where;
}

void StrQuery::setWhere(const std::string & where) {
    this->where = where;
}

void StrQuery::setWhere(std::string && where) {
    this->where = std::move(where);
}

const std::string & StrQuery::getJoin() const {
    return join;
}

void StrQuery::setJoin(const std::string & join) {
    this->join = join;
}

void StrQuery::setJoin(std::string && join) {
    this->join = std::move(join);
}

const std::string & StrQuery::getOrderBy() const {
    return orderBy;
}

void StrQuery::setOrderBy(const std::string & orderBy) {
    this->orderBy = orderBy;
}

void StrQuery::setOrderBy(std::string && orderBy) {
    this->orderBy = std::move(orderBy);
}

const std::string & StrQuery::getOrderByOption() const {
    return orderByOption;
}

void StrQuery::setOrderByOption(const std::string & orderByOption) {
    this->orderByOption = orderByOption;
}

void StrQuery::setOrderByOption(std::string && orderByOption) {
    this->orderByOption = std::move(orderByOption);
}

StrQuery & StrQuery::addSelect(const std::string & select) {

    this->select.append(this->select.empty() ? "SELECT " : ", ");
    this->select.append(select);

    return *this;
}

StrQuery & StrQuery::addFrom(const std::string & from) {

    this->from.append(this->from.empty() ? "FROM " : ", ");
    this->from.append(from);

    return *this;
}

StrQuery & StrQuery::addWhere(const std::string & where,
                              const std::string & connector) {

    this->where.append(this->where.empty() ? "WHERE " : " " + connector + " ");
    this->where.append(where);

    return *this;
}

StrQuery & StrQuery::addJoin(const std::string & table, const std::string & on,
    		             const std::string & joinType) {
    if (not join.empty()) {
        join.append(" ");
    }

    join.append(joinType);
    join.append(" JOIN ");
    join.append(table);
    join.append(" ON ");
    join.append(on);
    
    return *this;
}
  

StrQuery & StrQuery::addOrderBy(const std::string & orderBy) {

    this->orderBy.append(this->orderBy.empty() ? "ORDER BY " : ", ");
    this->orderBy.append(orderBy);

    return *this;
}

StrQuery & StrQuery::operator = (const StrQuery & strQuery) {

    if (&strQuery == this)
        return *this;

    select = strQuery.select;
    from = strQuery.from;
    where = strQuery.where;
    orderBy = strQuery.orderBy;
    orderByOption = strQuery.orderByOption;

    return *this;
}
  
StrQuery & StrQuery::operator = (StrQuery && strQuery) {

    std::swap(select, strQuery.select);
    std::swap(from, strQuery.from);
    std::swap(where, strQuery.where);
    std::swap(orderBy, strQuery.orderBy);
    std::swap(orderByOption, strQuery.orderByOption);

    return *this;
}

