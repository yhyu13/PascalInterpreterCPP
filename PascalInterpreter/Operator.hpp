#pragma once
#include "Token.hpp"

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
	SHARE_TOKEN_STRING exprBinaryDeciamlNumOp(SHARE_TOKEN_STRING left, SHARE_TOKEN_STRING right, SHARE_TOKEN_STRING op);

};