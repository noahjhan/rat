#include "assembly.hpp"

#define HERE std::cout << "here: " << __LINE__ << std::endl

/// @todo create a stack implementation of the current symbol table
/// @todo evaluate expressions
/// @todo optimize expressions
/// @todo do not allocate for literals

Compiler::Compiler(const std::unique_ptr<Node::AST> &ast, const std::string &filename)
: filename_(filename)
{
  if (!ast) {
    throw std::invalid_argument("null ast");
  }
  ast_ = std::make_shared<Node::AST>(std::move(*ast));
  // trunc();
  initializeLocal();
  dispatch(ast_);
  open();
  fs_ << globals_buffer_.str() << '\n' << file_buffer_.str();
}

void Compiler::initializeLocal()
{

  globals_buffer_ << '\n' << "; ModuleID = '" << filename_ << "'\n";
  globals_buffer_ << "target triple = \"arm64-apple-macosx15.0.0\""
                  << "\n\n";                         // for local system
  globals_buffer_ << "declare i32 @printf(ptr)\n\n"; // enable logging
}

void Compiler::dispatch(const std::shared_ptr<Node::AST> &tree)
{
  if (!tree || !tree->curr) {
    return; // end of AST
  }

  if (std::holds_alternative<Node::FunctionDecl>(*tree->curr)) {
    functionDeclaration(std::make_shared<Node::FunctionDecl>(
    std::get<Node::FunctionDecl>(std::move(*tree->curr))));
  }
  else if (std::holds_alternative<Node::GenericExpr>(*tree->curr)) {
    expression(std::make_unique<Node::GenericExpr>(
    std::move(std::get<Node::GenericExpr>(*tree->curr))));
  }

  if (tree->next) {
    dispatch(std::make_shared<Node::AST>(std::move(*tree->next))); // recursive call
  }
}

void Compiler::functionDeclaration(const std::shared_ptr<Node::FunctionDecl> &decl)
{
  if (!decl) {
    throw std::invalid_argument("null function declaration");
  }

  std::string asm_define = TYPE_ASM.at(decl->return_type);
  asm_define.append(" @" + decl->token.value);
  function_table_.insert({decl->token.value, asm_define});

  switch (decl->type) {
    case ConstituentToken::FUNCTION_DECLARATION_F:
      break;
    case ConstituentToken::FUNCTION_DECLARATION_F_VOID: {
      file_buffer_ << "define " << asm_define << declarationParameters(decl->parameters)
                   << " {" << '\n'; // put params here
      auto statement = decl->body.get();
      while (statement) {
        if (std::holds_alternative<Node::VariableDecl>(*statement->curr)) {
          *statement->curr = *allocateVariables(std::make_unique<Node::VariableDecl>(
          std::get<Node::VariableDecl>(std::move(*statement->curr))));
        }
        statement = (statement->next).get(); // this will cause issues
      }
      functionBody(decl->body);
    } break;
    case ConstituentToken::FUNCTION_DECLARATION_F_OPTIONAL:
      break;
    case ConstituentToken::FUNCTION_DECLARATION_F_LAMBDA:
      break;
    default:
      throw std::invalid_argument("expected function type");
  }
  file_buffer_ << appendable_buffer_.str();
  appendable_buffer_.str("");
  appendable_buffer_.clear();
  file_buffer_ << "}\n\n";
  num_registers_ = 0;
}

/// @todo support for any return type (pass as param to this function)
std::string Compiler::declarationParameters(
const std::vector<std::pair<std::string, ConstituentToken>> &parameters)
{
  if (parameters.empty()) {
    num_registers_ = 1;
    return "()";
  }

  std::string str = "(";
  for (const auto &[id, tok] : parameters) {
    std::string register_num = "%" + std::to_string(num_registers_++);
    str += TYPE_ASM.at(tok) + " noundef " + register_num + ", ";
    num_registers_++;
  }
  if (str.size() > 2) {
    str.pop_back();
    str.pop_back();
  }
  if (num_registers_ == 0) {
    num_registers_ = 1;
  }
  str += ")";
  return str;
}

void Compiler::functionBody(const std::shared_ptr<Node::AST> &body)
{
  if (!body || !body->curr) {
    return;
  }
  if (std::holds_alternative<Node::ReturnStatement>(*body->curr)) {
    returnStatement(std::make_unique<Node::ReturnStatement>(
    std::get<Node::ReturnStatement>(std::move(*body->curr))));
  }
  else if (std::holds_alternative<Node::GenericExpr>(*body->curr)) {
    expression(std::make_unique<Node::GenericExpr>(
    std::get<Node::GenericExpr>(std::move(*body->curr))));
  }
  else if (std::holds_alternative<Node::VariableDecl>(*body->curr)) {
    variableDeclaration(std::make_unique<Node::VariableDecl>(
    std::get<Node::VariableDecl>(std::move(*body->curr))));
  }

  if (body->next) {
    functionBody(std::make_shared<Node::AST>(std::move(*body->next)));
  }
}

void Compiler::functionCall(const std::unique_ptr<Node::FunctionCall> &call)
{
  if (!call) {
    return;
  }
  if (call->token.value == "print") {
    appendable_buffer_ << "\tcall i32 @printf(ptr @.str." << num_string_constants_
                       << ")\n";
  }

  expression(call->parameters[0]);
}

void Compiler::returnStatement(
const ::std::unique_ptr<Node::ReturnStatement> &return_statement)
{
  if (!return_statement) {
    throw std::invalid_argument("null return statement");
  }

  /// @todo figure out the associated register with the finalized return expression
  std::string identifier = "x";

  ConstituentToken return_type_token = return_statement->type;
  std::string return_type_asm = TYPE_ASM.at(return_type_token);

  if (return_type_token == ConstituentToken::TYPE_MAIN) {

    appendable_buffer_ << '\t' << "ret " << return_type_asm << " 0" << '\n';
  }
  else if (return_statement->token.value == "ret") {
    appendable_buffer_ << '\t' << "ret " << return_type_asm << ' '
                       << identifiers_.at(identifier).first << '\n';
  }
  else if (return_statement->token.value == "rev") {
    // return_type_asm can be hardcoded -> better it throws
    if (return_type_asm != "void") {
      throw std::invalid_argument("error: invalid return type for void function");
    }
    appendable_buffer_ << '\t' << "ret " << return_type_asm << '\n';
  }
}

/*
std::variant< Node::GenericExpr,
              Node::BinaryExpr,
              Node::UnaryExpr,
              Node::NumericLiteral,
              Node::StringLiteral,
              Node::Identifier,
              Node::FunctionCall>
*/

// I think expressions recurisvely call which return a string representation of the
// pointer to the returned operation

std::unique_ptr<Node::VariableDecl>
Compiler::allocateVariables(const std::unique_ptr<Node::VariableDecl> &decl)
{
  if (!decl) {
    throw std::invalid_argument("null variable declaration");
  }

  std::string type_asm = TYPE_ASM.at(decl->type);
  std::string alignment = ALIGN_ASM.at(decl->type);
  std::string register_num = "%" + std::to_string(num_registers_++);

  identifiers_.insert({decl->token.value, {register_num, "ptr"}});
  scoped_registers_.insert({register_num, type_asm});

  file_buffer_ << '\t' << register_num << " = alloca " << type_asm << ", " << alignment
               << '\n';

  return std::make_unique<Node::VariableDecl>(std::move(*decl));
}

std::unique_ptr<Expression>
Compiler::expression(const std::unique_ptr<Node::GenericExpr> &call)
{
  if (!call) {
    throw std::invalid_argument("expression is null");
  }

  if (std::holds_alternative<Node::GenericExpr>(*call->expr)) {
    throw std::invalid_argument("expressions cannot point to expressions");
  }
  else if (std::holds_alternative<Node::BinaryExpr>(*call->expr)) {
    auto expr = std::get<Node::BinaryExpr>(std::move(*call->expr));
    auto lhs = expression(expr.lhs);
    auto rhs = expression(expr.rhs);

    std::string type_asm = curr_expr_type;
    std::string alignment = STRING_TYPE_ALIGN.at(curr_expr_type);
    std::string register_num = "%" + std::to_string(num_registers_++);

    switch (expr.op) {
      case ConstituentToken::ARITHMETIC_ADD: {
        // <result> = add <ty> <op1>, <op2>
        appendable_buffer_ << '\t' << register_num << " = add " << curr_expr_type << ' '
                           << lhs->register_number << ", " << rhs->register_number
                           << '\n';
        Expression expr_struct(std::nullopt, curr_expr_type, register_num);
        return std::make_unique<Expression>(std::move(expr_struct));
      }
      case ConstituentToken::ARITHMETIC_SUB:
      case ConstituentToken::ARITHMETIC_MUL:
      case ConstituentToken::ARITHMETIC_DIV:
      case ConstituentToken::ARITHMETIC_MOD:
      case ConstituentToken::COMPARISON_EQ:
      case ConstituentToken::COMPARISON_LT:
      case ConstituentToken::COMPARISON_GT:
      case ConstituentToken::COMPARISON_LTE:
      case ConstituentToken::COMPARISON_GTE:
      case ConstituentToken::LOGICAL_AND:
      case ConstituentToken::LOGICAL_OR:
      case ConstituentToken::LOGICAL_NOT:
      case ConstituentToken::BITWISE_AND:
      case ConstituentToken::BITWISE_OR:
      case ConstituentToken::BITWISE_XOR:
      case ConstituentToken::BITWISE_SL:
      case ConstituentToken::BITWISE_SR:
      default:
        throw std::invalid_argument("unrecognized operator");
    }
  }
  else if (std::holds_alternative<Node::UnaryExpr>(*call->expr)) {
  }
  else if (std::holds_alternative<Node::NumericLiteral>(*call->expr)) {
    auto literal = std::get<Node::NumericLiteral>(std::move(*call->expr));

    // std::string type_asm = curr_expr_type;
    // std::string alignment = STRING_TYPE_ALIGN.at(curr_expr_type);
    // std::string register_num = "%" + std::to_string(num_registers_++);
    // // %1 = add i32 0, 42
    // // appendable_buffer_ << "\tstore " << type_asm << ' ' << literal.token.value << ",
    // // ptr "
    // //                    << register_num << ", " << alignment << '\n';

    // appendable_buffer_ << '\t' << register_num << " = add " << curr_expr_type << " 0, "
    //                    << literal.token.value << '\n';

    Expression expr_struct(std::nullopt, curr_expr_type, literal.token.value);
    // scoped_registers_.insert({register_num, type_asm});
    return std::make_unique<Expression>(std::move(expr_struct));
  }
  else if (std::holds_alternative<Node::StringLiteral>(*call->expr)) {
    auto node = std::get<Node::StringLiteral>(*call->expr);
    Expression expr_struc(std::nullopt, curr_expr_type, stringGlobal(node.token.value));
  }
  else if (std::holds_alternative<Node::Identifier>(*call->expr)) {
    auto identifier = std::get<Node::Identifier>(*call->expr);
    Expression id_ptr(identifier.token.value,
                      identifiers_.at(identifier.token.value).second,
                      identifiers_.at(identifier.token.value).first);

    // %4 = load i32, ptr %2, align 4
    std::string type_asm = curr_expr_type;
    std::string alignment = STRING_TYPE_ALIGN.at(curr_expr_type);
    std::string register_num = "%" + std::to_string(num_registers_++);
    appendable_buffer_ << '\t' << register_num << " = load " << type_asm << ", "
                       << id_ptr.type << ' ' << id_ptr.register_number << ", "
                       << alignment << '\n';

    scoped_registers_.insert({register_num, type_asm});

    Expression expr_struct(identifier.token.value, type_asm, register_num);
    return std::make_unique<Expression>(std::move(expr_struct));
  }
  else if (std::holds_alternative<Node::FunctionCall>(*call->expr)) {
    functionCall(std::make_unique<Node::FunctionCall>(
    std::get<Node::FunctionCall>(std::move(*call->expr))));
  }
  else {
    throw std::invalid_argument("unrecognized expression variant");
  }

  return std::make_unique<Expression>(Expression(std::nullopt, "ERROR", "TODO"));
}

std::string Compiler::stringGlobal(const std::string &str)
{
  std::string formatted(str.begin() + 1, str.end() - 1);
  std::string identifier = "@.str." + std::to_string(num_string_constants_++);

  globals_buffer_ << identifier << " = private unnamed_addr constant ["
                  << formatted.size() + 2 << " x i8] c\"" << formatted
                  << "\\0A\\00\", align 1\n";

  return identifier;
}

void Compiler::variableDeclaration(const std::unique_ptr<Node::VariableDecl> &decl)
{
  if (!decl) {
    throw std::invalid_argument("null variable declaration");
  }

  std::string type_asm = TYPE_ASM.at(decl->type);
  std::string alignment = ALIGN_ASM.at(decl->type);
  std::string register_num = identifiers_.at(decl->token.value).first;

  /// @todo in semantic optimize expressions of just numeric values to store only literals

  // if (decl->expr && std::holds_alternative<Node::NumericLiteral>(*decl->expr->expr)) {
  //   auto literal = std::get<Node::NumericLiteral>(*decl->expr->expr);
  //   appendable_buffer_ << '\t' << "store " << curr_expr_type << ' ' <<
  //   literal.token.value
  //                      << ", ptr " << register_num << ", " << alignment << '\n';
  // }
  // else {

  Expression expr_struct =
  *expression(std::make_unique<Node::GenericExpr>(std::move(*decl->expr)));
  // store i32 %temp, ptr %x, align 4
  appendable_buffer_ << '\t' << "store " << expr_struct.type << ' '
                     << expr_struct.register_number << ", ptr " << register_num << ", "
                     << alignment << '\n';
  // }
}
inline void Compiler::open()
{
  fs_.close();
  fs_.open(filename_, std::ios::out);
  if (!fs_) {
    throw std::invalid_argument("error: unable to open output file");
  }
}

inline void Compiler::close()
{
  fs_.close();
}

inline void Compiler::append()
{
  fs_.close();
  fs_.open(filename_, std::ios::app);
  if (!fs_) {
    throw std::invalid_argument("error: unable to append to output file");
  }
}

inline void Compiler::readWrite()
{
  fs_.close();
  fs_.open(filename_, std::ios::in | std::ios::out);
  if (!fs_) {
    throw std::invalid_argument("error: unable to read/write to output file");
  }
}
inline void Compiler::read()
{
  fs_.close();
  fs_.open(filename_, std::ios::in);
  if (!fs_) {
    throw std::invalid_argument("error: unable to read output file");
  }
}

inline void Compiler::trunc()
{
  fs_.close();
  fs_.open(filename_, std::ios::out | std::ios::trunc);
  if (!fs_) {
    throw std::invalid_argument("error: unable to read output file");
  }
}