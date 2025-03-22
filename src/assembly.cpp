#include "assembly.hpp"

#define HERE std::cout << "here: " << __LINE__ << std::endl

/// @todo a good way to format the document would be store each function or module in its
/// own buffer and paste as necessary for easier manipulation

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
}

/// @todo support for any return type (pass as param to this function)
std::string Compiler::declarationParameters(
const std::vector<std::pair<std::string, ConstituentToken>> &paramters)
{
  if (paramters.empty()) {
    return "()";
  }
  return ")(";
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
  if (std::holds_alternative<Node::GenericExpr>(*body->curr)) {
    expression(std::make_unique<Node::GenericExpr>(
    std::get<Node::GenericExpr>(std::move(*body->curr))));
  }
  if (std::holds_alternative<Node::VariableDecl>(*body->curr)) {
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
    appendable_buffer_ << "\tcall i32 @printf(ptr @.str" << num_string_constants_
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
                       << scoped_registers_.at(identifier).first << '\n';
  }
  else if (return_statement->token.value == "rev") {
    // return_type_asm can be hardcoded -> better it throws
    if (return_type_asm != "void") {
      throw std::invalid_argument("error: invalid return type for void function");
    }
    appendable_buffer_ << '\t' << "ret " << return_type_asm << '\n';
  }
}

void Compiler::expression(const std::unique_ptr<Node::GenericExpr> &call)
{
  if (std::holds_alternative<Node::FunctionCall>(*call->expr)) {
    functionCall(std::make_unique<Node::FunctionCall>(
    std::get<Node::FunctionCall>(std::move(*call->expr))));
  }
  else if (std::holds_alternative<Node::StringLiteral>(*call->expr)) {
    auto node = std::get<Node::StringLiteral>(*call->expr);
    stringGlobal(node.token.value);
  }
}
void Compiler::stringGlobal(const std::string &str)
{
  std::string formatted(str.begin() + 1, str.end() - 1);

  globals_buffer_ << "@.str" << num_string_constants_
                  << " = private unnamed_addr constant [" << formatted.size() + 2
                  << " x i8] c\"" << formatted << "\\0A\\00\", align 1\n";

  ++num_string_constants_;
}

void Compiler::variableDeclaration(const std::unique_ptr<Node::VariableDecl> &decl)
{

  if (!decl) {
    throw std::invalid_argument("null variable declaration");
  }

  // find top of relevant function
  // add allocation for memory
  // at declaration store those bytes
  /// @todo eval expressions
  std::string expr = "10";

  std::string type_asm = TYPE_ASM.at(decl->type);
  std::string alignment = ALIGN_ASM.at(decl->type);
  std::string register_num = "%";
  register_num.append(std::to_string(num_registers_));

  file_buffer_ << '\t' << register_num << " = alloca " << type_asm << ", " << alignment
               << '\n';

  // identifier -> "%reg_num, type"
  scoped_registers_.insert({decl->token.value, {register_num, type_asm}});

  appendable_buffer_ << "\tstore " << type_asm << ' ' << expr << ", ptr " << register_num
                     << ", " << alignment << '\n';

  num_registers_++;
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