#include <iostream>
#include <ctype.h>
#include <list>
#include <string>
#include <fstream>
#include <unordered_map>
#include <map>

enum TokenId : uint32_t
{
	Number,
	String,
	Identifier,
	Plus,
	Minus,
	Mul,
	Div,
	Mod,
	Pow,
	Comma,
	Quote,
	Lparen,
	Rparen,
	Equals,
	NotEquals,
	Less,
	Greater,
	Colon,
	Semicolon,
	NewLine,
	Abs,
	Sin,
	Cos,
	Tan,
	Atn,
	Sqr,
	Exp,
	Log,
	Sgn,
	Int,
	Fix,
	Let,
	Print,
	Goto
};

struct Token
{
	uint32_t nId;
	std::string sValue;
};

bool strmatch(std::string a, std::string b, int start)
{
	for (int i = 0; i < b.length(); i++)
		if (a[start + i] != b[i])
			return false;

	return true;
}

void Parse(std::list<std::string>& listInput, std::list<Token>& listTokenList, std::vector<std::vector<Token>>& vecOutput)
{
	int nCursor = 0;

	auto itLine = listInput.begin();
	std::vector<Token> vecLineOut;

	auto next_token = [&]() -> Token
	{
		std::string sNum = "";
		std::string sIdentifier = "";
		std::string sStr = "";

		while (isdigit((*itLine)[nCursor]) || (*itLine)[nCursor] == '.')
		{
			sNum += (*itLine)[nCursor++];

			if (sNum.length() > 0 && !isdigit((*itLine)[nCursor]) && (*itLine)[nCursor] != '.')
				return { Number, sNum };
		}

		if ((*itLine)[nCursor] == '"')
		{
			nCursor++;

			while ((*itLine)[nCursor] != '"')
				sStr += (*itLine)[nCursor++];

			nCursor++;
			return { String, sStr };
		}

		for (const auto& t : listTokenList)
			if (strmatch((*itLine), t.sValue, nCursor))
			{
				nCursor += t.sValue.length();
				return t;
			}

		while ((isalpha((*itLine)[nCursor]) || isdigit((*itLine)[nCursor])) && nCursor < (*itLine).length())
		{
			sIdentifier += (*itLine)[nCursor];
			nCursor++;
		}

		if (sIdentifier.length() == 0)
			nCursor++;

		return { Identifier, sIdentifier };
	};

	for (; itLine != listInput.end(); itLine++)
	{
		while (nCursor < (*itLine).length())
		{
			Token token = next_token();
			if (token.sValue.length() != 0)
				vecLineOut.push_back(token);
		}

		if (!vecLineOut.empty())
		{
			vecOutput.push_back(vecLineOut);
			vecLineOut.clear();
			nCursor = 0;
		}
	}
}

bool ReadFromFile(const std::string& sFileName, std::list<std::string>& sOutput)
{
	std::ifstream fFile("test.parser");
	if (!fFile.is_open()) return false;

	sOutput.clear();
	while (!fFile.eof())
	{
		std::string s;
		std::getline(fFile, s);
		sOutput.push_back(s);
	}

	fFile.close();
	return true;
}

struct Expression
{
	Expression(Token token) : token(token) {}
	Expression(Token token, Expression a) : token(token), args{ a } {}
	Expression(Token token, Expression a, Expression b) : token(token), args{ a, b } {}

	Token token;
	std::vector<Expression> args;
};

int GetPriority(Token& t)
{
	if (t.nId == Plus) return 1;
	if (t.nId == Minus) return 1;
	if (t.nId == Mul) return 2;
	if (t.nId == Div) return 2;
	if (t.nId == Mod) return 2;
	if (t.nId == Pow) return 3;

	return 0;
}

Expression DoSimplExpr(std::vector<Token>& vecTokens, int& nCurrentTok, bool bUpdateToken = true);

Expression DoBinExpr(std::vector<Token>& vecTokens, int& nCurrentTok, int nMinPriority, bool bUpdateToken = true)
{
	Expression eLeft = DoSimplExpr(vecTokens, nCurrentTok, bUpdateToken);

	while (1)
	{
		nCurrentTok++;

		Token op = (nCurrentTok < vecTokens.size()) ? vecTokens[nCurrentTok] : Token();

		int nPriority = (nCurrentTok < vecTokens.size()) ? GetPriority(op) : 0;

		if (nPriority <= nMinPriority)
		{
			nCurrentTok--;
			return eLeft;
		}

		Expression eRight = DoBinExpr(vecTokens, nCurrentTok, nPriority);
		eLeft = Expression(op, eLeft, eRight);
	}
}

long double Eval(Expression& e)
{
	switch (e.args.size())
	{
	case 2:
	{
		long double a = Eval(e.args[0]);
		long double b = Eval(e.args[1]);

		switch (e.token.nId)
		{
		case Plus: return a + b;
		case Minus: return a - b;
		case Mul: return a * b;
		case Div: return a / b;
		case Pow: return pow(a, b);
		case Mod: return (int)a % (int)b;
		}

		std::cerr << "[Error] Unknown binary operator: " << e.token.sValue << std::endl;
		exit(1);
	}
	break;

	case 1:
	{
		long double a = Eval(e.args[0]);
		
		switch (e.token.nId)
		{
		case Plus: return a;
		case Minus: return -a;
		case Abs: return abs(a);
		case Sin: return sin(a);
		case Cos: return cos(a);
		case Tan: return tan(a);
		case Atn: return atan(a);
		case Sqr: return sqrt(a);
		case Log: return log(a);
		case Exp: return exp(a);
		case Sgn: return (a > 0) ? 1.0 : 0.0;
		case Int: return (int)a;
		case Fix: return trunc(a);
		}

		std::cerr << "[Error] Unknown unary operator: " << e.token.sValue << std::endl;
		exit(1);
	}
	break;

	case 0:
		return std::stold(e.token.sValue);

	}

	std::cerr << "[Error] Unknown expression: " << e.token.sValue << std::endl;
	exit(1);
}

Expression DoSimplExpr(std::vector<Token>& vecTokens, int& nCurrentTok, bool bUpdateToken)
{
	if (bUpdateToken)
		nCurrentTok++;

	Token tToken = vecTokens[nCurrentTok];

	if (tToken.nId == Lparen)
	{
		Expression result = DoBinExpr(vecTokens, nCurrentTok, 0);

		nCurrentTok++;
		if (vecTokens[nCurrentTok].nId != Rparen)
		{
			std::cerr << "[Error] Expected ')' but got '" << vecTokens[nCurrentTok].sValue << "'" << std::endl;
			exit(1);
		}

		return result;
	}

	if (tToken.nId == Number)
		return Expression(vecTokens[nCurrentTok]);

	return Expression(tToken, DoSimplExpr(vecTokens, nCurrentTok));
};

void Interpret(std::vector<std::vector<Token>>& vecTokenized)
{
	std::unordered_map<std::string, std::string> mapVariables;
	std::map<int, int> mapLines;

	for (size_t i = 0; i < vecTokenized.size(); i++)
	{
		std::vector<Token> vecLine = vecTokenized[i];

		// Let's get number of line
		int nLine = 0;
		try { nLine = std::stoi(vecLine.front().sValue); }
		catch (std::error_code ec)
		{
			// If we can't convert String to Integer, then first token is not number
			std::cerr << "[Error] Expected number of line, but got " << vecLine.front().sValue << std::endl;
			exit(1);
		}

		// Update content of the line in map
		mapLines[nLine] = i;
		vecLine.erase(vecLine.begin());

		// 8-Bit Basic line structure:
		//	[Number of line] [Operand] [Other]
		
		// Getting operand (id, value)
		uint32_t nOperandId = vecLine.front().nId;

		// Remove it from the line
		vecLine.erase(vecLine.begin());

		// Some functions that are used
		// to parse current line of code

		switch (nOperandId)
		{
		case Let:
		{
			if (vecLine.front().nId != Identifier)
			{
				std::cerr << "[Error] Expected identifier, but got: " << vecLine.front().sValue << std::endl;
				exit(1);
			}

			std::string sName = vecLine.front().sValue;

			vecLine.erase(vecLine.begin());
			if (vecLine.front().nId != Equals)
			{
				std::cerr << "[Error] Expected '=', but got: " << vecLine.front().sValue << std::endl;
				exit(1);
			}

			vecLine.erase(vecLine.begin());

			for (auto& a : vecLine)
			{
				if (mapVariables.count(a.sValue) > 0)
				{
					a.nId = Number;
					a.sValue = mapVariables[a.sValue];
				}
			}

			int nCurrentTok = 0;
			Expression expr = DoBinExpr(vecLine, nCurrentTok, 0, false);
			long double dResult = Eval(expr);

			mapVariables[sName] = std::to_string(dResult);
		}
		break;

		case Print:
		{
			int nCurrentTok = 0;

			for (auto& a : vecLine)
			{
				if (mapVariables.count(a.sValue) > 0)
				{
					a.nId = Number;
					a.sValue = mapVariables[a.sValue];
				}

				if (a.nId == String)
					std::cout << a.sValue;
				else
				{
					Expression expr = DoBinExpr(vecLine, nCurrentTok, 0, false);
					std::cout << Eval(expr);
				}

				nCurrentTok++;

				if (nCurrentTok >= vecLine.size())
					break;
			}

			std::cout << std::endl;
		}
		break;

		case Goto:
		{
			if (vecLine.front().nId != Number)
			{
				std::cerr << "[Error] Expected number after GOTO, but got: " << vecLine.front().sValue << std::endl;
				exit(1);
			}

			int nGoto = std::stoi(vecLine.front().sValue);

			i = mapLines[nGoto] - 1;
			
		}
		break;

		}

	}

	
}

int main(int argc, char** argv)
{
	std::list<Token> listTokens = {
		{ Plus, "+" },
		{ Minus, "-" },
		{ Mul, "*" },
		{ Div, "/" },
		{ Mod, "%" },
		{ Pow, "^" },
		{ Comma, "," },
		{ Lparen, "(" },
		{ Rparen, ")" },
		{ Equals, "=" },
		{ NotEquals, "!=" },
		{ Less, "<" },
		{ Greater, ">" },
		{ Colon, ":" },
		{ Abs, "ABS" },
		{ Sin, "SIN" },
		{ Cos, "COS" },
		{ Tan, "TAN" },
		{ Atn, "ATN" },
		{ Sqr, "SQR" },
		{ Exp, "EXP" },
		{ Log, "LOG" },
		{ Sgn, "SGN" },
		{ Int, "INT" },
		{ Fix, "FIX" },
		{ Let, "LET" },
		{ Print, "PRINT" },
		{ Goto, "GOTO" },
	};

	std::list<std::string> listInput;
	ReadFromFile(argv[1], listInput);

	std::vector<std::vector<Token>> vecTokenized;

	Parse(listInput, listTokens, vecTokenized);
	Interpret(vecTokenized);

	return 0;
}