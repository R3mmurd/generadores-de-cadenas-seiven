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
  Este archivo contiene la definición de la clase DBConnection.
*/

# ifndef DB_CONNECTION_H
# define DB_CONNECTION_H

# include <memory>
# include <string>

# include <libpq-fe.h>

/** Abstrae un objeto de conexión a la base de datos en PostgreSQL.
 *
 * @author Alejandro J. Mujica
 */
class DBConnection {

    friend class DBQuery;

protected:
    PGconn * pgconn;

public:
    DBConnection();

    ~DBConnection();

    DBConnection(const DBConnection &) = delete;

    DBConnection & operator = (const DBConnection &) = delete;

    /// Abre la conexión a la base de datos.
    bool open();

    /// Cierra la conexión a la base de datos.
    void close();
};

# endif // DB_CONNECTION_H

