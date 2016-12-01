%{
#include <iostream>
using namespace std;
int yylval;
const int ID = 1;
const int RESERVED = 2;
const int NUMBER = 3;
const int INT = 4;
const int FLOAT = 5;
const int OPERATOR = 6;
FILE *fp1;
struct Node
{
	char *s;
	Node *next;
};
Node *h = NULL;
%}

delim {blank}|{\n}|{\t}
ws {delim}{delim}*
letter [A-Za-z]
digit [0-9]
id {letter}({letter}|{digit})*
int {digit}+
float ({digit}+\.{digit}*)|({digit}*\.{digit}+)

%%

{ws}		{state = 0; len = 0;}
"if"		{return(RESERVED );}
"then"		{return(RESERVED );}
"else"		{return(RESERVED );}
{id}		{yylval = (int) installID(); return(ID);}
{int}		{return(NUMBER);}
{float}		{return (FLOAT);}
"+"		{return (OPERATOR);}
"-"		{return (OPERATOR);}
"*"		{return (OPERATOR);}
"="		{return (OPERATOR);}
"<="		{return (OPERATOR);}
">="		{return (OPERATOR);}

%%


Node* installID()
{
	Node *p = NULL;
	s[len] = '\0';
	if (h == NULL)
	{
		h = new Node();
		h->next = NULL;
		h->s = new char[len + 1];
		strcpy_s(h->s, len+1, s);
		p = h;
	}
	else
	{ 
		bool found = false;
		for (p = h; p ->next != NULL; p = p->next)
		{
			if (strcmp(p->s, s) == 0)
				break;
		}
		if (strcmp(p->s, s) != 0)
		{
			Node *q = new Node;
			q->next = NULL;
			p->next = q;
			q->s = new char[len + 1];
			strcpy_s(q->s, len + 1, s);
			p = q;
		}
	}
	return p;
}
int installNum()
{
	return 1;
}
int main()
{
	cout << "Input the path of the code file:" << endl;
	char s1[100];
	cin >> s1;

	fopen_s(&fp1, s1, "r");
	if (fp1 == NULL)
	{
		cout << ("fail to open the code file");
		system("pause");
		exit(-1);
	}
	while (feof(fp1) == 0)
	{
		int x = GetRelop();
		cout << x <<" ";
		cout << s<<" ";
		if (x == 1)
		{
			cout << yylval;
		}
		cout << endl;
	}
	fclose(fp1);
	cout << "id table: ";
	for (Node *p = h; p != NULL; p = p->next)
	{
		cout << p->s << " ";
	}
	system("pause");
	return 1;
}