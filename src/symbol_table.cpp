#include "symbol_table.hpp"

SymbolTable::SymbolTable() {}

void SymbolTable::enterScope() { stack_.push_back(nullptr); }
void SymbolTable::exitScope()
{
  while (stack_.back()) {
    if (stack_.empty()) {
      throw std::invalid_argument("error: stack should not be empty");
    }
    if (std::holds_alternative<Node::FunctionDecl>(*stack_.back())) {
      auto function_id = std::get<Node::FunctionDecl>(*stack_.back()).token.value;
      function_table_.erase(function_id);
    }
    if (std::holds_alternative<Node::VariableDecl>(*stack_.back())) {
      auto variable_id = std::get<Node::VariableDecl>(*stack_.back()).token.value;
      variable_table_.erase(variable_id);
    }
    stack_.pop_back();
  }

  if (!stack_.back()) {
    stack_.pop_back(); // remove end of scope marker
  }
}

/// @todo if reference matches only one category, throw
std::shared_ptr<Node::FunctionDecl> SymbolTable::lookupFunction(const std::string &identifier)
{
  auto it = function_table_.find(identifier);
  if (it == function_table_.end()) {
    return nullptr;
  }
  return it->second;
}

/// @todo if reference matches only one category, throw
std::shared_ptr<Node::VariableDecl> SymbolTable::lookupVariable(const std::string &identifier)
{
  auto it = variable_table_.find(identifier);
  if (it == variable_table_.end()) {
    return nullptr;
  }
  return it->second;
}

/// @todo if reference matches only one category, throw
bool SymbolTable::findFunction(const std::string &identifier)
{
  auto it = function_table_.find(identifier);
  if (it == function_table_.end()) {
    return false;
  }
  return true;
}

/// @todo if reference matches only one category, throw
bool SymbolTable::findVariable(const std::string &identifier)
{
  auto it = variable_table_.find(identifier);
  if (it == variable_table_.end()) {
    return false;
  }
  return true;
  ;
}

void SymbolTable::addFunction(const std::string &identifier, const std::shared_ptr<Node::FunctionDecl> &declaration)
{
  if (lookupFunction(identifier)) {
    throw std::invalid_argument("error: function cannot have multiple declarations");
  }
  if (declaration) {
    auto function_decl = SYMBOL_VARIANT(*declaration);
    stack_.push_back(std::make_shared<SYMBOL_VARIANT>(function_decl));
  }
  function_table_.insert({identifier, declaration});
  return;
}

void SymbolTable::addVariable(const std::string &identifier, const std::shared_ptr<Node::VariableDecl> &declaration)
{
  if (lookupVariable(identifier)) {
    std::cerr << "received: '" << identifier << '\'' << std::endl;
    throw std::invalid_argument("error: variable cannot have multiple definitions");
  }
  if (declaration) {
    auto variable_decl = SYMBOL_VARIANT(*declaration);
    stack_.push_back(std::make_shared<SYMBOL_VARIANT>(variable_decl));
  }
  variable_table_.insert({identifier, declaration});
  return;
}

void SymbolTable::debugSize() { std::cout << "current size: " << stack_.size() << std::endl; }