#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <vector>
#include <string>
#include "data_list.h"

#define LENGTH_LINE 1000
#define NUM_RECORD 100000000
#define CMD_LENGTH 20
#define NUM_CMD 4
using std::vector;
using std::string;

enum Command{ GET = 0, CLICKED, IMPRESSED, PROFIT, NOT_FOUND };
const char *const commands[] = { "get", "clicked", "impressed", "profit" };

int FindCMDIndex(const string& cmd){
	for(int i=0; i < NUM_CMD; i++){
		if(cmd.compare(commands[i]) == 0)
				return i;
	}
	return NOT_FOUND;
}
void ReadFile(char* filename, DataListVector& data){
	FILE *fp = fopen(filename,"r");
	assert(fp != NULL);

	OneLineRecord tmp;
	char line[LENGTH_LINE];

	while(fgets(line, LENGTH_LINE, fp) != NULL){
		tmp.Line2Record(line);
		data.AddRecord(tmp);
//if(data.RecordSize() % 10000000 == 0)
//printf("has input %u lines\n",data.RecordSize());
	}
//std::cout << data.RecordSize() << " = line\n";

	fclose(fp);
}

int main(int argc,char *argv[]){

	DataListVector data(NUM_RECORD);

	ReadFile(argv[1], data);
	data.SortUAQDPAndBuildUserList();

	int userA, userB, ad, query, pos, depth;
	double rate;
	int cmd_index;
	string cmd;

	std::cin >> cmd;
	while(!std::cin.eof() && (cmd.compare("quit") != 0)){
		cmd_index = FindCMDIndex(cmd);
		printf("********************\n");
		switch(cmd_index){
			case GET:
				scanf("%d %d %d %d %d", &userA, &ad, &query, &pos, &depth);
				data.get(userA, ad, query, pos, depth);
				break;
			case CLICKED:
				scanf("%d", &userA);
				data.clicked(userA);
				break;
			case IMPRESSED:
				scanf("%d %d", &userA, &userB);
				data.impressed(userA, userB);
				break;
			case PROFIT:
				scanf("%d %lf", &ad, &rate);
				data.profit(ad, rate);
				break;
			default:
				printf("usage:\n\tget [u] [a] [q] [p] [d]\n\tclicked [u]\n\timpressed [u1] [u2]\n\tprofit [a] [theta]\n\tquit\n");
		}
		printf("********************\n");
		std::cin >> cmd;
	}

	return 0;
}
