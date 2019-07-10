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
	Interpreter()
	{
		m_parser = Parser();
	}
	explicit Interpreter(std::string text)
	{
		m_parser = Parser(text);
	}
	virtual ~Interpreter() {};

	void Reset()
	{
		m_parser.Reset();
	}

	void SetText(std::string text)
	{
		m_parser.SetText(text);
	}

	void PrintVaribalesMap() const noexcept
	{
		std::cout << "VaribalesMap contains:\n";
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
		default:
			Error("SyntaxError: " + op->ToString() + " is an UNKNOWN integer operation.\n");
		}
	}

public:

	/*
Functionality: an combnination of parsing and interpreting
Return: InterpretProgram
*/
	std::string InterpretProgram()
	{
		auto root = m_parser.GetProgramAST();
		auto result = InterpretProgramHelper(root);
		return *(result->GetValue());
	}


private:
	SHARE_TOKEN_STRING InterpretProgramHelper(SHARE_AST root)
	{
		if (!root)
			Error("SyntaxError: root of AST is null.");

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
			GLOBAL_SCOPE.insert(TOKEN_STRING_PAIR(root_4->GetVarName(), InterpretProgramHelper(root_4->GetRight())));
		}
		// Condition: is a variable/static
		else
		{
			DEBUG_RUN(PrintVaribalesMap());

			auto type = root->GetToken()->GetType();
			auto var = root->GetToken()->ToString();
			// is variable
			if (type == ID)
			{
				if (GLOBAL_SCOPE.find(var) != GLOBAL_SCOPE.end())
				{
					return GLOBAL_SCOPE.at(var);
				}
				else
				{
					Error("NameError(Interpreter): unknown variable '" + var + "'.");
				}
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
	Parser m_parser;
	TOKEN_STRING_MAP GLOBAL_SCOPE;
};

