#pragma once
#include <string>
#include "Token.hpp"

using namespace std;

class Lexer
{
public:
	Lexer()
		:
		m_text("\0"),
		m_pos(0),
		m_CurrentChar('\0'),
		m_sfd(nullptr)
	{}
	virtual ~Lexer() {};

	void Reset() noexcept;
	void SetText(std::string text) noexcept;
	void SetSFD(MyDebug::SrouceFileDebugger* sfd) noexcept;

protected:
	/*
	Funtionality: helper function to throw exception with a specific message
	*/
	inline void Error(const std::string& msg)
	{
		throw MyExceptions::MsgExecption(msg);
	}

	/*
	Funtionality: helper function to throw exception with a specific message and source file location
	*/
	inline void ErrorSFD(const std::string& msg)
	{
		throw MyExceptions::MsgExecption(msg, m_sfd, m_pos);
	}

	/*
	Funtionality: helper function to advance m_CurrentChar
	*/
	void advance_currentChar();

	/*
	Funtionality: helper function to advance m_CurrentChar
	*/
	char peek_nextChar();

	/*
	Funtionality: helper function to skip whitespace in m_text
	*/
	void skip_white_space();

	/*
	Funcationality: helper function to skip comment
	*/
	void skip_comment();

	/*
	Funtionality: get multi-digits integer from string
	Return: Integer Token
	*/
	SHARE_TOKEN_STRING GetDecimalNumberToken();

	/*
	Functinality: get varibale name or reserverd keywords
	Return: 
	*/
	SHARE_TOKEN_STRING GetIdToken();


public:
	/*
	Funtionality: tokenize m_text
	Return: an known Token. Otherwise, throw an exception
	*/
	SHARE_TOKEN_STRING GetNextToken();

private:
	std::string m_text;
	unsigned int m_pos;
	char m_CurrentChar;
	std::vector<std::string> reserverd_keywords = { BEGIN , END , PROGRAM, PROCEDURE, VAR};
	std::vector<std::string> type_keywords = { INTEGER, FLOAT };

	MyDebug::SrouceFileDebugger* m_sfd;
};