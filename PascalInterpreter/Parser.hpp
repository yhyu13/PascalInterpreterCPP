#pragma once

#include <iostream>
#include <sstream>
#include <string>

#include <vector>
#include <stack>

#include "Token.hpp"
#include "Lexer.hpp"
#include "AST.hpp"

class Parser
{
public:
	Parser()
		:
		m_lexer(nullptr),
		m_pAST(nullptr),
		m_CurrentToken(nullptr),
		m_sfd(nullptr)
	{}
		
	virtual ~Parser() {};
	void Reset();

	void SetLexer(Lexer* lexer) noexcept;

	void SetSFD(MyDebug::SrouceFileDebugger* sfd) noexcept;

protected:
	/*
	Funtionality: helper function to throw exception with a specific message
	*/
	inline void Error(const std::string& msg);

	/*
	Funtionality: helper function to throw exception with a specific message
	*/
	inline void ErrorSFD(const std::string& msg);

	/*
	Functionality: Simply call GetNextToken
	Return: current token equals __EOF___
	*/
	bool ConsumeToken();
	/*
	Functionality: Simply call GetNextToken with type matching
	Return: current token equals __EOF___
	*/
	bool ConsumeTokenType(std::string type);
	/*
	Functionality: Simply call GetNextToken with type matching
	Return: current token equals __EOF___
	*/
	bool TryConsumeTokenType(std::string type);
	/*
	Functionality: Simply call GetNextToken and return type
	Return: next token type
	*/
	std::string PeekNextConsumeTokenType();
protected:
	/*
		program: PROGRAM variable SEMI Block DOT
	*/
	SHARE_AST GetProgram();
	/*
		program: PROCEDURE variable SEMI Block DOT
	*/
	SHARE_AST GetProcedure();
	/*
		Block: Declaration  compound_statement
	*/
	SHARE_AST GetBlock();
	/*
	Declaration: Empty | (variable_declaration SEMI+ variable_declaration)
	*/
	SHARE_AST GetParamsDecal();
	/*
	Assignment: Empty | (assignment_statement ,+assignment_statement)
	*/
	SHARE_AST GetParamsAssigment();

	/*
		Declaration: Empty | VAR(variable_declaration SEMI)+ | PROCEDURE(parameter_declaration) SEMI+
	*/
	SHARE_AST GetDeclaration();
	/*
	variable declaration: ID (COMMA ID)* COLON type_spec
	*/
	SHARE_AST GetVariableDeclaration();
	/*
	return type token
	*/
	SHARE_AST GetTypeSpec();
	/*
		compound_statement: BEGIN statement_list END
	*/
	SHARE_AST GetCompoundStatements();
	/*
		statement_list : statement
                   | statement SEMI statement_list
	*/
	vector<SHARE_AST> GetStatementsList();
	/*
		statement : compound_statement
              | assignment_statement
              | empty
	*/
	SHARE_AST GetStatement();
	/*
		assignment_statement : variable ASSIGN expr
	*/
	SHARE_AST GetAssignStatement();
	/*
		variable : ID
	*/
	SHARE_AST GetVariable(std::string type = ID);
	/*
		An empty production
	*/
	SHARE_AST GetEmpty();
	/*
		factor : PLUS  factor
              | MINUS factor
              | INTEGER
              | LPAREN expr RPAREN
              | variable
	*/
	SHARE_AST GetFactor();
	/*
		2nd level of the Int Op expression, middle precedence:
		Handles integer mul/div
	*/
	SHARE_AST GetTerm();
	/*
		3rd level of the Int Op expression, lowest precedence:
		Handles integer plus/minus
	*/
	SHARE_AST GetExpr();

public:

	/*
	Functionality: parse the input text (should be a program format) into AST
	Return: pointer constant to the AST instance
	*/
	SHARE_AST GetProgramAST();

private:
	Lexer* m_lexer;
	SHARE_TOKEN_STRING m_CurrentToken;
	SHARE_AST m_pAST;
	std::vector<std::string> token_code_factor = { INTEGER, LEFT_PARATHESES, RIGHT_PARATHESES, PLUS, MINUS , ID, FLOAT,CALL_ID };
	std::vector<std::string> token_code_term = { MUL, DIV, INT_DIV };
	std::vector<std::string> token_code_expr = { PLUS, MINUS };

	MyDebug::SrouceFileDebugger* m_sfd;
};

