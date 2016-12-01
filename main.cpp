#include<iostream>
using namespace std;

/*
    This small program functions as a Lexical Analyzer Generator.
    First, a .l configure file is required. As it is demonstrated in ex.l.
  The structure of the file is adapted from the famous software Lex.
  Some parts of the grammar are simplified. In this program, some
  advanced operators like look ahead operator /, matching operator ? cannot
  be used.
    We can pay special attention to the sentence "delim {blank}|{\n}|{\t}"
  in the first line of declaration part. blank, \n, and \t are defined
  inside this program. You can simplify this sentence to a normal Lex
  declaration likes "delim [ \t\n]", because the blank is quite difficult to
  handle.
    To run this program, you need to specify the path of .l and the path
  where you want your .c file to be when the program asks to do so. Then,
  open .c file and compile it with a C++ compiler. Finally, we have this
  Lexical Scanner.
    The vital part of the scanner is GetRelop(), which is based on DFA.
  And the original design was adapted from the Dragon Book. So it is designed
  as a strong coupling function with a parser, although you can change it to
  a weak coupling function easily, just as I did in main().

    Some details:
	This program requires a maximum length of variable of 20. So delim, ws,
  and id are OK. On contrary, abbbbbbbbbbbbbbbbbbbb will cause the program
  comes to unpredictable result.
    Escape operator \ should be used before reserved symbol of .l file if
  you want to use this symbol as a letter, such as +, [, {, and * should be
  considered.
*/

const int VARLEN = 20;

int nfastates = 100;
int dfastates = 1000;

int alphabetsize = 0;
int alphabetcap = 60;
char *alphabet = new char[alphabetcap];
const int INCRE = 100;

int nfacount = 0;
int dfacount = 0;
int nfastartstates = 10;
int *nfastartset = new int[nfastartstates];
int nfascount = 0;
int nfafinalstates = 10;
int *nfafinalset = new int[nfafinalstates];
int *dfastateset;
int nfafcount = 0;
char path2[100];


inline int TwoToOneDim(const int, const int, const int);
struct Node
{
	int x;
	Node* next;
};
Node** CreateTable(char*);
int CheckAlphabet(char);
inline int Insert(Node **, int, Node*);
Node **Expand(Node **&, int &, int);
int Print(Node**, int, int);
bool Contain(Node *h, int x);
int length(Node *h);
Node *NextStatesWithECl(Node **a, Node *h, int x);
Node *NextStates(Node **a, Node *h, int x, int);
Node **SubSetConstruct(Node **a, int start, int finalset[]);
Node *EClosure(Node **a, Node *h);
Node *EClosure(Node **a, int row);
bool EqualSets(Node *h1, Node *h2);
Node *Union(Node *h1, Node *h2);
//int del(Node *&);
int Operand(char);
char *InToPost(char *);
int FindDeclare(char *s);
template <class T>T* Expand(T *&a, int &size, int incre);
char *Concat(char *&s1, int &size, char *s2);
char *Regularize(char *s);
bool Escaped(const char *s, int pos);
char *ComputeRE();
char* ConstructAlphabet(char *);
int Remove(Node *h);
Node **Simplify(Node **d);
int Transition(Node **d, int q, char *w, int i);
int Transition(Node **d, int q, char a);
int AddToAlphabet(char c);
bool Distinguishable1Step(Node**d, int row1, int row2, int a[]);
int Replace(Node **d, int x, int y);
int Initial();
char *GenerateCode(Node **d);

int declaresize = 20;
//char **declarations = new char*[declaresize];
//char *declarations[] = { "delim","[ \t\n]","ws","{delim}{delim}*","letter","[a-zA-Z]","digit","[0-9]","id","{letter}({letter}|{digit})*","number","{digit}+" ,NULL };
char **declarations = new char*[declaresize];
int rulesize = 20;
char **rules = new char*[rulesize];
//char *rules[] = { "\"if\"", "{return(IF);}", "{ws}","{/*no action*/}", "\"else\"", "{return(ELSE);}", "{id}", "{yylval = (int) installID(); return (ID);}", "{number}", "{yylval = (int) installNum(); return(NUMBER);}",NULL };



int main()
{
	Initial();
	/*char *s1 = Regularize("([A-Za-z][A-Za-z]*)((a)|(b))*[A-Za-z]");
	cout << (s1) << endl;
	cout << InToPost(s1);*/
	//char*s = "01.";
	//deal with infix to postfix
	char *s = ComputeRE();
	Node**a = CreateTable(s);
	if (nfascount > 1)
		cout << "too many start states";
	//cout << "NFA table" << endl;
	//Print(a, nfacount, alphabetsize);
	Node**d = SubSetConstruct(a, nfastartset[0], nfafinalset);

	/*cout << endl << "DFA table" << endl;
	Print(d, dfacount, alphabetsize);*/
	d = Simplify(d);
	//cout << endl << "Simplified DFA table" << endl;
	//Print(d, dfacount, alphabetsize);
	//Print(d, 10, alphabetsize);
	//for (int i = 0; i < dfacount; i++)
	//	cout << dfastateset[i] << endl;
	alphabet[alphabetsize] = '\0';
	//cout << endl;
	//for (int i = 0; i < alphabetsize; i++)
		//cout << alphabet[i];
	//cout << endl;
	//for (int i = 0; i < nfafcount; i++)
		//cout << nfafinalset[i] << " ";
	//cout<<Regularize("c|(b|(a|(a((a|b)b)*)))");
	/*int q = Transition(d, 0, "if", 0);
	cout << endl << "if  " << q;
	cout << "  " << rules[dfastateset[q] * 2] << rules[dfastateset[q] * 2 + 1];

	q = Transition(d, 0, " \t \n  ", 0);
	cout << endl << "delim  " << q;
	cout << "  " << rules[dfastateset[q] * 2] << rules[dfastateset[q] * 2 + 1];

	q = Transition(d, 0, "else", 0);
	cout << endl << "else  " << q;
	cout << "  " << rules[dfastateset[q] * 2] << rules[dfastateset[q] * 2 + 1];

	q = Transition(d, 0, "a21", 0);
	cout << endl << "id  " << q;
	cout << "  " << rules[dfastateset[q] * 2] << rules[dfastateset[q] * 2 + 1];

	q = Transition(d, 0, "12", 0);
	cout << endl << "number  " << q;
	cout << "  " << rules[dfastateset[q] * 2] << rules[dfastateset[q] * 2 + 1];

	q = Transition(d, 0, "ifa", 0);
	cout << endl << "id  " << q;
	cout << "  " << rules[dfastateset[q] * 2] << rules[dfastateset[q] * 2 + 1];

	q = Transition(d, 0, "1a", 0);
	cout << endl << "dead  " << q;
	//cout << "  " << rules[dfastateset[q] * 2] << rules[dfastateset[q] * 2 + 1];

	q = Transition(d, 0, " 2", 0);
	cout << endl << "dead  " << q;*/



	char *s1 = GenerateCode(d);
	return 1;
}

int Initial()
{
	cout << "Input the path of the .l file:" << endl;
	char s1[100];
	cin >> s1;

	FILE *fp1;
	fopen_s(&fp1, s1, "r");
	if (fp1 == NULL)
	{
		cout << ("fail to open the .l file");
		exit(-1);
	}
	char *s = new char[100];

	cout << "Input the path of the .c file:" << endl;
	cin >> path2;
	FILE *fp2;
	fopen_s(&fp2, path2, "w");
	if (fp2 == NULL)
	{
		cout << ("fail to open the .c file");
		return NULL;
	}

	fprintf_s(fp2, "%s", "char ch;\nint state = 0;\nchar s[100];\nint len = 0;\nint GetRelop();\n");

	char ch1 = fgetc(fp1);
	char ch2 = fgetc(fp1);
	if (ch1 == '%' &&ch2 == '{')
	{
		ch1 = fgetc(fp1);
		while (feof(fp1) == 0)
		{
			ch2 = fgetc(fp1);
			if (ch1 == '%'&& ch2 == '}')
				break;
			fputc(ch1, fp2);
			ch1 = ch2;
		}
		fputc('\n', fp2);
	}
	else
	{
		ungetc(ch2, fp1);
		ungetc(ch1, fp1);
	}



	declarations[0] = "\\n";
	declarations[1] = "\n";
	declarations[2] = "\\t";
	declarations[3] = "\t";
	declarations[4] = "blank";
	declarations[5] = " ";
	for (int i = 6; i < declaresize; i++)
		declarations[i] = NULL;
	for (int i = 0; i < rulesize; i++)
		rules[i] = NULL;
	for (int i = 6; feof(fp1) == 0; i += 2)
	{
		if (i + 2 >= declaresize)
		{
			Expand(declarations, declaresize, INCRE);
		}
		s = new char[VARLEN];
		fscanf_s(fp1, "%s", s, VARLEN);
		if (strcmp(s, "%%") == 0)
			break;
		declarations[i] = s;
		s = new char[100];
		fscanf_s(fp1, "%s", s, 100);
		if (strlen(s) == 1 && s[0] == '[')
		{
			char *stemp = new char[100];
			fscanf_s(fp1, "%s", stemp, 100);
			strcat_s(s, 100, " ");
			strcat_s(s, 100, stemp);
		}
		declarations[i + 1] = s;
	}
	if (strcmp(s, "%%") != 0)
	{
		cout << "no expected %% in .l file";
		system("pause");
		exit(-1);
	}
	for (int i = 0; feof(fp1) == 0; i += 2)
	{
		if (i + 2 >= rulesize)
		{
			Expand(rules, rulesize, INCRE);
		}
		s = new char[VARLEN];
		fscanf_s(fp1, "%s", s, VARLEN);
		if (strcmp(s, "%%") == 0)
			break;
		rules[i] = s;
		s = new char[100];
		fscanf_s(fp1, "%[^\n]", s, 100);
		rules[i + 1] = s;
	}
	if (strcmp(s, "%%") != 0)
	{
		cout << "no expected %% in .l file";
		system("pause");
		exit(-1);
	}
	ch1 = fgetc(fp1);
	while (ch1 != EOF)
	{
		fputc(ch1, fp2);
		ch1 = fgetc(fp1);
	}
	fprintf_s(fp2, "%s", "\nchar NextChar()\n{\n\tch = fgetc(fp1);\n\ts[len] = ch;\n\tlen++;\n\ts[len] = '\\0';\n\treturn ch;\n}\nint Retract()\n{\n\tungetc(ch, fp1);\n\tlen--;\n\ts[len] = '\\0'; \n\treturn 1;\n}\nint ERR()\n{\n\tcout << \"illegal!\";\n\tsystem(\"pause\");\n\texit(-1);\n}");
	fclose(fp1);
	fclose(fp2);
	return 1;
}

char *ComputeRE()
{
	int wlength = 50;
	char *w = new char[wlength];
	w[0] = '\0';
	for (int i = 0; rules[i] != NULL; i += 2)
	{
		if (rules[i][0] == '{')
		{
			char stemp[20];
			int j;
			for (j = 1; rules[i][j] != '\0'; j++)
			{
				stemp[j - 1] = rules[i][j];
			}
			stemp[j - 2] = '\0';//delete '}'
			int di = FindDeclare(stemp);
			if (di == -1)
			{//a variable in rule can not be found in declaration
				cout << "undeclared variable " << stemp << endl;
				continue;
			}
			int l0 = 0;
			int l1 = 1;
			int bodylen = strlen(declarations[di + 1]) * 2;//to have some extra space
			char *body = new char[bodylen];
			strcpy_s(body, bodylen, declarations[di + 1]);
			while (l0 != l1)
			{
				l0 = strlen(body);
				for (int k = 0; body[k] != '\0'; k++)
				{
					if (body[k] == '{' && !Escaped(body, k))
					{//derivation with several steps. We need to translate them into one production
						int j, i;
						char stemp[20];
						for (j = k + 1, i = 0; body[j] != '}'; j++, i++)
						{
							stemp[i] = body[j];
						}
						stemp[i] = '\0';//delete '}'
						int di = FindDeclare(stemp);
						if (di == -1)
						{//a variable in rule can not be found in declaration
							cout << "undeclared variable " << stemp << endl;
							continue;
						}
						if (strlen(declarations[di + 1]) + strlen(body) >= bodylen)
						{
							body = Expand(body, bodylen, strlen(declarations[di + 1]));
						}
						if (strlen(declarations[di + 1]) > strlen(stemp))
							for (i = strlen(body); i > j; i--)
							{//prepare exact empty space
								body[i + strlen(declarations[di + 1]) - strlen(stemp)] = body[i];
							}
						else if (strlen(declarations[di + 1]) < strlen(stemp))
							for (i = j + 1; i <= strlen(body); i++)
							{//prepare exact empty space
								body[i + strlen(declarations[di + 1]) - strlen(stemp)] = body[i];
							}
						body[k] = '(';//'{}' are no longer escape letters in RE, we use '()' to represent the wholesome
						int i2;
						for (i = k + 1, i2 = 0; i2 < strlen(declarations[di + 1]); i++, i2++)
							body[i] = declarations[di + 1][i2];//insert into
						body[i] = ')';
					}
					/*else if (body[k] == '[' && !Escaped(body, k))
					{
						int j, i;
						char stemp[110];//since [a-zA-Z] will be translated to 52+51=113 characters
						stemp[0] = '\0';
						for (j = k + 1, i = 0; body[j] != ']'; j++, i += 2)
						{
							if (body[j + 1] == '-')
							{
								for (char c = body[j]; c <= body[j + 2]; i += 2, c++)
								{
									stemp[i] = c;
									if (body[j + 3] != ']' || c + 1 <= body[j + 2])
										stemp[i + 1] = '|';
									else
										i--;
								}
								i -= 2;
								j += 2;
							}
							else
							{//cases like [acezf]
								stemp[i] = body[j];
								if (body[j + 1] != ']')
									stemp[i + 1] = '|';
								else
									i--;
							}
						}
						stemp[i] = '\0';
						if (strlen(stemp) + strlen(body) >= bodylen)
						{
							body = Expand(body, bodylen, strlen(stemp));
						}
						for (i = strlen(body); i > j; i--)
						{
							body[i + strlen(stemp) - (j - k - 1)] = body[i];
						}
						body[k] = '(';//'{}' are no longer escape letters in RE, we use '()' to represent the wholesome
						int i2;
						for (i = k + 1, i2 = 0; i2 < strlen(stemp); i++, i2++)
							body[i] = stemp[i2];
						body[i] = ')';
					}*/
				}
				l1 = strlen(body);
			}
			char *stemp2 = Regularize(body);
			char *stemp3 = InToPost(stemp2);
			delete[]stemp2;
			Concat(w, wlength, stemp3);
			delete[]stemp3;

		}
		else if (rules[i][0] == '"')
		{
			char stemp[VARLEN];
			if (strlen(rules[i]) > 20)
				cout << "too long rule name";
			else
			{
				for (int j = 1; rules[i][j - 1] != '\0'; j++)
				{//directly handle to the end of the string so that we do not have to care about escaped letters
					stemp[j - 1] = rules[i][j];
					if (rules[i][j] == '\0')
						stemp[j - 2] = '\0';//delete the two "s
				}
			}
			char *stemp2 = Regularize(stemp);
			char *stemp3 = InToPost(stemp2);
			delete[]stemp2;
			Concat(w, wlength, stemp3);
			delete[]stemp3;
		}
		else
		{
			cout << "illegal rules";
		}
	}
	return w;
}

char *Regularize(char *s)
{
	char *s2 = new char[strlen(s) * 2];
	int i, i2;
	for (i = 0, i2 = 0; i < strlen(s); i++, i2++)
	{

		if (s[i] == '[' && !Escaped(s, i))
		{
			for (; s[i] != ']'; i++, i2++)
			{
				s2[i2] = s[i];
			}
		}


		s2[i2] = s[i];
		if (i + 1 < strlen(s))
			if (s[i] != '|' && s[i + 1] != ')' && s[i + 1] != '*' && s[i + 1] != '|' && s[i + 1] != '+' && s[i] != '(' && !Escaped(s, i + 1) && i + 1 < strlen(s))
			{
				i2++;
				s2[i2] = '.';
			}
	}
	s2[i2] = '\0';
	return s2;
}

int FindDeclare(char *s)
{
	for (int i = 0; i < declaresize; i += 2)
		if (strcmp(s, declarations[i]) == 0)
			return i;
	return -1;//not found
}

int AddToAlphabet(char c)
{
	if (alphabetsize + 1 >= alphabetcap)
		Expand(alphabet, alphabetcap, INCRE);
	bool found = false;
	for (int j = 0; j < alphabetsize; j++)
		if (c == alphabet[j])
		{
			found = true;
			break;
		}
	if (!found)
	{
		alphabet[alphabetsize] = c;
		alphabetsize++;
		return 1;
	}
	return -1;
}

char *ConstructAlphabet(char *s)
{
	alphabet[0] = '¦Å';
	alphabetsize = 1;
	for (int i = 0; s[i] != '\0'; i++)
	{
		if (s[i] == '[' && !Escaped(s, i))
		{
			for (i = i + 1; s[i] != ']'; i++)
			{
				if (s[i + 1] == '-')
				{
					for (char c = s[i]; c <= s[i + 2]; c++)
					{
						AddToAlphabet(c);
					}
					i += 2;
				}
				else
				{
					AddToAlphabet(s[i]);
				}
			}
		}
		AddToAlphabet(s[i]);
	}
	return alphabet;
}

Node** CreateTable(char*s)
{
	ConstructAlphabet(s);
	Node **a = new Node*[nfastates*(alphabetsize)];//pay attention to epsilon(we put it in the 0th col)
	for (int i = 0; i < nfastates*(alphabetsize); i++)
		a[i] = NULL;

	for (int i = 0; s[i] != '\0'; i++)
	{
		if (nfacount + 3 >= nfastates)// we need to expand the dynamic array
		{
			a = Expand(a, nfastates, INCRE);
		}
		char c = s[i];
		if (c == '*' || c == '+')
		{
			Node* n = new Node;
			int prestart = nfastartset[nfascount - 1];
			n->x = prestart;
			Insert(a, TwoToOneDim(nfacount, 0, alphabetsize), n);//start to prestart
			nfastartset[nfascount - 1] = nfacount;
			nfacount++;
			int prefinal = nfafinalset[nfafcount - 1];
			nfafinalset[nfafcount - 1] = nfacount;
			n = new Node;
			n->x = nfacount;
			Insert(a, TwoToOneDim(prefinal, 0, alphabetsize), n);//prefinal to final
			n = new Node;
			n->x = prestart;
			Insert(a, TwoToOneDim(prefinal, 0, alphabetsize), n);//prefinal to prestart
			if (c == '*')
			{
				n = new Node;
				n->x = nfacount;
				Insert(a, TwoToOneDim(nfastartset[nfascount - 1], 0, alphabetsize), n);//start to final
			}
			nfacount++;
		}
		else if (c == '|')
		{
			Node* n = new Node;
			int prestart1 = nfastartset[nfascount - 1];
			nfascount--;
			int prestart2 = nfastartset[nfascount - 1];
			n->x = prestart1;
			Insert(a, TwoToOneDim(nfacount, 0, alphabetsize), n);
			n = new Node;
			n->x = prestart2;
			Insert(a, TwoToOneDim(nfacount, 0, alphabetsize), n);
			nfastartset[nfascount - 1] = nfacount;
			nfacount++;
			int prefinal1 = nfafinalset[nfafcount - 1];
			nfafcount--;
			int prefinal2 = nfafinalset[nfafcount - 1];
			n = new Node;
			n->x = nfacount;
			Insert(a, TwoToOneDim(prefinal1, 0, alphabetsize), n);
			n = new Node;
			n->x = nfacount;
			Insert(a, TwoToOneDim(prefinal2, 0, alphabetsize), n);
			nfafinalset[nfafcount - 1] = nfacount;
			nfacount++;
		}
		else if (c == '.')
		{
			int prefinal = nfafinalset[nfafcount - 2];
			int prestart = nfastartset[nfascount - 1];
			Node* n = new Node;
			n->x = prestart;
			Insert(a, TwoToOneDim(prefinal, 0, alphabetsize), n);
			nfafinalset[nfafcount - 2] = nfafinalset[nfafcount - 1];
			nfafcount--;
			nfascount--;
		}
		else if (c == '[')
		{
			if (nfascount + 1 >= nfastartstates)
			{//1 more start states will be recorded
				nfastartset = Expand(nfastartset, nfastartstates, INCRE);
			}
			if (nfafcount + 1 >= nfafinalstates)
			{
				nfafinalset = Expand(nfafinalset, nfafinalstates, INCRE);
			}
			for (i = i + 1; s[i] != ']'; i++)
			{
				c = s[i];
				if (s[i + 1] == '-')
				{
					for (char c = s[i]; c <= s[i + 2]; c++)
					{
						if (c == '\\')
						{//deal with escape letter
							i++;
							c = s[i];
						}
						int x = CheckAlphabet(c);
						Node* n = new Node;
						n->x = nfacount + 1;
						Insert(a, TwoToOneDim(nfacount, x, alphabetsize), n);
					}
					i += 2;
				}
				else
				{//cases like [acezf]
					if (c == '\\')
					{//deal with escape letter
						i++;
						c = s[i];
					}
					int x = CheckAlphabet(c);
					Node* n = new Node;
					n->x = nfacount + 1;
					Insert(a, TwoToOneDim(nfacount, x, alphabetsize), n);
				}
			}
			nfastartset[nfascount] = nfacount;
			nfafinalset[nfafcount] = nfacount + 1;
			nfascount++;
			nfafcount++;
			nfacount += 2;
		}
		else
		{//deal with letters
			if (c == '\\')
			{//deal with escape letter
				i++;
				c = s[i];
			}
			if (nfascount + 1 >= nfastartstates)
			{//1 more start states will be recorded
				nfastartset = Expand(nfastartset, nfastartstates, INCRE);
			}
			if (nfafcount + 1 >= nfafinalstates)
			{
				nfafinalset = Expand(nfafinalset, nfafinalstates, INCRE);
			}
			int x = CheckAlphabet(c);
			Node* n = new Node;
			n->x = nfacount + 1;
			Insert(a, TwoToOneDim(nfacount, x, alphabetsize), n);
			nfastartset[nfascount] = nfacount;
			nfascount++;
			nfafinalset[nfafcount] = nfacount + 1;
			nfafcount++;
			nfacount += 2;

		}
	}
	if (nfascount != nfafcount)
		cout << "num not equal in startstates and finalstates in CreateTable";
	if (nfascount > 1)
	{
		for (int i = 0; i < nfascount; i++)
		{
			Node* n = new Node;
			int prestart1 = nfastartset[i];
			n->x = prestart1;
			Insert(a, TwoToOneDim(nfacount, 0, alphabetsize), n);
		}
		nfastartset[0] = nfacount;
		nfascount = 1;
		nfacount++;
	}
	return a;
}

Node **SubSetConstruct(Node **a, int start, int finalset[])
{
	Node **d = new Node*[dfastates*(alphabetsize)];//pay attention to title for each row(we put it in the 0th col)
	for (int i = 0; i < dfastates*(alphabetsize); i++)
		d[i] = NULL;
	d[0] = EClosure(a, start);
	//cout << "debug1" << endl;
	//Print(a, nfacount, alphabetsize);
	for (int i = 1; i < alphabetsize; i++)
	{
		d[i] = NextStatesWithECl(a, d[0], i);
	}
	//cout << "debug2" << endl;
	//Print(a, nfacount, alphabetsize);
	dfacount++;
	for (int i = 1; i < dfacount*alphabetsize; i++)
	{
		//cout << "debug3" << endl;
		//Print(a, nfacount, alphabetsize);
		if (i%alphabetsize == 0)
			continue;//do not check the title of each row
		if (dfacount + 1 >= dfastates)
			d = Expand(d, dfastates, INCRE);
		bool contain = false;
		for (int j = 0; j < (dfacount + 1)*alphabetsize; j += alphabetsize)
			if (EqualSets(d[j], d[i]))
			{
				if (d[j] != d[i])
				{//if we us exaggerate places, we empty them
					Remove(d[i]);
					d[i] = d[j];
				}
				contain = true;
				break;
			}
		if (!contain)
		{
			d[dfacount*alphabetsize] = d[i];
			for (int k = 1; k < alphabetsize; k++)
			{
				d[dfacount*alphabetsize + k] = NextStatesWithECl(a, d[dfacount*alphabetsize], k);
			}
			dfacount++;
		}
	}
	return d;
}

Node **Simplify(Node **d)
{
	dfastateset = new int[dfacount];
	for (int i = 0, j = 0; j < (dfacount)*alphabetsize; i++, j += alphabetsize)
	{
		int f = -1;
		for (int k = 0; k < nfafcount; k++)
		{//identify finalstates in DFA, based on order priority
			if (Contain(d[j], nfafinalset[k]))
			{
				f = k;
				break;
			}
		}
		dfastateset[i] = f;
		Remove(d[j]->next);
		d[j]->x = i;
		d[j]->next = NULL;
	}

	//operate Hopcroft's Algorithm
	int *temp = new int[dfacount];
	//cout << endl;
	for (int i = 0; i < dfacount; i++)
	{
		temp[i] = dfastateset[i];
		//cout << temp[i] << " ";
	}
	int card = nfafcount + 1;//plus unaccepted class
	bool moreclass = true;
	while (moreclass)
	{
		moreclass = false;
		for (int i = -1; i < card - 1; i++)
		{
			bool moreclass2 = false;
			int newclass = card - 1;
			//cout << endl << "newclass" << newclass << endl;
			for (int j = 0; j < dfacount; j++)
			{
				if (temp[j] == i)
				{
					for (int k = j + 1; k < dfacount; k++)
					{
						if (temp[k] == temp[j] && Distinguishable1Step(d, j, k, temp))
						{
							moreclass = true;
							if (moreclass2 == false)
							{
								moreclass2 = true;
								card++;
							}
							temp[k] = newclass;
						}
					}
				}
			}
		}
	}
	for (int i = 0; i < dfacount; i++)
	{
		for (int j = i + 1; j < dfacount - 1;)
			if (temp[i] == temp[j])
			{
				Replace(d, j, i);
				for (int k = j + 1; k < dfacount; k++)
				{
					temp[k - 1] = temp[k];
					for (int i = k*alphabetsize; i < (k + 1)*alphabetsize; i++)
						d[i - alphabetsize] = d[i];
					d[(k - 1)*alphabetsize]->x--;
					dfastateset[k - 1] = dfastateset[k];
				}
				dfacount--;
			}
			else
				j++;
	}
	return d;
}

char *GenerateCode(Node **d)
{
	int size = 1000000;
	char *s = new char[size];
	s[0] = '\0';
	strcat_s(s, size, "\nint GetRelop()\n{\nstate = 0; len = 0;\nwhile(1) {\n\tswitch(state) {\n");
	for (int i = 0; i < dfacount; i++)
	{
		strcat_s(s, size, "\t\tcase ");
		char stemp[20];
		_itoa_s(i, stemp, 10);
		strcat_s(s, size, stemp);
		strcat_s(s, size, ": ch = NextChar();\n");
		strcat_s(s, size, "\t\t\tswitch(ch) {\n");
		for (int j = 1; j < alphabetsize; j++)
		{
			int next = Transition(d, i, alphabet[j]);
			if (next != -1)
			{
				strcat_s(s, size, "\t\t\t\tcase '");
				if (alphabet[j] == '\n')
				{//special case for escaped letters
					stemp[0] = '\\';
					stemp[1] = 'n';
					stemp[2] = '\0';
				}
				else if (alphabet[j] == '\t')
				{
					stemp[0] = '\\';
					stemp[1] = 't';
					stemp[2] = '\0';
				}
				else
				{
					stemp[0] = alphabet[j];
					stemp[1] = '\0';
				}
				strcat_s(s, size, stemp);
				strcat_s(s, size, "': state = ");
				_itoa_s(next, stemp, 10);
				strcat_s(s, size, stemp);
				strcat_s(s, size, "; break;\n");
			}
		}
		strcat_s(s, size, "\t\t\t\tdefault: ");
		if (dfastateset[i] != -1)
		{
			strcat_s(s, size, "Retract();");
			strcat_s(s, size, rules[dfastateset[i] * 2 + 1]);
			strcat_s(s, size, "; break;\n");
		}
		else
		{
			strcat_s(s, size, "ERR();\n");
		}
		strcat_s(s, size, "\t\t\t} break;\n");
	}
	strcat_s(s, size, "\t\t}}}");
	FILE *fp2;
	fopen_s(&fp2, path2, "a");
	if (fp2 == NULL)
	{
		cout << ("fail to open the .c file");
		return NULL;
	}
	fprintf_s(fp2, "%s", s);
	fclose(fp2);
	return s;
}

bool Distinguishable1Step(Node**d, int row1, int row2, int a[])
{
	for (int i = 0; i < alphabetsize; i++)
	{
		int q1 = Transition(d, row1, alphabet[i]);
		int q2 = Transition(d, row2, alphabet[i]);
		if ((!(q1 == -1 && q2 == -1)) && ((q1 == -1 && q2 != -1) || (q1 != -1 && q2 == -1) || (a[q1] != a[q2])))
			return true;
	}
	return false;
}

int Replace(Node **d, int x, int y)
{
	for (int i = 0; i < dfacount*alphabetsize; i += alphabetsize)
	{
		if (d[i]->x == x)
		{
			d[i]->x = y;
			break;
		}
	}
	return 1;
}

char *InToPost(char *s)
{
	int l = strlen(s);
	char *s2 = new char[l + 2];
	char *op = new char[l + 2];
	int i = 0;
	int i2 = 0;
	int i3 = 0;
	for (i = 0; s[i] != '\0'; i++)
	{
		int x = Operand(s[i]);
		if (x > 0 && !Escaped(s, i))
		{
			int j;
			for (j = i3 - 1; j >= 0; j--)
			{
				if (Operand(op[j]) >= x && s[i] != '(')
				{
					s2[i2] = op[j];
					i2++;
					i3--;
				}
				else
				{
					op[i3] = s[i];
					i3++;
					break;
				}
			}
			if (j < 0 && i3 != 0)
				cout << "i3 miscount in InToPost";
			if (j < 0)
			{
				op[i3] = s[i];
				i3++;
			}
		}
		else if (s[i] == ')' && !Escaped(s, i))
		{
			int j;
			for (j = i3 - 1; j >= 0; j--)
			{
				if (op[j] != '(')
				{
					s2[i2] = op[j];
					i2++;
					i3--;
				}
				else
				{
					i3--;
					break;
				}
			}
		}
		else
		{
			if (s[i] == '[' && !Escaped(s, i))
			{
				for (; s[i] != ']'; i++, i2++)
				{
					s2[i2] = s[i];
				}
			}
			s2[i2] = s[i];
			i2++;
		}
	}
	for (int j = i3 - 1; j >= 0; j--)
	{
		s2[i2] = op[j];
		i2++;
	}
	delete[]op;
	op = NULL;
	s2[i2] = '\0';
	return s2;
}

int CheckAlphabet(char c)
{
	for (int i = 0; i < alphabetsize; i++)
	{
		if (alphabet[i] == c)
			return i;
	}
	cout << "CheckAlphabet error";
	return -1;
}

inline int TwoToOneDim(int row, int column, int totalCol)
{
	return row*totalCol + column;
}

inline int Insert(Node **a, int pos, Node* p)
{
	Node** p2 = &a[pos];
	p->next = *p2;
	*p2 = p;
	return 1;
}

template <class T>
T* Expand(T *&a, int &size, int incre)
{
	T *p = new T[size + incre];
	for (int j = 0; j < size; j++)
		p[j] = a[j];
	for (int j = size; j < size + incre; j++)
		p[j] = NULL;
	delete[]a;
	a = p;
	size += incre;
	return p;
}

int Remove(Node *h)
{
	while (h != NULL)
	{
		Node *p = h;
		h = h->next;
		delete p;
	}
	return 1;
}

Node **Expand(Node **&a, int &states, int incre)
{
	Node **p = new Node*[(states + incre)*(alphabetsize)];
	for (int j = 0; j < states*alphabetsize; j++)
		p[j] = a[j];
	delete[]a;
	for (int j = states*alphabetsize; j < (states + incre)*(alphabetsize); j++)
		p[j] = NULL;
	a = p;
	states += incre;
	return a;
}

int Print(Node** a, int row, int col)
{
	for (int i = 0; i < row; i++)
	{
		cout << i << ' ';
		for (int j = 0; j < col; j++)
		{
			cout << '{';
			for (Node *p = a[i*col + j]; p != NULL; p = p->next)
			{
				cout << p->x << ',';
			}
			cout << '}';
		}
		cout << endl;
	}
	return 1;
}

bool Contain(Node *h, int x)
{
	for (Node *p = h; p != NULL; p = p->next)
	{
		if (p->x == x)
			return true;
	}
	return false;
}

bool EqualSets(Node *h1, Node *h2)
{
	if (h1 == h2)
		return true;
	for (Node *p = h1; p != NULL; p = p->next)
		if (!Contain(h2, p->x))
			return false;
	for (Node *p = h2; p != NULL; p = p->next)
		if (!Contain(h1, p->x))
			return false;
	return true;
}

Node *NextStatesWithECl(Node **a, Node *h, int x)
{
	Node *h1 = NextStates(a, h, x, length(h));
	h1 = EClosure(a, h1);
	return h1;
}

Node *EClosure(Node **a, int row)
{
	Node *h = new Node;
	h->x = row;
	h->next = NULL;
	h = Union(h, a[row*alphabetsize]);
	h = EClosure(a, h);
	return h;
}

Node *EClosure(Node **a, Node *h)
{
	int l1 = 0;
	int l2 = length(h);
	while (l1 != l2)
	{
		int add = l2 - l1;
		l1 = length(h);
		Node *temp = NextStates(a, h, 0, add);
		h = Union(h, temp);
		Remove(temp);
		temp = NULL;
		l2 = length(h);
	}
	return h;
}

Node *NextStates(Node **a, Node *h, int x, int add)
{
	Node *h1 = NULL;
	Node *p;
	int i;
	for (p = h, i = 0; p != NULL && i < add; p = p->next)
	{
		//cout << p->x << " " << x << " " << a[p->x*alphabetsize + x]->x << endl;
		h1 = Union(h1, a[p->x*alphabetsize + x]);
	}
	return h1;
}

Node *Union(Node *h1, Node *h2)
{//We should pay attention to memory leak
 /*Node *h = NULL;

 for (Node *p = h1; p != NULL; p = p->next)
 {
 if (!Contain(h, p->x))
 {
 Node *q = new Node;
 q->x = p->x;
 q->next = h;
 h = q;
 }
 }
 for (Node *p = h2; p != NULL; p = p->next)
 {
 if (!Contain(h, p->x))
 {
 Node *q = new Node;
 q->x = p->x;
 q->next = h;
 h = q;
 }
 }
 return h;*/
 /*for (Node *p = h2; p != NULL; )
 {
 Node *r = p->next;
 if (!Contain(h1, p->x))
 {
 p->next = h1;
 h1 = p;
 }
 else
 {
 delete p;
 }
 p = r;
 }*/
	for (Node *p = h2; p != NULL; p = p->next)
	{
		if (!Contain(h1, p->x))
		{//insert in the head
			Node *q = new Node;
			q->x = p->x;
			q->next = h1;
			h1 = q;
		}
	}
	return h1;
}

int length(Node *h)
{
	int i = 0;
	for (Node *p = h; p != NULL; p = p->next, i++);
	return i;
}

/*int del(Node *&h)
{
for (Node *p = h; p != NULL;)
{
Node *q = p->next;
delete p;
p = q;
}
h = NULL;
return 1;
}*/

int Operand(char c)
{
	switch (c)
	{
	case '.':return 5;
	case '|':return 2;
	case '*':return 10;
	case '+':return 10;
	case '(':return 1;
	default:return -1;
	}
}

char *Concat(char *&s1, int &size, char *s2)
{
	//cout << s1 << endl;
	if (strlen(s1) + strlen(s2) >= size)
		s1 = Expand(s1, size, strlen(s2));
	//cout << size << endl;
	//cout << s1 << endl;
	//cout << s2 << endl;
	strcat_s(s1, size, s2);
	return s1;
}

bool Escaped(const char *s, int pos)
{
	if (pos == 0)
		return false;
	if (s[pos - 1] == '\\' && !Escaped(s, pos - 1))
		return true;
	else
		return false;
}

int Transition(Node **d, int q, char *w, int i)
{
	if (q == -1)
		return -1;
	if (w[i] == '\0')
		return q;
	return (Transition(d, Transition(d, q, w[i]), w, i + 1));
}

int Transition(Node **d, int q, char a)
{
	if (d == NULL || q == -1)
		return -1;
	int i = CheckAlphabet(a);
	if (i == -1)
		return -1;
	Node *h = d[q*alphabetsize + i];
	if (h == NULL)
		return -1;
	return h->x;
}