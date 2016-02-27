#include"compiler.h"

FILE *FIN;  //文件输入指针
FILE *FOUT;	//文件读出指针
char fp[20];		//文件名

int cifafenxi;//词法分析序号

extern int linenum;//存行数
extern int errornum;//错误数
extern int errorline[MAXERROR];//错误行号
extern int error[MAXERROR];//错误数
extern struct table tbl[MAXTABLE];//符号表
extern int top_of_table;//符号表栈顶
extern struct pcode pcod[MAXPCODE];//中间代码
extern int top_of_pcode;//中间代码栈顶
extern struct runstack run[MAXSTACK];//运行栈
extern int top_of_runstack;//运行栈栈顶
extern int num_of_var[MAXTABLE];//记录各层变量个数
extern int top_of_numofvar;//num_of_var栈顶
extern struct runstack parastack[MAXVARDEFINE];//记录参数值
extern int top_of_parastack;//parastack栈顶
extern struct runstack funcstack[MAXVARDEFINE];//记录函数返回值
extern int top_of_funcstack;//funcstack栈顶
extern char printstring[100][MAXSTRING];//存放输出的字符串
extern int top_of_printstr;

extern int lev;//当前层
extern int temp_linenum;//记录临时行数
extern char name[MAXTABLE][MAXWORD];		//标识符名字
extern int arraylength;//数组长度
extern int data_allocation_index;//数据分配索引
extern int func_assaign_flag;//标记函数中是否有函数赋值语句
extern int base;//基址
extern int p;//目标代码索引
extern int t;//运行栈顶

extern char word[MAXWORD]; //存token
extern int sym;	//存词法符号
extern int num;	//存整数
extern double fnum;	//存实数
extern char symchar;	//存字符
extern char symstring[MAXSTRING];	//存字符串
extern char print_string[1000];

extern char *Keyword[];//关键字
extern char *symtype[];//词法类型
extern char *operation[];//目标代码操作
extern char *err_msg[];//错误类型

//层差为l的数据区的基址
int basefind(int l)
{
	int b1;
	b1=base;
	while(l>0)
	{
		b1=run[b1].x;
		l--;
	}
	return b1;
}

int lit()
{
	switch(pcod[p].lev)
	{
		case INT_TYPE:
			run[t].x=pcod[p].x;
			run[t++].kind=INT_TYPE;
			break;
		case REAL_TYPE:
			run[t].fx=pcod[p].fx;
			run[t++].kind=REAL_TYPE;
			break;
		case CHAR_TYPE:
			run[t].x=pcod[p].x;
			run[t++].kind=CHAR_TYPE;
			break;
		default:
			fprintf(FOUT,"lit() error! PCode line %d\n",p);
			return 0;
	}
	return 1;
}

int opr()
{
	switch(pcod[p].x)
	{
		case 0:
			t=base;
			p=run[t+2].x;
			base=run[t+1].x;
			break;
		case 1:
			if(run[t-1].kind==INT_TYPE||run[t-1].kind==CHAR_TYPE)
				run[t-1].x=-run[t-1].x;
			else if(run[t-1].kind==REAL_TYPE)
				run[t-1].fx=-run[t-1].fx;
			break;
		case 2:
			t=t-1;
			if(run[t-1].kind==REAL_TYPE&&run[t].kind==REAL_TYPE)
			{
				run[t-1].fx=run[t-1].fx+run[t].fx;
				run[t-1].kind=REAL_TYPE;
			}
			else if(run[t-1].kind==REAL_TYPE&&run[t].kind!=REAL_TYPE)
			{
				run[t-1].fx=run[t-1].fx+run[t].x;
				run[t-1].kind=REAL_TYPE;
			}
			else if(run[t-1].kind!=REAL_TYPE&&run[t].kind==REAL_TYPE)
			{
				run[t-1].fx=run[t-1].x+run[t].fx;
				run[t-1].kind=REAL_TYPE;
			}
			else if(run[t-1].kind!=REAL_TYPE&&run[t].kind!=REAL_TYPE)
			{
				run[t-1].x=run[t-1].x+run[t].x;
				run[t-1].kind=INT_TYPE;
			}
			break;
		case 3:
			t=t-1;
			if(run[t-1].kind==REAL_TYPE&&run[t].kind==REAL_TYPE)
			{
				run[t-1].fx=run[t-1].fx-run[t].fx;
				run[t-1].kind=REAL_TYPE;
			}
			else if(run[t-1].kind==REAL_TYPE&&run[t].kind!=REAL_TYPE)
			{
				run[t-1].fx=run[t-1].fx-run[t].x;
				run[t-1].kind=REAL_TYPE;
			}
			else if(run[t-1].kind!=REAL_TYPE&&run[t].kind==REAL_TYPE)
			{
				run[t-1].fx=run[t-1].x-run[t].fx;
				run[t-1].kind=REAL_TYPE;
			}
			else if(run[t-1].kind!=REAL_TYPE&&run[t].kind!=REAL_TYPE)
			{
				run[t-1].x=run[t-1].x-run[t].x;
				run[t-1].kind=INT_TYPE;
			}
			break;
		case 4:
			t=t-1;
			if(run[t-1].kind==REAL_TYPE&&run[t].kind==REAL_TYPE)
			{
				run[t-1].fx=run[t-1].fx*run[t].fx;
				run[t-1].kind=REAL_TYPE;
			}
			else if(run[t-1].kind==REAL_TYPE&&run[t].kind!=REAL_TYPE)
			{
				run[t-1].fx=run[t-1].fx*run[t].x;
				run[t-1].kind=REAL_TYPE;
			}
			else if(run[t-1].kind!=REAL_TYPE&&run[t].kind==REAL_TYPE)
			{
				run[t-1].fx=run[t-1].x*run[t].fx;
				run[t-1].kind=REAL_TYPE;
			}
			else if(run[t-1].kind!=REAL_TYPE&&run[t].kind!=REAL_TYPE)
			{
				run[t-1].x=run[t-1].x*run[t].x;
				run[t-1].kind=INT_TYPE;
			}
			break;
		case 5:
			t=t-1;
			if(run[t-1].kind==REAL_TYPE&&run[t].kind==REAL_TYPE)
			{
				if(run[t].fx==0)
				{
					fprintf(FOUT,"error! PCode line %d：除数不能为0",p);
					return 0;
				}
				run[t-1].fx=run[t-1].fx/run[t].fx;
				run[t-1].kind=REAL_TYPE;
			}
			else if(run[t-1].kind==REAL_TYPE&&run[t].kind!=REAL_TYPE)
			{
				if(run[t].x==0)
				{
					fprintf(FOUT,"error! PCode line %d：除数不能为0",p);
					return 0;
				}
				run[t-1].fx=run[t-1].fx/run[t].x;
				run[t-1].kind=REAL_TYPE;
			}
			else if(run[t-1].kind!=REAL_TYPE&&run[t].kind==REAL_TYPE)
			{
				if(run[t].fx==0)
				{
					fprintf(FOUT,"error! PCode line %d：除数不能为0",p);
					return 0;
				}
				run[t-1].fx=run[t-1].x/run[t].fx;
				run[t-1].kind=REAL_TYPE;
			}
			else if(run[t-1].kind!=REAL_TYPE&&run[t].kind!=REAL_TYPE)
			{
				if(run[t].x==0)
				{
					fprintf(FOUT,"error! PCode line %d：除数不能为0",p);
					return 0;
				}
				run[t-1].x=run[t-1].x/run[t].x;
				run[t-1].kind=INT_TYPE;
			}
			break;
		case 6:
			if(run[t-1].kind!=REAL_TYPE)
			{
				fprintf(FOUT,"error! PCode line %d：实型不能判断奇偶",p);
					return 0;
			}
			run[t-1].x=((run[t-1].x%2)==1)?1:0;
			break;
		case 7:
			t=t-1;
			if(run[t-1].kind==REAL_TYPE&&run[t].kind==REAL_TYPE)
			{
				run[t-1].x=(run[t-1].fx==run[t].fx)?1:0;
				run[t-1].kind=INT_TYPE;
			}
			else if(run[t-1].kind==REAL_TYPE&&run[t].kind!=REAL_TYPE)
			{
				run[t-1].x=(run[t-1].fx==run[t].x)?1:0;
				run[t-1].kind=INT_TYPE;
			}
			else if(run[t-1].kind!=REAL_TYPE&&run[t].kind==REAL_TYPE)
			{
				run[t-1].x=(run[t-1].x==run[t].fx)?1:0;
				run[t-1].kind=INT_TYPE;
			}
			else if(run[t-1].kind!=REAL_TYPE&&run[t].kind!=REAL_TYPE)
			{
				run[t-1].x=(run[t-1].x==run[t].x)?1:0;
				run[t-1].kind=INT_TYPE;
			}
			break;
		case 8:
			t=t-1;
			if(run[t-1].kind==REAL_TYPE&&run[t].kind==REAL_TYPE)
			{
				run[t-1].x=(run[t-1].fx!=run[t].fx)?1:0;
				run[t-1].kind=INT_TYPE;
			}
			else if(run[t-1].kind==REAL_TYPE&&run[t].kind!=REAL_TYPE)
			{
				run[t-1].x=(run[t-1].fx!=run[t].x)?1:0;
				run[t-1].kind=INT_TYPE;
			}
			else if(run[t-1].kind!=REAL_TYPE&&run[t].kind==REAL_TYPE)
			{
				run[t-1].x=(run[t-1].x!=run[t].fx)?1:0;
				run[t-1].kind=INT_TYPE;
			}
			else if(run[t-1].kind!=REAL_TYPE&&run[t].kind!=REAL_TYPE)
			{
				run[t-1].x=(run[t-1].x!=run[t].x)?1:0;
				run[t-1].kind=INT_TYPE;
			}
			break;
		case 9:
			t=t-1;
			if(run[t-1].kind==REAL_TYPE&&run[t].kind==REAL_TYPE)
			{
				run[t-1].x=(run[t-1].fx<run[t].fx)?1:0;
				run[t-1].kind=INT_TYPE;
			}
			else if(run[t-1].kind==REAL_TYPE&&run[t].kind!=REAL_TYPE)
			{
				run[t-1].x=(run[t-1].fx<run[t].x)?1:0;
				run[t-1].kind=INT_TYPE;
			}
			else if(run[t-1].kind!=REAL_TYPE&&run[t].kind==REAL_TYPE)
			{
				run[t-1].x=(run[t-1].x<run[t].fx)?1:0;
				run[t-1].kind=INT_TYPE;
			}
			else if(run[t-1].kind!=REAL_TYPE&&run[t].kind!=REAL_TYPE)
			{
				run[t-1].x=(run[t-1].x<run[t].x)?1:0;
				run[t-1].kind=INT_TYPE;
			}
			break;
		case 10:
			t=t-1;
			if(run[t-1].kind==REAL_TYPE&&run[t].kind==REAL_TYPE)
			{
				run[t-1].x=(run[t-1].fx<=run[t].fx)?1:0;
				run[t-1].kind=INT_TYPE;
			}
			else if(run[t-1].kind==REAL_TYPE&&run[t].kind!=REAL_TYPE)
			{
				run[t-1].x=(run[t-1].fx<=run[t].x)?1:0;
				run[t-1].kind=INT_TYPE;
			}
			else if(run[t-1].kind!=REAL_TYPE&&run[t].kind==REAL_TYPE)
			{
				run[t-1].x=(run[t-1].x<=run[t].fx)?1:0;
				run[t-1].kind=INT_TYPE;
			}
			else if(run[t-1].kind!=REAL_TYPE&&run[t].kind!=REAL_TYPE)
			{
				run[t-1].x=(run[t-1].x<=run[t].x)?1:0;
				run[t-1].kind=INT_TYPE;
			}
			break;
		case 11:
			t=t-1;
			if(run[t-1].kind==REAL_TYPE&&run[t].kind==REAL_TYPE)
			{
				run[t-1].x=(run[t-1].fx>run[t].fx)?1:0;
				run[t-1].kind=INT_TYPE;
			}
			else if(run[t-1].kind==REAL_TYPE&&run[t].kind!=REAL_TYPE)
			{
				run[t-1].x=(run[t-1].fx>run[t].x)?1:0;
				run[t-1].kind=INT_TYPE;
			}
			else if(run[t-1].kind!=REAL_TYPE&&run[t].kind==REAL_TYPE)
			{
				run[t-1].x=(run[t-1].x>run[t].fx)?1:0;
				run[t-1].kind=INT_TYPE;
			}
			else if(run[t-1].kind!=REAL_TYPE&&run[t].kind!=REAL_TYPE)
			{
				run[t-1].x=(run[t-1].x>run[t].x)?1:0;
				run[t-1].kind=INT_TYPE;
			}
			break;
		case 12:
			t=t-1;
			if(run[t-1].kind==REAL_TYPE&&run[t].kind==REAL_TYPE)
			{
				run[t-1].x=(run[t-1].fx>=run[t].fx)?1:0;
				run[t-1].kind=INT_TYPE;
			}
			else if(run[t-1].kind==REAL_TYPE&&run[t].kind!=REAL_TYPE)
			{
				run[t-1].x=(run[t-1].fx>=run[t].x)?1:0;
				run[t-1].kind=INT_TYPE;
			}
			else if(run[t-1].kind!=REAL_TYPE&&run[t].kind==REAL_TYPE)
			{
				run[t-1].x=(run[t-1].x>=run[t].fx)?1:0;
				run[t-1].kind=INT_TYPE;
			}
			else if(run[t-1].kind!=REAL_TYPE&&run[t].kind!=REAL_TYPE)
			{
				run[t-1].x=(run[t-1].x>=run[t].x)?1:0;
				run[t-1].kind=INT_TYPE;
			}
			break;
		case 13:
			if(run[t-1].kind==INT_TYPE||run[t-1].kind==CHAR_TYPE)
				run[t-1].x++;
			else if(run[t-1].kind==REAL_TYPE)
				run[t-1].fx++;
			break;
		case 14:
			if(run[t-1].kind==INT_TYPE||run[t-1].kind==CHAR_TYPE)
				run[t-1].x--;
			else if(run[t-1].kind==REAL_TYPE)
				run[t-1].fx--;
			break;
		case 15:
			if(run[t-2].kind==REAL_TYPE&&run[t-1].kind==REAL_TYPE)
			{
				run[t-1].x=(run[t-2].fx==run[t-1].fx)?1:0;
				run[t-1].kind=INT_TYPE;
			}
			else if(run[t-2].kind==REAL_TYPE&&run[t-1].kind!=REAL_TYPE)
			{
				run[t-1].x=(run[t-2].fx==run[t-1].x)?1:0;
				run[t-1].kind=INT_TYPE;
			}
			else if(run[t-2].kind!=REAL_TYPE&&run[t-1].kind==REAL_TYPE)
			{
				run[t-1].x=(run[t-2].x==run[t-1].fx)?1:0;
				run[t-1].kind=INT_TYPE;
			}
			else if(run[t-2].kind!=REAL_TYPE&&run[t-1].kind!=REAL_TYPE)
			{
				run[t-1].x=(run[t-2].x==run[t-1].x)?1:0;
				run[t-1].kind=INT_TYPE;
			}
			break;
		case 16:
			if(run[t-2].kind==REAL_TYPE&&run[t-1].kind==REAL_TYPE)
			{
				run[t-1].x=(run[t-2].fx<run[t-1].fx)?1:0;
				run[t-1].kind=INT_TYPE;
			}
			else if(run[t-2].kind==REAL_TYPE&&run[t-1].kind!=REAL_TYPE)
			{
				run[t-1].x=(run[t-2].fx<run[t-1].x)?1:0;
				run[t-1].kind=INT_TYPE;
			}
			else if(run[t-2].kind!=REAL_TYPE&&run[t-1].kind==REAL_TYPE)
			{
				run[t-1].x=(run[t-2].x<run[t-1].fx)?1:0;
				run[t-1].kind=INT_TYPE;
			}
			else if(run[t-2].kind!=REAL_TYPE&&run[t-1].kind!=REAL_TYPE)
			{
				run[t-1].x=(run[t-2].x<run[t-1].x)?1:0;
				run[t-1].kind=INT_TYPE;
			}
			break;
		case 17:
			if(run[t-2].kind==REAL_TYPE&&run[t-1].kind==REAL_TYPE)
			{
				run[t-1].x=(run[t-2].fx>run[t-1].fx)?1:0;
				run[t-1].kind=INT_TYPE;
			}
			else if(run[t-2].kind==REAL_TYPE&&run[t-1].kind!=REAL_TYPE)
			{
				run[t-1].x=(run[t-2].fx>run[t-1].x)?1:0;
				run[t-1].kind=INT_TYPE;
			}
			else if(run[t-2].kind!=REAL_TYPE&&run[t-1].kind==REAL_TYPE)
			{
				run[t-1].x=(run[t-2].x>run[t-1].fx)?1:0;
				run[t-1].kind=INT_TYPE;
			}
			else if(run[t-2].kind!=REAL_TYPE&&run[t-1].kind!=REAL_TYPE)
			{
				run[t-1].x=(run[t-2].x>run[t-1].x)?1:0;
				run[t-1].kind=INT_TYPE;
			}
			break;
		case 18:
			if(run[t-1].kind==REAL_TYPE||run[t-1].x<0||run[t-1].x>pcod[p].lev)
			{
				fprintf(FOUT,"error! PCode line %d ：数组越界或下标类型错误\n",p);
			}
			break;
		default:
			fprintf(FOUT,"opr() error! PCode line %d\n",p);
			return 0;
	}
	return 1;
}

int lodadr()
{
	run[t].x=basefind(pcod[p].lev)+pcod[p].x;
	run[t++].kind=INT_TYPE;
	return 1;
}

int lod()
{
	switch(pcod[p].lev)
	{
		case 0:
			if(run[run[t-1].x].kind==REAL_TYPE)
			{
				run[t-1].fx=run[run[t-1].x].fx;
				run[t-1].kind=REAL_TYPE;
			}
			else if(run[run[t-1].x].kind==INT_TYPE)
			{
				run[t-1].x=run[run[t-1].x].x;
				run[t-1].kind=INT_TYPE;
			}
			else if(run[run[t-1].x].kind==CHAR_TYPE)
			{
				run[t-1].x=run[run[t-1].x].x;
				run[t-1].kind=CHAR_TYPE;
			}
			break;
		case -1:
			if(funcstack[top_of_funcstack].kind==REAL_TYPE)
			{
				run[t++].fx=funcstack[top_of_funcstack--].fx;
				run[t-1].kind=REAL_TYPE;
			}
			else if(funcstack[top_of_funcstack].kind==INT_TYPE)
			{
				run[t++].x=funcstack[top_of_funcstack--].x;
				run[t-1].kind=INT_TYPE;
			}
			else if(funcstack[top_of_funcstack].kind==CHAR_TYPE)
			{
				run[t++].x=funcstack[top_of_funcstack--].x;
				run[t-1].kind=CHAR_TYPE;
			}
			break;
		default:
			fprintf(FOUT,"lod() error! PCode line %d\n",p);
			return 0;

	}
	return 1;
}

int sto()
{
	switch(pcod[p].lev)
	{
		case 1:
			if(pcod[p].x==INT_TYPE)
			{
				if(run[t-2].kind==REAL_TYPE)
				{
					run[run[t-1].x].x=(int)run[t-2].fx;
					run[run[t-1].x].kind=INT_TYPE;
				}
				else if(run[t-2].kind==INT_TYPE)
				{
					run[run[t-1].x].x=run[t-2].x;
					run[run[t-1].x].kind=INT_TYPE;
				}
				else if(run[t-2].kind==CHAR_TYPE)
				{
					run[run[t-1].x].x=run[t-2].x;
					run[run[t-1].x].kind=INT_TYPE;
				}
				t=t-2;
			}
			else if(pcod[p].x==REAL_TYPE)
			{
				if(run[t-2].kind==REAL_TYPE)
				{
					run[run[t-1].x].fx=run[t-2].fx;
					run[run[t-1].x].kind=REAL_TYPE;
				}
				else if(run[t-2].kind==INT_TYPE)
				{
					run[run[t-1].x].fx=run[t-2].x;
					run[run[t-1].x].kind=REAL_TYPE;
				}
				else if(run[t-2].kind==CHAR_TYPE)
				{
					run[run[t-1].x].fx=run[t-2].x;
					run[run[t-1].x].kind=REAL_TYPE;
				}
				t=t-2;
			}
			else if(pcod[p].x==CHAR_TYPE)
			{
				if(run[t-2].kind==REAL_TYPE)
				{
					run[run[t-1].x].x=(char)run[t-2].fx;
					run[run[t-1].x].kind=CHAR_TYPE;
				}
				else if(run[t-2].kind==INT_TYPE)
				{
					run[run[t-1].x].x=(char)run[t-2].x;
					run[run[t-1].x].kind=CHAR_TYPE;
				}
				else if(run[t-2].kind==CHAR_TYPE)
				{
					run[run[t-1].x].x=run[t-2].x;
					run[run[t-1].x].kind=CHAR_TYPE;
				}
				t=t-2;
			}
			break;
		case 0:
			if(pcod[p].x==INT_TYPE)
			{
				if(run[t-1].kind==REAL_TYPE)
				{
					run[run[t-2].x].x=(int)run[t-1].fx;
					run[run[t-2].x].kind=INT_TYPE;
				}
				else if(run[t-1].kind==INT_TYPE)
				{
					run[run[t-2].x].x=run[t-1].x;
					run[run[t-2].x].kind=INT_TYPE;
				}
				else if(run[t-1].kind==CHAR_TYPE)
				{
					run[run[t-2].x].x=run[t-1].x;
					run[run[t-2].x].kind=INT_TYPE;
				}
				t=t-2;
			}
			else if(pcod[p].x==REAL_TYPE)
			{
				if(run[t-1].kind==REAL_TYPE)
				{
					run[run[t-2].x].fx=run[t-1].fx;
					run[run[t-2].x].kind=REAL_TYPE;
				}
				else if(run[t-1].kind==INT_TYPE)
				{
					run[run[t-2].x].fx=run[t-1].x;
					run[run[t-2].x].kind=REAL_TYPE;
				}
				else if(run[t-1].kind==CHAR_TYPE)
				{
					run[run[t-2].x].fx=run[t-1].x;
					run[run[t-2].x].kind=REAL_TYPE;
				}
				t=t-2;
			}
			else if(pcod[p].x==CHAR_TYPE)
			{
				if(run[t-1].kind==REAL_TYPE)
				{
					run[run[t-2].x].x=(char)run[t-1].fx;
					run[run[t-2].x].kind=CHAR_TYPE;
				}
				else if(run[t-1].kind==INT_TYPE)
				{
					run[run[t-2].x].x=(char)run[t-1].x;
					run[run[t-2].x].kind=CHAR_TYPE;
				}
				else if(run[t-1].kind==CHAR_TYPE)
				{
					run[run[t-2].x].x=run[t-1].x;
					run[run[t-2].x].kind=CHAR_TYPE;
				}
				t=t-2;
			}
			break;
		case -1:
			if(run[t-1].kind==REAL_TYPE)
			{
				parastack[top_of_parastack++].fx=run[t-1].fx;
				parastack[top_of_parastack-1].kind=REAL_TYPE;
			}
			else if(run[t-1].kind==INT_TYPE)
			{
				parastack[top_of_parastack++].x=run[t-1].x;
				parastack[top_of_parastack-1].kind=INT_TYPE;
			}
			else if(run[t-1].kind==CHAR_TYPE)
			{
				parastack[top_of_parastack++].x=run[t-1].x;
				parastack[top_of_parastack-1].kind=CHAR_TYPE;
			}
			t=t-1;
			break;
		case -2:
			if(pcod[p].x==INT_TYPE)
			{
				if(parastack[top_of_parastack-1].kind==REAL_TYPE)
				{
					run[run[t-1].x].x=(int)parastack[--top_of_parastack].fx;
					run[run[t-1].x].kind=INT_TYPE;
				}
				else if(parastack[top_of_parastack-1].kind==INT_TYPE)
				{
					run[run[t-1].x].x=parastack[--top_of_parastack].x;
					run[run[t-1].x].kind=INT_TYPE;
				}
				else if(parastack[top_of_parastack].kind==CHAR_TYPE)
				{
					run[run[t-1].x].x=parastack[--top_of_parastack].x;
					run[run[t-1].x].kind=INT_TYPE;
				}
				t=t-1;
			}
			else if(pcod[p].x==REAL_TYPE)
			{
				if(parastack[top_of_parastack-1].kind==REAL_TYPE)
				{
					run[run[t-1].x].fx=parastack[--top_of_parastack].fx;
					run[run[t-1].x].kind=REAL_TYPE;
				}
				else if(parastack[top_of_parastack-1].kind==INT_TYPE)
				{
					run[run[t-1].x].fx=parastack[--top_of_parastack].x;
					run[run[t-1].x].kind=REAL_TYPE;
				}
				else if(parastack[top_of_parastack].kind==CHAR_TYPE)
				{
					run[run[t-1].x].fx=parastack[--top_of_parastack].x;
					run[run[t-1].x].kind=REAL_TYPE;
				}
				t=t-1;
			}
			else if(pcod[p].x==CHAR_TYPE)
			{
				if(parastack[top_of_parastack-1].kind==REAL_TYPE)
				{
					run[run[t-1].x].x=(char)parastack[--top_of_parastack].fx;
					run[run[t-1].x].kind=CHAR_TYPE;
				}
				else if(parastack[top_of_parastack-1].kind==INT_TYPE)
				{
					run[run[t-1].x].x=(char)parastack[--top_of_parastack].x;
					run[run[t-1].x].kind=CHAR_TYPE;
				}
				else if(parastack[top_of_parastack].kind==CHAR_TYPE)
				{
					run[run[t-1].x].x=parastack[--top_of_parastack].x;
					run[run[t-1].x].kind=CHAR_TYPE;
				}
				t=t-1;
			}
			break;
		case -3:
			funcstack[top_of_funcstack].x=0;
			funcstack[top_of_funcstack].fx=0;
			funcstack[top_of_funcstack].kind=INT_TYPE;				
			break;
		case -4:
			if(pcod[p].x==INT_TYPE)
			{
				if(run[t-1].kind==REAL_TYPE)
				{
					funcstack[top_of_funcstack].x=(int)run[t-1].fx;
					funcstack[top_of_funcstack].kind=INT_TYPE;
				}
				else if(run[t-1].kind==INT_TYPE)
				{
					funcstack[top_of_funcstack].x=run[t-1].x;
					funcstack[top_of_funcstack].kind=INT_TYPE;
				}
				else if(run[t-1].kind==CHAR_TYPE)
				{
					funcstack[top_of_funcstack].x=(int)run[t-1].x;
					funcstack[top_of_funcstack].kind=INT_TYPE;
				}
				t=t-1;
			}
			else if(pcod[p].x==REAL_TYPE)
			{
				if(run[t-1].kind==REAL_TYPE)
				{
					funcstack[top_of_funcstack].fx=run[t-1].fx;
					funcstack[top_of_funcstack].kind=REAL_TYPE;
				}
				else if(run[t-1].kind==INT_TYPE)
				{
					funcstack[top_of_funcstack].fx=run[t-1].x;
					funcstack[top_of_funcstack].kind=REAL_TYPE;
				}
				else if(run[t-1].kind==CHAR_TYPE)
				{
					funcstack[top_of_funcstack].fx=run[t-1].x;
					funcstack[top_of_funcstack].kind=REAL_TYPE;
				}
				t=t-1;
			}
			else if(pcod[p].x==CHAR_TYPE)
			{
				if(run[t-1].kind==REAL_TYPE)
				{
					funcstack[top_of_funcstack].x=(char)run[t-1].fx;
					funcstack[top_of_funcstack].kind=CHAR_TYPE;
				}
				else if(run[t-1].kind==INT_TYPE)
				{
					funcstack[top_of_funcstack].x=(char)run[t-1].x;
					funcstack[top_of_funcstack].kind=CHAR_TYPE;
				}
				else if(run[t-1].kind==CHAR_TYPE)
				{
					funcstack[top_of_funcstack].x=run[t-1].x;
					funcstack[top_of_funcstack].kind=CHAR_TYPE;
				}
				t=t-1;
			}
			break;
		default:
			fprintf(FOUT,"sto() error! PCode line %d\n",p);
			return 0;
	}
	return 1;
}

int cal()
{
	run[t].x=basefind(pcod[p].lev);
	run[t].kind=INT_TYPE;
	run[t+1].x=base;
	run[t+1].kind=INT_TYPE;
	run[t+2].x=p;
	run[t+2].kind=INT_TYPE;
	base=t;
	p=pcod[p].x;
	return 1;
}

int int1()
{
	if(pcod[p].lev==0)
		t=t+pcod[p].x;
	else
		top_of_funcstack+=pcod[p].x;
	return 1;
}

int jmp()
{
	p=pcod[p].x;
	return 1;
}

int jpc()
{
	switch(pcod[p].lev)
	{
		case 0:
			if(run[--t].x==0)
			{
				p=pcod[p].x;
			}
			else
				p++;
			break;
		case 1:
			if(run[--t].x==1)
			{
				p=pcod[p].x;
			}
			else
				p++;
			break;
	}
	return 1;
}

int red()
{
	char temp_char;
	t=t-1;
	switch(pcod[p].lev)
	{
		case INT_TYPE:
			scanf("%d",&run[run[t].x].x);
			run[run[t].x].kind=INT_TYPE;
			break;
		case REAL_TYPE:
			scanf("%lf",&run[run[t].x].fx);
			run[run[t].x].kind=REAL_TYPE;
			break;
		case CHAR_TYPE:
			scanf("%c",&temp_char);
			run[run[t].x].x=(char)temp_char;
			run[run[t].x].kind=CHAR_TYPE;
			break;
	}
	return 1;
}

int wrt()
{
	char temp[MAXSTRING]="";
	switch(pcod[p].lev)
	{
		case 0:
			sprintf(temp,"%s",printstring[pcod[p].x]);
			strcat(print_string,temp);
			break;
		case 1:
			if(run[--t].kind==REAL_TYPE)
			{
				sprintf(temp,"%lf",run[t].fx);
				strcat(print_string,temp);
			}
			else if(run[t].kind==INT_TYPE)
			{
				sprintf(temp,"%d",run[t].x);
				strcat(print_string,temp);
			}
			else if(run[t].kind==CHAR_TYPE)
			{
				sprintf(temp,"%c",run[t].x);
				strcat(print_string,temp);
			}
			break;
	}
	return 1;
}



//解释执行
int interpret()
{
	int interpret_error_flag=0;
	while(p<top_of_pcode)
	{
		switch(pcod[p].op)
		{
			case LIT:
				if(!lit())
				{
					interpret_error_flag=1;
				}
				p++;
				break;
			case OPR:
				if(!opr())
				{
					interpret_error_flag=1;
				}
				p++;
				break;
			case LODADR:
				if(!lodadr())
				{
					interpret_error_flag=1;
				}
				p++;
				break;
			case LOD:
				if(!lod())
				{
					interpret_error_flag=1;
				}
				p++;
				break;
			case STO:
				if(!sto())
				{
					interpret_error_flag=1;
				}
				p++;
				break;
			case CAL:
				if(!cal())
				{
					interpret_error_flag=1;
				}
				//p++;
				break;
			case INT:
				if(!int1())
				{
					interpret_error_flag=1;
				}
				p++;
				break;
			case JMP:
				if(!jmp())
				{
					interpret_error_flag=1;
				}
				break;
			case JPC:
				if(!jpc())
				{
					interpret_error_flag=1;
				}
				break;
			case RED:
				if(!red())
				{
					interpret_error_flag=1;
				}
				p++;
				break;
			case WRT:
				if(!wrt())
				{
					interpret_error_flag=1;
				}
				p++;
				break;
			default:
				interpret_error_flag=1;
				p++;
				break;
		}
	}
	if(interpret_error_flag)
		return 0;
	return 1;
}


int main(int argc,char *argv[])
{
//	char c;
	int i=1;
	int a;
	if(argc!=2){
		printf("请输入编译源程序文件名：\n");
		scanf("%s",fp);
	}
	else
		strcpy(fp,argv[1]);
	
	if(strchr(fp,'.')==NULL)
		strcat(fp,".txt");

	FIN=fopen(fp,"r");
	//FOUT=fopen("source.txt","w");
	FOUT=stdout;
	
	if(FIN==NULL)
	{
		printf("读文件错误\n");
		exit(1);
	}
	fprintf(FOUT,"源程序:\n1: ");
	//语法分析
	if(program())
	{
		fprintf(FOUT,"\n程序结束!\n");
	}
	else
	{
		fprintf(FOUT,"\n程序出现错误，程序结束!\n");
	}
	//打印错误
	if(errornum>1)
	{
		fprintf(FOUT,"\n程序错误:\n");
		for(i=1;i<errornum;i++)
			error_msg(i);
//		fclose(FIN);
//		return 0;
	}
	if(!interpret())
	{
		//fprintf(FOUT,"执行出错\n");
	}
	fprintf(FOUT,"\n----------------------\n");
	fprintf(FOUT,"运行结果 :\n");
	fprintf(FOUT,"%s\n",print_string);
	fprintf(FOUT,"\n----------------------\n");
	while(1)
	{
		printf("输入1，打印符号表;输入2，打印目标代码;输入0，退出程序\n");
		scanf("%d",&a);
		if(a==0)
		{
			break;
		}
		else if(a==1)
		{
			fprintf(FOUT,"符号表 :\n");
			fprintf(FOUT,"name\ttype\tlev\tadr\tkind\tcount\n");
			for(i=0;i<top_of_table;i++)
				fprintf(FOUT,"%s\t%d\t%d\t%d\t%d\t%d\n",tbl[i].name,tbl[i].type,tbl[i].lev,tbl[i].adr,tbl[i].kind,tbl[i].count);
			fprintf(FOUT,"\n----------------------\n");
		}
		else if(a==2)
		{
			fprintf(FOUT,"\n目标代码：\n");
			for(i=0;i<top_of_pcode;i++)
				fprintf(FOUT,"(%d)\t%s\t%d\t%d\t%lf\n",i,operation[pcod[i].op],pcod[i].lev,pcod[i].x,pcod[i].fx);
			fprintf(FOUT,"\n----------------------\n");
		}
	}
	
	
	//cifafenxi=1;
	//while(i!=0)
	//{
//		i=getsym();
//	}
	fclose(FIN);
	return 0;
}