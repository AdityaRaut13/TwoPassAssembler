#include <bits/stdc++.h>
#include "constant.h"
#include <fstream>
#include <ios>
#include <sstream>
#include <regex>
#include <boost/algorithm/string.hpp>
#include <string>
#include <unistd.h>
#include <utility>
#include <vector>
std::unordered_map<int,std::pair<std::string,int>> SYMBOL_TABLE;  // id,(variable_string,address)
std::unordered_map<int,std::pair<std::string,int>> LITERAL_TABLE; // id,(variable_string,address)
int loc_cntr=0;
int littab_ptr=1;
int littab_ptr_assigned=1;
int symtab_ptr=1;
int classify(std::string str)
{
	if(IS.find(str)!=IS.end())
		return IS_IS;
	else if (DL.find(str)!=DL.end())
		return IS_DL;
	else if (AD.find(str)!=AD.end())
		return IS_AD;
	else if (CC.find(str)!=CC.end())
		return IS_CC;
	else if (REGISTER.find(str)!=REGISTER.end())
		return IS_REGISTER;
	else if (regex_match(str,std::regex("='[0-9]+'|=[0-9]+")))
		return IS_LITERAL;
	else 
		return IS_SYMBOL;
}
void fatal(std::string error_message,int line_number)
{
	std::cout<<"\033[0;31m[ERROR]:"<<error_message;
	std::cout<<"\n\tThe line is at :"<<line_number<<"\n";
	std::cout<<"\033[0m\n";
	exit(0);
}
int which_statement(std::vector<std::string>& list_words,int line_number)
{
	//std::cout<<"[LOG]:The list of words are : ";
	//for(auto i:list_words)
	//{
	//	std::cout<<i<<" ";
	//}
	//std::cout<<"\n";
	if(list_words.size()>4)
		fatal("Exhaustive token ",line_number);
	if(classify(list_words[0])==IS_IS)
		return IS_IS;
	else if (classify(list_words[0])==IS_AD)
		return IS_AD;
	else if (classify(list_words[0])==IS_SYMBOL)
	{
		//This is for AD,IS,DL

		if(classify(list_words[1])==IS_SYMBOL or classify(list_words[1])==IS_REGISTER or classify(list_words[1])==IS_LITERAL or classify(list_words[1])==IS_CC)
			fatal("There Must be an instruction.",line_number);
		return classify(list_words[1]);
	}
}
void split_str( std::string const &str, const char delim,std::vector <std::string> &out )  
{  
    // create a stream from the string  
    std::stringstream s(str);  
      
    std::string s2;  
    while (std:: getline (s, s2, delim) )  
    {  
        out.push_back(s2); // store the string in s2  
    }  
}
std::vector<std::string> tokenize(std::stringstream& line)
{
	std::vector<std::string> list_string;
	std::string word1,word2,word3;
	std::regex comma("[A-Za-z0-9_]*,.*");
	while(std::getline(line,word1,' '))
	{
		if(word1=="")
			continue;
		else if(std::regex_match(word1,comma))
		{
			std::stringstream stream(word1);
			std::getline(stream,word2,',');
			std::getline(stream,word3,',');
			boost::trim(word2);
			boost::trim(word3);			
			
			list_string.push_back(word2);
			list_string.push_back(word3);
		}
		else 
		{
			boost::trim(word1);
			list_string.push_back(word1);
		}			
	}
	return list_string;
}
int search_symbol(std::string word)
{
	for(int i=1;i<symtab_ptr;i++)
	{
		if(SYMBOL_TABLE[i].first==word)
			return i;
	}
	return -1;
}
void For_DL(std::vector<std::string>& statement,std::ofstream& file,int line_number)
{
	std::regex number("[1-9][0-9]*");
	if(statement.size()!=3)
		fatal("DL must take 3 token only.",line_number);
	else if (!std::regex_match(statement[2],number))
		fatal("The number must of the form : [1-9][0-9]* .",line_number);
	if(statement[1]=="DS")
	{ 
		// This is for DS
		int offset=stoi(statement[2]);
		file<<loc_cntr<<".(DS,02)"<<"(C,"<<offset<<")"<<std::endl;
		int position=search_symbol(statement[0]);
		if(position!=-1)
		SYMBOL_TABLE[position]=std::make_pair(statement[0],loc_cntr);
		else fatal("The variable is not used .",line_number);
		loc_cntr+=offset;
	}
	else
	{
		// This is for DC 
		int value=stoi(statement[2]);
		file<<loc_cntr<<".(DC,01)"<<"(C,"<<value<<")"<<std::endl;
		int position=search_symbol(statement[0]);
		if(position!=-1)
			SYMBOL_TABLE[position]=std::make_pair(statement[0],loc_cntr);
		else fatal("The variable is not used.",line_number);
		loc_cntr++;
	}
}

int operand_evaluation(std::string operand,int line_number)
{
	int value=0;
	std::regex add("[A-Za-z0-9_]+\\+[0-9]+");
	if(regex_match(operand,add))
	{
		// this is of the form 
		// FOO+constant
		// First word --> symbole
		// second word --> constant
		std::stringstream stream_operand(operand);
		std::string word1,word2;
		std::getline(stream_operand,word1,'+');
		std::getline(stream_operand,word2,'+');
		int index=search_symbol(word1);
		if(SYMBOL_TABLE[index].second==-1) fatal("The symbol is not defined yet.",line_number);
		else value=SYMBOL_TABLE[index].second+stoi(word2);
	}
	else if (std::regex_match(operand,std::regex("[1-9][0-9]+")))
		value=stoi(operand);
	else
		fatal("The operand must be a numeric or Symbol+numeric.",line_number);
	return value;
}
void print_literal()
{
	std::cout<<"\nLITERAL_TABLE_PTR : "<<littab_ptr;
	std::cout<<"\n\nLITERAL TABLE : \n";
	std::cout.width(5);
	std::cout<<"ID";
	std::cout.width(15);
	std::cout<<"LITERAL";
	std::cout.width(10);
	std::cout<<"ADDRESS\n\n";
	for(int i=1;i<littab_ptr;i++)
	{
		std::cout.width(5);
		std::cout<<i;
		std::cout.width(15);
		std::cout<<LITERAL_TABLE[i].first;
		std::cout.width(10);
		std::cout<<LITERAL_TABLE[i].second;
		std::cout<<"\n";
	}

}
void process(std::ofstream& file)
{
	// this will add the address for the literal .
	// This will increment the literal pointer littab_ptr_assigned .
	if(littab_ptr_assigned!=littab_ptr)
	// Litttab_ptr_assigned=5
	// littab_ptr=10
	{
		for(;littab_ptr_assigned<littab_ptr;littab_ptr_assigned++)
		{
			LITERAL_TABLE[littab_ptr_assigned].second=(loc_cntr++);
			file<<(loc_cntr-1)<<"."<<LITERAL_TABLE[littab_ptr_assigned].first<<"\n";
		}
		POOL_TABLE.push_back(littab_ptr);
	}
}
void For_AD(std::vector<std::string>& statement,std::ofstream& output_file,int line_number)
{
	std::regex number("[1-9][0-9]+");
	if(statement[0]=="START")
	{
		if(statement.size()==2)loc_cntr=stoi(statement[1]);
		else if (!std::regex_match(statement[1],number))
			fatal("Number not proper",line_number);
		else if(statement.size()>2)fatal("Start only takes one operand",line_number);
		output_file<<"\t(AD,01)"<<"(C,"<<loc_cntr<<")\n";
	}
	else if (statement[0]=="END")
	{
		if(statement.size()>2)fatal("END does not expect more than 1 tokens",line_number);
		else if (statement.size()==2 and std::regex_match(statement[1],number) ) loc_cntr=stoi(statement[1]);
		output_file<<"\t(AD,02)"<<"\n";
		process(output_file);
	}
	else if (statement[0]=="ORIGIN")
	{
		if (statement.size()!=2) fatal("ORGIGN only expect 1 token with it.",line_number);
		loc_cntr=operand_evaluation(statement[1],line_number);
		output_file<<"\t(AD,03)"<<"(C,"<<loc_cntr<<")\n";
	}
	else if (statement.size()>1 and statement[1]=="EQU")
	{
		int id1=search_symbol(statement[0]);
		int id2=search_symbol(statement[2]);
		 
		if(id1==-1 or id2==-1) fatal("Not encountered this symbol before.",line_number);
		SYMBOL_TABLE[id1].second=SYMBOL_TABLE[id2].second;
		output_file<<"\t(AD,04)"<<"\n";
	}
	else if (statement[0]=="LTORG")
	{
		if (statement.size()!=1) fatal("LTORG does not except a token.",line_number);
		output_file<<"\t(AD,05)"<<"\n";
		process(output_file);
	}

}
/**
 * @returns : The  id of duplicate literal in the same pool.
 * if not there returns -1.
 *
 */
int search_literal_one_pool(std::string literal)
{
	for(int i=(littab_ptr-1);i>littab_ptr_assigned;i--)
	{
		if(LITERAL_TABLE[i].first==literal)
			return i;
	}
	return -1;
}
void handle_symbol_operand(std::string operand,std::ofstream& file,int line_number)
{
	int position=search_symbol(operand);
	std::smatch symbol_constant;
	if (position!=-1) file<<"(S,"<<position<<")\n";
	else if(operand.find('+')==std::string::npos)
	{
	
		SYMBOL_TABLE[symtab_ptr++]=std::make_pair(operand,-1);
		file<<"(S,"<<(symtab_ptr-1)<<")\n";
	}
	else if ( std::regex_search(operand,symbol_constant,std::regex("([A-Za-z0-9_]+)\\+([0-9]+)")))
	{ 
		// FOO+constant
		position=search_symbol(symbol_constant.str(1));
		if (position==-1)
	    	{
			SYMBOL_TABLE[symtab_ptr++]=std::make_pair( symbol_constant.str(1),-1);
			file<<"(C,"<<"(S,"<<(symtab_ptr-1)<<")+"<<stoi(symbol_constant.str(2))<<")\n";
		}
		else file<<"(C,"<<"(S,"<<position<<")+"<<stoi(symbol_constant.str(2))<<")\n";
	}
	else
		fatal("The word is not a symbol.",line_number);
}
void add_into_literal_table(std::string number,std::ofstream& file)
{
	int position=search_literal_one_pool(number);
	if (position==-1) 
	{
		// we need to add in the literal table .
		LITERAL_TABLE[littab_ptr++]=std::make_pair(number,-1);
		file<<"(L,"<<(littab_ptr-1)<<")\n";
	}
	else file<<"(L,"<<position<<"\n";
}
void handle_literal_operand(std::string operand,std::ofstream& file)
{
	// operand "='([0-9]+)'"
	std::smatch match,match_number;
	std::regex re("='([0-9]+)'");
	std::regex re_number("=([0-9]+)");
	if(std::regex_search(operand,match,re))
		add_into_literal_table(match.str(1),file);
	else if (std::regex_search(operand,match_number,re_number))
		add_into_literal_table(match_number.str(1),file);

}
void handle_operand(std::string operand1,std::string operand2,std::ofstream& file,int line_number)
{
	int type_operand1=classify(operand1);
	int type_operand2=classify(operand2);
	if(type_operand1==IS_REGISTER) file<<"(R,"<<REGISTER[operand1]<<")";
	else fatal("The first operand must be a register.",line_number);
	if (type_operand2==IS_SYMBOL) handle_symbol_operand(operand2,file,line_number);
	else if (type_operand2==IS_LITERAL) handle_literal_operand(operand2,file);
	else fatal("The second operand must be a SYMBOL or LITERAL",line_number);
}
void For_IS(std::vector<std::string>& list_words,std::ofstream& file,int line_number)
{
	if(classify(list_words[0])==IS_SYMBOL)
	{
		//The First statement is a symbol then we need to add it to the symbol table.
		int position=search_symbol(list_words[0]);
		if(position==-1)
		SYMBOL_TABLE[symtab_ptr++]=std::make_pair(list_words[0],loc_cntr);
		else if (SYMBOL_TABLE[position].second!=-1) //  if the variable is redefined we need to raise an error.
			fatal("the label is defined earlier ",line_number);
		else 
			SYMBOL_TABLE[position].second=loc_cntr;
		list_words.erase(list_words.begin());
	}
	switch(IS[list_words[0]])
	{
		case STOP:
		{
			if(list_words.size()!=1) fatal("STOP doesn't takes any operand",line_number);
			file<<loc_cntr<<".(IS,00)\n";
			break;
		}
		case ADD:
		{
			//done
			file<<loc_cntr<<".(IS,01)";
			if(list_words.size()!=3) fatal("ADD takes only 2 operands .",line_number);
			handle_operand(list_words[1],list_words[2],file,line_number);
			break;
		}
		case SUB:
	    	{
			//done
			file<<loc_cntr<<".(IS,02)";
			if(list_words.size()!=3) fatal("SUB takes only 2 operands .",line_number);
			handle_operand(list_words[1],list_words[2],file,line_number);
			break;
		}
		case MULT: 
		{
			//done
			file<<loc_cntr<<".(IS,03)";
			if(list_words.size()!=3) fatal("MULT takes only 2 operands .",line_number);
			handle_operand(list_words[1],list_words[2],file,line_number);
			break;
		}
		case MOVER: 
		{
			//done
			file<<loc_cntr<<".(IS,04)";
			if(list_words.size()!=3) fatal("MOVER takes only 2 operands .",line_number);
			handle_operand(list_words[1],list_words[2],file,line_number);
			break;
		}
		case MOVEM: 
		{
			// done
			file<<loc_cntr<<".(IS,05)";
			if(list_words.size()!=3) fatal("MOVEM takes only 2 operands .",line_number);
			handle_operand(list_words[1],list_words[2],file,line_number);
			break;
		}
		case COMP: 
		{
			//done
			file<<loc_cntr<<".(IS,06)";
			if(list_words.size()!=3) fatal("COMP takes only 2 operands .",line_number);
			handle_operand(list_words[1],list_words[2],file,line_number);
			break;
		}
		case BC:
		{
			//done
			file<<loc_cntr<<".(IS,07)";   
			if (list_words.size()!=3)                    fatal("The instruction takes only 2 tokens",line_number);
			else if(classify(list_words[1])!=IS_CC)      fatal("After BC ,there must be a condition code. ",line_number);
			else if (classify(list_words[2])!=IS_SYMBOL) fatal("The second operand must be a symbol",line_number);
			file<<"(C,"<<CC[list_words[1]]<<")";
			handle_symbol_operand(list_words[2],file,line_number);
			break;
		}
		case DIV:
		{
			file<<loc_cntr<<".(IS,08)";
			if(list_words.size()!=3) fatal("DIV takes only 2 operands .",line_number);
			handle_operand(list_words[1],list_words[2],file,line_number);
			break;
		}
		case READ:
		{
			file<<loc_cntr<<".(IS,09)";
			int type_operand=classify(list_words[1]);
			if (list_words.size()!=2) fatal("The READ statement must have only 1 operand.",line_number);
			else if(type_operand==IS_SYMBOL) handle_symbol_operand(list_words[1],file,line_number);
			else if (type_operand==IS_LITERAL) handle_literal_operand(list_words[1],file);
			else  fatal("The READ statement must be a SYMBOL or literal.",line_number);
			break;
		}
		case PRINT:
		{
			file<<loc_cntr<<".(IS,10)";
			int type_operand=classify(list_words[1]);
			if(list_words.size()!=2) fatal("The PRINT statement must have only 1 operand.",line_number);
			else if(type_operand==IS_SYMBOL) handle_symbol_operand(list_words[1],file,line_number);
			else if (type_operand==IS_LITERAL) handle_literal_operand(list_words[1],file);
			else 
				fatal("The PRINT statement is wrong.",line_number);
			break;
		}
	}
	loc_cntr++;
}
void parser(std::string assemble_file,std::string output_path)
{
	std::ifstream input_file(assemble_file);
	std::ofstream output_file(output_path);
	std::string line;
	int line_number=1;
	while(getline(input_file,line))
	{
		std::stringstream statement(line);
		// This list will contains only one sentence i.e one line only. 
		std::vector<std::string> list_string=tokenize(statement);
		int value = which_statement(list_string,line_number);
		if (value==IS_IS)      For_IS(list_string,output_file,line_number);
		else if (value==IS_AD) For_AD(list_string,output_file,line_number);
		else if (value==IS_DL) For_DL(list_string,output_file,line_number);
		line_number++;
	}
	input_file.close();
	output_file.close();
}
void print()
{
	std::cout<<"\nSYMBOL_TABLE_PTR : "<<symtab_ptr;
	std::cout<<"\n\nSYMBOL TABLE : \n";
	std::cout.width(5);
	std::cout<<"ID";
	std::cout.width(15);
	std::cout<<"SYMBOL";
	std::cout.width(10);
	std::cout<<"ADDRESS\n\n";
	for(int i=1;i<symtab_ptr;i++)
	{
		std::cout.width(5);
		std::cout<<i;
		std::cout.width(15);
		std::cout<<SYMBOL_TABLE[i].first;
		std::cout.width(10);
		std::cout<<SYMBOL_TABLE[i].second;
		std::cout<<"\n";
	}
}

void machine_code(const std::string& input_path,const std::string& output_path)
{
	std::ifstream input_file(input_path);
	std::ofstream output_file(output_path);
	std::string line;
	std::regex is_IS("IS.([0-9]+)");
	std::regex symbol("\\(S,([0-9]+)");
	std::regex literal("L,([0-9]+)");
	std::regex reg("R.([1-4])");
	std::regex constant("C,([0-9]+)");
	// (C,(S,3)+34)
	std::regex constant_symbol("C.+S,([0-9]+).+([0-9]+)");
	std::smatch match_is,match_symbol,match_reg,match_literal,match_constant,match_constant_symbol;
	while(getline(input_file,line))
	{
		output_file.width(4);
		if(std::regex_search(line,match_is,is_IS))
		{
			output_file<<line.substr(0,line.find('.')+1);
			output_file.width(3);
			output_file<<match_is.str(1);
			if(std::regex_search(line,match_reg,reg))
			{
				output_file.width(3);
				output_file<<match_reg.str(1);
		 	}
			else  output_file<<"   ";
			if(std::regex_search(line,match_constant,constant))
			{
				// This is for the constant 
				std::cout<<match_constant.str(1)<<"\n";
				output_file.width(4);
				output_file<<match_constant.str(1);

			}
			else if (std::regex_search(line,match_constant_symbol,constant_symbol))
			{
				// (IS,09)(R,3)(C,(S,3)+495)
				// This is for the constant + symbol
				output_file.width(4);
				output_file<<(SYMBOL_TABLE[stoi(match_constant_symbol.str(1))].second+stoi(match_constant_symbol.str(2)));
			}
			else if(std::regex_search(line,match_symbol,symbol))
			{
				// This is for the only symbol .
				// S,2
				// S,3
				output_file.width(4);
 				output_file<<SYMBOL_TABLE[stoi(match_symbol.str(1))].second;
			}
			if(std::regex_search(line,match_literal,literal))
			{
				// This is for the literal match
				output_file.width(4);
				output_file<<LITERAL_TABLE[stoi(match_literal.str(1))].second;
			}
			output_file<<"\n";
		}
		else if(line.find("DS")!=std::string::npos)
		{
			// 200.(DL,0)(C,32)
			// 200.(DS)(C,3)
			// 200.
			// 201.
			// 202.
			int number=stoi(line.substr(line.find('C')+2,line.find(')')));
			int loc=stoi(line.substr(0,line.find('.')));
			for(int i=0;i<number;i++,loc++)
			{
				output_file.width(3);
				output_file<<loc<<".\n";
			}
		}
		else if(line.find("DC")!=std::string::npos)
			output_file<<line.substr(0,line.find('.')+1)<<"\n";
		else if(line.compare("\t(AD,05)")==0 or line.compare("\t(AD,02)")==0)
		{
			// LTORg
			// 00 0 023
			// 00 0 005 
			// 200.5
			// 200 IS,09
			getline(input_file,line);
			std::regex address("[0-9]+\\.([0-9]+)");
			while(std::regex_match(line,address))
			{
				output_file.width(4);
				output_file<<line.substr(0,line.find('.')+1);
				output_file.width(10);
				output_file<<line.substr(line.find('.')+1)<<"\n";
				getline(input_file,line);
			}
			input_file.seekg((int)input_file.tellg()-line.length()-1);
		}
	}
	input_file.close();
	output_file.close();
}
auto print_pool=[](auto const& str, auto const& v)
{
	std::cout<<str;
	for(auto i: v)
		std::cout<<i<<" ";
	std::cout<<std::endl;
};
void fatal(std::string error_message,std::string variable)
{
	std::cout<<"\033[0;31m[ERROR]:"<<error_message;
	std::cout<<"\n\tThe symbol is '"<<variable<<"'";
	std::cout<<"\033[0m\n";
	exit(0);
}
void check_symbol()
{
	for(int i=0;i<symtab_ptr;i++)
	{
		if(SYMBOL_TABLE[i].second==-1)
			fatal("The symbol is not defined ",SYMBOL_TABLE[i].first);
	}
}

int main(int argc,char** argv)
{
	init();
	parser(argv[1],argv[2]);
	check_symbol();
	print();
	print_literal();
	print_pool("Poool table : ",POOL_TABLE);
	machine_code(argv[2],"machine_code_generated.txt");
	return 0;
}
