#pragma once

#include <iostream>
#include <sstream>
#include <string>

#include <vector>
#include <stack>

#include "MyMacros.hpp"
#include "MyTemplates.hpp"
#include "MyExceptions.hpp"
#include "Token.hpp"
#include "Lexer.hpp"
#include "AST.hpp"

class Parser
{
public:
	Parser() :m_pAST(nullptr) {};
	explicit Parser(std::string text) : m_text(text), m_pAST(nullptr)
	{
	}
	virtual ~Parser()
	{
	}
	void Reset()
	{
		m_pAST.reset();
		m_text = "\0";
	}
	void SetText(std::string text)
	{
		m_text = text;
	}


protected:
	/*
	Funtionality: helper function to throw exception with a specific message
	*/
	inline void Error(const std::string& msg)
	{
		throw MyExceptions::InterpreterExecption(msg);
	}

	inline void PtrError(void* ptr, const std::string& msg)
	{
		if (!ptr)
			throw MyExceptions::InterpreterExecption(msg);
	}

	/*
	Functionality: Simply call GetNextToken
	Return: current token equals __EOF___
	*/
	bool ConsumeToken()
	{
		m_CurrentToken = m_lexer.GetNextToken();
		return m_CurrentToken->GetType() != __EOF__;
	}
	/*
	Functionality: Simply call GetNextToken with type matching
	Return: current token equals __EOF___
	*/
	bool ConsumeTokenType(std::string type)
	{

		if (m_CurrentToken->GetType() == type)
		{
			m_CurrentToken = m_lexer.GetNextToken();
			return m_CurrentToken->GetType() != __EOF__;
		}
		else
		{
			Error("SynatxError(parser): should comsume " + type + ", instead, comsuming " + m_CurrentToken->GetType());
		}
	}
	/*
		program: compound_statement DOT
	*/
	SHARE_AST GetProgram()
	{
		auto result = GetCompoundStatements();
		ConsumeTokenType(DOT);
		return result;
	}
	/*
		compound_statement: BEGIN statement_list END
	*/
	SHARE_AST GetCompoundStatements()
	{
		ConsumeTokenType(BEGIN);
		auto results = GetStatementsList();
		ConsumeTokenType(END);
		
		CREATE_SHARE_COMPOUND_AST(root);
		for (auto& r : results)
		{
			root->AddStatements(r);
		}
		return root;
	}
	/*
		statement_list : statement
                   | statement SEMI statement_list
	*/
	vector<SHARE_AST> GetStatementsList()
	{
		auto result = GetStatement();
		std::vector<SHARE_AST> result_list;
		result_list.push_back(result);
		
		while ((m_CurrentToken->GetType() == SEMI))
		{
			ConsumeTokenType(SEMI);
			result_list.push_back(GetStatement());
		}
		return result_list;
	}
	/*
		statement : compound_statement
              | assignment_statement
              | empty
	*/
	SHARE_AST GetStatement()
	{
		auto token = m_CurrentToken;
		if (token->GetType() == BEGIN)
		{
			auto result = GetCompoundStatements();
			return result;
		}
		else if (token->GetType() == ID)
		{
			auto result = GetAssignStatement();
			return result;
		}
		else
		{
			auto result = GetEmpty();
			return result;
		}
	}
	/*
		assignment_statement : variable ASSIGN expr
	*/
	SHARE_AST GetAssignStatement()
	{
		auto left = GetVariable();
		auto op = MAKE_SHARE_AST(m_CurrentToken);
		ConsumeTokenType(ASSIGN);
		auto right = GetExpr();
		return MAKE_SHARE_ASSIGN_AST(left, right, op);
	}
	/*
		variable : ID
	*/
	SHARE_AST GetVariable()
	{
		auto token = m_CurrentToken;
		ConsumeTokenType(ID);
		return MAKE_SHARE_AST(token);
	}
	/*
		An empty production
	*/
	SHARE_AST GetEmpty()
	{
		return MAKE_SHARE_EMPTY_AST();
	}
	/*
		factor : PLUS  factor
              | MINUS factor
              | INTEGER
              | LPAREN expr RPAREN
              | variable
	*/
	SHARE_AST GetFactor()
	{
		auto token = m_CurrentToken;
		std::string token_code[7] = { INTEGER, LEFT_PARATHESES, RIGHT_PARATHESES, PLUS, MINUS , ID, FLOAT};
		// Handle integer
		if (token->GetType() == token_code[0] || token->GetType() == token_code[6])
		{
			ConsumeTokenType(token->GetType());
			return MAKE_SHARE_AST(token);
		}
		// Handle paratheses
		else if (token->GetType() == token_code[1])
		{
			ConsumeTokenType(token_code[1]);
			auto result = GetExpr();
			m_pAST = result;
			ConsumeTokenType(token_code[2]);
			return result;
		}
		// Handle unary operator
		else if (token->GetType() == token_code[3] || token->GetType() == token_code[4])
		{
			ConsumeTokenType(token->GetType());
			CREATE_SHARE_UNARY_AST(result, token, GetFactor());
			m_pAST = result;
			return result;
		}
		// Handle variable
		else if (token->GetType() == token_code[5])
		{
			return GetVariable();
		}
		else
		{
			Error("SynatxError(parser): unknown factor: " + token->GetType() + ".");
		}
	}
	/*
		2nd level of the Int Op expression, middle precedence:
		Handles integer mul/div
	*/
	SHARE_AST GetTerm()
	{
		auto result = GetFactor();
		m_pAST = result;
		std::string token_code[2] = { MUL, DIV };

		while ((m_CurrentToken->GetType() == token_code[0]) ||
			(m_CurrentToken->GetType() == token_code[1]))
		{
			auto token = m_CurrentToken;
			ConsumeTokenType(token->GetType());
			auto temp = GetFactor();
			result = MAKE_SHARE_BINARY_AST(result, temp, MAKE_SHARE_AST(token));
			m_pAST = result;
		}
		return result;
	}
	/*
		3rd level of the Int Op expression, lowest precedence:
		Handles integer plus/minus
	*/
	SHARE_AST GetExpr()
	{
		auto result = GetTerm();
		m_pAST = result;
		std::string token_code[2] = { PLUS, MINUS };

		while ((m_CurrentToken->GetType() == token_code[0]) ||
			(m_CurrentToken->GetType() == token_code[1]))
		{
			auto token = m_CurrentToken;
			ConsumeTokenType(token->GetType());
			auto temp = GetTerm();
			result = MAKE_SHARE_BINARY_AST(result, temp, MAKE_SHARE_AST(token));
			m_pAST = result;
		}
		return result;
	}

public:

	/*
	Functionality: parse the input text (should be a program format) into AST
	Return: pointer constant to the AST instance
	*/
	SHARE_AST GetProgramAST()
	{
		m_lexer = Lexer(m_text);
		m_CurrentToken = m_lexer.GetNextToken();
		m_pAST = GetProgram();
		return m_pAST;
	}

private:
	std::string m_text;
	Lexer m_lexer;
	SHARE_TOKEN_STRING m_CurrentToken;
	SHARE_AST m_pAST;
};

