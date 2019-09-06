#include "Parser.hpp"

void Parser::Reset()
{
	m_lexer = nullptr;
	m_pAST.reset();
	m_CurrentToken.reset();
	m_sfd = nullptr;
}

void Parser::SetLexer(Lexer* lexer) noexcept
{
	m_lexer = lexer;
	m_CurrentToken = m_lexer->GetNextToken();
}

void Parser::SetSFD(MyDebug::SrouceFileDebugger* sfd) noexcept
{
	m_sfd = sfd;
}

/*
Funtionality: helper function to throw exception with a specific message
*/

inline void Parser::Error(const std::string& msg)
{
	throw MyExceptions::MsgExecption(msg);
}

/*
Funtionality: helper function to throw exception with a specific message
*/

inline void Parser::ErrorSFD(const std::string& msg)
{
	throw MyExceptions::MsgExecption(msg, m_sfd, m_CurrentToken->GetPos());
}

/*
Functionality: Simply call GetNextToken
Return: current token equals __EOF___
*/

inline bool Parser::ConsumeToken()
{
	m_CurrentToken = m_lexer->GetNextToken();
	return m_CurrentToken->GetType() != __EOF__;
}

/*
Functionality: Simply call GetNextToken with type matching
Return: current token equals __EOF___
*/

inline bool Parser::ConsumeTokenType(std::string type)
{

	if (m_CurrentToken->GetType() == type)
	{
		m_CurrentToken = m_lexer->GetNextToken();
		return m_CurrentToken->GetType() != __EOF__;
	}
	else
	{
		ErrorSFD("SynatxError(parser): should comsume " + type + ", instead, comsuming " + m_CurrentToken->ToString());
		return false;
	}
}

/*
Functionality: Simply call GetNextToken with type matching
Return: current token equals __EOF___
*/

inline bool Parser::TryConsumeTokenType(std::string type)
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

/*
Functionality: Simply call GetNextToken and return type
Return: next token type
*/

inline std::string Parser::PeekNextConsumeTokenType()
{

	return m_lexer->GetNextToken()->GetType();
}

/*
program: PROGRAM variable SEMI Block DOT
*/

inline SHARE_AST Parser::GetProgram()
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

inline SHARE_AST Parser::GetProcedure()
{
	ConsumeTokenType(PROCEDURE);
	auto programName = GetVariable(CALL_ID);
	auto params = GetParamsDecal();
	ConsumeTokenType(SEMI);

	auto block = GetBlock();

	return MAKE_SHARE_PROCEDURE_AST(programName, params, block);
}

/*
Block: Declaration  compound_statement
*/

inline SHARE_AST Parser::GetBlock()
{
	auto decal = GetDeclaration();
	auto comp = GetCompoundStatements();
	return MAKE_SHARE_BLOCK_AST(decal, comp);
}

/*
Declaration: Empty | (variable_declaration SEMI+ variable_declaration)
*/

inline SHARE_AST Parser::GetParamsDecal()
{
	CREATE_SHARE_DECLARATION_AST(results);
	ConsumeTokenType(LEFT_PARATHESES);
	while (m_CurrentToken->GetType() == ID)
	{
		results->AddVarDecal(GetVariableDeclaration());
		if (TryConsumeTokenType(SEMI))
			continue;
		else
			break;
	}
	ConsumeTokenType(RIGHT_PARATHESES);
	return (results->IsEmpty()) ? GetEmpty() : results;
}

/*
Assignment: Empty | (assignment_statement ,+assignment_statement)
*/

inline SHARE_AST Parser::GetParamsAssigment()
{
	ConsumeTokenType(LEFT_PARATHESES);

	SHARE_AST result = GetStatement();
	std::vector<SHARE_AST> result_list;
	result_list.push_back(result);

	if (dynamic_pointer_cast<Empty_AST>(result))
	{
		ConsumeTokenType(RIGHT_PARATHESES);
		return GetEmpty();
	}
	else
	{
		while (TryConsumeTokenType(COMMA))
		{
			result = GetStatement();
			result_list.push_back(result);
		}
		ConsumeTokenType(RIGHT_PARATHESES);

		CREATE_SHARE_COMPOUND_AST(root);
		for (auto& r : result_list)
		{
			root->AddStatements(r);
		}
		return root;
	}
}

/*
Declaration: Empty | VAR(variable_declaration SEMI)+ | PROCEDURE(parameter_declaration) SEMI+
*/

inline SHARE_AST Parser::GetDeclaration()
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

	return (results->IsEmpty()) ? GetEmpty() : results;
}

/*
variable declaration: ID (COMMA ID)* COLON type_spec
*/

inline SHARE_AST Parser::GetVariableDeclaration()
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

inline SHARE_AST Parser::GetTypeSpec()
{
	auto token = m_CurrentToken;
	ConsumeTokenType(TYPE);
	return MAKE_SHARE_AST(token);
}

/*
compound_statement: BEGIN statement_list END
*/

inline SHARE_AST Parser::GetCompoundStatements()
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

inline vector<SHARE_AST> Parser::GetStatementsList()
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

inline SHARE_AST Parser::GetStatement()
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
		return GetExpr();
	}
}

/*
assignment_statement : variable ASSIGN expr
*/

inline SHARE_AST Parser::GetAssignStatement()
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

inline SHARE_AST Parser::GetVariable(std::string type)
{
	auto token = m_CurrentToken;
	ConsumeTokenType(type);
	return MAKE_SHARE_AST(token);
}

/*
An empty production
*/

inline SHARE_AST Parser::GetEmpty()
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

inline SHARE_AST Parser::GetFactor()
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
	// Handle call
	else if (token->GetType() == token_code_factor[7])
	{
		auto name = GetVariable(CALL_ID);
		auto result = GetParamsAssigment();
		return MAKE_SHARE_PROCEDURE_AST(name, result, GetEmpty());
	}
	else
	{
		//ErrorSFD("SynatxError(parser): unknown factor: " + token->GetType() + ".");
		return GetEmpty();
	}
}

/*
2nd level of the Int Op expression, middle precedence:
Handles integer mul/div
*/

inline SHARE_AST Parser::GetTerm()
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

inline SHARE_AST Parser::GetExpr()
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

/*
Functionality: parse the input text (should be a program format) into AST
Return: pointer constant to the AST instance
*/

SHARE_AST Parser::GetProgramAST()
{
	m_pAST = GetProgram();
	return m_pAST;
}
