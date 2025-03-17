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

bool SymbolTable::lookupFunction();
bool SymbolTable::lookupVariable();

void SymbolTable::addFunction();
void SymbolTable::addVariable();
