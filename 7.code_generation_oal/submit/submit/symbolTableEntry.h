#ifndef SYMBOL_TABLE_ENTRY_H
#define SYMBOL_TABLE_ENTRY_H

#include <string>
using namespace std;

#define UNDEFINED  			-1   // Type codes
#define PROCEDURE			0
#define INT				1
#define CHAR				2
#define INT_OR_CHAR			3
#define BOOL				4
#define INT_OR_BOOL			5
#define CHAR_OR_BOOL			6
#define INT_OR_CHAR_OR_BOOL		7
#define ARRAY				8
#define INDEX_RANGE			9
#define PROGRAM				10

#define NOT_APPLICABLE 		-1

typedef struct 
{ 
  int type;        // one of the above type codes
  int startIndex;  // if array, starting index
  int endIndex;    //           ending index
  int baseType;    //           base type (one of above codes)
  int level;
  int offset;
  int label;
} TYPE_INFO;

class SYMBOL_TABLE_ENTRY 
{
private:
  // Member variables
  string name;
  TYPE_INFO typeInfo;  

public:
  // Constructors
  SYMBOL_TABLE_ENTRY( ) 
  { 
    name = ""; 
    typeInfo.type = UNDEFINED;                       
    typeInfo.startIndex = UNDEFINED; 			    
    typeInfo.endIndex = UNDEFINED; 				    
    typeInfo.baseType = UNDEFINED; 
  }

  SYMBOL_TABLE_ENTRY(const string theName, const int theType,
                     const int theStart, const int theEnd,
                     const int theBaseType) 
  {
    name = theName;
    typeInfo.type = theType;
    typeInfo.startIndex = theStart;
    typeInfo.endIndex = theEnd;
    typeInfo.baseType = theBaseType;
  }

  SYMBOL_TABLE_ENTRY(const string theName, 
                     const TYPE_INFO info) 
  {
    name = theName;
    typeInfo.type = info.type;
    typeInfo.startIndex = info.startIndex;
    typeInfo.endIndex = info.endIndex;
    typeInfo.baseType = info.baseType;
    typeInfo.level = info.level;
    typeInfo.offset = info.offset;
    typeInfo.label = info.label;
  }

  // Accessors
  string getName() const { return name; }
  TYPE_INFO getTypeInfo() const { return typeInfo; }
  int getTypeCode() const { return typeInfo.type; }
  int getStartIndex() const { return typeInfo.startIndex; }
  int getEndIndex() const { return typeInfo.endIndex; }
  int getBaseType() const { return typeInfo.baseType; }
  int getNestLevel() const { return typeInfo.level; }
  int getLabel() const { return typeInfo.label; }
};

#endif  // SYMBOL_TABLE_ENTRY_H
