#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <map>
#include <string>
#include "symbolTableEntry.h"
using namespace std;

class SYMBOL_TABLE 
{
private:
  std::map<string, SYMBOL_TABLE_ENTRY> hashTable;

public:
  //Constructor
  SYMBOL_TABLE( ) { }

  // Add SYMBOL_TABLE_ENTRY x to this symbol table.
  // If successful, return true; otherwise, return false.
  bool addEntry(SYMBOL_TABLE_ENTRY x) 
  {
    // Make sure there isn't already an entry with the same name
    map<string, SYMBOL_TABLE_ENTRY>::iterator itr;
    if ((itr = hashTable.find(x.getName())) == hashTable.end()) 
    {
      hashTable.insert(make_pair(x.getName(), x));
      return(true);
    }
    else return(false);
  }

  // If a symbol table entry with name theName is
  // found in this symbol table, then return its token type
  // info; otherwise, return token info with type UNDEFINED.
  TYPE_INFO findEntry(string theName) 
  {
    TYPE_INFO info = {UNDEFINED, NOT_APPLICABLE, NOT_APPLICABLE,
                      NOT_APPLICABLE};
    map<string, SYMBOL_TABLE_ENTRY>::iterator itr;
    if ((itr = hashTable.find(theName)) == hashTable.end())
      return(info);
    else return(itr->second.getTypeInfo());
  }

  
  // Count variables in SYMBOL_TABLE_ENTRY x in this symbol table.
  // Return number of variables including all variables in arrays.
  int countEntry() 
  {
    // Make sure there isn't already an entry with the same name
    std::map<std::string, SYMBOL_TABLE_ENTRY>::iterator itr = hashTable.begin();
    
    // variables
    int count = 0;
    while (itr != hashTable.end())
    {
        // set symbol table entry item
        //SYMBOL_TABLE_ENTRY y = itr->second;
        
        // see if the type is an Array type
        int baseName = itr->second.getTypeCode();
        if(baseName == ARRAY)
        {
            // if so then unravel and see how many elements there are
            int arrayNum = itr->second.getEndIndex() - itr->second.getStartIndex();
            
            if (arrayNum == 0)
            {
                count++;
            }
            else
            {
                count = count + arrayNum;
            }
        }
        if ((baseName == INT) ||
           (baseName == CHAR) ||
           (baseName == BOOL))
           {
               // at most one variable will be stored at a time per symbol table entry, besides arrays
               count++;
           }
        itr++;
    }
    return(count);
  }
  

};

#endif  // SYMBOL_TABLE_H
