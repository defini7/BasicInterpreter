#include "cEvaluator.h"

int cEvaluator::GetPriority(Token& t)
{
	if (t.nId == Equals) return 1;
	if (t.nId == Less) return 1;
	if (t.nId == Greater) return 1;
	if (t.nId == LessEquals) return 1;
	if (t.nId == GreaterEquals) return 1;
	if (t.nId == NotEquals) return 1;
	if (t.nId == Plus) return 1;
	if (t.nId == Minus) return 1;
	if (t.nId == Mul) return 2;
	if (t.nId == Div) return 2;
	if (t.nId == Mod) return 2;
	if (t.nId == Pow) return 3;

	return 0;
}

Expression cEvaluator::DoBinExpr(std::vector<Token>& vecTokens, int& nCurrentTok, int nMinPriority, bool bUpdateToken)
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

Expression cEvaluator::DoSimplExpr(std::vector<Token>& vecTokens, int& nCurrentTok, bool bUpdateToken)
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
}

long double cEvaluator::Get(std::vector<Token>& vecLine)
{
	int nCurrentTok = 0;
	Expression expr = DoBinExpr(vecLine, nCurrentTok, 0, false);
	return Eval(expr);
}

long double cEvaluator::Eval(Expression& e)
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
		case Mod: return double((int)a % (int)b);
		case Equals: return double(a == b);
		case Less: return double(a < b);
		case Greater: return double(a > b);
		case LessEquals: return double(a <= b);
		case GreaterEquals: return double(a >= b);
		case NotEquals: return double(a != b);
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
