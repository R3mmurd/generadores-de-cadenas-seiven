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
  Este archivo contiene la definición de la clase StrQuery.
*/

# ifndef StrQuery_H
# define StrQuery_H

# include <iostream>
# include <string>

/** Representa una cadena de consulta que se construye dinámicamente por las
 *  operaciones select, from, y where.
 *
 * @author Alejandro J. Mujica
 */
class StrQuery {

    std::string select;

    std::string from;

    std::string where;

    std::string join;

    std::string orderBy;

    std::string orderByOption;

public:
    StrQuery();

    StrQuery(const StrQuery &);

    StrQuery(StrQuery &&);

    /// Limpia la cadena.
    void clear();

    /// Consulta la sección select.
    const std::string & getSelect() const;

    /// Asigna la sección select (l-values)
    void setSelect(const std::string &);

    /// Asigna la sección select (r-values)
    void setSelect(std::string &&);

    /// Consulta la sección from.
    const std::string & getFrom() const;

    /// Asigna la sección from (l-values)
    void setFrom(const std::string &);

    /// Asigna la sección from (r-values)
    void setFrom(std::string &&);

    /// Consulta la sección where.
    const std::string & getWhere() const;

    /// Asigna la sección where (l-values)
    void setWhere(const std::string &);

    /// Asigna la sección where (r-values)
    void setWhere(std::string &&);

    const std::string & getJoin() const;

    /// Asigna la sección join (l-values)
    void setJoin(const std::string &);

    /// Asigna la sección join (r-values)
    void setJoin(std::string &&);

    /// Consulta la sección orderBy.
    const std::string & getOrderBy() const;

    /// Asigna la sección orderBy (l-values)
    void setOrderBy(const std::string &);

    /// Asigna la sección orderBy (r-values)
    void setOrderBy(std::string &&);

    /// Consulta la sección orderByOption.
    const std::string & getOrderByOption() const;

    /// Asigna la sección orderByOption (l-values)
    void setOrderByOption(const std::string &);

    /// Asigna la sección orderByOption (r-values)
    void setOrderByOption(std::string &&);

    /// Añade elemento a la sección select.
    StrQuery & addSelect(const std::string &);

    /// Añade elemento a la sección from.
    StrQuery & addFrom(const std::string &);

    /// Añade elemento a la sección where.
    StrQuery & addWhere(const std::string &,
                        const std::string & connector = "AND");

    /// Añade elemento a la sección join
    StrQuery & addJoin(const std::string &, const std::string &,
   		       const std::string & joinType = "INNER");

    /// Añade elemento a la sección orderBy.
    StrQuery & addOrderBy(const std::string &);

    StrQuery & operator = (const StrQuery &);

    StrQuery & operator = (StrQuery &&);

    /// Casting a std::string
    operator std::string () {

        std::string str;

        str.append(select);
        str.append(" ");
        str.append(from);

	if (not join.empty()) {
  	    str.append(" ");
	    str.append(join);
	}

        if (not where.empty()) {
            str.append(" ");
            str.append(where);
        }

        if (not orderBy.empty()) {
            str.append(" ");
            str.append(orderBy);

            if (not orderByOption.empty()) {
                str.append(" ");
                str.append(orderByOption);
            }
        }

        return str;
    }

    friend std::ostream & operator << (std::ostream & out,
                                       const StrQuery & strQuery) {
        out << strQuery.getSelect()
            << " " << strQuery.getFrom();

	if (not strQuery.getJoin().empty())
            out << " " << strQuery.getJoin();

        if (not strQuery.getWhere().empty())
            out << " " << strQuery.getWhere();

        if (not strQuery.getOrderBy().empty()) {
            out << " " << strQuery.getOrderBy();

            if (not strQuery.getOrderByOption().empty())
                out << " " << strQuery.getOrderByOption();
        }

        return out;
    }
};

# endif // StrQuery_H

