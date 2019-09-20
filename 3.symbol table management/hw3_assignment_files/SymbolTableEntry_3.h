#ifndef SYMBOL_TABLE_ENTRY_H
#define SYMBOL_TABLE_ENTRY_H

#include <string>
using namespace std;

#define UNDEFINED  -1

class SYMBOL_TABLE_ENTRY
{
private:
  // Member variables
  string name;
  int typeCode;

public:
  // Constructors
  SYMBOL_TABLE_ENTRY( ) { name = ""; typeCode = UNDEFINED; }

  SYMBOL_TABLE_ENTRY(const string theName, const int theType)
  {
    name = theName;
    typeCode = theType;
  }

  // Accessors
  string getName() const { return name; }
  int getTypeCode() const { return typeCode; }
};

#endif  // SYMBOL_TABLE_ENTRY_H
