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
  Fecha de creación: 26/07/2013
  Este archivo contiene la definición de la clase DBQuery.
*/

# ifndef DB_QUERY_H
# define DB_QUERY_H

# include <dbConnection.H>

/** Abstrae un objeto de consulta a la base de datos en PostgreSQL 
 *
 *  Permite ejecutar consultas e iterar sobre el resultado.
 *
 *  @author Alejandro J. Mujica (amujica at cenditel dot gob dot ve)
 */
class DBQuery {

    DBConnection & connection;

    PGresult * pgresult;

    size_t numCols;

    size_t numRows;

    size_t currRow;

public:
    DBQuery(DBConnection &);

    ~DBQuery();

    /** Ejecuta un comando SQL.
     *
     *  @param strQuery cadena de caracteres con el comando SQL.
     *  @return true si la consulta de ejecuta correctamente, false si ocurre
     *          algún error.
     */
    bool exec(const std::string & strQuery);

    /// Limpia el resultado de una consulta.
    void clear();

    /// Ubica al iterador del resultado en la tupla inicial.
    void reset();

    /** Mueve el iterador del resultado a la siguiente tupla.
     *
     *  @return true si se pudo mover a una tupla siguiente, false si no hay más
     *          tuplas para moverse.
     */
    bool next();

    /** Indica si hay resultado de una consulta.
     *
     *  @retun true si hay tuplas, false si no hay.
     */
    bool hasResult() const;

    /** Consulta el valor de una columna dada (por número) en la tupla actual.
     *
     *  @param colNumber número de columna que se va a consultar.
     *  @return cadena de caracteres con el valor de la columna dada.
     *  @throw domain_error si no hay tuplas en el resultado.
     *  @throw overflow_error si el iterador pasó de la última tupla o si se
     *         consulta un número de columna no existente.
     */
    char * getValue(const size_t & colNumber);

    /** Consulta el valor de una columna dada (por nombre) en la tupla actual.
     *
     *  @param colName nombre de columna que se va a consultar.
     *  @return cadena de caracteres con el valor de la columna dada.
     *  @throw domain_error si no hay tuplas en el resultado.
     *  @throw overflow_error si el iterador pasó de la última tupla o si se
     *         consulta un nombre de columna no existente.
     */
    char * getValue(const std::string & colName);
};

# endif // DB_QUERY_H

