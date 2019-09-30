#ifndef SYMBOL_TABLE_ENTRY_H
#define SYMBOL_TABLE_ENTRY_H

#include <string>
using namespace std;

// type code declarations
#define UNDEFINED  -1
#define NULL_TYPE   0
/*
  Defining these first five types as powers of two allows XORing
  any number of them together to create the unique type code we 
  need.
  FUNCTION doesn't combine with anything, but also having it as a
  power of two prevents any of the other type codes from adding
  up to it.
*/
#define INT         2
#define STR         4
#define BOOL        8
#define FLOAT       16
#define LIST        32
#define FUNCTION    64

// every unique combination of two type codes
#define INT_OR_STR        6
#define INT_OR_BOOL       10
#define INT_OR_FLOAT      18
#define STR_OR_BOOL       12
#define STR_OR_FLOAT      20
#define BOOL_OR_FLOAT     24
#define LIST_OR_INT       34
#define LIST_OR_STR       36
#define LIST_OR_BOOL      40
#define LIST_OR_FLOAT     48

// every unique combination of three type codes
#define INT_OR_STR_OR_BOOL        14
#define INT_OR_STR_OR_FLOAT       22
#define INT_OR_BOOL_OR_FLOAT      26
#define STR_OR_BOOL_OR_FLOAT      28
#define LIST_OR_INT_OR_STR        38
#define LIST_OR_INT_OR_BOOL       42
#define LIST_OR_INT_OR_FLOAT      50
#define LIST_OR_STR_OR_BOOL       44
#define LIST_OR_STR_OR_FLOAT      52
#define LIST_OR_BOOL_OR_FLOAT     56

// every unique combinations of four type codes
#define INT_OR_STR_OR_FLOAT_OR_BOOL     30
#define LIST_OR_FLOAT_OR_BOOL_OR_STR    60
#define LIST_OR_BOOL_OR_STR_OR_INT      46
#define LIST_OR_FLOAT_OR_STR_OR_INT     54
#define INT_OR_BOOL_OR_FLOAT_OR_LIST    58

// only one combination of five type codes
#define INT_OR_BOOL_OR_STR_OR_FLOAT_OR_LIST     62

#define NOT_APPLICABLE  -1

typedef struct {
  int type;         	// one of the above type codes
  int numParams;    	// # of parameters if function type
  int returnType;   	// return type if function
  bool isParam;		// true if ident is a function param
} TYPE_INFO;

class SYMBOL_TABLE_ENTRY
{
private:
  // Member variables
  string name;
  TYPE_INFO typeInfo;

public:
  // Constructors
  SYMBOL_TABLE_ENTRY( ) {
    name = "";
    typeInfo.type = UNDEFINED;
    typeInfo.numParams = UNDEFINED;
    typeInfo.returnType = UNDEFINED;
    typeInfo.isParam = false;
  }

  SYMBOL_TABLE_ENTRY(const string theName, 
                     const TYPE_INFO theType)
  {
    name = theName;
    typeInfo.type = theType.type;
    typeInfo.numParams = theType.numParams;
    typeInfo.returnType = theType.returnType;
    typeInfo.isParam = theType.isParam;
  }

  // Accessors
  string getName() const { return name; }
  TYPE_INFO getTypeInfo() const { return typeInfo; }

};

#endif  // SYMBOL_TABLE_ENTRY_H
