#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <vector>

#define MAX_LINE_SIZE 1000000

using std::vector;

enum errorType{ WRONG_INPUT = 1, DIVIDED_BY_ZERO };
enum itemType{ OPERATOR, OPERAND, COMMA, END, OTHER};
const char unaryOpr[] = {'m','p','!', 'S', 'C', 'E', 'L', 'Q', 'F'};
const char* oprChars[] = {"&&", "||", "sin", "cos", "exp", "log", "pow", "sqrt", "fabs"};
const char code2Opr[] = {'a', 'o', 'S', 'C', 'E', 'L', 'P', 'Q', 'F'};
const char oprChar[] = {'!','(',')','*','/','+','-'};
const char* precedenceList[] = {"SCELQFP!mp", "*/", "+-", "a","o","("};

int getNext(char* &expPos, char& opr, double& number, bool nextIsBinaryOpr){
	while((*expPos) == ' ' || (*expPos) == '\t')
		expPos++;

	if((*expPos) == ','){
		expPos++;
		return COMMA;
	}
	if((*expPos) == '\n')
		return END;

	if(((*expPos) >= '0' && (*expPos) <= '9') || ((*expPos) == '.')){
		number = strtod(expPos, &expPos);
		return OPERAND;
	}

	bool match = false;
	for(unsigned int i=0; !match && i < sizeof(code2Opr)/sizeof(char); i++){
		match = true;
		for(unsigned int j=0;match && j < strlen(oprChars[i]); j++){
			if(oprChars[i][j] != expPos[j])
				match = false;
		}
		if(!match)
			continue;
		opr = code2Opr[i];
		expPos += strlen(oprChars[i]);
		return OPERATOR;
	}
	if((*expPos) == '+' || (*expPos) == '-'){
		if(nextIsBinaryOpr){
			opr = (*expPos);
		}else{
			opr = ((*expPos) == '+')? 'p':'m';
		}
		expPos++;
		return OPERATOR;
	}
	for(unsigned int i=0; i < sizeof(oprChar)/sizeof(char); i++){
		if((*expPos) == oprChar[i]){
			opr = (*expPos);
			expPos++;
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
void calculate(char opr, vector<double>& doubleStack){
	double opdF, opdL;
	opdL = doubleStack.back();
	doubleStack.pop_back();
	if(isUnary(opr)){
		switch(opr){
			case '!':
				doubleStack.push_back(!opdL);
				return;
			case 'm':
				doubleStack.push_back(-opdL);
				return;
			case 'p':
				doubleStack.push_back(+opdL);
				return;
			case 'S':
				doubleStack.push_back(sin(opdL));
				return;
			case 'C':
				doubleStack.push_back(cos(opdL));
				return;
			case 'E':
				doubleStack.push_back(exp(opdL));
				return;
			case 'L':
				doubleStack.push_back(log(opdL));
				return;
			case 'Q':
				doubleStack.push_back(sqrt(opdL));
				return;
			case 'F':
				doubleStack.push_back(fabs(opdL));
				return;
		}
	}else{
		opdF = doubleStack.back();
		doubleStack.pop_back();
		switch(opr){
			case '+':
				doubleStack.push_back(opdF + opdL);
				return;
			case '-':
				doubleStack.push_back(opdF - opdL);
				return;
			case '*':
				doubleStack.push_back(opdF * opdL);
				return;
			case '/':
				doubleStack.push_back(opdF / opdL);
				return;
			case 'a':
				doubleStack.push_back(opdF && opdL);
				return;
			case 'o':
				doubleStack.push_back(opdF || opdL);
				return;
			case 'P':
				doubleStack.push_back(pow(opdF, opdL));
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
			strcpy(oprSymbol, oprChars[i]);
			return ;
		}
	}
	sprintf(oprSymbol, "%c", opr);
}


int show_infix2postfix(char lineInput[], char postfixExp[], double& answer){
	vector<double> doubleStack;
	vector<char> oprStack;
	vector<int> precedenceStack;
	char opr, *curPos = lineInput, *expPos = postfixExp;
	char oprSymbol[5];
	double number;
	int precedence;
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
						calculate(oprStack.back(), doubleStack);

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
						calculate(oprStack.back(), doubleStack);

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
				printf("encounter %lf:\n\tpush to output\n", number);
				sprintf(expPos, " %lf",number);
				expPos += strlen(expPos);
				doubleStack.push_back(number);
				nextIsBinaryOpr = true;
				break;

			case COMMA:
					printf("encounter ,:\n\tflush the stack to output until meeting '('\n");
					while(oprStack.back() != '('){
						calculate(oprStack.back(), doubleStack);

						code2OprSymbol(oprStack.back(), oprSymbol);
						sprintf(expPos, " %s", oprSymbol);
						expPos += strlen(expPos);

						oprStack.pop_back();
						precedenceStack.pop_back();
					}
				nextIsBinaryOpr = false;
				break;

			case END:
				printf("encounter END:\n\tflush the stack to output\n");
				while(!oprStack.empty()){
					calculate(oprStack.back(), doubleStack);

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

		printf("\tcurrent stack:");
		for(unsigned int i=0; i < oprStack.size(); i++)
			printf(" %c",oprStack[i]);
		printf("\n");
		printf("\tcurrent output:%s\n", postfixExp);
	}
	assert(doubleStack.size() == 1);
	printf("# postfix expression transforming complete\n");
	answer = doubleStack.back();
	return 0;
}


int main(){
	char line[MAX_LINE_SIZE+1];
	char postfixExp[2*MAX_LINE_SIZE+1];
	int error;
	double answer;

	while(fgets(line, MAX_LINE_SIZE, stdin) != NULL){
		if(line[0] == '\n')
			continue;


		error = 0;
		if((error = show_infix2postfix(line, postfixExp, answer)) == 0){
			printf("Postfix Exp:%s\n", postfixExp);
			printf("RESULT: %lf\n", answer);
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

