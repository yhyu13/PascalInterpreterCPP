#pragma once

/*
MACRO
*/
#include <iostream>
#include <memory>
#include <algorithm>
#include <vector>
#include "MyDebug.hpp"
using namespace std;

//----------------------------------------------------------------------------------------------
#define Myprintln(var) std::cout << var->ToString() << std::endl;
#define ITEM_IN_VEC(item, vec) (find(vec.begin(), vec.end(), item) != vec.end())


//----------------------------------------------------------------------------------------------
#define SHARE_STRING std::shared_ptr<std::string>
#define SHARE_TOKEN_STRING std::shared_ptr<Token<std::shared_ptr<std::string>>>
#define SHARE_AST std::shared_ptr<AST>
#define SHARE_UNARY_AST std::shared_ptr<UnaryOp_AST>
#define SHARE_BINARY_AST std::shared_ptr<BinaryOp_AST>
#define SHARE_COMPOUND_AST std::shared_ptr<Compound_AST>
#define SHARE_ASSIGN_AST std::shared_ptr<Assign_AST>
#define SHARE_EMPTY_AST std::shared_ptr<Empty_Op>
#define SHARE_PROGRAM_AST std::shared_ptr<Program_AST>
#define SHARE_BLOCk_AST std::shared_ptr<Block_AST>

#define SHARE_DECLARATION_AST std::shared_ptr<Declaration_AST>
#define SHARE_DECLCONTAINER_AST std::shared_ptr<DeclContainer_AST>
#define SHARE_VARDECL_AST std::shared_ptr<VarDecl_AST>

//----------------------------------------------------------------------------------------------
#define TOKEN_STRING_MAP std::map<std::string, SHARE_TOKEN_STRING>
#define TOKEN_STRING_PAIR std::pair<std::string, SHARE_TOKEN_STRING>
#define SYMBOL_MAP std::map<std::string, VarSymbol>
#define SYMBOL_PAIR std::pair<std::string, VarSymbol>

//----------------------------------------------------------------------------------------------
#define MAKE_SHARE_STRING(var) std::make_shared<std::string>(var)
#define MAKE_SHARE_TOKEN(type,var) std::make_shared<Token<decltype(var)>>(type,var)
#define MAKE_SHARE_AST(token) std::make_shared<AST>(token)
#define MAKE_SHARE_UNARY_AST(op, expr) std::make_shared<UnaryOp_AST>(op, expr)
#define MAKE_SHARE_BINARY_AST(left, right, op) std::make_shared<BinaryOp_AST>(left, right, op)
#define MAKE_SHARE_COMPOUND_AST() std::make_shared<Compound_AST>()
#define MAKE_SHARE_ASSIGN_AST(left, right, op) std::make_shared<Assign_AST>(left, right, op)
#define	MAKE_SHARE_EMPTY_AST() std::make_shared<Empty_Op>()
#define MAKE_SHARE_PROGRAM_AST(name, block) std::make_shared<Program_AST>(name, block)
#define MAKE_SHARE_BLOCK_AST(declaration, compound) std::make_shared<Block_AST>(declaration, compound)

#define MAKE_SHARE_DECLARATION_AST() std::make_shared<Declaration_AST>()
#define MAKE_SHARE_DECLCONTAINER_AST() std::make_shared<DeclContainer_AST>()
#define MAKE_SHARE_VARDECL_AST(var, type) std::make_shared<VarDecl_AST>(var, type)

//----------------------------------------------------------------------------------------------
#define CREATE_SHARE_STRING(name, var) std::shared_ptr<std::string> name(new std::string(var));
#define CREATE_SHARE_TOKEN(name, type, var) std::shared_ptr<Token<std::shared_ptr<std::string>>> name(new Token<std::shared_ptr<std::string>>(type, var));
#define CREATE_SHARE_AST(name, token) std::shared_ptr<AST> name(new AST(token));
#define CREATE_SHARE_UNARY_AST(name, op, expr) std::shared_ptr<UnaryOp_AST> name(new UnaryOp_AST(op, expr));
#define CREATE_SHARE_BINARY_AST(name, left, right, op) std::shared_ptr<BinaryOp_AST> name(new BinaryOp_AST(left, right, op));
#define CREATE_SHARE_COMPOUND_AST(name) std::shared_ptr<Compound_AST> name(new Compound_AST());
#define CREATE_SHARE_ASSIGN_AST(name, left, right, op) std::shared_ptr<Assign_AST> name(new Assign_AST(left, right, op));
#define CREATE_SHARE_EMPTY_AST(name) std::shared_ptr<Empty_Op> name(new Empty_Op());
#define CREATE_SHARE_PROGRAM_AST(name, programName, block) std::shared_ptr<Program_AST> name(new Program_AST(programName, block));
#define CREATE_SHARE_BLOCK_AST(name, declaration, compound) std::shared_ptr<Block_AST> name(new Block_AST(declaration, compound));

#define CREATE_SHARE_DECLARATION_AST(name) std::shared_ptr<Declaration_AST> name(new Declaration_AST());
#define CREATE_SHARE_DECLCONTAINER_AST(name) std::shared_ptr<DeclContainer_AST> name(new DeclContainer_AST());
#define CREATE_SHARE_VARDECL_AST(name, var, type) std::shared_ptr<VarDecl_AST> name(new VarDecl_AST(var, type));