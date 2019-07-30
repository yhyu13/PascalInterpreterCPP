/*
AST Parser Interpreter
*/


#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <map>

#include "Parser.hpp"
#include "Operator.hpp"

class Interpreter
{
public:
	Interpreter(): m_sfd(nullptr) {};
	virtual ~Interpreter() {};
	
	void Reset() noexcept
	{
		m_symbolTable.Reset();
		m_sfd = nullptr;
	}

	void SetSFD(MyDebug::SrouceFileDebugger* sfd) noexcept
	{
		m_sfd = sfd;
	}

	void PrintVaribalesMap() const noexcept
	{
		std::cout << "GLOBAL_SCOPE contains---> {\n";
		for (auto it = GLOBAL_SCOPE.begin(); it != GLOBAL_SCOPE.end(); it++)
			std::cout << it->first << " => " << it->second->ToString() << '\n';
		std::cout << '}' << std::endl;
	}

private:
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
	inline void ErrorSFD(const std::string& msg, unsigned int pos)
	{
		throw MyExceptions::MsgExecption(msg, m_sfd, pos);
	}

public:
	/*
Functionality: an combnination of parsing and interpreting
Return: InterpretProgram
*/
	void InterpretProgram(SHARE_AST root)
	{
		InterpretProgramEntryHelper(root);
	}

private:
	SHARE_TOKEN_STRING InterpretProgramEntryHelper(SHARE_AST root)
	{
		if (!root)
			Error("ASTError(Interpreter): root of InterpretProgramEntryHelper is null.");

		// Condition: is a program start
		if (SHARE_PROGRAM_AST root_0 = dynamic_pointer_cast<Program_AST>(root))
		{
			DEBUG_MSG("Running program---> " + root_0->GetName());
			return InterpretProgramEntryHelper(root_0->GetBlock());
		}
		// Condition: is a block right after the program start
		else if (SHARE_BLOCk_AST root_2 = dynamic_pointer_cast<Block_AST>(root))
		{
			// Process declarations.
			if (SHARE_DECLARATION_AST declaration = dynamic_pointer_cast<Declaration_AST>(root_2->GetDeclaration()))
			{
				for (SHARE_AST& decal : declaration->GetAllChildren())
				{
					// Condition: is a variable declaration
					if (SHARE_DECLCONTAINER_AST _declConatiner = dynamic_pointer_cast<DeclContainer_AST>(decal))
					{
						for (SHARE_AST& varDecal : _declConatiner->GetAllChildren())
						{
							if (SHARE_VARDECL_AST _varDecal = dynamic_pointer_cast<VarDecl_AST>(varDecal))
							{
								std::string type = _varDecal->GetTypeString();
								std::string name = _varDecal->GetVarString();
								m_symbolTable.define(VarSymbol(name, BuiltInTypeSymbol(type)));
							}
							else
							{
								Error("SyntaxError(Interpreter): unknown variable declaration");
							}
						}
						DEBUG_RUN(m_symbolTable.PrintTable());
					}
					// Condition: is a procedure start
					else if (SHARE_PROCEDURE_AST _procedure = dynamic_pointer_cast<Procedure_AST>(decal))
					{						
						DEBUG_MSG("Running procedure---> " + _procedure->GetName());
						InterpretProgramEntryHelper(_procedure->GetBlock());
					}
					else
					{
						Error("ASTError(Interpreter): unknown declaration");
					}
				}
			}
			else
			{
				//DEBUG_MSG("Has no declaration.");
			}
			// Process the rest of the program.
			return InterpretProgramHelper(root_2->GetCompound());
		}
		else
		{
			Error("SyntaxError(Interpreter): program entry not defined");
		}
	}

	SHARE_TOKEN_STRING InterpretProgramHelper(SHARE_AST root)
	{
		if (!root)
			Error("ASTError(Interpreter): root of InterpretProgramHelper is null.");

		// Condition: is a compound statment
		if (SHARE_COMPOUND_AST root_0 = dynamic_pointer_cast<Compound_AST>(root))
		{
			for (auto& child : root_0->GetAllChildren())
			{
				DEBUG_MSG("Running statements list---> " + child->ToString());
				InterpretProgramHelper(child);
			}
		}
		// Condition: is a binary operation
		else if (SHARE_BINARY_AST root_1 = dynamic_pointer_cast<BinaryOp_AST>(root))
		{
			SHARE_TOKEN_STRING left = InterpretProgramHelper(root_1->GetLeft());
			SHARE_TOKEN_STRING right = InterpretProgramHelper(root_1->GetRight());
			SHARE_TOKEN_STRING op = InterpretProgramHelper(root_1->GetOp());
			DEBUG_MSG("Running binary operation---> Left: " + left->ToString() + " Right: " + right->ToString() + " OP: " + op->ToString());
			return m_opeartor.exprBinaryDeciamlNumOp(left, right, op);
		}
		// Condition: is a unary operation
		else if (SHARE_UNARY_AST root_2 = dynamic_pointer_cast<UnaryOp_AST>(root))
		{
			SHARE_TOKEN_STRING result = InterpretProgramHelper(root_2->GetExpr());
			if (result->GetType() == INTEGER)
			{
				// e.g when "---+++1" as a input, the interpreter receives it as reversed by how the parser works.
				// so '1' is the first input, and all '+' signs are ignored by purpose
				// we then iterativly resolve all '-' sign to get the correct expression, which is '-1'
				DEBUG_MSG("Before Unary handle---> " + result->ToString());
				if (root_2->GetToken()->GetType() == MINUS)
				{
					if (result->GetValue()->front() == '-')
					{
						std::string s(1, result->GetValue()->back());
						result = MAKE_SHARE_TOKEN(INTEGER, MAKE_SHARE_STRING(s), result->GetPos());
					}
					else
					{
						std::string s(*(result->GetValue()));
						s.insert(0, "-");
						result = MAKE_SHARE_TOKEN(INTEGER, MAKE_SHARE_STRING(s), result->GetPos());
					}
				}
				DEBUG_MSG("After Unary handle---> " + result->ToString());
			} 
			else
			{
				ErrorSFD("ASTError(Interpreter): " + root_2->ToString() + " on " + result->ToString() + " is not valid.",root_2->GetToken()->GetPos());
			}
			return result;
		}
		// Condition: is a empty statement
		else if (SHARE_EMPTY_AST root_3 = dynamic_pointer_cast<Empty_AST>(root))
		{
			return MAKE_SHARE_TOKEN(EMPTY, MAKE_SHARE_STRING("\0"),0);
		}
		// Condition: is a assign statement
		else if (SHARE_ASSIGN_AST root_4 = dynamic_pointer_cast<Assign_AST>(root))
		{
			std::string varname = root_4->GetVarName();
			m_symbolTable.lookup(varname);
			auto rhs = InterpretProgramHelper(root_4->GetRight());
			m_symbolTable.check(varname, rhs->GetType());
			GLOBAL_SCOPE[root_4->GetVarName()] = InterpretProgramHelper(root_4->GetRight());
		}
		// Condition: is a variable/static
		else
		{
			std::string type = root->GetToken()->GetType();
			
			// is variable
			if (type == ID)
			{
				DEBUG_RUN(PrintVaribalesMap());
				std::string name = *(root->GetToken()->GetValue());
				m_symbolTable.lookup(name);
				if (GLOBAL_SCOPE.find(name) != GLOBAL_SCOPE.end())
				{
					return GLOBAL_SCOPE.at(name);
				}
				else
				{
					ErrorSFD("SymbolError(Interpreter): variable " + name + " used before reference.", root->GetToken()->GetPos());
				}
			}
			// is a type declaration
			else if (type == TYPE)
			{
				// TODO: change code below
				return root->GetToken();
			}
			// is static
			else
			{
				return root->GetToken();
			}
		}

		return MAKE_SHARE_TOKEN(EMPTY, MAKE_SHARE_STRING("\0"),0);
	}

private:
	TOKEN_STRING_MAP GLOBAL_SCOPE;
	SymbolTable m_symbolTable;
	Operator m_opeartor;

	MyDebug::SrouceFileDebugger* m_sfd;
};

