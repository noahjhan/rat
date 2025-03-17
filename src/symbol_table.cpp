#include "symbol_table.hpp"

SymbolTable::SymbolTable() {}

void SymbolTable::enterScope()
{
  unsigned int scope_frame_size = 0;
  frame_size_.push(scope_frame_size);
}
void SymbolTable::exitScope()
{
  unsigned int scope_frame_size = frame_size_.top();
  frame_size_.pop();

  for (unsigned i = 0; i < scope_frame_size; ++i)
  {
    if (stack_.empty())
    {
      throw std::invalid_argument("error: stack should not be empty");
    }
    stack_.pop();
  }
}

bool SymbolTable::lookupFunction(
const std::string &identifier,
const std::vector<std::pair<std::string, ConstituentToken>> &parameters,
const ConstituentToken &return_type)
{
  // find identifier (within size of scope)
  // if not found, return false
  // if found, validate return type and param
  return false;
}

bool SymbolTable::lookupVariable(const std::string &identifier,
                                 const ConstituentToken &type,
                                 const std::string &value)
{
  // find identifier (within size of scope)
  // if not found, return false
  // if found, validate type and value
  return false;
}

void SymbolTable::addFunction(
const std::string &identifier,
const std::vector<std::pair<std::string, ConstituentToken>> &parameters,
const ConstituentToken &return_type)
{
  // lookup
  // if found, throw (redefinition)
  // if not found, top of ++frame_size_
  // create struct
  // add to top of stack
  return;
}

void SymbolTable::addVariable(const std::string &identifier,
                              const ConstituentToken &type,
                              const std::string &value)
{
  // lookup
  // if found, throw (redefinition)
  // if not found, top of ++frame_size_
  // create struct
  // add to top of stack
  return;
}
