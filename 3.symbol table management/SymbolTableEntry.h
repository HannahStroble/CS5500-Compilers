#ifndef SYMBOL_TABLE_ENTRY_H
#define SYMBOL_TABLE_ENTRY_H

#include <string>
using namespace std;

#define UNDEFINED  -1
#define FUNCTION     0
#define INT                1
#define STR               2
#define NOT_APPLICABLE -1

typedef struct
{
	int type;				// one of the above type codes
	int numParams;		// numParams and returnType only applicable if type == FUNCTION
	int returnType;
}TYPE_INFO;

class SYMBOL_TABLE_ENTRY
{
private:
  // Member variables
  string name;
  TYPE_INFO typeCode;

public:
  // Constructors
  SYMBOL_TABLE_ENTRY( ) 
  { name = ""; 
  typeCode.type = UNDEFINED; 
  typeCode.numParams = UNDEFINED;
  typeCode.returnType = UNDEFINED;
  }

  SYMBOL_TABLE_ENTRY(const string theName, TYPE_INFO typeinfo)
  {
    name = theName;
    typeCode.type = typeinfo.type;
	typeCode.numParams = typeinfo.numParams;
	typeCode.returnType = typeinfo.returnType;
  }
 

  // Accessors
  string getName() const { return name; }
  int getTypeCode() const { return typeCode; }
};

#endif  // SYMBOL_TABLE_ENTRY_H
