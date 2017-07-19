#include <stdio.h>
#include <iostream>
#include "sqlite3.h"
#include <fstream>
#include <sstream>
#include <string.h>
#include <algorithm>


std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) 
{
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos)
	{
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}


int makeTable(sqlite3* db, std::string filename, std::string table, std::string insertCommand, int numFields, int largestFieldSize)
{
	//CREATE THE TABLE
	
	std::string end = ".txt";
	std::string create = "create table " + filename + "(" + table + ");";
	std::string drop = "DROP TABLE IF EXISTS " + filename + ";";
	std::ifstream inFile((filename + end).c_str()); //error check?
	sqlite3_exec(db, drop.c_str(), NULL, NULL, NULL); //drops table if it exists
	sqlite3_exec(db, create.c_str(), NULL, NULL, NULL); //creates table with specified filename as table name and specified fields 
	std::cout << "Indexing file : " + filename << std::endl;
	
	
	//POPULATE THE TABLE WITH ENTRIES
	
	//for each line in input...
	std::string var;
	std::string line;
	std::string insert;
	std::string fullCommand;
	std::string fields[numFields];
	char *tokens[largestFieldSize];
	int entries = 1;
	while (std::getline(inFile, line)) //delimits line by line (\n)
	{
		std::cout << entries << std::endl;
		entries++;
		
		//char *tokens[largestFieldSize];
		int i = 0;
		line = ReplaceAll(line, std::string("^^"), std::string("^.^")); //assumption here is that ^.^ never occurs naturally in the database
		tokens[i] = strtok((char*) line.c_str(), "^");
		//std::string var;
		while (tokens[i] != NULL)
		{
			var = (tokens[i]);
			//check if var is ~~ or . 
			if ((var.compare("~~") == 0) || (var.compare(".") == 0)) 
			{
				fields[i] = "NULL";
				i++;
				tokens[i] = strtok(NULL, "^");
				continue;
			}
			
			//Else, remove tildes and set field to token
			var.erase(std::remove(var.begin(), var.end(), '~'), var.end());
			//tokens[i] = (char*) var.c_str(); //might be able to remove
			fields[i] = tokens[i];
			i++;
			tokens[i] = strtok(NULL, "^");
		}
		
		fullCommand = insertCommand + "\'" + fields[0] + "\'";
		
		for (int j = 1; j < numFields; j++)
		{
			fullCommand += ", \'" + fields[j] + "\'";
		}
		fullCommand += ");";
		
		insert = "insert into " + filename + fullCommand; //THIS SHOULD BE THE ONLY ONE OF THESE 3 LINES REMAINING
		sqlite3_exec(db, insert.c_str(), NULL, NULL, NULL); //creates table with specified filename as table name and specified fields 
	}
	return 0;
}



int main(int argc, char* argv[])
{

	sqlite3 *db;
	int conn;
	
	conn = sqlite3_open("nutrients.db", &db);
	
	if (conn)
	{
		fprintf(stderr, "Unable to open the database: %s\n", sqlite3_errmsg(db)); //sqlite3 api
		return(0);
	}
	else fprintf(stderr, "database opened successfully\n");

	//CREATE AND POPULATE ALL OF THE TABLES
	std::string filename;
	std::string table;
	std::string insertCommand;
	int numFields;
	int largestFieldSize;
	
	//FD_GROUP table
	filename = "FD_GROUP"; //this will need to be different for every table
	table = "FdGrp_Cd text(4) PRIMARY KEY, FdGrp_Desc text(60)"; //this will need to be different for every table
	insertCommand = "(FdGrp_Cd, FdGrp_Desc) values (";
	numFields = 2;
	largestFieldSize = 60; //should be of type t_size
	makeTable(db, filename, table, insertCommand, numFields, largestFieldSize);
	
	//LANGUAL table
	filename = "LANGUAL"; //this will need to be different for every table
	table = "NDB_No text(5), Factor_Code text(5), PRIMARY KEY(NDB_No, Factor_Code)"; //this will need to be different for every table
	insertCommand = "(NDB_No, Factor_Code) values (";
	numFields = 2;
	largestFieldSize = 5; //should be of type t_size
	makeTable(db, filename, table, insertCommand, numFields, largestFieldSize);
	
	//LANGDESC table
	filename = "LANGDESC"; //this will need to be different for every table
	table = "Factor_Code text(5) PRIMARY KEY, Description text(140)"; //this will need to be different for every table
	insertCommand = "(Factor_Code, Description) values (";
	numFields = 2;
	largestFieldSize = 140; //should be of type t_size
	makeTable(db, filename, table, insertCommand, numFields, largestFieldSize);
	
	//NUT_DATA table HAS NUMBERS
	filename = "NUT_DATA"; //this will need to be different for every table
	table = "NDB_No text(5), Nutr_No text(3), Nutr_Val num(10.3), Num_Data_Pts num(5.0), Std_Error num(8.3), Src_Cd text(2), Deriv_Cd text(4), Ref_NDB_No text(5), Add_Nutr_Mark text(1), Num_Studies num(2), Min num(10.3), Max num(10.3), DF text(4), Low_EB num(10.3), Up_EB num(10.3), Stat_cmt text(10), AddMod_Date text(10), CC text(1), PRIMARY KEY(NDB_No, Nutr_No)"; //this will need to be different for every table
	insertCommand = "(NDB_No, Nutr_No, Nutr_Val, Num_Data_Pts, Std_Error, Src_Cd, Deriv_Cd, Ref_NDB_No, Add_Nutr_Mark, Num_Studies, Min, Max, DF, Low_EB, Up_EB, Stat_cmt, AddMod_Date, CC) values (";
	numFields = 18;
	largestFieldSize = 20; //should be of type t_size
	makeTable(db, filename, table, insertCommand, numFields, largestFieldSize);
	
	//NUTR_DEF table HAS NUMBERS
	filename = "NUTR_DEF"; //this will need to be different for every table
	table = "Nutr_No text(3) PRIMARY KEY, Units text(7), Tagname text(20), NutrDesc text(60), Num_Desc text(1), SR_Order num(6)"; //this will need to be different for every table
	insertCommand = "(Nutr_No, Units, Tagname, NutrDesc, Num_Desc, SR_Order) values (";
	numFields = 6;
	largestFieldSize = 60; //should be of type t_size
	makeTable(db, filename, table, insertCommand, numFields, largestFieldSize);
	
	//SRC_CD table
	filename = "SRC_CD"; //this will need to be different for every table
	table = "Src_Cd text(2) PRIMARY KEY, SrcCd_Desc text(60)"; //this will need to be different for every table
	insertCommand = "(Src_Cd, SrcCd_Desc) values (";
	numFields = 2;
	largestFieldSize = 60; //should be of type t_size
	makeTable(db, filename, table, insertCommand, numFields, largestFieldSize);
	
	//DERIV_CD table
	filename = "DERIV_CD"; //this will need to be different for every table
	table = "Deriv_Cd text(4) PRIMARY KEY, Deriv_Desc text(120)"; //this will need to be different for every table
	insertCommand = "(Deriv_Cd, Deriv_Desc) values (";
	numFields = 2;
	largestFieldSize = 120; //should be of type t_size
	makeTable(db, filename, table, insertCommand, numFields, largestFieldSize);
	
	//WEIGHT table HAS NUMBERS
	filename = "WEIGHT"; //this will need to be different for every table
	table = "NDB_No text(5), Seq text(2), Amount num(5.3), Msre_Desc text(84), Gm_Wgt num(7.1), Num_Data_Pts num(3), Std_Dev num(7.3), PRIMARY KEY(NDB_No, Seq)"; //this will need to be different for every table
	insertCommand = "(NDB_No, Seq, Amount, Msre_Desc, Gm_Wgt, Num_Data_Pts, Std_Dev) values (";
	numFields = 7;
	largestFieldSize = 85; //should be of type t_size
	makeTable(db, filename, table, insertCommand, numFields, largestFieldSize);
	
	//FOOTNOTE table
	filename = "FOOTNOTE"; //this will need to be different for every table
	table = "NDB_No text(5), Footnt_No text(4), Footnt_Typ text(1), Nutr_No text(3), Footnt_Txt text(200)"; //this will need to be different for every table
	insertCommand = "(NDB_No, Footnt_No, Footnt_Typ, Nutr_No, Footnt_Txt) values (";
	numFields = 5;
	largestFieldSize = 200; //should be of type t_size
	makeTable(db, filename, table, insertCommand, numFields, largestFieldSize);
	
	//DATSRCLN table
	filename = "DATSRCLN"; //this will need to be different for every table
	table = "NDB_No text(5), Nutr_No text(3), DataSrc_ID text(6), PRIMARY KEY(NDB_No, Nutr_No, DataSrc_ID)"; //this will need to be different for every table
	insertCommand = "(NDB_No, Nutr_No, DataSrc_ID) values (";
	numFields = 3;
	largestFieldSize = 10; //should be of type t_size
	makeTable(db, filename, table, insertCommand, numFields, largestFieldSize);
	
	//DATA_SRC table
	filename = "DATA_SRC"; //this will need to be different for every table
	table = "DataSrc_ID text(6) PRIMARY KEY, Authors text(255), Title text(255), Year text(4), Journal text(135), Vol_City text(16), Issue_State text(5), Start_Page text(5), End_Page text(5)"; //this will need to be different for every table
	insertCommand = "(DataSrc_ID, Authors, Title, Year, Journal, Vol_City, Issue_State, Start_Page, End_Page) values (";
	numFields = 9;
	largestFieldSize = 255; //should be of type t_size
	makeTable(db, filename, table, insertCommand, numFields, largestFieldSize);
	
	//FOOD_DES table HAS NUMBERS
	filename = "FOOD_DES"; //this will need to be different for every table
	table = "NDB_No text(5) PRIMARY KEY, FdGrp_Cd text(4), Long_Desc text(200), Shrt_Desc text(60), ComName text(100), ManufacName text(65), Survey text(1), Ref_desc text(135), Refuse num(2), SciName text(65), N_Factor num(4.2), Pro_Factor num(4.2), Fat_Factor num(4.2), CHO_Factor num(4.2)"; //this will need to be different for every table
	insertCommand = "(NDB_No, FdGrp_Cd, Long_Desc, Shrt_Desc, ComName, ManufacName, Survey, Ref_desc, Refuse, SciName, N_Factor, Pro_Factor, Fat_Factor, CHO_Factor) values (";
	numFields = 14;
	largestFieldSize = 200; //should be of type t_size
	makeTable(db, filename, table, insertCommand, numFields, largestFieldSize);
	
	//FOOD_DES, WEIGHT, NUT_DATA, NUT_DEF all have numbers
	//NUT_DATA, WEIGHT, DATSRCLN, LANGUAL NOT SHOWING UP
	
	
	sqlite3_close(db); //sqlite3 api
}