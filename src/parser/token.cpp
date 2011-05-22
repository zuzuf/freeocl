#include "token.h"
#include "native_type.h"

namespace FreeOCL
{
	Token::Token(const std::string &str, const int tokenID)
		: tokenID(tokenID),
		str(str)
	{
	}

	Token::~Token()
	{
	}

	smartptr<Type> Token::getType() const
	{
		return NativeType::t_void;
	}

	void Token::write(std::ostream& out) const
	{
		switch(tokenID)
		{
		case '{':
		case '}':
			out << std::endl;
		case ';':
		case ':':
			out << str << std::endl;
			break;
		default:
			out	<< str << ' ';
		}
	}
}
