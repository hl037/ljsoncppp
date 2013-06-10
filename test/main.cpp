/*
 * Copyright (c) 2013 Léo Flaventin Hauchecorne <hl037.prog@gmail.com>
 *
 * This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include "../ljsonp.hpp"
#include <fstream>

using namespace std;
using namespace ljsoncpp;

void indent(int n)
{
   for(int i=0 ; i<n ; ++i)
   {
      cout<<"   ";
   }
}

void printValue(Value *v, int ind = 0)
{
   if(!v) return;
   cout<<"Value: ";
   switch(v->type())
   {
   case NUMBER:
      cout<<v->get<long double>()<<endl;
      return;
   case STRING:
      cout<<"\""<<v->get<string>()<<"\""<<endl;
      return;
   case BOOL:
      cout<<(v->get<bool>()? "true" : "false")<<endl;
      return;
   case NULL_VALUE:
      cout<<"null"<<endl;
      return;
   case ARRAY:
   {
      cout<<"["<<endl;
      for(Value * val : *(v->get<Array*>() ))
      {
         indent(ind);
         printValue(val, ind+1);
      }
      indent(ind);
      cout<<"]"<<endl;
      return;
   }
   case OBJECT:
      cout<<"{"<<endl;
      for(auto p : *(v->get<Object*>()))
      {
         indent(ind+1);
         cout<<"\""<<p.first<<"\" : ";
         printValue(p.second, ind+1);
      }
      indent(ind);
      cout<<"}"<<endl;
      return ;
   }
}



int main (int argc, char * argv[])
{
   if(argc!=3)
   {
      cout<<"test fichier.json sortie.json"<<endl;
      return 0;
   }

   ifstream f(argv[1], ifstream::in);
   Value * v = Parser::parse(f);
   printValue(v);
   cout<<"--------------------------------"<<endl;
   Writter::write(cout, v);
   ofstream f2(argv[2], ofstream::out);
   Writter::write(f2, v);
   
   delete v;

}
