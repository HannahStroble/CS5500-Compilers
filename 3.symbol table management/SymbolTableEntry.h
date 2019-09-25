#ifndef SYMBOL_TABLE_ENTRY_H
#define SYMBOL_TABLE_ENTRY_H

#include <string>
#include "parser.h"
using namespace std;

#define UNDEFINED  -1

struct array_parts
{
  int start;
  int end;
  int type;
};

class SYMBOL_TABLE_ENTRY
{
private:
  // Member variables
  string name;
  bool isArray;
  int typeCode;
  array_parts parts;


public:
  // Constructors
  SYMBOL_TABLE_ENTRY( ) { name = ""; typeCode = UNDEFINED; }

  SYMBOL_TABLE_ENTRY(const string theName, const int theType)
  {
    name = theName;
    typeCode = theType;
    isArray = false;
  }
  
  SYMBOL_TABLE_ENTRY(const string theName, const array_parts array)
  {
    name = theName;
    parts = array;
    isArray = true;
  }




  // Accessors
  string getName() const { return name; }
  bool getArray() const { return isArray; }
  int getTypeCode() const { return typeCode; }
  array_parts getParts() const { return parts; }
};

#endif  // SYMBOL_TABLE_ENTRY_H
