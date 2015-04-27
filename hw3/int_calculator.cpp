#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <vector>

#define MAX_LINE_SIZE 1000000

using std::vector;

enum errorType{ WRONG_INPUT = 1, DIVIDED_BY_ZERO };
enum itemType{ OPERATOR, OPERAND, END, OTHER};
const char unaryOpr[] = {'~','m','p','!'};
const char* oprTwoChar[] = { "<<", ">>", "&&", "||"};
const char code2Opr[] = { 'l', 'r', 'a', 'o'};
const char oprChar[] = {'&','^','|','~','!','(',')','*','/','%','+','-'};
const char* precedenceList[] = {"!~mp", "*/%", "+-", "lr", "&", "^", "|", "a","o","("};

int getNext(char* &str, char& opr, int& number, bool nextIsBinaryOpr){
	while((*str) == ' ' || (*str) == '\t')
		str++;

	if((*str) == '\n')
		return END;

	if((*str) >= '0' && (*str) <= '9'){
		number = (int)strtol(str, &str, 10);
		return OPERAND;
	}

	bool match = false;
	for(unsigned int i=0; !match && i < sizeof(code2Opr)/sizeof(char); i++){
		match = true;
		for(unsigned int j=0;match && j < strlen(oprTwoChar[i]); j++){
			if(oprTwoChar[i][j] != str[j])
				match = false;
		}
		if(!match)
			continue;
		opr = code2Opr[i];
		str += strlen(oprTwoChar[i]);
		return OPERATOR;
	}
	if((*str) == '+' || (*str) == '-'){
		if(nextIsBinaryOpr){
			opr = (*str);
		}else{
			opr = ((*str) == '+')? 'p':'m';
		}
		str++;
		return OPERATOR;
	}
	for(unsigned int i=0; i < sizeof(oprChar)/sizeof(char); i++){
		if((*str) == oprChar[i]){
			opr = (*str);
			str++;
			return OPERATOR;
		}
	}
	return OTHER;
}
int find_precedence(char opr){
	for(unsigned int i=0; i < sizeof(precedenceList)/sizeof(char*); i++){
		for(unsigned int j=0; j < strlen(precedenceList[i]); j++){
			if(opr == precedenceList[i][j]){
				return i;}
		}
	}
	return -1;
}
bool isUnary(char opr){
	for(unsigned int i=0; i < sizeof(unaryOpr)/sizeof(char); i++){
		if(opr == unaryOpr[i])
			return true;
	}
	return false;
}
void calculate(char opr, vector<int>& intStack){
	int opdF, opdL;
	opdL = intStack.back();
	intStack.pop_back();
	if(isUnary(opr)){
		switch(opr){
			case '!':
				intStack.push_back(!opdL);
				return;
			case '~':
				intStack.push_back(~opdL);
				return;
			case 'm':
				intStack.push_back(-opdL);
				return;
			case 'p':
				intStack.push_back(+opdL);
				return;
		}
	}else{
		opdF = intStack.back();
		intStack.pop_back();
		switch(opr){
			case '+':
				intStack.push_back(opdF + opdL);
				return;
			case '-':
				intStack.push_back(opdF - opdL);
				return;
			case '*':
				intStack.push_back(opdF * opdL);
				return;
			case '/':
				intStack.push_back(opdF / opdL);
				return;
			case '%':
				intStack.push_back(opdF % opdL);
				return;
			case '&':
				intStack.push_back(opdF & opdL);
				return;
			case '^':
				intStack.push_back(opdF ^ opdL);
				return;
			case '|':
				intStack.push_back(opdF | opdL);
				return;
			case 'l':
				intStack.push_back(opdF << opdL);
				return;
			case 'r':
				intStack.push_back(opdF >> opdL);
				return;
			case 'a':
				intStack.push_back(opdF && opdL);
				return;
			case 'o':
				intStack.push_back(opdF || opdL);
				return;
		}
	}
}
void code2OprSymbol(char opr, char oprSymbol[]){
	if(opr == 'p'){
		sprintf(oprSymbol, "%c", '+');
		return;
	}
	if(opr == 'm'){
		sprintf(oprSymbol, "%c", '-');
		return;
	}
		
	for(unsigned int i=0; i < sizeof(code2Opr)/sizeof(char); i++){
		if(opr == code2Opr[i]){
			strcpy(oprSymbol, oprTwoChar[i]);
			return ;
		}
	}
	sprintf(oprSymbol, "%c", opr);
}


int show_infix2postfix(char lineInput[], char postfixExp[], int& answer){
	vector<int> intStack;
	vector<char> oprStack;
	vector<int> precedenceStack;
	char opr, *curPos = lineInput, *expPos = postfixExp;
	char oprSymbol[5];
	int number, precedence;
	bool end = false, nextIsBinaryOpr = false, shouldPop;

	postfixExp[0] = '\0';

	printf("# transform from indix to postfix\n");
	while(!end){
		switch(getNext(curPos, opr, number, nextIsBinaryOpr)){
			case OPERATOR:
				printf("encounter %c:\n",opr);
				if(opr == ')'){
					printf("\tflush the stack to output until meeting '('\n");
					while(oprStack.back() != '('){
						calculate(oprStack.back(), intStack);

						code2OprSymbol(oprStack.back(), oprSymbol);
						sprintf(expPos, " %s", oprSymbol);
						expPos += strlen(expPos);

						oprStack.pop_back();
						precedenceStack.pop_back();
					}
					oprStack.pop_back();
					precedenceStack.pop_back();
					nextIsBinaryOpr = true;
				}else{
					precedence = find_precedence(opr);
					shouldPop = (opr == '(')? false:true;
					assert(precedence != -1);

					while(oprStack.size() > 0 && shouldPop == true){
						if(precedence < precedenceStack.back()
						   || (precedence == precedenceStack.back() && isUnary(opr))){
							shouldPop = false;
							continue;
						}
						printf("\t*** stack.top() has greater precedence. pop '%c' ***\n", oprStack.back());
						calculate(oprStack.back(), intStack);

						code2OprSymbol(oprStack.back(), oprSymbol);
						sprintf(expPos, " %s", oprSymbol);
						expPos += strlen(expPos);

						oprStack.pop_back();
						precedenceStack.pop_back();
					}
					printf("\tpush '%c' to stack\n",opr);
					oprStack.push_back(opr);
					precedenceStack.push_back(precedence);
					nextIsBinaryOpr = false;
				}
				break;

			case OPERAND:
				printf("encounter %d:\n\tpush to output\n", number);
				sprintf(expPos, " %d",number);
				expPos += strlen(expPos);
				intStack.push_back(number);
				nextIsBinaryOpr = true;
				break;

			case END:
				printf("encounter END:\n\tflush the stack to output\n");
				while(!oprStack.empty()){
					calculate(oprStack.back(), intStack);

					code2OprSymbol(oprStack.back(), oprSymbol);
					sprintf(expPos, " %s", oprSymbol);
					expPos += strlen(expPos);

					oprStack.pop_back();
				}
				end = true;
				break;

			default:
				return WRONG_INPUT;
		}
		assert(strlen(postfixExp) < sizeof(postfixExp)-15); 
		printf("\tcurrent stack:");
		for(unsigned int i=0; i < oprStack.size(); i++)
			printf(" %c",oprStack[i]);
		printf("\n");
		printf("\tcurrent output:%s\n", postfixExp);
	}
	assert(intStack.size() == 1);
	printf("# postfix expression transforming complete\n");
	answer = intStack.back();
	return 0;
}


int main(){
	char line[MAX_LINE_SIZE+1];
	char postfixExp[2*MAX_LINE_SIZE+1];
	int answer, error;

	while(fgets(line, MAX_LINE_SIZE, stdin) != NULL){
		if(line[0] == '\n')
			continue;


		error = 0;
		if((error = show_infix2postfix(line, postfixExp, answer)) == 0){
			printf("Postfix Exp:%s\n", postfixExp);
			printf("RESULT: %d\n", answer);
		}else{
			printf("error ");
			switch(error){
				case DIVIDED_BY_ZERO:
					printf(": divide something by zero\n");
					break;
				case WRONG_INPUT:
					printf(": wrong input\n");
					break;
				default:
					printf("\n");
			}
		}
	}
	return 0;
}

