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

# ifndef LEVENSHTEIN_H
# define LEVENSHTEIN_H

size_t levenshtein(const string &s1, const string &s2)
{
  size_t N1 = s1.size();
  size_t N2 = s2.size();
  size_t i, j;
  vector<size_t> T(N2+1);

   for (i = 0; i <= N2; i++)
      T[i] = i;

   for (i = 0; i < N1; i++) 
   {
     T[0] = i+1;
     size_t corner = i;
     for (j = 0; j < N2; j++)
       {
         size_t upper = T[j+1];
         if (s1[i] == s2[j])
	   T[j+1] = corner;
         else
	   T[j+1] = min(T[j], min(upper, corner)) + 1;
         corner = upper;
       }
   }
   return T[N2];
}

# endif // LEVENSHTEIN_H
