#pragma once

#include "Common.h"

class cScanner
{
public:
	cScanner() = default;
	cScanner(const std::string& sFileName, std::list<Token>& listTokenList);

private:
	bool ReadFromFile(const std::string& sFileName);
	bool Match(std::string sFirst, std::string sSecond, int nStart);

public:
	void Scan();
	void Read(const std::string& sFileName, std::list<Token>& listTokenList);

private:
	std::list<std::string> m_listInput;
	std::list<Token> m_listTokenList;

public:
	std::vector<std::vector<Token>> vecOutput;

};

