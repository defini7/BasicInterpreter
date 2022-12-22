#include "cInterpreter.h"

cInterpreter::cInterpreter(std::vector<std::vector<Token>>& vecTokenized)
{
	Load(vecTokenized);
}

void cInterpreter::Do()
{
	cEvaluator eval;

	std::unordered_map<std::string, std::string> mapVariables;
	std::map<int, int> mapLines;

	for (size_t i = 0; i < m_vecTokenized.size(); i++)
	{
		std::vector<Token> vecLine = m_vecTokenized[i];

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

			long double dResult = eval.Get(vecLine);
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
					Expression expr = eval.DoBinExpr(vecLine, nCurrentTok, 0, false);
					std::cout << eval.Eval(expr);
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

		case If:
		{
			std::vector<Token> vecExpr;

			// IF <expr> THEN <...> ELSE <...>
			while (vecLine.front().nId != Then)
			{
				if (mapVariables.count(vecLine.front().sValue) > 0)
				{
					vecLine.front().nId = Number;
					vecLine.front().sValue = mapVariables[vecLine.front().sValue];
				}

				vecExpr.push_back(vecLine.front());
				vecLine.erase(vecLine.begin());
			}

			// Remove THEN
			vecLine.erase(vecLine.begin());

			// If expression's result is TRUE, then
			if (eval.Get(vecExpr) == 1.0)
			{
				// We erase all tokens after ELSE token
				while (vecLine.back().nId != Else)
					vecLine.pop_back();

				// Now erase ELSE token
				vecLine.pop_back();

				// Set current line to what we got
				m_vecTokenized[i].clear();

				// First token is always number of line
				m_vecTokenized[i].push_back({ Number, std::to_string(nLine) });

				// Then append other tokens
				for (auto& t : vecLine)
					m_vecTokenized[i].push_back(t);

				// decrease 'i', because in the next iteration
				// of for loop 'i' will be increased, but
				// we want to parse the same line again
				i--;
			}
			else
			{
				// We should skip full IF statement
				// and stay with ELSE only

				// Erase full IF body
				while (vecLine.front().nId != Else)
					vecLine.erase(vecLine.begin());

				int nElsePos = -1;
				for (size_t j = vecLine.size() - 1; j > 0; j--)
				{
					if (vecLine[j].nId == Else)
						nElsePos = j;
				}

				if (nElsePos != -1)
					vecLine.erase(vecLine.begin(), vecLine.begin() + nElsePos);

				// Now erase ELSE token
				vecLine.erase(vecLine.begin());

				// The same reason as with positive result
				m_vecTokenized[i].clear();
				m_vecTokenized[i].push_back({ Number, std::to_string(nLine) });

				for (auto& t : vecLine)
					m_vecTokenized[i].push_back(t);

				i--;
			}

		}
		break;

		case Sleep:
		{
			if (vecLine.front().nId != Number)
			{
				std::cerr << "[Error] Expected number after GOTO, but got: " << vecLine.front().sValue << std::endl;
				exit(1);
			}

			int nSleepFor = std::stoi(vecLine.front().sValue);

			std::this_thread::sleep_for(std::chrono::seconds(nSleepFor));
		}
		break;

		}

	}


}

void cInterpreter::Load(std::vector<std::vector<Token>>& vecTokenized)
{
	for (auto& t : vecTokenized)
		m_vecTokenized.push_back(t);
}
