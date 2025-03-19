#include "symbol_table.hpp"

SymbolTable::SymbolTable() {}

void SymbolTable::enterScope()
{
  SYMBOL_VARIANT scope_marker = nullptr;
  stack_.push_back(scope_marker);
}
void SymbolTable::exitScope()
{
  while (!std::holds_alternative<void *>(stack_.back())) {
    if (stack_.empty()) {
      throw std::invalid_argument("error: stack should not be empty");
    }
    if (std::holds_alternative<FunctionSymbol>(stack_.back())) {
      FunctionSymbol function = std::get<FunctionSymbol>(stack_.back());
      function_table_.erase(function.identifier);
    }
    // this check is redundant
    if (std::holds_alternative<VariableSymbol>(stack_.back())) {
      VariableSymbol variable = std::get<VariableSymbol>(stack_.back());
      variable_table_.erase(variable.identifier);
    }
    stack_.pop_back();
  }

  if (std::holds_alternative<void *>(stack_.front())) {
    stack_.pop_back(); // remove end of scope marker
  }
}

/// @todo if reference matches only one category, throw
bool SymbolTable::lookupFunction(const std::string &identifier,
                                 const std::vector<std::pair<std::string, ConstituentToken>> &parameters,
                                 const ConstituentToken &return_type)
{
  if (function_table_.find(identifier) == function_table_.end()) {
    return false;
  }

  FunctionSymbol variable = function_table_.at(identifier);

  if (variable.identifier == identifier && variable.parameters == parameters && variable.return_type == return_type) {
    return true;
  }

  throw std::invalid_argument("error: function cannot have multiple declarations");
}
/// @todo if reference matches only one category, throw
bool SymbolTable::lookupVariable(const std::string &identifier, const ConstituentToken &type)
{

  if (variable_table_.find(identifier) == variable_table_.end()) {
    return false;
  }

  VariableSymbol variable = variable_table_.at(identifier);

  if (variable.identifier == identifier && variable.type == type) {
    return true;
  }

  throw std::invalid_argument("error: variable cannot have multiple definitions");
}

void SymbolTable::addFunction(const std::string &identifier,
                              const std::vector<std::pair<std::string, ConstituentToken>> &parameters,
                              const ConstituentToken &return_type)
{
  if (lookupFunction(identifier, parameters, return_type)) {
    throw std::invalid_argument("error: function cannot have multiple declarations");
  }
  FunctionSymbol function;
  function.identifier = identifier;
  function.parameters = parameters;
  function.return_type = return_type;
  stack_.push_back(function);
  function_table_.insert({identifier, function});
  return;
}

void SymbolTable::addVariable(const std::string &identifier, const ConstituentToken &type)
{
  if (lookupVariable(identifier, type)) {
    throw std::invalid_argument("error: variable cannot have multiple definitions");
  }
  VariableSymbol variable;
  variable.identifier = identifier;
  variable.type = type;
  stack_.push_back(variable);

  variable_table_.insert({identifier, variable});
  return;
}
