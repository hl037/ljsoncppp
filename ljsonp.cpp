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

#include "ljsonp.hpp"
#include <utility>
#include <limits>
#include <climits>

namespace ljsoncpp
{


using namespace std;


#define UCHAR_MIN 0
#define USHRT_MIN 0
#define UINT_MIN 0
#define ULONG_MIN 0
#define ULLONG_MIN 0

#define Value__get(ty,u) \
template<>\
ty Value::get<ty>(bool * ok)\
{\
   if(ok) *ok = true;\
   if(t != NUMBER || d.n > u##_MAX || d.n < u##_MIN)\
   {\
      if(ok) *ok = false;\
      return 0;\
   }\
   return d.n;\
}
#define Value__set(ty) \
template<>\
void Value::set<ty>(const ty & n)\
{\
   destroy();\
   t = NUMBER;\
   d.n = n;\
}

Value__get(char, CHAR)
Value__get(short, SHRT)
Value__get(int, INT)
Value__get(long, LONG)
Value__get(long long, LLONG)

Value__get(unsigned char, UCHAR)
Value__get(unsigned short, USHRT)
Value__get(unsigned int, UINT)
Value__get(unsigned long, ULONG)
Value__get(unsigned long long, ULLONG)


Value__set(char)
Value__set(short)
Value__set(int)
Value__set(long)
Value__set(long long)

Value__set(unsigned char)
Value__set(unsigned short)
Value__set(unsigned int)
Value__set(unsigned long)
Value__set(unsigned long long)
Value__set(float)
Value__set(double)
Value__set(long double)




#undef Value__get
#undef Value__set
#define Value__get(ty,vb,ret) \
template<> \
ty Value::get<ty>(bool * ok)\
{\
   if(ok) *ok = true;\
   if(t != vb)\
   {\
      if(ok) *ok = false;\
      return 0;\
   }\
   return ret;\
}
#define Value__set(ty, vb, p, assign) \
template<>\
void Value::set<ty>(ty const & p)\
{\
   destroy();\
   t = vb;\
   d.ptr = assign;\
}

Value__get(string, STRING, *static_cast<string*>(d.ptr))
Value__get(Object*, OBJECT, static_cast<Object*>(d.ptr))
Value__get(bool, BOOL, d.b)
Value__get(Array*, ARRAY, static_cast<Array*>(d.ptr))
Value__get(float, NUMBER, d.n)
Value__get(double, NUMBER, d.n)
Value__get(long double, NUMBER, d.n)

Value__set(string, STRING, str, new string(str))
Value__set(Object*, OBJECT, obj, obj)
Value__set(Array*, ARRAY, arr, arr)

template<>
void Value::set<bool>(const bool & b)
{
   destroy();
   t = BOOL;
   d.b = b;
}


void Value::setNull()
{
   destroy();
   t = NULL_VALUE;
}

#undef Value__get
#undef Value__set

#define NextChar(c, in) c=in.get(), cerr<<(char)c
void Value::destroy()
{
   switch(t)
   {
   case STRING:
      delete static_cast<string*>(d.ptr);
      break;
   case NUMBER:
      d.n = 0;
      break;
   case OBJECT:
   {
      Object * o = static_cast<Object*>(d.ptr);
      for(auto p : *o)
      {
         delete p.second;
      }
      delete o;
      break;
   }
   case ARRAY:
   {
      Array * a = static_cast<Array*>(d.ptr);
      for(Value * val : *a)
      {
         delete val;
      }
      delete a;
      break;
   }
   case BOOL:
      d.b = false;
      break;
   case NULL_VALUE:
      break;
   }
   t = NULL_VALUE;
}

Value::~Value()
{
   destroy();
}




Parser::Parser(istream &in) :
in(in),
c(0)
{

}

bool Parser::readCom()
{
   in>>c;
   if(c == '/')
   {
      in.ignore(numeric_limits<streamsize>::max(), '\n');
      return true;
   }
   if(c == '*')
   {
      do
      {
         in.ignore(numeric_limits<streamsize>::max(), '*');
      } while(in.get()!='/');
      return true;
   }
   return false;
}


string Parser::readString()
{
   if(!c) NextChar(c, in);
   stringstream str;
   stringstream str2;
   for(; c != '"' ; NextChar(c, in))
   {
      if(c == '\\')
      {
         NextChar(c, in);
         switch(c)
         {
         case '"':
         case '\\':
         case '/':
            str.put(c);
            break;
         case 'b':
            str.put('\b');
            break;
         case 'f':
            str.put('\f');
            break;
         case 'n':
            str.put('\n');
            break;
         case 'r':
            str.put('\r');
            break;
         case 't':
            str.put('\t');
            break;
         case 'u':
         {
            str2.str("");
            str2.put(c);
            int nb = 0;
            for(int i=0 ; i<4 ; ++i)
            {
               nb*=16;
               NextChar(c, in);
               str2.put(c);
               if(c>='0' && c<='9')
               {
                  nb+=(c-'0');
               }
               else if(c>='a' && c<='z')
               {
                  nb+=(c-'a');
               }
               else if(c>='A' && c<='Z')
               {
                  nb+=(c-'A');
               }
               else goto FAIL;
            }
            str.put(nb);
            break;
            FAIL:
            str<<str2;
            break;
         }
         default:
            str.put('\\');
            str.put(c);
         }
      }
      else
      {
         str.put(c);
      }
   }while( c != '"');
   c=0;
   return str.str();
}


Value * Parser::readArray()
{
   Array * a = new Array(10);

   if(!c) NextChar(c, in);
   for(;;NextChar(c, in))
   {
      switch(c)
      {
      case ']':
         if(!a->empty()) goto FAIL;
         c = 0;
         return new Value(a);
      default:
         Value * v = readValue();
         if(!v) goto FAIL;
         a->push_back(v);
         if(!c) NextChar(c, in);

         while(c != ',' && c != ']')
         {
            if(!isSpace(c)) goto FAIL;
            NextChar(c, in);
         }
         if(c == ']')
         {
            c = 0;
            return new Value(a);
         }
      }
   }
   FAIL :
   delete a;
   return nullptr;
}

Value * Parser::readObject()
{
   Object * o = new Object(30);

   string name;
   if(!c) NextChar(c, in);
   for(;; NextChar(c, in))
   {
      switch(c)
      {
      case '}':
         if(!o->empty()) goto FAIL;
         c = 0;
         return new Value(o);

      case '"':
      {
         c=0;
         name = readString();
         if(!c) NextChar(c, in);
         while(c != ':')
         {
            NextChar(c, in);
            if(!isSpace(c)) goto FAIL;
         }
         c=0;
         Value * v = readValue();
         if(!v) goto FAIL;
         o->insert(pair<string, Value*>(name, v));
         if(!c) NextChar(c, in);
         while(c != ',' && c!='}')
         {
            if(!isSpace(c)) goto FAIL;
            NextChar(c, in);
         }
         if(c == '}')
         {
            c = 0;
            return new Value(o);
         }
         break;
      }
      caseSpace:
         break;
      default:
         goto FAIL;
      }
   }
   FAIL :

   delete o;
   return nullptr;
}


Value * Parser::readValue()
{
   if(!c) NextChar(c, in);
   switch(c)
   {
   case '{':
      c=0;
      return readObject();
   case '[':
      c=0;
      return readArray();
   case '"':
      c=0;
      return new Value(readString());
   case '/':
      if(!readCom()) return nullptr;
      c=0;
      return readValue();
   caseSpace:
      c=0;
      return readValue();
   case 't':
      if(in.get() != 'r' || in.get()!='u' || in.get()!='e') return nullptr;
      c=0;
      return new Value(true);
   case 'f':
      if(in.get() != 'a' || in.get()!='l' || in.get()!='s' || in.get()!='e') return nullptr;
      c=0;
      return new Value(false);
   case 'n':
      if(in.get() != 'u' || in.get()!='l' || in.get()!='l') return nullptr;
      c=0;
      return new Value();
   default:
      if((c>='0' && c<='9') || c=='-')
      {
         stringstream str;
         do
         {
            str<<(char)c;
            NextChar(c, in);
         }while(c>='0' && c<='9');
         if(c == '.')
         {
            do
            {
               str<<(char)c;
               NextChar(c, in);
            }while(c>='0'&&c<='9');
         }
         if(c == 'e' || c == 'E')
         {
            str<<(char)c;
            NextChar(c, in);
            if(c != '+' && c != '-' && (c < '0' || c> '9')) return nullptr;
            do
            {
               str<<(char)c;
               NextChar(c, in);
            }while(c>='0'&& c<='9');
         }
         return new Value(stold(str.str()));
      }
      return nullptr;
   }
}

Value * Parser::parse(istream &in)
{
   Parser p(in);
   return p.readValue();
}

}




