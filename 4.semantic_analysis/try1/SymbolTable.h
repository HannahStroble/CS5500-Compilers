#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <map>
#include <string>
#include "SymbolTableEntry.h"
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
  
  // Change entry in the symbol table
  bool changeEntry(SYMBOL_TABLE_ENTRY x)
  {
    // Make sure the entry we want to change in the symbol table 
    // is actually in the symbol table.
    map<string, SYMBOL_TABLE_ENTRY>::iterator itr;
    if ((itr = hashTable.find(x.getName())) != hashTable.end())
    {
      hashTable.erase(itr);
      hashTable.insert(make_pair(x.getName(), x));
      return true;
    }
    else return(false);
  }

  // If a SYMBOL_TABLE_ENTRY with name theName is
  // found in this symbol table, then return true;
  // otherwise, return false.
  TYPE_INFO findEntry(string theName)
  {
    TYPE_INFO info;
    info.start = UNDEFINED;
    info.end = UNDEFINED;
    info.type = UNDEFINED;      
    info.numParams = UNDEFINED; 
    info.returnType = UNDEFINED;
    info.isParam = UNDEFINED;  
    info.isArray = UNDEFINED;
    map<string, SYMBOL_TABLE_ENTRY>::iterator itr;
    if ((itr = hashTable.find(theName)) == hashTable.end())
      return(info);
    else return(itr->second.getTypeParts());
  }
  
  // Return # entries in the symbol table
  int getNumEntries()
  {
    return(hashTable.size());
  }

};

#endif  // SYMBOL_TABLE_H
