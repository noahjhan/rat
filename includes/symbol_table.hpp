#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <stack>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "token.hpp"

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

  bool lookupFunction(
  const std::string &identifier,
  const std::vector<std::pair<std::string, ConstituentToken>> &parameters,
  const ConstituentToken &return_type);
  bool lookupVariable(const std::string &identifier,
                      const ConstituentToken &type, const std::string &value);

  void addFunction(
  const std::string &identifier,
  const std::vector<std::pair<std::string, ConstituentToken>> &parameters,
  const ConstituentToken &return_type);
  void addVariable(const std::string &identifier, const ConstituentToken &type,
                   const std::string &value);

  private:
  std::stack<std::variant<FunctionSymbol, VariableSymbol>> stack_;
  std::stack<unsigned int> frame_size_;
};

#endif // SYMBOL_TABLE_HPP
