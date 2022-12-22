#include "cScanner.h"

cScanner::cScanner(const std::string& sFileName, std::list<Token>& listTokenList)
{
	Read(sFileName, listTokenList);
}

bool cScanner::ReadFromFile(const std::string& sFileName)
{
	std::ifstream fFile(sFileName);
	if (!fFile.is_open()) return false;

	m_listInput.clear();
	while (!fFile.eof())
	{
		std::string s;
		std::getline(fFile, s);
		m_listInput.push_back(s);
	}

	fFile.close();
	return true;
}

bool cScanner::Match(std::string sFirst, std::string sSecond, int nStart)
{
	for (int i = 0; i < sSecond.length(); i++)
		if (sFirst[nStart + i] != sSecond[i])
			return false;

	return true;
}

void cScanner::Scan()
{
	int nCursor = 0;

	auto itLine = m_listInput.begin();
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

		Token tMatch = { -1 };
		for (const auto& t : m_listTokenList)
			if (Match((*itLine), t.sValue, nCursor))
			{
				tMatch.nId = t.nId;
				tMatch.sValue = t.sValue;
			}

		if (tMatch.nId != -1)
		{
			nCursor += tMatch.sValue.length();
			return tMatch;
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

	for (; itLine != m_listInput.end(); itLine++)
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

void cScanner::Read(const std::string& sFileName, std::list<Token>& listTokenList)
{
	if (!ReadFromFile(sFileName))
	{
		std::cerr << "[Error] Could not load the file" << std::endl;
		return;
	}

	for (const auto& t : listTokenList)
		m_listTokenList.push_back(t);

	std::cout << "[Info] File " << sFileName << " loaded successfully!" << std::endl;
}
