#pragma once

#include <iostream>
#include <cctype>
#include <list>
#include <string>
#include <fstream>
#include <unordered_map>
#include <map>
#include <chrono>
#include <thread>

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
	Goto,
	If,
	Else,
	Then,
	Sleep,
	LessEquals,
	GreaterEquals
};

struct Token
{
	uint32_t nId;
	std::string sValue;

	bool operator==(Token t)
	{
		return nId == t.nId && sValue == t.sValue;
	}
};

struct Expression
{
	Expression(Token token) : token(token) {}
	Expression(Token token, Expression a) : token(token), args{ a } {}
	Expression(Token token, Expression a, Expression b) : token(token), args{ a, b } {}

	Token token;
	std::vector<Expression> args;
};
