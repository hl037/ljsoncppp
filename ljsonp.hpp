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

/**
 * @brief Valuetype enumerate the JavaScript base data type. You can get and set a value in Value class calling
 * respectively Value::get<T>() and Value::set<T>() where T is the c++ type representing the js type wanted (see the documentation below)
 */
enum Valuetype
{
   STRING=0, ///< js string type, represented by std::string
   NUMBER, ///< js number type, represented by any c++ number type
   OBJECT, ///< js Object type (without methods), represented by a pointer to an Object (inherits std::unordered_map<string, Value*>)
   ARRAY, ///< js Array type, represented by a std::vector<Value*>
   BOOL, ///< js boolean, represented by bool
   NULL_VALUE ///< js null value, not represented, but testable calling Value::isNull()
};

/**
 * @brief The Value class is an object to represent any type of javascript variable \sa Valuetype
  *It represents also a node in the json tree.
 */
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

   /**
    * Constructs a Value representing the value given
    */
   template<class T>
   Value(const T & v);
   /**
    * @brief Constructs a NULL_VALUE
    */
   inline Value() : t(NULL_VALUE) {}
   ~Value();

   /**
    * @brief Get the value represented by the object.
    */
   template<class T>
   T get(bool * ok = 0);

   /**
    * @brief Set the value represented by the object.
    */
   template<class T>
   void set(const T &);

   void setNull();
   inline bool isNull() { return t == NULL_VALUE; }
};

class Object : public unordered_map<string, Value*>
{
   /*
   typedef string Key;
   typedef Value* T;
   typedef hash<Key> Hash;
   typedef equal_to<Key> Pred;
   typedef allocator< pair<const Key,T> > Alloc;
*/
public:
   inline explicit Object ( size_type n = 10,
                            const hasher& hf = hasher(),
                            const key_equal& eql = key_equal(),
                            const allocator_type& alloc = allocator_type() ) :
   unordered_map(n, hf, eql, alloc)
   { }

   inline Object ( const unordered_map& ump ):
   unordered_map(ump)
   { }

   inline Object ( unordered_map&& ump ):
   unordered_map(ump)
   { }

//   inline explicit Object ( const allocator_type& alloc ):
//   unordered_map(alloc)
//   { }

//   inline Object ( const unordered_map& ump, const allocator_type& alloc ):
//   unordered_map(ump, alloc)
//   { }

//   inline Object ( unordered_map&& ump, const allocator_type& alloc ):
//   unordered_map(ump, alloc)
//   { }

   template <class InputIterator>
   inline Object ( InputIterator first, InputIterator last,
                            size_type n = 10,
                            const hasher& hf = hasher(),
                            const key_equal& eql = key_equal(),
                            const allocator_type& alloc = allocator_type() ) :
   unordered_map(first, last, n, hf, eql, alloc)
   { }

   inline Object ( initializer_list<value_type> il,
                   size_type n = 10,
                   const hasher& hf = hasher(),
                   const key_equal& eql = key_equal(),
                   const allocator_type& alloc = allocator_type() ) :
   unordered_map(il, n, hf, eql, alloc)
   { }

   template<class T>
   T getAttr(const string & k, bool * ok = nullptr)
   {
      auto it = find(k);
      if(it == end())
      {
         if(ok) *ok = false;
         return Value().get<T>();
      }
      return it->second->get<T>(ok);
   }

   Value * getValAttr(const string & k)
   {
      auto it = find(k);
      if(it == end())
      {
         return nullptr;
      }
      return it->second;
   }
};

/**
 * @brief Array of Value, internally a typedef of std::vector<Value*>
 */
typedef std::vector<Value *> Array;



template<class T>
Value::Value(const T & v):
t(NULL_VALUE)
{
   set<T>(v);
}

#define isSpace(c) (c==' ' || c=='\t' || c=='\n' || c=='\r')
#define caseSpace case ' ': case '\t': case '\n': case '\r'


/**
 * @brief The Parser class is the object to use to transphorm a json document to a Value* tree.
 * Not this object is not instensable, call Parser::parse() instead
 */
class Parser
{
protected:
   /**
    * @brief the input stream
    */
   istream & in;

   /**
    * @brief last char read
    */
   int c;

   /**
    * @brief [internal] skip a comment from the input stream
    * @return true if comment was skip with success, false if the token doesn't match a comment token (synthaxe error)
    */
   bool readCom();

   /**
    * @brief [internal] read a string until reach '"' or EOF from the input stream
    * @return the string
    */
   string readString();

   /**
    * @brief [internal]
    * @return The value read or nullptr on error
    */
   Value * readArray();

   /**
    * @brief [internal]
    * @return The value read or nullptr on error
    */
   Value * readObject();

   /**
    * @brief [internal]
    * @return The value read or nullptr on error
    */
   Value * readValue();


   /**
    * @brief [internal] Parser consttructor launching the parsing
    * @param in standard input stream
    * @return The value reprensenting the root object in success, otherwise nullptr.
    */
   Parser(istream & in);

public:


   /**
    * @brief Parse a json file
    * @param in standard input stream
    * @return The value reprensenting the root object in success, otherwise nullptr.
    */
   static Value * parse(istream & in);

};



}
