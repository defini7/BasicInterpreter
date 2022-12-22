#pragma once

#include "Common.h"

class cEvaluator
{
public:
	cEvaluator() = default;
	~cEvaluator() = default;

public:
	int GetPriority(Token& t);

	Expression DoBinExpr(std::vector<Token>& vecTokens, int& nCurrentTok, int nMinPriority, bool bUpdateToken = true);
	Expression DoSimplExpr(std::vector<Token>& vecTokens, int& nCurrentTok, bool bUpdateToken = true);

	long double Get(std::vector<Token>& vecLine);
	long double Eval(Expression& e);
};

