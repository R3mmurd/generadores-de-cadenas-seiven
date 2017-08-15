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
  Este archivo contiene la implementación de la clase DBQuery.
*/

# include <stdexcept>
# include <sstream>
# include <dbQuery.H>

DBQuery::DBQuery(DBConnection & connection)
    : connection(connection), pgresult(NULL), numCols(0), numRows(0),
      currRow(0) {

    // Empty
}

DBQuery::~DBQuery() {
    clear();
}

bool DBQuery::exec(const std::string & strQuery) {

    pgresult = PQexec(connection.pgconn, strQuery.c_str());

    ExecStatusType est = PQresultStatus(pgresult);

    if (est == PGRES_TUPLES_OK) {
        numCols = PQnfields(pgresult);
        numRows = PQntuples(pgresult);
        return true;
    }

    clear();

    return false;    
}

void DBQuery::clear() {

    if (pgresult == NULL)
        return;

    PQclear(pgresult);
    pgresult = NULL;
    numCols = 0;
    numRows = 0;
    currRow = 0;
}

void DBQuery::reset() {
    currRow = 0;
}

bool DBQuery::next() {
    return currRow++ < numRows;
}

bool DBQuery::hasResult() const {
    return numRows > 0;
}

char * DBQuery::getValue(const size_t & colNumber) {

    if (not hasResult())
        throw std::domain_error("there is not result");

    if (currRow > numRows)
        throw std::overflow_error("there is not more tuples");

    if (colNumber >= numCols)
        throw std::out_of_range("column number is out of range");

    return PQgetvalue(pgresult, currRow - 1, colNumber);
}

char * DBQuery::getValue(const std::string & colName) {
    try {
        return getValue(PQfnumber(pgresult, colName.c_str()));
    }
    catch (const std::out_of_range &) {
        std::stringstream s;
        s << "Column " << colName << " doesn't exist";
        throw std::out_of_range(s.str());
    }
}

