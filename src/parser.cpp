
#include "parser.hpp"
/**
 * @todo copy over the debug functions from lexer, maybe make a generic
 * debug.cpp file
 *
 * @todo update grammar for new ops
 *
 * @todo require proper control flow i.e. else after if only
 *
 * @todo marker for end of function
 */

Parser::Parser(std::deque<Token> &tokens, RatSource &source_file) : tokens_(tokens), source_file_(source_file)
{
  symbol_table_.enterScope();
}

std::unique_ptr<Node::AST> Parser::dispatch()
{
  if (tokens_.empty()) {
    symbol_table_.exitScope();
    return nullptr;
  }

  while (peek().value == ";") {
    pop();
  }

  if (tokens_.empty()) {
    symbol_table_.exitScope();
    return nullptr;
  }

  if (peek().value == "{") {
    symbol_table_.enterScope();
    pop();
    return dispatch();
  }
  else if (peek().value == "}") {
    symbol_table_.exitScope();
    pop();
    return nullptr;
  }

  auto createASTNode = [&](auto parsingFunction, const std::string error_message) -> std::unique_ptr<Node::AST> {
    auto ptr = parsingFunction();
    if (!ptr) {
      throw std::invalid_argument(error_message);
    }
    auto ast = std::make_unique<Node::AST>();
    ast->curr = std::make_unique<AST_VARIANT>(std::move(*ptr));
    if (!tokens_.empty() && peek().value != ";" && peek().value != "else") {
      throw std::invalid_argument("expected newline");
    }
    ast->next = dispatch();
    return ast;
  };

  if (peek().type == GenericToken::IDENTIFIER) {
    if (symbol_table_.findFunction(peek().value)) {
      return createASTNode([this] { return functionCall(); }, "null function call");
    }
    else {
      std::cerr << "received: '" << peek().value << '\'' << std::endl;
      throw std::invalid_argument("syntax error: unrecognized literal in expression");
    }
  }

  if (peek().type != GenericToken::KEYWORD) {
    std::cerr << "received: '" << peek().value << '\'' << std::endl;
    throw std::invalid_argument("syntax error: unrecognized expression");
  }

  const std::string &value = peek().value;

  /// @todo support for other function types
  if (value == "fn") {
    return createASTNode([this] { return functionDeclaration(); }, "null function declaration");
  }
  if (value == "fn_") {
    return createASTNode([this] { return voidFunctionDeclaration(); }, "null function declaration");
  }
  if (value == "let" || value == "op") {
    return createASTNode([this] { return variableDeclaration(); }, "null variable declaration");
  }
  if (value == "if") {
    return createASTNode([this] { return conditionalStatement(); }, "null conditional statement");
  }
  if (value == "rev" || value == "ret") {
    return createASTNode([this] { return returnStatment(); }, "null return statement");
  }

  if (!tokens_.empty() && peek().type == GenericToken::KEYWORD) {
    std::cerr << "received: '" << peek().value << '\'' << std::endl;
    throw std::invalid_argument("syntax error: unexpected keyword");
  }

  symbol_table_.exitScope();
  return nullptr;
}

std::shared_ptr<Node::VariableDecl> Parser::variableDeclaration()
{
  auto require = [&](bool condition, const std::string &error_message) {
    if (!condition)
      throw std::invalid_argument(error_message);
  };

  require(peek().value == "let", "error: expected keyword in variable declaration");

  pop();

  require(peek().type == GenericToken::IDENTIFIER, "error: expected identifier in variable declaration");
  Node::VariableDecl variable_decl;
  variable_decl.token = pop();

  require(peek().value == ":", "error: expected punctuator in variable declaration");

  pop();

  require(peek().type == GenericToken::TYPE, "error: expected type in variable declaration");
  require(dictionary_.find(peek().value) != dictionary_.end(), "error: unrecognized type in variable declaration");

  variable_decl.type = dictionary_.at(pop().value);

  require(pop().value == "=", "error: expected assignment operator in expression");

  variable_decl.expr = recurseExpr();
  auto declaration = std::make_shared<Node::VariableDecl>(variable_decl);
  symbol_table_.addVariable(variable_decl.token.value, declaration);

  require(tokens_.empty() || peek().value == ";", "expected newline in expression");
  return declaration;
}

std::vector<std::pair<std::string, ConstituentToken>> Parser::parameterlist()
{
  auto require = [&](bool condition, const std::string &error_message) {
    if (!condition)
      throw std::invalid_argument(error_message);
  };

  require(pop().value == "(", "error expected '(' in function declaration");

  std::vector<std::pair<std::string, ConstituentToken>> parameters;
  while (peek().value != ")") {
    require(peek().type == GenericToken::IDENTIFIER, "error: expected identifier in function declaration");
    std::string identifier = pop().value;

    require(pop().value == ":", "error: expected ':' in function declaration");

    require(peek().type == GenericToken::TYPE, "error: expected type in function declaration");
    require(dictionary_.find(peek().value) != dictionary_.end(), "error: unknown type in function declaration");
    ConstituentToken type = dictionary_.at(pop().value);

    parameters.push_back({identifier, type});
    require(peek().value == "," || peek().value == ")", "syntax error in function declaration");
    if (peek().value == ",") {
      pop();
    }
  }

  require(pop().value == ")", "expecting ')' in function declaration");

  return parameters;
}
/// @todo support for lambdas
std::shared_ptr<Node::FunctionDecl> Parser::functionDeclaration()
{
  auto require = [&](bool condition, const std::string &error_message) {
    if (!condition)
      throw std::invalid_argument(error_message);
  };

  require(peek().value == "fn", "error: expected keyword in function declaration");

  require(dictionary_.find(peek().value) != dictionary_.end(), "error: unrecognized keyword");
  Node::FunctionDecl function_decl;
  function_decl.type = dictionary_.at(pop().value);
  require(peek().type == GenericToken::IDENTIFIER, "error: expected identifier in function declaration");
  auto identifier = peek().value;
  function_decl.token = pop();

  require(peek().value == "(", "error: expected punctuator in function declaration");
  std::vector<std::pair<std::string, ConstituentToken>> parameters = parameterlist();
  function_decl.parameters = parameters;
  symbol_table_.enterScope();
  for (const auto &parameter : parameters) {
    symbol_table_.addVariable(parameter.first, nullptr);
  }

  require(pop().value == ":", "error: expected ':' in function declaraiton");

  require(peek().type == GenericToken::TYPE, "error: expected return type in function declaration");
  auto return_type = pop().value;
  require(dictionary_.find(return_type) != dictionary_.end(), "error: unrecognized type");
  function_decl.return_type = dictionary_.at(return_type);
  function_decl.body = dispatch();
  symbol_table_.exitScope();
  auto function = std::make_shared<Node::FunctionDecl>(std::move(function_decl));
  symbol_table_.addFunction(identifier, function);
  return function;
}

std::shared_ptr<Node::FunctionDecl> Parser::voidFunctionDeclaration()
{
  auto require = [&](bool condition, const std::string &error_message) {
    if (!condition)
      throw std::invalid_argument(error_message);
  };

  require(peek().value == "fn_", "error: expected keyword in function declaration");

  require(dictionary_.find(peek().value) != dictionary_.end(), "error: unrecognized keyword");
  Node::FunctionDecl function_decl;
  function_decl.type = dictionary_.at(pop().value);
  require(peek().type == GenericToken::IDENTIFIER, "error: expected identifier in function declaration");
  auto identifier = peek().value;
  function_decl.token = pop();

  require(peek().value == "(", "error: expected punctuator in function declaration");
  std::vector<std::pair<std::string, ConstituentToken>> parameters = parameterlist();
  function_decl.parameters = parameters;
  symbol_table_.enterScope();
  for (const auto &parameter : parameters) {
    symbol_table_.addVariable(parameter.first, nullptr);
  }
  function_decl.return_type = ConstituentToken::TYPE_VOID;
  function_decl.body = dispatch();

  symbol_table_.exitScope();
  auto function = std::make_shared<Node::FunctionDecl>(std::move(function_decl));
  symbol_table_.addFunction(identifier, function);
  return function;
}

// calls statement such that the top of the tokens is the expression
std::unique_ptr<Node::ConditionalStatement> Parser::conditionalStatement()
{
  const Token token = peek();
  Node::ConditionalStatement cond;
  cond.token = token;
  cond.next = nullptr; // Explicitly set to null if unused

  if (token.value == "if") {
    pop();
    cond.expr = recurseExpr();
    cond.body = dispatch(); // Move unique_ptr directly
    if (peek().value == "else") {
      cond.next = conditionalStatement();
    }
    return std::make_unique<Node::ConditionalStatement>(std::move(cond));
  }

  if (token.value == "else") {
    pop();
  }
  else {
    throw std::invalid_argument("error: expected keyword in conditional expression");
  }

  const Token &next = peek();

  if (next.value == "{") {
    cond.expr = nullptr; // Sentinel value for else statements
    cond.body = dispatch();
    return std::make_unique<Node::ConditionalStatement>(std::move(cond));
  }
  else if (next.value == "if") {
    pop();
    cond.token.value = "else if";
    cond.expr = recurseExpr();
    cond.body = dispatch();
    if (peek().value == "else") {
      cond.next = conditionalStatement();
    }
    return std::make_unique<Node::ConditionalStatement>(std::move(cond));
  }
  else {
    throw std::invalid_argument("error: expected keyword in conditional expression");
  }

  return nullptr;
}

std::unique_ptr<Node::GenericExpr> Parser::recurseNumeric()
{
  if (!tokens_.empty() && (peek().type == GenericToken::NUMERIC_LITERAL || peek().type == GenericToken::IDENTIFIER ||
                           peek().type == GenericToken::STRING_LITERAL)) {
    auto expr = tokenToExpr();
    if (!expr) {
      return functionCall();
    }
    return expr;
  }
  return nullptr;
}

std::unique_ptr<Node::GenericExpr> Parser::recurseFactor()
{
  if (tokens_.empty())
    return nullptr;
  if (peek().value == ";") {
    throw std::invalid_argument("unexpected newline in expression");
    pop();
    return nullptr;
  }
  if (peek().value == "!" || peek().value == "~") {
    std::string op = peek().value;
    pop();

    if (tokens_.empty()) {
      std::cerr << "missing operand after unary operator '" << op << "'\n";
      throw std::invalid_argument("syntax error");
      return nullptr;
    }

    Node::UnaryExpr un_expr;
    un_expr.op = (op == "!") ? ConstituentToken::LOGICAL_NOT : ConstituentToken::BITWISE_NEG;

    un_expr.expr = recurseFactor();
    if (!un_expr.expr) {
      std::cerr << "invalid expression after unary operator '" << op << "'\n";
      throw std::invalid_argument("syntax error");
      return nullptr;
    }
    return std::make_unique<Node::GenericExpr>(
    Node::GenericExpr{std::make_unique<EXPRESSION_VARIANT>(std::move(un_expr))});
  }

  if (peek().type == GenericToken::NUMERIC_LITERAL || peek().type == GenericToken::IDENTIFIER ||
      peek().type == GenericToken::STRING_LITERAL) {
    return recurseNumeric();
  }

  if (peek().value == "(") {
    pop(); // Consume '('
    auto expr = recurseExpr();
    if (tokens_.empty()) {
      throw std::invalid_argument("syntax error: invalid parentheses expression");
    }
    pop(); // Consume ')'
    return expr;
  }
  return nullptr;
}

std::unique_ptr<Node::GenericExpr> Parser::recurseTerm()
{
  auto factor = recurseFactor();
  while (!tokens_.empty() && (peek().value == "*" || peek().value == "/" || peek().value == "%")) {
    if (!factor) {
      throw std::invalid_argument("error: expecting lhs for binary expression");
    }
    Node::BinaryExpr bin_expr;
    bin_expr.lhs = std::move(factor);

    if (peek().value == "*")
      bin_expr.op = ConstituentToken::ARITHMETIC_MUL;
    else if (peek().value == "/")
      bin_expr.op = ConstituentToken::ARITHMETIC_DIV;
    else if (peek().value == "%")
      bin_expr.op = ConstituentToken::ARITHMETIC_MOD;

    pop();
    auto factor_rhs = recurseFactor();
    if (!factor_rhs) {
      throw std::invalid_argument("error: expecting rhs for binary expression");
    }
    bin_expr.rhs = std::move(factor_rhs);
    factor =
    std::make_unique<Node::GenericExpr>(Node::GenericExpr{std::make_unique<EXPRESSION_VARIANT>(std::move(bin_expr))});
  }
  return factor;
}

std::unique_ptr<Node::GenericExpr> Parser::recurseAdditive()
{
  auto term = recurseTerm();
  while (!tokens_.empty() && (peek().value == "+" || peek().value == "-")) {
    if (!term) {
      throw std::invalid_argument("error: expecting lhs for binary expression");
    }
    Node::BinaryExpr bin_expr;
    bin_expr.lhs = std::move(term);
    bin_expr.op = (peek().value == "+") ? ConstituentToken::ARITHMETIC_ADD : ConstituentToken::ARITHMETIC_SUB;
    pop();
    auto term_rhs = recurseTerm();
    if (!term_rhs) {
      throw std::invalid_argument("error: expecting rhs for binary expression");
    }
    bin_expr.rhs = std::move(term_rhs);
    term =
    std::make_unique<Node::GenericExpr>(Node::GenericExpr{std::make_unique<EXPRESSION_VARIANT>(std::move(bin_expr))});
  }
  return term;
}

std::unique_ptr<Node::GenericExpr> Parser::recurseShift()
{
  auto additive = recurseAdditive();
  while (!tokens_.empty() && (peek().value == "<<" || peek().value == ">>")) {
    if (!additive) {
      throw std::invalid_argument("error: expecting lhs for binary expression");
    }
    Node::BinaryExpr bin_expr;
    bin_expr.lhs = std::move(additive);

    if (peek().value == "<<")
      bin_expr.op = ConstituentToken::BITWISE_SL;
    else if (peek().value == ">>")
      bin_expr.op = ConstituentToken::BITWISE_SR;

    pop();
    auto additive_rhs = recurseAdditive();
    if (!additive_rhs) {
      throw std::invalid_argument("error: expecting rhs for binary expression");
    }
    bin_expr.rhs = std::move(additive_rhs);
    additive =
    std::make_unique<Node::GenericExpr>(Node::GenericExpr{std::make_unique<EXPRESSION_VARIANT>(std::move(bin_expr))});
  }
  return additive;
}

std::unique_ptr<Node::GenericExpr> Parser::recurseComparison()
{
  auto shift = recurseShift();
  while (!tokens_.empty() && (peek().value == "==" || peek().value == "!=" || peek().value == "<" ||
                              peek().value == "<=" || peek().value == ">" || peek().value == ">=")) {
    if (!shift) {
      throw std::invalid_argument("error: expecting lhs for binary expression");
    }

    Node::BinaryExpr bin_expr;
    bin_expr.lhs = std::move(shift);

    if (peek().value == "==")
      bin_expr.op = ConstituentToken::COMPARISON_EQ;
    else if (peek().value == "!=")
      bin_expr.op = ConstituentToken::COMPARISON_NEQ;
    else if (peek().value == "<")
      bin_expr.op = ConstituentToken::COMPARISON_LT;
    else if (peek().value == "<=")
      bin_expr.op = ConstituentToken::COMPARISON_LTE;
    else if (peek().value == ">")
      bin_expr.op = ConstituentToken::COMPARISON_GT;
    else if (peek().value == ">=")
      bin_expr.op = ConstituentToken::COMPARISON_GTE;

    pop();
    auto shift_rhs = recurseShift();
    if (!shift_rhs) {
      throw std::invalid_argument("error: expecting rhs for binary expression");
    }
    bin_expr.rhs = std::move(shift_rhs);
    shift =
    std::make_unique<Node::GenericExpr>(Node::GenericExpr{std::make_unique<EXPRESSION_VARIANT>(std::move(bin_expr))});
  }
  return shift;
}

std::unique_ptr<Node::GenericExpr> Parser::recurseLogical()
{
  auto comparison = recurseComparison();
  while (!tokens_.empty() && (peek().value == "&" || peek().value == "^" || peek().value == "|" ||
                              peek().value == "&&" || peek().value == "||")) {
    if (!comparison) {
      throw std::invalid_argument("error: expecting lhs for binary expression");
    }
    Node::BinaryExpr bin_expr;
    bin_expr.lhs = std::move(comparison);

    if (peek().value == "&")
      bin_expr.op = ConstituentToken::BITWISE_AND;
    else if (peek().value == "^")
      bin_expr.op = ConstituentToken::BITWISE_XOR;
    else if (peek().value == "|")
      bin_expr.op = ConstituentToken::BITWISE_OR;
    else if (peek().value == "&&")
      bin_expr.op = ConstituentToken::LOGICAL_AND;
    else if (peek().value == "||")
      bin_expr.op = ConstituentToken::LOGICAL_OR;

    pop();

    auto comparison_rhs = recurseComparison();
    if (!comparison_rhs) {
      throw std::invalid_argument("error: expecting rhs for binary expression");
    }
    bin_expr.rhs = std::move(comparison_rhs);
    comparison =
    std::make_unique<Node::GenericExpr>(Node::GenericExpr{std::make_unique<EXPRESSION_VARIANT>(std::move(bin_expr))});
  }
  return comparison;
}

std::unique_ptr<Node::GenericExpr> Parser::recurseExpr()
{
  auto logical = recurseLogical();
  if (!logical) {
    if (tokens_.empty()) {
      throw std::runtime_error("token deque empty");
    }
    Token token = peek();
    std::cerr << "received: '" << token.value << '\'' << std::endl;
    debugLineCol(token.line_num, token.col_num);
    debugPrintln(token.line_num);
    throw std::invalid_argument("error: invalid syntax");
  }
  if (std::holds_alternative<Node::BinaryExpr>(*(logical->expr))) {
    if (!std::get<Node::BinaryExpr>(*(logical->expr)).lhs) {
      throw std::invalid_argument("error: expecting lhs for binary expression");
    }
    if (!std::get<Node::BinaryExpr>(*(logical->expr)).rhs) {
      throw std::invalid_argument("error: expecting rhs for binary expression");
    }
  }
  return logical;
}

std::unique_ptr<Node::GenericExpr> Parser::tokenToExpr()
{
  Token token = peek();
  if (tokens_.empty()) {
    throw std::runtime_error("error: empty token deque");
  }

  std::unique_ptr<Node::GenericExpr> gen_expr_ptr;
  pop(); // maybe here maybe at the end

  switch (token.type) {
    /// @todo seperate functions maybe seperated struct for function and
    /// variable identifiers
    case GenericToken::IDENTIFIER: {
      if (peek().value == "(") {
        restore(token);
        return nullptr;
      }
      if (!symbol_table_.findVariable(token.value)) {
        throw std::invalid_argument("error: undeclared variable");
      }
      Node::Identifier node;
      node.token = token;
      node.type = ConstituentToken::VARIABLE_ID;
      auto ptr = std::make_unique<EXPRESSION_VARIANT>(node);
      Node::GenericExpr gen_expr;
      gen_expr.expr = std::move(ptr);
      gen_expr_ptr = std::make_unique<Node::GenericExpr>(std::move(gen_expr));
    } break;
    case GenericToken::KEYWORD:
      throw std::invalid_argument("error: keyword found in expression");
    case GenericToken::NUMERIC_LITERAL: {
      Node::NumericLiteral node;
      node.token = token;
      node.type = inferTypeNumericLiteral(token.value);
      auto ptr = std::make_unique<EXPRESSION_VARIANT>(node);
      Node::GenericExpr gen_expr;
      gen_expr.expr = std::move(ptr);
      gen_expr_ptr = std::make_unique<Node::GenericExpr>(std::move(gen_expr));
    } break;
    case GenericToken::STRING_LITERAL: {
      Node::StringLiteral node;
      node.token = token;
      node.type = ConstituentToken::TYPE_STRING;
      auto ptr = std::make_unique<EXPRESSION_VARIANT>(node);
      Node::GenericExpr gen_expr;
      gen_expr.expr = std::move(ptr);
      gen_expr_ptr = std::make_unique<Node::GenericExpr>(std::move(gen_expr));
    } break;
    case GenericToken::CHAR_LITERAL: {
      Node::NumericLiteral node;
      node.token = token;
      node.type = ConstituentToken::TYPE_CHAR;
      auto ptr = std::make_unique<EXPRESSION_VARIANT>(node);
      Node::GenericExpr gen_expr;
      gen_expr.expr = std::move(ptr);
      gen_expr_ptr = std::make_unique<Node::GenericExpr>(std::move(gen_expr));
    } break;
    case GenericToken::PUNCTUATOR:
      break;
    case GenericToken::OPERATOR:
      break;
    case GenericToken::TYPE:
      throw std::invalid_argument("error: keyword found in expression");
  }
  if (gen_expr_ptr) {
    return gen_expr_ptr;
  }
  std::cerr << "received: '" << token.value << '\'' << std::endl;
  throw std::invalid_argument("error: unrecognized token in expression");
}

std::vector<std::unique_ptr<Node::GenericExpr>> Parser::callParameters()
{
  auto require = [&](bool condition, const std::string &error_message) {
    if (!condition)
      throw std::invalid_argument(error_message);
  };

  require(pop().value == "(", "error expected '(' in function call");

  std::vector<std::unique_ptr<Node::GenericExpr>> parameters;
  while (peek().value != ")") {
    auto expr = recurseExpr();
    if (!expr) {
      throw std::invalid_argument("expecting parameter in function call");
    }
    parameters.push_back(std::move(expr));
    if (peek().value == ",") {
      pop();
    }
  }

  require(pop().value == ")", "expecting ')' in function declaration");

  return parameters;
}

std::unique_ptr<Node::GenericExpr> Parser::functionCall()
{
  Node::FunctionCall call;
  if (peek().type != GenericToken::IDENTIFIER) {
    std::cerr << "received: '" << peek().value << '\'' << std::endl;
    throw std::invalid_argument("expected identifier in function call");
  }
  call.token = pop();
  call.parameters = callParameters();
  call.function = symbol_table_.lookupFunction(call.token.value);
  Node::GenericExpr expr;
  expr.expr = std::make_unique<EXPRESSION_VARIANT>(std::move(call));
  return std::make_unique<Node::GenericExpr>(std::move(expr));
}

std::unique_ptr<Node::ReturnStatement> Parser::returnStatment()
{
  if (peek().value == "rev") {
    // require this?
    Node::ReturnStatement rev;
    rev.token = peek();
    pop();
    rev.type = ConstituentToken::TYPE_VOID;
    rev.expr = nullptr;
    return std::make_unique<Node::ReturnStatement>(std::move(rev));
  }
  if (peek().value == "ret") {
    // require this?
    Node::ReturnStatement ret;
    ret.token = peek();
    pop();
    /// @todo find proper return type for error requireing
    ret.type = ConstituentToken::TYPE_VOID;
    ret.expr = recurseExpr();
    return std::make_unique<Node::ReturnStatement>(std::move(ret));
  }
  throw std::invalid_argument("unrecognized keyword in return statement");
}
/// @todo suppport for optional
/// @todo move some of this to lexer
/// @todo create usuable regex
ConstituentToken Parser::inferTypeNumericLiteral(const std::string &value)
{
  bool is_u_type = value.find('u') != std::string::npos;
  bool is_f_type =
  value.find('f') != std::string::npos || value.find('d') != std::string::npos || value.find('.') != std::string::npos;

  // this is already requireed in lexer.cpp
  if (is_u_type && is_f_type) {
    throw std::invalid_argument("ambiguous numeric literal: '" + value + "'");
  }

  // type inference default int
  if (std ::isdigit(value.back())) {
    return is_f_type ? ConstituentToken::TYPE_DOUBLE : ConstituentToken::TYPE_INT;
  }

  switch (value.back()) {
    case 'u':
      return ConstituentToken::TYPE_UINT;
    case 'i':
      return is_u_type ? ConstituentToken::TYPE_UINT : ConstituentToken::TYPE_INT;
    case 'l':
      return is_u_type ? ConstituentToken::TYPE_ULONG : ConstituentToken::TYPE_LONG;
    case 's':
      return is_u_type ? ConstituentToken::TYPE_USHORT : ConstituentToken::TYPE_SHORT;
    case 'c':
      return is_u_type ? ConstituentToken::TYPE_UCHAR : ConstituentToken::TYPE_CHAR;
    case 'f':
      return ConstituentToken::TYPE_FLOAT;
    case 'd':
      return ConstituentToken::TYPE_DOUBLE;
    case '\0':
      return is_u_type ? ConstituentToken::TYPE_UINT : ConstituentToken::TYPE_INT;
    default:
      std::cerr << value << std::endl;
      throw std::invalid_argument("ambiguous numeric literal token");
  }
}

void Parser::debugASTPrinter(Node::AST &node)
{
  auto curr = node.curr.get();
  if (!curr) {
    return;
  }
  if (std::holds_alternative<Node::GenericExpr>(*curr)) {
    auto &variant = std::get<Node::GenericExpr>(*curr);
    debugExprPrinterRecursive(variant, 0);
  }
  else if (std::holds_alternative<Node::VariableDecl>(*curr)) {
    auto &variant = std::get<Node::VariableDecl>(*curr);
    debugVariableDeclPrinter(variant);
  }
  else if (std::holds_alternative<Node::ConditionalStatement>(*curr)) {
    auto &variant = std::get<Node::ConditionalStatement>(*curr);
    debugConditionalStatement(variant);
  }
  else if (std::holds_alternative<Node::FunctionDecl>(*curr)) {
    auto &variant = std::get<Node::FunctionDecl>(*curr);
    debugFunctionDeclaration(variant);
  }
  else if (std::holds_alternative<Node::ReturnStatement>(*curr)) {
    auto &variant = std::get<Node::ReturnStatement>(*curr);
    debugReturnStatement(variant);
  }
  if (node.next) {
    debugASTPrinter(*node.next);
  }
}

void Parser::debugExprPrinterRecursive(Node::GenericExpr &node, int depth)
{
  auto variant = node.expr.get();
  if (!variant) {
    std::cout << std::string(depth, ' ') << "variant is null" << std::endl;
    return;
  }

  const std::string indent = std::string(depth * 4, ' ');

  if (std::holds_alternative<Node::GenericExpr>(*variant)) {
    std::cout << indent << "recursively holds expression" << std::endl;
    debugExprPrinterRecursive(std::get<Node::GenericExpr>(*variant), depth + 1);
  }
  else if (std::holds_alternative<Node::BinaryExpr>(*variant)) {
    const auto &binaryExpr = std::get<Node::BinaryExpr>(*variant);
    std::cout << indent << "holds binary expression: " << reverse_dictionary_.at(binaryExpr.op) << std::endl;
    if (!binaryExpr.lhs || !binaryExpr.rhs) {
      throw std::invalid_argument("error: missing operand in binary expression");
    }
    std::cout << indent << "lhs:" << std::endl;
    debugExprPrinterRecursive(*binaryExpr.lhs, depth + 1);
    std::cout << indent << "rhs:" << std::endl;
    debugExprPrinterRecursive(*binaryExpr.rhs, depth + 1);
  }
  else if (std::holds_alternative<Node::UnaryExpr>(*variant)) {
    const auto &unaryExpr = std::get<Node::UnaryExpr>(*variant);
    std::cout << indent << "holds unary expression: " << reverse_dictionary_.at(unaryExpr.op) << std::endl;
    if (unaryExpr.expr) {
      debugExprPrinterRecursive(*unaryExpr.expr, depth + 1);
    }
  }
  else if (std::holds_alternative<Node::NumericLiteral>(*variant)) {
    std::cout << indent << "holds numeric literal: " << std::get<Node::NumericLiteral>(*variant).token.value
              << std::endl;
  }
  else if (std::holds_alternative<Node::Identifier>(*variant)) {
    std::cout << indent << "holds identifier: " << std::get<Node::Identifier>(*variant).token.value << std::endl;
  }
  else if (std::holds_alternative<Node::StringLiteral>(*variant)) {
    std::cout << indent << "holds string literal: \"" << std::get<Node::StringLiteral>(*variant).token.value << "\""
              << std::endl;
  }
  else if (std::holds_alternative<Node::FunctionCall>(*variant)) {
    const auto &functionCall = std::get<Node::FunctionCall>(*variant);
    std::cout << indent << "holds function call: " << functionCall.token.value << std::endl;
    std::cout << indent << "parameters:" << std::endl;
    for (const auto &param : functionCall.parameters) {
      debugExprPrinterRecursive(*param, depth + 1);
    }
  }
  else {
    std::cout << indent << "holds ambiguous state" << std::endl;
  }
}

void Parser::debugVariableDeclPrinter(Node::VariableDecl &node)
{
  std::cout << "holds variable declaration" << '\n';
  std::cout << "identifier: " << node.token.value << '\n';
  if (reverse_dictionary_.find(node.type) == reverse_dictionary_.end()) {
    throw std::invalid_argument("error: type not found");
  }
  std::cout << "type: " << reverse_dictionary_.at(node.type) << '\n';
  std::cout << "value:\n";
  debugExprPrinterRecursive(*node.expr, 0);
  std::cout << std::endl;
}

void Parser::debugConditionalStatement(Node::ConditionalStatement &node)
{
  std::cout << "holds conditional statement" << '\n';
  std::cout << "type of conditional: " << node.token.value << '\n';
  if (node.expr) {
    std::cout << "expression:\n";
    debugExprPrinterRecursive(*node.expr, 0);
  }
  if (!node.body) {
    throw std::invalid_argument("conditional statement must have a body");
  }
  std::cout << "body:\n";
  debugASTPrinter(*node.body);
  if (node.next) {
    debugConditionalStatement(*node.next);
  }
  std::cout << std::endl;
}

void Parser::debugFunctionDeclaration(Node::FunctionDecl &node)
{
  std::cout << "holds function declaration" << '\n';
  std::cout << "type of function: " << reverse_dictionary_.at(node.type) << '\n';
  for (const auto &var : node.parameters) {
    std::cout << "identifier: " << var.first << '\n';
    std::cout << "type: " << reverse_dictionary_.at(var.second) << '\n';
  }
  std::cout << "return type: " << reverse_dictionary_.at(node.return_type) << '\n';
  if (!node.body) {
    throw std::invalid_argument("function must have a body");
  }
  std::cout << "body:\n";
  debugASTPrinter(*node.body);
}

void Parser::debugReturnStatement(Node::ReturnStatement &node)
{
  std::cout << "holds return statement" << std::endl;
  if (node.expr) {
    std::cout << "expression: " << '\n';
    debugExprPrinterRecursive(*node.expr, 0);
  }
}

void Parser::debugPrintln(const unsigned int &line_num)
{
  source_file_.seekLine(line_num);
  std::cerr << line_num << " | " << source_file_.readLine() << std::endl;
}

void Parser::debugLineCol(const unsigned int &line_num, const unsigned int &col_num)
{
  std::cerr << "at line: " << line_num << ", col: " << col_num << std::endl;
}

inline Token Parser::pop()
{
  if (tokens_.empty()) {
    throw std::invalid_argument("out of tokens, cannot pop");
  }
  auto token = tokens_.front();
  tokens_.pop_front();
  return token;
}

inline Token Parser::peek()
{
  if (tokens_.empty()) {
    throw std::invalid_argument("out of tokens, cannot peek");
  }
  return tokens_.front();
}

inline void Parser::restore(const Token &token)
{
  tokens_.push_front(token);
}

int Parser::numTokens() const
{
  return tokens_.size();
}