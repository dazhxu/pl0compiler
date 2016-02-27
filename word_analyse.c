#include"compiler.h"

extern FILE *FIN;  //文件输入指针
extern FILE *FOUT;	//文件读出指针
extern char fp[20];		//文件名
extern int cifafenxi;

int linenum=1;//存行数
int errornum=1;//错误数
int errorline[MAXERROR];//错误行号
int error[MAXERROR];//错误数
struct table tbl[MAXTABLE];//符号表
int top_of_table=0;//符号表栈顶
int initial_table_index;//初始符号表索引
struct pcode pcod[MAXPCODE];//中间代码
int top_of_pcode=1;//中间代码栈顶
int initial_pcode_index;//初始中间代码索引
struct runstack run[MAXSTACK];//运行栈
int top_of_runstack=0;//运行栈栈顶
int num_of_var[MAXTABLE];//记录各层变量个数
int top_of_numofvar=0;//num_of_var栈顶
struct runstack parastack[MAXVARDEFINE];//记录参数值
int top_of_parastack=0;//parastack栈顶
struct runstack funcstack[MAXVARDEFINE];//记录函数返回值
int top_of_funcstack=-1;//funcstack栈顶
int func_flag[MAXVARDEFINE];//记录是否为函数
int top_of_funcflag=0;
char printstring[100][MAXSTRING];//存放输出的字符串
int top_of_printstr=0;
int staticlink[MAXSTACK];//静态链，存放上层过程的基地址
int top_of_sl=0;
int dynamiclink[MAXSTACK];//动态链接，存放调用函数的基地址
int top_of_dl=0;
int return_adr[MAXPCODE];//返回地址,存放返回地址
int top_of_ra=0;



int lev=0;//当前层次
int temp_linenum;//临时行数
char name[MAXTABLE][MAXWORD];		//标识符名字
int arraylength;//数组长度
int data_allocation_index=3;//数据分配索引
int func_assaign_flag=0;//标记函数中是否有函数赋值语句
int base=0;//基址
int p=0;//目标代码索引
int t=0;//运行栈顶
char print_string[1000]="";

char word[MAXWORD]; //存token
int sym;	//存词法符号
int num;	//存整数
double fnum;	//存实数
char symchar;	//存字符
char symstring[MAXSTRING];	//存字符串

char *Keyword[]={	"const","integer","real","char",				//关键字
	"var","array","of","if","then","else",
	"switch","case", "for","to","downto","do",
	"procedure","function",
	"read","write","begin","end","odd","while"
};

char *symtype[]={	"","IDEN","INTCON","REALCON","CHARCON","STRCON",  //词法分析输出使用
	"CONSTTK","INTTK","FLOATTK","CHARTK","VARTK","ARRAYTK","OFTK",
	"IFTK","THENTK","ELSETK","SWITCHTK","CASETK","FORTK","TOTK",
	"DOWNTOTK","DOTK","PROCETK","FUNCTK","READTK","WRITETK","BEGINTK",
	"ENDTK","ODDTK","PLUS","MINU","MULT","DIV","LSS","LEQ","GRE","GEQ",
	"EQL","NEQ","ASSIGN","SEMICN","COMMA","PERIOD","COLON","QMARK",
	"DQMARK","LPARENT","RPARENT","LBRACK","RBRACK","LBRACE","RBRACE","WHILETK"
};

char *operation[]={"LIT",
	"OPR",
	"LODADR",
	"LOD",
	"STO",
	"CAL",
	"INT",
	"JMP",
	"JPC",
	"RED",
	"WRT",
};

char* err_msg[] =					//定义出错提示信息
{
/*0*/ "",
/*1*/ "标识符或数字过长或数字有前导0",
/*2*/ "小数点前缺少数字",
/*3*/ "字符非法或缺少\'",
/*4*/ "字符串非法或过长或缺少\"",
/*5*/ "非法单词或非法语法结构",
/*6*/ "缺少.",
/*7*/ "缺少,或;",
/*8*/ "缺少标识符",
/*9*/ "此处应有'='",
/*10*/ "常量定义非法",
/*11*/ "缺少,或:",
/*12*/ "缺少数组长度或下标或数组长度下标错误",
/*13*/ "缺少‘[’或‘]’",
/*14*/ "缺少类型",
/*15*/ "缺少of",
/*16*/ "标识符重复定义",
/*17*/ "符号表越界",
/*18*/ "复合语句或语句处理错误",
/*19*/ "编译生成目标代码数过多",
/*20*/ "缺少“（”或“）”",
/*21*/ "缺少过程名或函数名",
/*22*/ "未定义的标识符",
/*23*/ "实参与形参类型不匹配",
/*24*/ "参数传递错误或形参与实参长度不匹配或缺少“）”",
/*25*/ "未识别标识符",
/*26*/ "缺少或未识别的因子",
/*27*/ "缺少:=",
/*28*/ "缺少条件判断符号",
/*29*/ "缺少then",
/*30*/ "此类型不能被赋值",
/*31*/ "缺少to或downto",
/*32*/ "缺少do",
/*33*/  "case语句中情况表类型错误",
/*34*/  "函数中缺少函数赋值语句",
/*35*/  "实型不能赋给整形和字符型",
/*36*/  "缺少“（”或“）”或非法输出结构",
/*37*/  "此标识符类型不可读",
/*38*/  "缺少，或end",
/*39*/  "case后表达式不能为实型",
};

//词法分析
int getsym()
{
	char ch;
	int i=0,j=0,m=0,flag=0;

	ch=fgetc(FIN);
	if(ch==EOF)
		return FILEOVER;
	//跳过空格
	while(isspace(ch))
	{
		putc(ch,FOUT);
		if(ch=='\n')
		{
			linenum++;//遇见回车符，行号加1
			fprintf(FOUT,"%d: ",linenum);
		}
		ch=fgetc(FIN);
		if(ch==EOF)
			return FILEOVER;
	}
	
	word[i++]=ch;


	//判断是否为标识符
	if(isalpha(ch))
	{
		putc(ch,FOUT);
		ch=fgetc(FIN);
		while(isalpha(ch)||isdigit(ch))
		{
			word[i++]=ch;
			//判断标识符是否过长，过长则截断
			if(i==MAXWORD)
			{
				errorline[errornum]=linenum;
				error[errornum++]=1;
				i--;
				word[i]='\0';
				ungetc(ch,FIN);
			//	printf("%d %s %s\n",cifafenxi++,symtype[IDEN],word);
				return IDEN;
			}
			putc(ch,FOUT);
			ch=fgetc(FIN);
		}
		ungetc(ch,FIN);
		word[i]='\0';

		for(m=0;m<24;m++)
		{
			if(!(strcmp(Keyword[m],word)))	//读取的字符串符合对key word的判断
			{
				if(m==23)//处理while,只为词法分析 文法中无
				{
				//	printf("%d %s %s\n",cifafenxi++,symtype[WHILETK],word);
					return WHILETK;//返回相应的keyword的对应数字
				}
			//	printf("%d %s %s\n",cifafenxi++,symtype[m+6],word);
				return m+6;//返回相应的keyword的对应数字
			}
		}
	//	printf("%d %s %s\n",cifafenxi++,symtype[IDEN],word);
		return IDEN;
	}


	//处理无符号整数或无符号实数
	if(isdigit(ch))
	{
		putc(ch,FOUT);
		ch=getc(FIN);
		while(isdigit(ch))
		{
			word[i++]=ch;
			if(i==MAXWORD)//长度大于最大值，截断，并产生错误
			{
				errorline[errornum]=linenum;
				error[errornum++]=1;
				i--;
				word[i]='\0';
				ungetc(ch,FIN);
				num=atoi(word);
			//	printf("%d %s %s\n",cifafenxi++,symtype[INTCON],word);
				return INTCON;
			}
			putc(ch,FOUT);
			ch=getc(FIN);
		}
		if(ch=='.')//判断是否为无符号实数
		{
			word[i++]=ch;
			putc(ch,FOUT);
			ch=getc(FIN);
			if(!isdigit(ch))//小数点后无数字，产生错误
			{
				errorline[errornum]=linenum;
				error[errornum++]=2;
			}
			while(isdigit(ch))
			{
				word[i++]=ch;
				if(i==MAXWORD)//长度大于最大值，截断，并产生错误
				{
					errorline[errornum]=linenum;
					error[errornum++]=1;
					i--;
					word[i]='\0';
					ungetc(ch,FIN);
					fnum=atof(word);
				//	printf("%d %s %s\n",cifafenxi++,symtype[REALCON],word);
					return REALCON;
				}
				putc(ch,FOUT);
				ch=getc(FIN);
			}
			word[i]='\0';
			ungetc(ch,FIN);
			fnum=atof(word);
		//	printf("%d %s %s\n",cifafenxi++,symtype[REALCON],word);
			return REALCON;//单词为无符号实数
		}
		word[i]='\0';
		ungetc(ch,FIN);
		num=atoi(word);
	//	printf("%d %s %s\n",cifafenxi++,symtype[INTCON],word);
		return INTCON;//单词为无符号整数
	}


	//处理字符常量
	if(ch=='\'')
	{
		putc(ch,FOUT);
		ch=fgetc(FIN);
		if(isalpha(ch)||isdigit(ch))
		{
			word[i++]=ch;
			symchar=ch;
			putc(ch,FOUT);
			ch=fgetc(FIN);
			if(ch=='\'')
			{
				putc(ch,FOUT);
				word[i++]=ch;
				word[i]='\0';
		//		printf("%d %s %s\n",cifafenxi++,symtype[CHARCON],word);
				return CHARCON;//单词为字符常量
			}
			else//缺少'
			{
				errorline[errornum]=linenum;
				error[errornum++]=3;
				while(!isspace(ch))//跳过其余字符直到空白符
				{
					putc(ch,FOUT);
					ch=fgetc(FIN);
				}
				ungetc(ch,FIN);
				return CHARCON;//删去
			}
		}
		else//字符常量非法
		{
			errorline[errornum]=linenum;
			error[errornum++]=3;
			while(!isspace(ch))//跳过其余字符直到空白符
			{
				putc(ch,FOUT);
				ch=fgetc(FIN);
			}
			ungetc(ch,FIN);
			return CHARCON;//删去
		}
	}


	//处理字符串常量
	if(ch=='\"')
	{
		i--;
		putc(ch,FOUT);
		ch=fgetc(FIN);
		while(ch>=32&&ch<=126&&ch!=34)
		{
			word[i++]=ch;
			if(i==MAXSTRING)//长度大于字符串最大值，截断，并产生错误
			{
				errorline[errornum]=linenum;
				error[errornum++]=4;
				i--;
				word[i]='\0';
				strcpy(symstring,word);
				ungetc(ch,FIN);
//				printf("%d %s %s\n",cifafenxi++,symtype[STRCON],word);
				return STRCON;
			}
			putc(ch,FOUT);
			ch=fgetc(FIN);
		}
		if(ch=='\"')
		{
			putc(ch,FOUT);
			//word[i]=ch;
			word[i]='\0';
			strcpy(symstring,word);
	//		printf("%d %s %s\n",cifafenxi++,symtype[STRCON],word);
			return STRCON;
		}
		else//缺少"或字符串字符非法
		{
			errorline[errornum]=linenum;
			error[errornum++]=4;
			while(!isspace(ch))//跳过其余字符直到空白符
			{
				putc(ch,FOUT);
				ch=fgetc(FIN);
			}
			ungetc(ch,FIN);
			return STRCON;//删去
		}
	}


	//处理<  <=  <>
	if(ch=='<')
	{
		putc(ch,FOUT);
		ch=fgetc(FIN);
		if(ch=='=')//判断<=
		{
			putc(ch,FOUT);
			word[i++]=ch;
			word[i]='\0';
//			printf("%d %s %s\n",cifafenxi++,symtype[LEQ],word);
			return LEQ;
		}
		if(ch=='>')//判断<>
		{
			putc(ch,FOUT);
			word[i++]=ch;
			word[i]='\0';
//			printf("%d %s %s\n",cifafenxi++,symtype[NEQ],word);
			return NEQ;
		}
		word[i]='\0';
		ungetc(ch,FIN);
//		printf("%d %s %s\n",cifafenxi++,symtype[LSS],word);
		return LSS;//单词为<
	}


	//处理>  >=
	if(ch=='>')
	{
		putc(ch,FOUT);
		ch=fgetc(FIN);
		if(ch=='=')//判断>=
		{
			putc(ch,FOUT);
			word[i++]=ch;
			word[i]='\0';
//			printf("%d %s %s\n",cifafenxi++,symtype[GEQ],word);
			return GEQ;
		}
		word[i]='\0';
		ungetc(ch,FIN);
//		printf("%d %s %s\n",cifafenxi++,symtype[GRE],word);
		return GRE;//单词为>
	}


	//处理:  :=
	if(ch==':')
	{
		putc(ch,FOUT);
		ch=fgetc(FIN);
		if(ch=='=')//判断:=
		{
			putc(ch,FOUT);
			word[i++]=ch;
			word[i]='\0';
//			printf("%d %s %s\n",cifafenxi++,symtype[ASSIGN],word);
			return ASSIGN;
		}
		word[i]='\0';
		ungetc(ch,FIN);
//		printf("%d %s %s\n",cifafenxi++,symtype[COLON],word);
		return COLON;//单词为:
	}

	//处理+
	if(ch=='+')
	{
		putc(ch,FOUT);
		word[i]='\0';
//		printf("%d %s %s\n",cifafenxi++,symtype[PLUS],word);
		return PLUS;//单词为+
	}


	//处理-
	if(ch=='-')
	{
		putc(ch,FOUT);
		word[i]='\0';
//		printf("%d %s %s\n",cifafenxi++,symtype[MINU],word);
		return MINUS;//单词为-
	}


	//处理*
	if(ch=='*')
	{
		putc(ch,FOUT);
		word[i]='\0';
//		printf("%d %s %s\n",cifafenxi++,symtype[MULT],word);
		return MULT;//单词为
	}


	//处理/
	if(ch=='/')
	{
		putc(ch,FOUT);
		word[i]='\0';
//		printf("%d %s %s\n",cifafenxi++,symtype[DIV],word);
		return DIV;//单词为
	}


	//处理=
	if(ch=='=')
	{
		putc(ch,FOUT);
		word[i]='\0';
//		printf("%d %s %s\n",cifafenxi++,symtype[EQL],word);
		return EQL;//单词为=
	}


	//处理;
	if(ch==';')
	{
		putc(ch,FOUT);
		word[i]='\0';
//		printf("%d %s %s\n",cifafenxi++,symtype[SEMICN],word);
		return SEMICN;//单词为;
	}


	//处理,
	if(ch==',')
	{
		putc(ch,FOUT);
		word[i]='\0';
//		printf("%d %s %s\n",cifafenxi++,symtype[COMMA],word);
		return COMMA;//单词为,
	}


	//处理.
	if(ch=='.')
	{
		putc(ch,FOUT);
		word[i]='\0';
//		printf("%d %s %s\n",cifafenxi++,symtype[PERIOD],word);
		return PERIOD;//单词为.
	}


	//处理(
	if(ch=='(')
	{
		putc(ch,FOUT);
		word[i]='\0';
//		printf("%d %s %s\n",cifafenxi++,symtype[LPARENT],word);
		return LPARENT;//单词为(
	}


	//处理)
	if(ch==')')
	{
		putc(ch,FOUT);
		word[i]='\0';
//		printf("%d %s %s\n",cifafenxi++,symtype[RPARENT],word);
		return RPARENT;//单词为)
	}


	//处理[
	if(ch=='[')
	{
		putc(ch,FOUT);
		word[i]='\0';
//		printf("%d %s %s\n",cifafenxi++,symtype[LBRACK],word);
		return LBRACK;//单词为[
	}


	//处理]
	if(ch==']')
	{
		putc(ch,FOUT);
		word[i]='\0';
//		printf("%d %s %s\n",cifafenxi++,symtype[RBRACK],word);
		return RBRACK;//单词为]
	}



	//处理{
	if(ch=='{')
	{
		putc(ch,FOUT);
		word[i]='\0';
//		printf("%d %s %s\n",cifafenxi++,symtype[LBRACE],word);
		return LBRACE;//单词为{
	}


	//处理}
	if(ch=='}')
	{
		putc(ch,FOUT);
		word[i]='\0';
//		printf("%d %s %s\n",cifafenxi++,symtype[RBRACE],word);
		return RBRACE;//单词为}
	}
	if(ch==EOF)
		return FILEOVER;
	//未识别单词符号
	errorline[errornum]=linenum;
	error[errornum++]=5;
	return 0;
}
