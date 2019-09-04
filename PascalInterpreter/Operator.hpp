#pragma once

#include <iostream>
#include <sstream>
#include <string>

#include "MyMacros.hpp"

class Operator
{
public:
	Operator() {};
	virtual ~Operator() {};

	/*
	Funtionality: helper function to throw exception with a specific message
	*/
	inline void Error(const std::string& msg)
	{
		throw MyExceptions::MsgExecption(msg);
	}

	/*
	Functionality: express a basic interger binary operation
	Return: calculated result in string type
	*/
	SHARE_TOKEN_STRING exprBinaryDeciamlNumOp(SHARE_TOKEN_STRING left, SHARE_TOKEN_STRING right, SHARE_TOKEN_STRING op)
	{


		if (left->GetType() != INTEGER && right->GetType() != INTEGER && left->GetType() != FLOAT && right->GetType() != FLOAT)
		{
			Error("SyntaxError: " + left->ToString() + " or " + right->ToString() + " is an not a integer/float or both.\n");
			return MAKE_EMPTY_MEMORY;
		}

		bool isInt = false;
		float _left = std::stof(*(left->GetValue()));
		float _right = std::stof(*(right->GetValue()));
		if (left->GetType() == INTEGER && right->GetType() == INTEGER)
			isInt = true;

		switch (GetEnumNumOp(op->GetType()))
		{
		case ePLUS:
			return (isInt) ? MAKE_SHARE_TOKEN(INTEGER, MAKE_SHARE_STRING(MyTemplates::Str((int)_left + (int)_right)), left->GetPos()) :
				MAKE_SHARE_TOKEN(FLOAT, MAKE_SHARE_STRING(MyTemplates::Str(_left + _right)), left->GetPos());
		case eMINUS:
			return (isInt) ? MAKE_SHARE_TOKEN(INTEGER, MAKE_SHARE_STRING(MyTemplates::Str((int)_left - (int)_right)), left->GetPos()) :
				MAKE_SHARE_TOKEN(FLOAT, MAKE_SHARE_STRING(MyTemplates::Str(_left - _right)), left->GetPos());
		case eMULTIPLY:
			return (isInt) ? MAKE_SHARE_TOKEN(INTEGER, MAKE_SHARE_STRING(MyTemplates::Str((int)_left * (int)_right)), left->GetPos()) :
				MAKE_SHARE_TOKEN(FLOAT, MAKE_SHARE_STRING(MyTemplates::Str(_left * _right)), left->GetPos());
		case eDIVIDE:
			if (_right != 0)
			{
				return (isInt) ? MAKE_SHARE_TOKEN(INTEGER, MAKE_SHARE_STRING(MyTemplates::Str((int)_left / (int)_right)), left->GetPos()) :
					MAKE_SHARE_TOKEN(FLOAT, MAKE_SHARE_STRING(MyTemplates::Str(_left / _right)), left->GetPos());
			}
			else
			{
				Error("SyntaxError: Decimal number division by zero.");
				return MAKE_EMPTY_MEMORY;
			}
		case eINT_DIV:
			if (_right != 0)
			{
				if (!isInt)
				{
					Error("SyntaxError: integer devision applied to non-integer type.");
					return MAKE_EMPTY_MEMORY;
				}
				else
					return MAKE_SHARE_TOKEN(INTEGER, MAKE_SHARE_STRING(MyTemplates::Str((int)_left / (int)_right)), left->GetPos());
			}
			else
			{
				Error("SyntaxError: Decimal number division by zero.");
				return MAKE_EMPTY_MEMORY;
			}
		default:
			Error("SyntaxError: " + op->ToString() + " is an UNKNOWN integer operation.\n");
			return MAKE_EMPTY_MEMORY;
		}
	}

};