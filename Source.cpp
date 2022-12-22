#include "Common.h"

#include "cScanner.h"
#include "cInterpreter.h"

int main()
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
		{ LessEquals, "<=" },
		{ GreaterEquals, ">=" },
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
		{ If, "IF" },
		{ Then, "THEN" },
		{ Sleep, "SLEEP" },
		{ Else, "ELSE" }
	};

	cScanner scanner;

	scanner.Read("Main.bas", listTokens);
	scanner.Scan();

	cInterpreter interpreter;

	interpreter.Load(scanner.vecOutput);
	interpreter.Do();

	return 0;
}
