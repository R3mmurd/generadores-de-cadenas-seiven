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
  Este archivo contiene la definición de la clase DBProperties.
*/

# ifndef DB_PROPERTIES_H
# define DB_PROPERTIES_H

# include <string>
# include <memory>

class DBProperties {

    std::string host;

    std::string port;

    std::string user;

    std::string pass;

    std::string dbname;

    static std::unique_ptr <DBProperties> instance;

    DBProperties();

public:
    static DBProperties & getInstance();

    const std::string & getHost() const;

    void setHost(const std::string &);

    void setHost(std::string &&);

    const std::string & getPort() const;

    void setPort(const std::string &);

    void setPort(std::string &&);

    const std::string & getUser() const;

    void setUser(const std::string &);

    void setUser(std::string &&);

    const std::string & getPassword() const;

    void setPassword(const std::string &);

    void setPassword(std::string &&);

    const std::string & getDbname() const;

    void setDbname(const std::string &);

    void setDbname(std::string &&);

    std::string getConnectionInfo();

    void readFile(const std::string &);
};

# endif // DB_PROPERTIES_H

