#pragma once
#include <string>
#include "MyExceptions.hpp"
#include "MyMacros.hpp"
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

	void Reset() noexcept
	{
		m_text = "";
		m_pos = 0;
		m_CurrentChar = '\0';
		m_sfd = nullptr;
	}

	void SetText(std::string text) noexcept
	{
		m_text = text;
		m_pos = 0;
		m_CurrentChar = m_text[m_pos];
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
	Funtionality: helper function to throw exception with a specific message and source file location
	*/
	inline void ErrorSFD(const std::string& msg)
	{
		throw MyExceptions::MsgExecption(msg, m_sfd, m_pos);
	}

	/*
	Funtionality: helper function to advance m_CurrentChar
	*/
	inline void advance_currentChar()
	{
		++m_pos;
		if (m_pos > (m_text.size() - 1))
		{
			m_CurrentChar = '\0';
		}
		else
		{
			m_CurrentChar = m_text[m_pos];
		}
	}

	/*
	Funtionality: helper function to advance m_CurrentChar
	*/
	char peek_nextChar()
	{
		auto peek_pos = m_pos + 1;
		if (peek_pos > (m_text.size() - 1))
		{
			return '\0';
		}
		else
		{
			return m_text[peek_pos];
		}
	}

	/*
	Funtionality: helper function to skip whitespace in m_text
	*/
	inline void skip_white_space()
	{
		while ((std::isspace(m_CurrentChar) || m_CurrentChar == '\n') && m_CurrentChar != '\0')
		{
			advance_currentChar();
		}
	}

	/*
	Funcationality: helper function to skip comment
	*/
	inline void skip_comment()
	{
		char end = (m_CurrentChar == '#') ? '\n' : '}';
		while (m_CurrentChar != end && m_CurrentChar != '\0')
		{
			advance_currentChar();
		}
		advance_currentChar();
	}

	/*
	Funtionality: get multi-digits integer from string
	Return: Integer Token
	*/
	SHARE_TOKEN_STRING GetDecimalNumberToken()
	{
		std::string charBuffer = "";
		bool bDecimal = false;
		unsigned int _pos = m_pos;

		if (m_CurrentChar == '.')
			charBuffer == "0";

		while ((std::isdigit(m_CurrentChar) || m_CurrentChar == '.') && m_CurrentChar != '\0')
		{
			if (m_CurrentChar == '.')
			{
				if (bDecimal)
					ErrorSFD("SynatxError(lexer): multiple decimals in number.");
				else
					bDecimal = true;
			}

			charBuffer += MyTemplates::Str(m_CurrentChar);
			advance_currentChar();
		}

		return (bDecimal)? MAKE_SHARE_TOKEN(FLOAT, MAKE_SHARE_STRING(charBuffer), _pos) :
			MAKE_SHARE_TOKEN(INTEGER, MAKE_SHARE_STRING(charBuffer), _pos);
	}

	/*
	Functinality: get varibale name or reserverd keywords
	Return: 
	*/
	SHARE_TOKEN_STRING GetIdToken()
	{
		SHARE_TOKEN_STRING result;
		std::string charBuffer = "";
		unsigned int _pos = m_pos;
		while ((m_CurrentChar == '_' || std::isalnum(m_CurrentChar)) && m_CurrentChar != '\0')
		{
			charBuffer += MyTemplates::Str(m_CurrentChar);
			advance_currentChar();
		}

		if (ITEM_IN_VEC(charBuffer, reserverd_keywords))
		{
			result = MAKE_SHARE_TOKEN(charBuffer, MAKE_SHARE_STRING(charBuffer), _pos);
		}
		else if (ITEM_IN_VEC(charBuffer, type_keywords))
		{
			result = MAKE_SHARE_TOKEN(TYPE, MAKE_SHARE_STRING(charBuffer), _pos);
		}
		else if (m_CurrentChar == '(')
		{
			result = MAKE_SHARE_TOKEN(CALL_ID, MAKE_SHARE_STRING(charBuffer), _pos);
		}
		else
		{
			result = MAKE_SHARE_TOKEN(ID, MAKE_SHARE_STRING(charBuffer), _pos);
		}
		return result;
	}


public:
	/*
	Funtionality: tokenize m_text
	Return: an known Token. Otherwise, throw an exception
	*/
	SHARE_TOKEN_STRING GetNextToken()
	{
		while (m_CurrentChar != '\0')
		{
			// Skip white spaces and change line
			if (std::isspace(m_CurrentChar) || m_CurrentChar == '\n')
			{
				skip_white_space();
				continue;
			}
			// Skip comment starts with '#' and all the way to the end of a line, or starts with '{' and end with '}'
			if (m_CurrentChar == '#' || m_CurrentChar == '{')
			{
				skip_comment();
				continue;
			}
			// Get statics integer/float token
			else if (std::isdigit(m_CurrentChar))
			{
				return GetDecimalNumberToken();
			}
			// Variables must start with a undersocre or a alphabet
			else if ((m_CurrentChar == '_' || std::isalpha(m_CurrentChar)))
			{
				return GetIdToken();
			}
			else if (m_CurrentChar == ':')
			{
				// Variable assignment be like 'a := 1.0'
				if (peek_nextChar() == '=')
				{
					advance_currentChar();
					advance_currentChar();
					return MAKE_SHARE_TOKEN(ASSIGN, MAKE_SHARE_STRING(":="),m_pos-2);
				}
				else
				{
					advance_currentChar();
					return MAKE_SHARE_TOKEN(COLON, MAKE_SHARE_STRING(":"), m_pos-1);
				}
			}
			else if (m_CurrentChar == ',')
			{
				advance_currentChar();
				return MAKE_SHARE_TOKEN(COMMA, MAKE_SHARE_STRING(","), m_pos-1);
			}
			// Semi colon segements statements
			else if (m_CurrentChar == ';')
			{
				advance_currentChar();
				return MAKE_SHARE_TOKEN(SEMI, MAKE_SHARE_STRING(";"), m_pos-1);
			}
			// Every program ends with a '.'
			else if (m_CurrentChar == '.')
			{
				// a = .4 is a vaild assignment, a is now equals to 0.4
				if (std::isdigit(peek_nextChar()))
				{
					return GetDecimalNumberToken();
				}
				else
				{
					advance_currentChar();
					return MAKE_SHARE_TOKEN(DOT, MAKE_SHARE_STRING("."), m_pos-1);
				}
			}
			else if (m_CurrentChar == '+')
			{
				advance_currentChar();
				return MAKE_SHARE_TOKEN(PLUS, MAKE_SHARE_STRING("+"), m_pos-1);
			}
			else if (m_CurrentChar == '-')
			{
				advance_currentChar();
				return MAKE_SHARE_TOKEN(MINUS, MAKE_SHARE_STRING("-"), m_pos-1);
			}
			else if (m_CurrentChar == '*')
			{
				advance_currentChar();
				return MAKE_SHARE_TOKEN(MUL, MAKE_SHARE_STRING("*"), m_pos-1);
			}
			else if (m_CurrentChar == '/')
			{
				// '//' indicates integer division
				if (peek_nextChar() == '/')
				{
					advance_currentChar();
					advance_currentChar();
					return MAKE_SHARE_TOKEN(INT_DIV, MAKE_SHARE_STRING("//"), m_pos-2);
				}
				else
				{
					advance_currentChar();
					return MAKE_SHARE_TOKEN(DIV, MAKE_SHARE_STRING("/"), m_pos-1);
				}
			}
			else if (m_CurrentChar == '(')
			{
				advance_currentChar();
				return MAKE_SHARE_TOKEN(LEFT_PARATHESES, MAKE_SHARE_STRING("("), m_pos-1);
			}
			else if (m_CurrentChar == ')')
			{
				advance_currentChar();
				return MAKE_SHARE_TOKEN(RIGHT_PARATHESES, MAKE_SHARE_STRING(")"), m_pos-1);
			}
			// No known token returned, raise excpetion.
			else
			{
				ErrorSFD("NameError(lexer): unknown keyword: '" + MyTemplates::Str(m_CurrentChar) + "'.");
			}
		}
		// Reaching EOF
		return MAKE_SHARE_TOKEN(__EOF__, MAKE_SHARE_STRING("\0"), m_pos-1);
	}

private:
	std::string m_text;
	unsigned int m_pos;
	char m_CurrentChar;
	std::vector<std::string> reserverd_keywords = { BEGIN , END , PROGRAM, PROCEDURE, VAR};
	std::vector<std::string> type_keywords = { INTEGER, FLOAT };

	MyDebug::SrouceFileDebugger* m_sfd;
};