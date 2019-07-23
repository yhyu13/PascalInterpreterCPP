/*
AST Parser Interpreter
*/


#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <map>

#include "Parser.hpp"

class Interpreter
{
public:
	Interpreter() {};
	virtual ~Interpreter() {};
	
	void Reset() noexcept
	{
		m_symbolTable.Reset();
	}

	void PrintVaribalesMap() const noexcept
	{
		std::cout << "GLOBAL_SCOPE contains:\n";
		for (auto it = GLOBAL_SCOPE.begin(); it != GLOBAL_SCOPE.end(); it++)
			std::cout << it->first << " => " << it->second->ToString() << '\n';
	}

private:
	/*
	Funtionality: helper function to throw exception with a specific message
	*/
	inline void Error(const std::string& msg)
	{
		throw MyExceptions::InterpreterExecption(msg);
	}

	/*
	Token related helper functions
	*/

	/*
	Functionality: express a basic interger binary operation
	Return: calculated result in string type
	*/
	SHARE_TOKEN_STRING exprBinaryDeciamlNumOp(SHARE_TOKEN_STRING left, SHARE_TOKEN_STRING right, SHARE_TOKEN_STRING op)
	{

		
		if (left->GetType() != INTEGER && right->GetType() != INTEGER && left->GetType() != FLOAT && right->GetType() != FLOAT)
		{
			Error("SyntaxError: " + left->ToString() + " or " + right->ToString() + " is an not a integer/float or both.\n");
		}

		bool isInt = false;
		float _left = std::stof(*(left->GetValue()));
		float _right = std::stof(*(right->GetValue()));
		if (left->GetType() == INTEGER && right->GetType() == INTEGER)
			isInt = true;

		switch (GetEnumNumOp(op->GetType()))
		{
		case ePLUS:
			return (isInt) ? MAKE_SHARE_TOKEN(INTEGER, MAKE_SHARE_STRING(MyTemplates::Str((int)_left + (int)_right))) :
				MAKE_SHARE_TOKEN(FLOAT, MAKE_SHARE_STRING(MyTemplates::Str(_left + _right)));
		case eMINUS:
			return (isInt) ? MAKE_SHARE_TOKEN(INTEGER, MAKE_SHARE_STRING(MyTemplates::Str((int)_left - (int)_right))) :
				MAKE_SHARE_TOKEN(FLOAT, MAKE_SHARE_STRING(MyTemplates::Str(_left - _right)));
		case eMULTIPLY:
			return (isInt) ? MAKE_SHARE_TOKEN(INTEGER, MAKE_SHARE_STRING(MyTemplates::Str((int)_left * (int)_right))) :
				MAKE_SHARE_TOKEN(FLOAT, MAKE_SHARE_STRING(MyTemplates::Str(_left * _right)));
		case eDIVIDE:
			if (_right != 0)
			{
				return (isInt) ? MAKE_SHARE_TOKEN(INTEGER, MAKE_SHARE_STRING(MyTemplates::Str((int)_left / (int)_right))) :
					MAKE_SHARE_TOKEN(FLOAT, MAKE_SHARE_STRING(MyTemplates::Str(_left / _right)));
			}
			else
			{
				Error("SyntaxError: Decimal number division by zero.");
			}
		case eINT_DIV:
			if (_right != 0)
			{
				if (!isInt)
					Error("SyntaxError: integer devision applied to non-integer type.");
				return MAKE_SHARE_TOKEN(INTEGER, MAKE_SHARE_STRING(MyTemplates::Str((int)_left / (int)_right)));
			}
			else
			{
				Error("SyntaxError: Decimal number division by zero.");
			}
		default:
			Error("SyntaxError: " + op->ToString() + " is an UNKNOWN integer operation.\n");
		}
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
			Error("SyntaxError(Interpreter): root of InterpretProgramEntreHelper is null.");

		// Condition: is a program start
		if (SHARE_PROGRAM_AST root_1 = dynamic_pointer_cast<Program_AST>(root))
		{
			DEBUG_MSG("Running program: " + root_1->GetName());
			return InterpretProgramEntryHelper(root_1->GetBlock());
		}
		// Condition: is a block right after the program start
		else if (SHARE_BLOCk_AST root_2 = dynamic_pointer_cast<Block_AST>(root))
		{
			// Process declarations.
			if (SHARE_DECLARATION_AST declaration = dynamic_pointer_cast<Declaration_AST>(root_2->GetDeclaration()))
			{
				for (SHARE_AST& decalConatiner : declaration->GetAllChildren())
				{
					SHARE_DECLCONTAINER_AST _declConatiner = static_pointer_cast<DeclContainer_AST>(decalConatiner);
					for (SHARE_AST& decal : _declConatiner->GetAllChildren())
					{
						if (SHARE_VARDECL_AST _varDecal = dynamic_pointer_cast<VarDecl_AST>(decal))
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
				}
				DEBUG_RUN(m_symbolTable.PrintTable());
			}
			else
			{
				DEBUG_MSG("Program has no declaration.");
			}
			// Process the rest of the program.
			return InterpretProgramHelper(root_2->GetCompound());
		}
		else
		{
			Error("SyntaxError(Interpreter): entry not defined");
		}
	}

	SHARE_TOKEN_STRING InterpretProgramHelper(SHARE_AST root)
	{
		if (!root)
			Error("SyntaxError(Interpreter): root of InterpretProgramHelper is null.");

		// Condition: is a compound statment
		if (SHARE_COMPOUND_AST root_0 = dynamic_pointer_cast<Compound_AST>(root))
		{
			for (auto& child : root_0->GetAllChildren())
			{
				DEBUG_MSG(child->ToString());
				InterpretProgramHelper(child);
			}
		}
		// Condition: is a binary operation
		else if (SHARE_BINARY_AST root_1 = dynamic_pointer_cast<BinaryOp_AST>(root))
		{
			SHARE_TOKEN_STRING left = InterpretProgramHelper(root_1->GetLeft());
			SHARE_TOKEN_STRING right = InterpretProgramHelper(root_1->GetRight());
			SHARE_TOKEN_STRING op = InterpretProgramHelper(root_1->GetOp());
			DEBUG_MSG("Left: " + left->ToString() + " Right: " + right->ToString() + " OP: " + op->ToString());
			return exprBinaryDeciamlNumOp(left, right, op);
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
				DEBUG_MSG("Before Unary handle: " + result->ToString());
				if (root_2->GetToken()->GetType() == MINUS)
				{
					if (result->GetValue()->front() == '-')
					{
						std::string s(1, result->GetValue()->back());
						result = MAKE_SHARE_TOKEN(INTEGER, MAKE_SHARE_STRING(s));
					}
					else
					{
						std::string s(*(result->GetValue()));
						s.insert(0, "-");
						result = MAKE_SHARE_TOKEN(INTEGER, MAKE_SHARE_STRING(s));
					}
				}
				DEBUG_MSG("After Unary handle: " + result->ToString());
			} 
			else
			{
				Error(root_2->ToString() + " on " + result->ToString() + " is not valid.");
			}
			return result;
		}
		// Condition: is a empty statement
		else if (SHARE_EMPTY_AST root_3 = dynamic_pointer_cast<Empty_Op>(root))
		{
			return MAKE_SHARE_TOKEN(EMPTY, MAKE_SHARE_STRING("\0"));
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
					Error("NameError(Interpreter): variable " + name + " does not have a assigned value.");
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

		return MAKE_SHARE_TOKEN(EMPTY, MAKE_SHARE_STRING("\0"));
	}

private:
	TOKEN_STRING_MAP GLOBAL_SCOPE;
	SymbolTable m_symbolTable;
};

