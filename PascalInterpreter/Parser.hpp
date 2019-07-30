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
	Parser()
		:
		m_lexer(nullptr),
		m_pAST(nullptr),
		m_CurrentToken(nullptr),
		m_sfd(nullptr)
	{}

	virtual ~Parser() {};
	void Reset()
	{
		m_lexer = nullptr;
		m_pAST.reset();
		m_CurrentToken.reset();
		m_sfd = nullptr;
	}

	void SetLexer(Lexer* lexer) noexcept
	{
		m_lexer = lexer;
		m_CurrentToken = m_lexer->GetNextToken();
	}

	void SetSFD(MyDebug::SrouceFileDebugger* sfd) noexcept
	{
		m_sfd = sfd;
	}

protected:
	/*
	Funtionality: helper function to throw exception with a specific message
	*/
	inline void Error(const std::string& msg)
	{
		throw MyExceptions::MsgExecption(msg);
	}

	/*
	Funtionality: helper function to throw exception with a specific message
	*/
	inline void ErrorSFD(const std::string& msg)
	{
		throw MyExceptions::MsgExecption(msg, m_sfd, m_CurrentToken->GetPos());
	}

	/*
	Functionality: Simply call GetNextToken
	Return: current token equals __EOF___
	*/
	bool ConsumeToken()
	{
		m_CurrentToken = m_lexer->GetNextToken();
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
			m_CurrentToken = m_lexer->GetNextToken();
			return m_CurrentToken->GetType() != __EOF__;
		}
		else
		{
			ErrorSFD("SynatxError(parser): should comsume " + type + ", instead, comsuming " + m_CurrentToken->ToString());
		}
	}
	/*
	Functionality: Simply call GetNextToken with type matching
	Return: current token equals __EOF___
	*/
	bool TryConsumeTokenType(std::string type)
	{

		if (m_CurrentToken->GetType() == type)
		{
			m_CurrentToken = m_lexer->GetNextToken();
			return m_CurrentToken->GetType() != __EOF__;
		}
		else
		{
			return false;
		}
	}
protected:
	/*
		program: PROGRAM variable SEMI Block DOT
	*/
	SHARE_AST GetProgram()
	{
		SHARE_AST result;
		// Declared a prorgam name
		if (TryConsumeTokenType(PROGRAM))
		{
			
			auto programName = GetVariable();
			
			ConsumeTokenType(SEMI);
			auto block = GetBlock();
			ConsumeTokenType(DOT);
			result = MAKE_SHARE_PROGRAM_AST(programName, block);
		}
		// No program name declared
		else
		{
			result = GetBlock();
			ConsumeTokenType(DOT);
		}
		return result;
	}
	/*
		program: PROCEDURE variable SEMI Block DOT
	*/
	SHARE_AST GetProcedure()
	{
		ConsumeTokenType(PROCEDURE);
		auto programName = GetVariable();
		ConsumeTokenType(SEMI);

		auto block = GetBlock();

		return MAKE_SHARE_PROCEDURE_AST(programName, block);
	}
	/*
		Block: Declaration  compound_statement
	*/
	SHARE_AST GetBlock()
	{
		auto decal = GetDeclaration();
		auto comp = GetCompoundStatements();
		return MAKE_SHARE_BLOCK_AST(decal, comp);
	}
	/*
		Declaration: Empty | VAR(variable_declaration SEMI)+
	*/
	SHARE_AST GetDeclaration()
	{
		CREATE_SHARE_DECLARATION_AST(results);
		if (TryConsumeTokenType(VAR))
		{
			while (m_CurrentToken->GetType() == ID)
			{
				results->AddVarDecal(GetVariableDeclaration());
				ConsumeTokenType(SEMI);
			}
		}
		while (m_CurrentToken->GetType() == PROCEDURE)
		{
			results->AddVarDecal(GetProcedure());
			ConsumeTokenType(SEMI);
		}

		if (results->IsEmpty())
		{
			return MAKE_SHARE_EMPTY_AST();
		}
		else
		{
			return results;
		}
	}
	/*
	variable declaration: ID (COMMA ID)* COLON type_spec
	*/
	SHARE_AST GetVariableDeclaration()
	{
		CREATE_SHARE_DECLCONTAINER_AST(temp);
		CREATE_SHARE_DECLCONTAINER_AST(results);
		temp->AddItem(GetVariable());
		while (TryConsumeTokenType(COMMA))
		{
			temp->AddItem(GetVariable());
		}
		ConsumeTokenType(COLON);
		auto type = GetTypeSpec();
		// Add VarDecal(var, type) pair into Decal Container
		for (auto& item : temp->GetAllChildren())
		{
			results->AddItem(MAKE_SHARE_VARDECL_AST(item, type));
		}
		return results;
	}
	/*
	return type token
	*/
	SHARE_AST GetTypeSpec()
	{
		auto token = m_CurrentToken;
		ConsumeTokenType(TYPE);
		return MAKE_SHARE_AST(token);
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
		SHARE_AST result = GetStatement();
		std::vector<SHARE_AST> result_list;
		result_list.push_back(result);

		while (!dynamic_pointer_cast<Empty_AST>(result))
		{
			ConsumeTokenType(SEMI);
			result = GetStatement();
			result_list.push_back(result);
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
			return GetCompoundStatements();
		}
		else if (token->GetType() == ID)
		{
			return GetAssignStatement();
		}
		else
		{
			return GetEmpty();
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
		// Handle integer
		if (token->GetType() == token_code_factor[0] || token->GetType() == token_code_factor[6])
		{
			ConsumeTokenType(token->GetType());
			return MAKE_SHARE_AST(token);
		}
		// Handle paratheses
		else if (token->GetType() == token_code_factor[1])
		{
			ConsumeTokenType(token_code_factor[1]);
			auto result = GetExpr();
			m_pAST = result;
			ConsumeTokenType(token_code_factor[2]);
			return result;
		}
		// Handle unary operator
		else if (token->GetType() == token_code_factor[3] || token->GetType() == token_code_factor[4])
		{
			ConsumeTokenType(token->GetType());
			CREATE_SHARE_UNARY_AST(result, token, GetFactor());
			m_pAST = result;
			return result;
		}
		// Handle variable
		else if (token->GetType() == token_code_factor[5])
		{
			return GetVariable();
		}
		else
		{
			ErrorSFD("SynatxError(parser): unknown factor: " + token->GetType() + ".");
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

		while (ITEM_IN_VEC(m_CurrentToken->GetType(), token_code_term))
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

		while (ITEM_IN_VEC(m_CurrentToken->GetType(), token_code_expr))
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
		m_pAST = GetProgram();
		return m_pAST;
	}

private:
	Lexer* m_lexer;
	SHARE_TOKEN_STRING m_CurrentToken;
	SHARE_AST m_pAST;
	std::vector<std::string> token_code_factor = { INTEGER, LEFT_PARATHESES, RIGHT_PARATHESES, PLUS, MINUS , ID, FLOAT };
	std::vector<std::string> token_code_term = { MUL, DIV, INT_DIV };
	std::vector<std::string> token_code_expr = { PLUS, MINUS };

	MyDebug::SrouceFileDebugger* m_sfd;
};

