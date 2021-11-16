#pragma once
#include <cstdlib>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
std::unordered_map<std::string,int> IS;
std::unordered_map<std::string,int> AD;
std::unordered_map<std::string,int> DL;
std::unordered_map<std::string,int> CC;
std::unordered_map<std::string,int> REGISTER;
std::vector<int> POOL_TABLE;
enum possiblity
{
	IS_AD, // assembler directive
	IS_IS, //  Instruction set
	IS_DL, // declaration statement
	IS_CC, // condition code
	IS_REGISTER,
	IS_SYMBOL, 
	IS_LABEL,
	IS_LITERAL
};
enum IS_Possibilty
{
	STOP,
	ADD,
	SUB,
	MULT,
	MOVER,
	MOVEM,
	COMP,
	BC,
	DIV,
	READ,
	PRINT
};
void init()
{
	// This is for Instruction set. 
	IS["STOP"]=0;
	IS["ADD"]=1;
	IS["SUB"]=2;
	IS["MULT"]=3;
	IS["MOVER"]=4;
	IS["MOVEM"]=5;
	IS["COMP"]=6;
	IS["BC"]=7;
	IS["DIV"]=8;
	IS["READ"]=9;
	IS["PRINT"]=10;

	// This is for Assembler directives .
	AD["START"]=1;
	AD["END"]=2;
	AD["ORIGIN"]=3;
	AD["EQU"]=4;
	AD["LTORG"]=5;

	// This is for Declartion Statement .
	DL["DS"]=1;
	DL["DC"]=2;

	// This is Condition Code .
	CC["LT"]=1;
	CC["LE"]=2;
	CC["EQ"]=3;
	CC["GT"]=4;
	CC["GE"]=5;
	CC["ANY"]=6;

	// This is for Register 
	REGISTER["AREG"]=1;
	REGISTER["BREG"]=2;
	REGISTER["CREG"]=3;
	REGISTER["DREG"]=4;
	
	
	// This is the first pool which will start at 1.
	POOL_TABLE.push_back(1);

};
