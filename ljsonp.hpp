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


#include <vector>
#include <iostream>
#include <istream>
#include <sstream>
#include <stack>
#include <unordered_map>

namespace ljsoncpp
{
using namespace std;

enum Valuetype
{
   STRING,
   NUMBER,
   OBJECT,
   ARRAY,
   BOOL,
   NULL_VALUE
};

class Value
{
protected :
   union ValueBase
   {
      bool b;
      long double n;
      void * ptr;
   };
   Valuetype t;
   ValueBase d;
   void destroy();
public:
   inline Valuetype type() { return t; }
   template<class T>
   Value(const T & v);
   inline Value() : t(NULL_VALUE) {}
   ~Value();

   template<class T>
   T get(bool * ok = 0);

   template<class T>
   void set(const T &);

   void setNull();
};

typedef unordered_map<string, Value*> Object;
typedef std::vector<Value *> Array;



template<class T>
Value::Value(const T & v):
t(NULL_VALUE)
{
   set<T>(v);
}

#define isSpace(c) (c==' ' || c=='\t' || c=='\n' || c=='\r')
#define caseSpace case ' ': case '\t': case '\n': case '\r'

class Parser
{
protected:
   istream & in;
   int c;

   bool readCom();
   string readString();
   Value * readArray();
   Value * readObject();
   Value * readValue();
   Parser(istream & in);

public:

   static Value * parse(istream & in);

};



}
