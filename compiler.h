
#ifndef STDIOF
#define STDIOF
#include"stdio.h"
#endif

#ifndef STRINGF
#define STRINGF
#include"string.h"
#endif

#ifndef CTYPEF
#define CTYPEF
#include"ctype.h"
#endif

#ifndef STDLIBF
#define STDLIBF
#include"stdlib.h"
#endif

#define MAXWORD 20 //token最大长度
#define MAXSTRING  30 //字符串最大长度
#define MAXVARDEFINE 20//每条语句最大变量声明
#define MAXERROR 100	//最大错误数
#define MAXTABLE 500    //最大符号表项目
#define MAXPCODE 1000		//最大中间代码条数
#define MAXSTACK 200		//最大运行栈空间
#define MAXLEV	10	//最大调用层数

#define CONST_KIND 1    //常量类型（符号表用）
#define VAR_KIND 2    //变量类型（符号表用）
#define ARRAY_KIND 3    //数组变量类型（符号表用）
#define PROC_KIND 4    //过程类型（符号表用）
#define FUNC_KIND 5    //函数类型（符号表用）
#define PARA_KIND 6    //形参类型（符号表用）


#define INT_TYPE 1		//整数类型
#define REAL_TYPE 2		//实数类型
#define CHAR_TYPE 3		//字符类型


#define IDEN 1    //标识符
#define INTCON 2  //整型常量
#define REALCON 3 //实型常量
#define CHARCON 4 //字符常量
#define STRCON 5  //字符串常量

#define CONSTTK 6 //const
#define INTTK 7	  //integer
#define FLOATTK 8 //real 
#define CHARTK 9  //char
#define VARTK 10//var
#define ARRAYTK 11//array
#define OFTK 12//of
#define IFTK 13//if
#define THENTK 14//then
#define ELSETK 15//else
#define SWITCHTK 16//switch
#define CASETK 17//case
#define FORTK 18//for
#define TOTK 19//to
#define DOWNTOTK 20//downto
#define DOTK 21//do
#define PROCETK 22//procedure
#define FUNCTK 23//function
#define READTK 24//read
#define WRITETK 25//write
#define BEGINTK 26//begin
#define ENDTK 27//end
#define ODDTK 28//odd
#define PLUS 29//	+
#define MINUS 30//	-
#define MULT 31//	*
#define DIV 32//	/
#define LSS 33//	<
#define LEQ 34//	<=
#define GRE 35//	>
#define GEQ 36//	>=
#define EQL 37//	=
#define NEQ 38//	<>
#define ASSIGN 39//	:=
#define SEMICN 40//	;
#define COMMA 41//	,
#define PERIOD 42//	.
#define COLON 43//	:
#define QMARK 44//	'
#define DQMARK 45//	"
#define LPARENT 46//	(
#define RPARENT 47//	)
#define LBRACK 48//	[
#define RBRACK 49//	]
#define LBRACE 50//	{
#define RBRACE 51//	}
#define WHILETK 52// while
#define FILEOVER 53//文件结束

#define LIT 0//取常量a放在数据栈栈顶
#define OPR 1//执行运算，a表示执行何种运算
#define LODADR 2//把地址存到栈顶
#define LOD 3//取变量（相对地址为a，层次差为l）放在数据栈栈顶
#define STO 4//将数据栈栈顶内容存入变量（相对地址为a，层次差为l）
#define CAL 5//调用过程（入口指令地址为a，层次差为l）
#define INT 6//数据栈栈顶指针增加a
#define JMP 7//无条件转移到a
#define JPC 8//条件转移到指令地址a
#define RED 9//读数据并存入变量（相对地址a,层次差为l）
#define WRT 10//将栈顶内容输出


struct pcode{							//中间代码 
	int op,lev,x;
	double fx;
};

union Cha{
	int num;
	double fnum;
	char ch;
};
struct table{					//符号表
	char *name ;
	int type,lev,adr,kind;//adr 存函数对应的pcode指令的起始地址
	int count;//数组长度
	union Cha uchar;
};

struct runstack{//运行栈数据结构
	int x;
	double fx;
	int kind;
};

int getsym();	//词法分析
void error_msg(int i);//错误处理
int enter(int in_lev,int type,int kind);//登陆符号表
int enterarray(int in_lev,int type,int kind,int size);//数组登陆符号表
int test();//检查标识符是否重复定义
int find();//查表
int gen(int op,int in_lev,int x);//生成目标代码
int gen1(int op,int in_lev,double fx);//生成目标代码
int program();	//程序
int block();	//分程序
int constdefine();//常量说明
int vardefine();//变量说明
int vardefinestatement();//变量声明语句
int proc();//过程说明
int func();//函数说明
int paradefine();//参数表
int paradefinestatement();//形式参数段
int compound();//复合语句
int statement();//语句
int proccall(int proc_name_index);//过程调用
int funccall(int func_name_index);//函数调用
int assignstatement(int iden_index);//赋值语句
int ifstatement();//条件语句
int casestatement();//情况语句
int readstatement();//读语句
int writestatement();//写语句
int forstatement();//for循环语句
int expression();//表达式
int term();//项
int factor();//项
int basefind(int l);//层差为l的数据区的基址
int interpret();//解释执行

