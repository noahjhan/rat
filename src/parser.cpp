#include "parser.hpp"

/**
 * @todo copy over the debug functions from lexer, maybe make a generic
 * debug.cpp file
 *
 * @todo update grammar for new ops
 *
 * @todo require proper control flow i.e. else after if only
 */

Parser::Parser(std::deque<Token> &tokens, RatSource &source_file) : tokens_(tokens), source_file_(source_file)
{

  for (const auto &token : tokens_) {
    if (token.value.empty()) {
      throw std::invalid_argument("syntax error: empty token");
    }
    switch (token.type) {
      case GenericToken::IDENTIFIER: break;
      case GenericToken::KEYWORD:
        if (dictionary_.find(token.value) == dictionary_.end()) {
          std::cerr << "recieved: '" << token.value << '\'' << std::endl;
          throw std::invalid_argument("syntax error: unrecognized keyword");
        }
        break;
      case GenericToken::NUMERIC_LITERAL: break;
      case GenericToken::STRING_LITERAL: break;
      case GenericToken::CHAR_LITERAL: break;
      case GenericToken::PUNCTUATOR:
        if (dictionary_.find(token.value) == dictionary_.end()) {
          std::cerr << "recieved: '" << token.value << '\'' << std::endl;
          throw std::invalid_argument("syntax error: unrecognized punctuator");
        }
        break;
      case GenericToken::OPERATOR:
        if (dictionary_.find(token.value) == dictionary_.end()) {
          std::cerr << "recieved: '" << token.value << '\'' << std::endl;
          throw std::invalid_argument("syntax error: unrecognized operator");
        }
        break;
      case GenericToken::TYPE:
        if (dictionary_.find(token.value) == dictionary_.end()) {
          std::cerr << "recieved: '" << token.value << '\'' << std::endl;
          throw std::invalid_argument("syntax error: unrecognized type");
        }
        break;
    }
  }
  // may need a destructor just to clear out global scope
  symbol_table_.enterScope();
}
/// @todo scopes
std::shared_ptr<Node::AST> Parser::dispatch()
{
  if (tokens_.empty()) {
    symbol_table_.exitScope();
    return nullptr;
  }

  while (tokens_.front().value == ";") {
    tokens_.pop_front();
  }

  if (tokens_.empty()) {
    symbol_table_.exitScope();
    return nullptr;
  }

  if (tokens_.front().value == "{") {
    symbol_table_.enterScope();
    tokens_.pop_front();
    return dispatch();
    /// @todo make this a child ast node
  }
  else if (tokens_.front().value == "}") {
    symbol_table_.exitScope();
    tokens_.pop_front();
    return nullptr;
  }

  if (tokens_.front().type != GenericToken::KEYWORD) {
    throw std::invalid_argument("syntax error: unrecognized expression");
  }

  const std::string &value = tokens_.front().value;

  auto createASTNode = [&](auto parsingFunction, const std::string error_message) -> std::shared_ptr<Node::AST> {
    auto ptr = parsingFunction();
    if (!ptr) {
      throw std::invalid_argument(error_message);
    }
    auto ast = std::make_shared<Node::AST>();
    ast->curr = std::make_shared<AST_VARIANT>(std::move(*ptr));
    ast->next = dispatch();
    return ast;
  };

  if (value == "fn" || value == "fn_" || value == "fn/" || value == "fn?") {
    return createASTNode([this] { return functionDeclaration(); }, "null function declaration");
  }
  if (value == "let" || value == "op") {
    return createASTNode([this] { return variableDeclaration(); }, "null variable declaration");
  }
  if (value == "if" || value == "else") {
    return createASTNode([this] { return conditionalStatement(); }, "null conditional statement");
  }
  if (value == "rev" || value == "ret") {
    return createASTNode([this] { return returnStatment(); }, "null return statement");
  }
  symbol_table_.exitScope();
  return nullptr;
}

std::shared_ptr<Node::VariableDecl> Parser::variableDeclaration()
{
  // remove me (i.e handle in dispatch)
  while (tokens_.front().value == ";") {
    tokens_.pop_front();
  }
  if (tokens_.front().value != "let") throw std::invalid_argument("error: expected keyword in variable declaration");

  tokens_.pop_front();
  if (tokens_.empty()) throw std::invalid_argument("out of tokens");

  if (tokens_.front().type != GenericToken::IDENTIFIER)
    throw std::invalid_argument("error: expected identifier in variable delcaration");

  Node::VariableDecl variable_decl;
  variable_decl.token = tokens_.front();
  tokens_.pop_front();

  if (tokens_.empty()) throw std::invalid_argument("out of tokens");
  if (tokens_.front().value != ":") throw std::invalid_argument("error: expected punctuator in variable declaration");

  tokens_.pop_front();

  if (tokens_.empty()) throw std::invalid_argument("out of tokens");
  if (tokens_.front().type != GenericToken::TYPE)
    throw std::invalid_argument("error: expected type in variable declaration");
  if (dictionary_.find(tokens_.front().value) == dictionary_.end())
    throw std::invalid_argument("error: unrecognized type in variable declatation");

  variable_decl.type = dictionary_.at(tokens_.front().value);
  tokens_.pop_front();

  if (tokens_.empty()) throw std::invalid_argument("out of tokens");
  if (tokens_.front().value != "=") throw std::invalid_argument("error: expected assignment operator in expression");

  tokens_.pop_front();
  if (tokens_.empty()) throw std::invalid_argument("out of tokens");

  variable_decl.expr = recurseExpr();

  symbol_table_.addVariable(variable_decl.token.value, variable_decl.type);

  if (!tokens_.empty() && tokens_.front().value != ";") {
    throw std::invalid_argument("expected newline in expression");
  }
  return std::make_unique<Node::VariableDecl>(std::move(variable_decl));
}

/// @todo support for lambdas
std::shared_ptr<Node::FunctionDecl> Parser::functionDeclaration()
{
  // remove me (i.e handle in dispatch)
  while (tokens_.front().value == ";") {
    tokens_.pop_front();
  }

  return nullptr;
}

// calls statement such that the top of the tokens is the expression
std::shared_ptr<Node::ConditionalStatement> Parser::conditionalStatement()
{

  const Token token = tokens_.front();
  Node::ConditionalStatement cond;
  cond.token = token;

  if (token.value == "if") {
    tokens_.pop_front();
    cond.expr = recurseExpr();
    cond.body = std::make_unique<Node::AST>(std::move(*dispatch()));
    return std::make_unique<Node::ConditionalStatement>(std::move(cond));
  }

  if (token.value == "else") {
    tokens_.pop_front();
  }
  else {
    throw std::invalid_argument("error: expected keyword in conditional expression");
  }

  const Token &next = tokens_.front();

  if (next.value == "{") {
    cond.expr = nullptr; // sentinal value for else statements
    cond.body = std::make_unique<Node::AST>(std::move(*dispatch()));
    return std::make_unique<Node::ConditionalStatement>(std::move(cond));
  }
  else if (next.value == "if") {
    tokens_.pop_front();
    cond.token.value = "else if";
    cond.expr = recurseExpr();
    cond.body = std::make_unique<Node::AST>(std::move(*dispatch()));
    return std::make_unique<Node::ConditionalStatement>(std::move(cond));
  }
  else {
    throw std::invalid_argument("error: expected keyword in conditional expression");
  }
  return nullptr;
}

std::unique_ptr<Node::GenericExpr> Parser::recurseNumeric()
{
  if (!tokens_.empty() &&
      (tokens_.front().type == GenericToken::NUMERIC_LITERAL || tokens_.front().type == GenericToken::IDENTIFIER)) {
    return tokenToExpr();
  }
  return nullptr;
}

std::unique_ptr<Node::GenericExpr> Parser::recurseFactor()
{
  if (tokens_.empty()) return nullptr;
  if (tokens_.front().value == ";") {
    throw std::invalid_argument("unexpectd newline in expression");
    tokens_.pop_front();
    return nullptr;
  }
  if (tokens_.front().value == "!" || tokens_.front().value == "~") {
    std::string op = tokens_.front().value;
    tokens_.pop_front();

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

  if (tokens_.front().type == GenericToken::NUMERIC_LITERAL || tokens_.front().type == GenericToken::IDENTIFIER) {
    return recurseNumeric();
  }

  if (tokens_.front().value == "(") {
    tokens_.pop_front(); // Consume '('
    auto expr = recurseExpr();
    if (tokens_.empty()) {
      throw std::invalid_argument("syntax error: invalid parentheses expression");
    }
    tokens_.pop_front(); // Consume ')'
    return expr;
  }
  return nullptr;
}

std::unique_ptr<Node::GenericExpr> Parser::recurseTerm()
{
  auto factor = recurseFactor();
  while (!tokens_.empty() &&
         (tokens_.front().value == "*" || tokens_.front().value == "/" || tokens_.front().value == "%")) {
    if (!factor) {
      throw std::invalid_argument("error: expecting lhs for binary expression");
    }
    Node::BinaryExpr bin_expr;
    bin_expr.lhs = std::move(factor);

    if (tokens_.front().value == "*")
      bin_expr.op = ConstituentToken::ARITHMETIC_MUL;
    else if (tokens_.front().value == "/")
      bin_expr.op = ConstituentToken::ARITHMETIC_DIV;
    else if (tokens_.front().value == "%")
      bin_expr.op = ConstituentToken::ARITHMETIC_MOD;

    tokens_.pop_front();
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
  while (!tokens_.empty() && (tokens_.front().value == "+" || tokens_.front().value == "-")) {
    if (!term) {
      throw std::invalid_argument("error: expecting lhs for binary expression");
    }
    Node::BinaryExpr bin_expr;
    bin_expr.lhs = std::move(term);
    bin_expr.op = (tokens_.front().value == "+") ? ConstituentToken::ARITHMETIC_ADD : ConstituentToken::ARITHMETIC_SUB;
    tokens_.pop_front();
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
  while (!tokens_.empty() && (tokens_.front().value == "<<" || tokens_.front().value == ">>")) {
    if (!additive) {
      throw std::invalid_argument("error: expecting lhs for binary expression");
    }
    Node::BinaryExpr bin_expr;
    bin_expr.lhs = std::move(additive);

    if (tokens_.front().value == "<<")
      bin_expr.op = ConstituentToken::BITWISE_SL;
    else if (tokens_.front().value == ">>")
      bin_expr.op = ConstituentToken::BITWISE_SR;

    tokens_.pop_front();
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
  while (!tokens_.empty() &&
         (tokens_.front().value == "==" || tokens_.front().value == "!=" || tokens_.front().value == "<" ||
          tokens_.front().value == "<=" || tokens_.front().value == ">" || tokens_.front().value == ">=")) {
    if (!shift) {
      throw std::invalid_argument("error: expecting lhs for binary expression");
    }

    Node::BinaryExpr bin_expr;
    bin_expr.lhs = std::move(shift);

    if (tokens_.front().value == "==")
      bin_expr.op = ConstituentToken::COMPARISON_EQ;
    else if (tokens_.front().value == "!=")
      bin_expr.op = ConstituentToken::COMPARISON_NEQ;
    else if (tokens_.front().value == "<")
      bin_expr.op = ConstituentToken::COMPARISON_LT;
    else if (tokens_.front().value == "<=")
      bin_expr.op = ConstituentToken::COMPARISON_LTE;
    else if (tokens_.front().value == ">")
      bin_expr.op = ConstituentToken::COMPARISON_GT;
    else if (tokens_.front().value == ">=")
      bin_expr.op = ConstituentToken::COMPARISON_GTE;

    tokens_.pop_front();
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
  while (!tokens_.empty() &&
         (tokens_.front().value == "&" || tokens_.front().value == "^" || tokens_.front().value == "|" ||
          tokens_.front().value == "&&" || tokens_.front().value == "||")) {
    if (!comparison) {
      throw std::invalid_argument("error: expecting lhs for binary expression");
    }
    Node::BinaryExpr bin_expr;
    bin_expr.lhs = std::move(comparison);

    if (tokens_.front().value == "&")
      bin_expr.op = ConstituentToken::BITWISE_AND;
    else if (tokens_.front().value == "^")
      bin_expr.op = ConstituentToken::BITWISE_XOR;
    else if (tokens_.front().value == "|")
      bin_expr.op = ConstituentToken::BITWISE_OR;
    else if (tokens_.front().value == "&&")
      bin_expr.op = ConstituentToken::LOGICAL_AND;
    else if (tokens_.front().value == "||")
      bin_expr.op = ConstituentToken::LOGICAL_OR;

    tokens_.pop_front();

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
    Token token = tokens_.front();
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
  Token token = tokens_.front();
  if (tokens_.empty()) {
    throw std::runtime_error("error: empty token deque");
  }

  std::unique_ptr<Node::GenericExpr> gen_expr_ptr;
  tokens_.pop_front(); // maybe here maybe at the end

  switch (token.type) {
    /// @todo seperate functions maybe seperated struct for function and
    /// variable identifiers
    case GenericToken::IDENTIFIER: {
      Node::Identifier node;
      node.token = token;
      node.type = ConstituentToken::VARIABLE_ID; // not exactly true -> does
                                                 // not support functions
      auto ptr = std::make_unique<EXPRESSION_VARIANT>(node);
      Node::GenericExpr gen_expr;
      gen_expr.expr = std::move(ptr);
      gen_expr_ptr = std::make_unique<Node::GenericExpr>(std::move(gen_expr));
    } break;
    case GenericToken::KEYWORD: throw std::invalid_argument("error: keyword found in expression");
    case GenericToken::NUMERIC_LITERAL: {
      Node::NumericLiteral node;
      node.token = token;
      node.type = inferTypeNumericLiteral(token.value);
      auto ptr = std::make_unique<EXPRESSION_VARIANT>(node);
      Node::GenericExpr gen_expr;
      gen_expr.expr = std::move(ptr);
      gen_expr_ptr = std::make_unique<Node::GenericExpr>(std::move(gen_expr));
    } break;
    case GenericToken::STRING_LITERAL:
      std::cerr << "recieved: '" << token.value << '\'' << std::endl;
      throw std::runtime_error("string literal : @todo");
    case GenericToken::CHAR_LITERAL: {
      Node::NumericLiteral node;
      node.token = token;
      node.type = ConstituentToken::TYPE_CHAR;
      auto ptr = std::make_unique<EXPRESSION_VARIANT>(node);
      Node::GenericExpr gen_expr;
      gen_expr.expr = std::move(ptr);
      gen_expr_ptr = std::make_unique<Node::GenericExpr>(std::move(gen_expr));
    } break;
    case GenericToken::PUNCTUATOR: break;
    case GenericToken::OPERATOR: break;
    case GenericToken::TYPE: throw std::invalid_argument("error: keyword found in expression");
  }
  if (gen_expr_ptr) {
    return gen_expr_ptr;
  }
  std::cerr << "received: '" << token.value << '\'' << std::endl;
  throw std::invalid_argument("error: unrecognized token in expression");
}

std::unique_ptr<Node::ReturnStatement> Parser::returnStatment()
{
  if (tokens_.front().value == "rev") {
    // check this?
    Node::ReturnStatement rev;
    rev.token = tokens_.front();
    tokens_.pop_front();
    rev.type = ConstituentToken::TYPE_VOID;
    rev.expr = nullptr;
    return std::make_unique<Node::ReturnStatement>(std::move(rev));
  }
  if (tokens_.front().value == "ret") {
    // check this?
    Node::ReturnStatement ret;
    ret.token = tokens_.front();
    tokens_.pop_front();
    /// @todo find proper return type for error checking
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

  // this is already checked in lexer.cpp
  if (is_u_type && is_f_type) {
    throw std::invalid_argument("ambiguous numeric literal");
  }

  // type inference default int
  if (std ::isdigit(value.back())) {
    return is_f_type ? ConstituentToken::TYPE_DOUBLE : ConstituentToken::TYPE_INT;
  }

  switch (value.back()) {
    case 'u': return ConstituentToken::TYPE_UINT;
    case 'i': return is_u_type ? ConstituentToken::TYPE_UINT : ConstituentToken::TYPE_INT;
    case 'l': return is_u_type ? ConstituentToken::TYPE_ULONG : ConstituentToken::TYPE_LONG;
    case 's': return is_u_type ? ConstituentToken::TYPE_USHORT : ConstituentToken::TYPE_SHORT;
    case 'c': return is_u_type ? ConstituentToken::TYPE_UCHAR : ConstituentToken::TYPE_CHAR;
    case 'f': return ConstituentToken::TYPE_FLOAT;
    case 'd': return ConstituentToken::TYPE_DOUBLE;
    case '\0': return is_u_type ? ConstituentToken::TYPE_UINT : ConstituentToken::TYPE_INT;
    default: std::cerr << value << std::endl; throw std::invalid_argument("ambiguous numeric literal token");
  }
}

int Parser::numTokens() const { return tokens_.size(); }

void Parser::debugASTPrinter(Node::AST &node)
{
  auto curr = node.curr.get();
  if (!curr) {
    return;
  }
  if (std::holds_alternative<Node::GenericExpr>(*curr)) {
    auto variant = std::get<Node::GenericExpr>(std::move(*curr));
    debugExprPrinterRecursive(variant, 0);
  }
  else if (std::holds_alternative<Node::VariableDecl>(*curr)) {
    auto variant = std::get<Node::VariableDecl>(std::move(*curr));
    debugVariableDeclPrinter(variant);
  }
  else if (std::holds_alternative<Node::ConditionalStatement>(*curr)) {
    auto variant = std::get<Node::ConditionalStatement>(std::move(*curr));
    debugConditionalStatment(variant);
  }

  if (node.next) {
    debugASTPrinter(*node.next);
  }
}

void Parser::debugExprPrinterRecursive(Node::GenericExpr &node, int depth)
{
  // auto variant =
  // std::make_unique<EXPRESSION_VARIANT>(std::move(*(node.expr)));

  auto variant = node.expr.get();
  if (!variant) {
    std::cout << std::string(depth, ' ') << "variant is null" << std::endl;
    return;
  }

  if (std::holds_alternative<Node::GenericExpr>(*variant)) {
    std::cout << std::string(depth, ' ') << "recursively holds expression" << std::endl;
    debugExprPrinterRecursive(std::get<Node::GenericExpr>(*variant), depth + 1);
  }
  else if (std::holds_alternative<Node::BinaryExpr>(*variant)) {
    auto op = reverse_dictionary_.at(std::get<Node::BinaryExpr>(*variant).op); // validate that op is real
    std::cout << "holds binary expression: " << op << std::endl;
    const auto &binaryExpr = std::get<Node::BinaryExpr>(*variant);
    if (binaryExpr.lhs) {
      std::cout << std::string(depth * 4, ' ') << "lhs: ";
      debugExprPrinterRecursive(*binaryExpr.lhs, depth + 1);
    }
    else {
      throw std::invalid_argument("error: expecting lhs for binary expression");
    }
    if (binaryExpr.rhs) {
      std::cout << std::string(depth * 4, ' ') << "rhs: ";
      debugExprPrinterRecursive(*binaryExpr.rhs, depth + 1);
    }
    else {
      throw std::invalid_argument("error: expecting rhs for binary expression");
    }
  }
  else if (std::holds_alternative<Node::UnaryExpr>(*variant)) {
    auto op = reverse_dictionary_.at(std::get<Node::UnaryExpr>(*variant).op);
    std::cout << "holds unary expression: " << op << std::endl;
    const auto &unaryExpr = std::get<Node::UnaryExpr>(*variant);
    if (unaryExpr.expr) {
      std::cout << std::string(depth * 4, ' ') << "expr: ";
      debugExprPrinterRecursive(*unaryExpr.expr, depth + 1);
    }
  }
  else if (std::holds_alternative<Node::NumericLiteral>(*variant)) {
    auto literal = std::get<Node::NumericLiteral>(*variant);
    std::cout << "holds numeric literal: " << literal.token.value << std::endl;
  }
  else if (std::holds_alternative<Node::Identifier>(*variant)) {
    auto id = std::get<Node::Identifier>(*variant);
    std::cout << "holds identifier: " << id.token.value << std::endl;
  }
  else {
    std::cout << "holds ambiguous state" << std::endl;
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
  // more
  // comment // comment // comment
}

void Parser::debugConditionalStatment(Node::ConditionalStatement &node)
{
  std::cout << "holds conditional statmeent" << '\n';
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
  std::cout << std::endl;
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