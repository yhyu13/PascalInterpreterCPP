#include "Lexer.hpp"

void Lexer::Reset() noexcept
{
	m_text = "";
	m_pos = 0;
	m_CurrentChar = '\0';
	m_sfd = nullptr;
}

void Lexer::SetText(std::string text) noexcept
{
	m_text = text;
	m_pos = 0;
	m_CurrentChar = m_text[m_pos];
}

void Lexer::SetSFD(MyDebug::SrouceFileDebugger* sfd) noexcept
{
	m_sfd = sfd;
}

void Lexer::advance_currentChar()
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

char Lexer::peek_nextChar()
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

void Lexer::skip_white_space()
{
	while ((std::isspace(m_CurrentChar) || m_CurrentChar == '\n') && m_CurrentChar != '\0')
	{
		advance_currentChar();
	}
}

void Lexer::skip_comment()
{
	char end = (m_CurrentChar == '#') ? '\n' : '}';
	while (m_CurrentChar != end && m_CurrentChar != '\0')
	{
		advance_currentChar();
	}
	advance_currentChar();
}

SHARE_TOKEN_STRING Lexer::GetDecimalNumberToken()
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

	return (bDecimal) ? MAKE_SHARE_TOKEN(FLOAT, MAKE_SHARE_STRING(charBuffer), _pos) :
		MAKE_SHARE_TOKEN(INTEGER, MAKE_SHARE_STRING(charBuffer), _pos);
}

SHARE_TOKEN_STRING Lexer::GetIdToken()
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

SHARE_TOKEN_STRING Lexer::GetNextToken()
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
				return MAKE_SHARE_TOKEN(ASSIGN, MAKE_SHARE_STRING(":="), m_pos - 2);
			}
			else
			{
				advance_currentChar();
				return MAKE_SHARE_TOKEN(COLON, MAKE_SHARE_STRING(":"), m_pos - 1);
			}
		}
		else if (m_CurrentChar == ',')
		{
			advance_currentChar();
			return MAKE_SHARE_TOKEN(COMMA, MAKE_SHARE_STRING(","), m_pos - 1);
		}
		// Semi colon segements statements
		else if (m_CurrentChar == ';')
		{
			advance_currentChar();
			return MAKE_SHARE_TOKEN(SEMI, MAKE_SHARE_STRING(";"), m_pos - 1);
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
				return MAKE_SHARE_TOKEN(DOT, MAKE_SHARE_STRING("."), m_pos - 1);
			}
		}
		else if (m_CurrentChar == '+')
		{
			advance_currentChar();
			return MAKE_SHARE_TOKEN(PLUS, MAKE_SHARE_STRING("+"), m_pos - 1);
		}
		else if (m_CurrentChar == '-')
		{
			advance_currentChar();
			return MAKE_SHARE_TOKEN(MINUS, MAKE_SHARE_STRING("-"), m_pos - 1);
		}
		else if (m_CurrentChar == '*')
		{
			advance_currentChar();
			return MAKE_SHARE_TOKEN(MUL, MAKE_SHARE_STRING("*"), m_pos - 1);
		}
		else if (m_CurrentChar == '/')
		{
			// '//' indicates integer division
			if (peek_nextChar() == '/')
			{
				advance_currentChar();
				advance_currentChar();
				return MAKE_SHARE_TOKEN(INT_DIV, MAKE_SHARE_STRING("//"), m_pos - 2);
			}
			else
			{
				advance_currentChar();
				return MAKE_SHARE_TOKEN(DIV, MAKE_SHARE_STRING("/"), m_pos - 1);
			}
		}
		else if (m_CurrentChar == '(')
		{
			advance_currentChar();
			return MAKE_SHARE_TOKEN(LEFT_PARATHESES, MAKE_SHARE_STRING("("), m_pos - 1);
		}
		else if (m_CurrentChar == ')')
		{
			advance_currentChar();
			return MAKE_SHARE_TOKEN(RIGHT_PARATHESES, MAKE_SHARE_STRING(")"), m_pos - 1);
		}
		// No known token returned, raise excpetion.
		else
		{
			ErrorSFD("NameError(lexer): unknown keyword: '" + MyTemplates::Str(m_CurrentChar) + "'.");
		}
	}
	// Reaching EOF
	return MAKE_SHARE_TOKEN(__EOF__, MAKE_SHARE_STRING("\0"), m_pos - 1);
}