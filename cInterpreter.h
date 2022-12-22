#pragma once

#include "Common.h"
#include "cEvaluator.h"

class cInterpreter
{
public:
	cInterpreter() = default;
	cInterpreter(std::vector<std::vector<Token>>& vecTokenized);
	~cInterpreter() = default;

public:
	void Do();
	void Load(std::vector<std::vector<Token>>& vecTokenized);

private:
	std::vector<std::vector<Token>> m_vecTokenized;

};

