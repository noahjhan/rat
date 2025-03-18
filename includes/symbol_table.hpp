#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <cassert>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "token.hpp"

#define SYMBOL_VARIANT std::variant<FunctionSymbol, VariableSymbol, void *>

struct FunctionSymbol
{
  std::string identifier;
  std::vector<std::pair<std::string, ConstituentToken>> parameters;
  ConstituentToken return_type;
};

struct VariableSymbol
{
  std::string identifier;
  ConstituentToken type;
  std::string value;
};

class SymbolTable
{
  public:
  SymbolTable();
  void enterScope();
  void exitScope();

  bool lookupFunction(const std::string &identifier,
                      const std::vector<std::pair<std::string, ConstituentToken>> &parameters,
                      const ConstituentToken &return_type);
  bool lookupVariable(const std::string &identifier, const ConstituentToken &type);

  void addFunction(const std::string &identifier,
                   const std::vector<std::pair<std::string, ConstituentToken>> &parameters,
                   const ConstituentToken &return_type);
  void addVariable(const std::string &identifier, const ConstituentToken &type);

  private:
  std::vector<SYMBOL_VARIANT> stack_;
  std::unordered_map<std::string, FunctionSymbol> function_table_;
  std::unordered_map<std::string, VariableSymbol> variable_table_;
};

#endif // SYMBOL_TABLE_HPP
