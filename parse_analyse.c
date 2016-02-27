#include"compiler.h"

extern FILE *FIN;  //文件输入指针
extern FILE *FOUT;	//文件读出指针
extern char fp[20];		//文件名
extern int cifafenxi;

extern int linenum;//存行数
extern int errornum;//错误数
extern int errorline[MAXERROR];//错误行号
extern int error[MAXERROR];//错误数
extern struct table tbl[MAXTABLE];//符号表
extern int top_of_table;//符号表栈顶
extern int initial_table_index;//初始符号表索引
extern struct pcode pcod[MAXPCODE];//中间代码
extern int top_of_pcode;//中间代码栈顶
extern int initial_pcode_index;//初始中间代码索引
extern struct runstack run[MAXSTACK];//运行栈
extern int top_of_runstack;//运行栈栈顶
extern int num_of_var[MAXTABLE];//记录各层变量个数
extern int top_of_numofvar;//num_of_var栈顶
extern struct runstack parastack[MAXVARDEFINE];//记录参数值
extern int top_of_parastack;//parastack栈顶
extern struct runstack funcstack[MAXVARDEFINE];//记录函数返回值
extern int top_of_funcstack;//funcstack栈顶
extern int func_flag[MAXVARDEFINE];//记录是否为函数
extern int top_of_funcflag;
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

//打印错误信息，格式为：line(1)	error7:		缺少.
void error_msg(int i)
{
	fprintf(FOUT,"line(%2d)\terror%d:\t\t%s\n",errorline[i],error[i],err_msg[error[i]]);
}

//登陆符号表
int enter(int in_lev,int type,int kind)
{
	int enter_error_flag=0;
	if(top_of_table<(MAXTABLE-1))
	{
		if(!test())
		{
			errorline[errornum]=linenum;
			error[errornum++]=16;
		//	enter_error_flag=1;
			return 0;
		}
		tbl[top_of_table].lev=in_lev;
		tbl[top_of_table].kind=kind;
		tbl[top_of_table].type=type;
		tbl[top_of_table].name=name[top_of_table];
		tbl[top_of_table].count=0;
		if(kind==PROC_KIND||kind==FUNC_KIND||kind==CONST_KIND)
		{
			tbl[top_of_table].adr=top_of_pcode;
		}
		else
		{
			tbl[top_of_table].adr=data_allocation_index;
			data_allocation_index++;
		}
		if(kind==CONST_KIND)//如果是常量，常量值放入符号表中
		{
			if(type==INT_TYPE)
				tbl[top_of_table].uchar.num=num;
			else if(type==REAL_TYPE)
				tbl[top_of_table].uchar.fnum=fnum;
			else
				tbl[top_of_table].uchar.ch=symchar;
		}
		top_of_table++;
	}
	else
	{
		errorline[errornum]=linenum;
		error[errornum++]=17;
		return 0;
	}
	
	return 1;
}

//数组登陆符号表
int enterarray(int in_lev,int type,int kind,int size)
{
	if(top_of_table<(MAXTABLE-1))
	{
		if(!test())
		{
			errorline[errornum]=linenum;
			error[errornum++]=16;
		//	enter_error_flag=1;
			return 0;
		}
		tbl[top_of_table].lev=in_lev;
		tbl[top_of_table].kind=kind;
		tbl[top_of_table].type=type;
		tbl[top_of_table].name=name[top_of_table];
		tbl[top_of_table].count=size-1;
		if(kind==PROC_KIND||kind==FUNC_KIND)
		{
			tbl[top_of_table].adr=top_of_pcode;
		}
		else
		{
			tbl[top_of_table].adr=data_allocation_index;
			data_allocation_index+=size;
		}
		top_of_table++;
	}
	else
	{
		errorline[errornum]=linenum;
		error[errornum++]=17;
		return 0;
	}	
	return 1;
}

//当前单词是否合法 
int test()
{
	int i;
	for(i=top_of_table-1;i>=0;i--){
		if(tbl[i].lev<lev)//符号表项的层数小于当前层，返回合法
			return 1;
		else{
			if(!strcmp(tbl[i].name,name[top_of_table]))//符号重复声明
				return 0;
		}
	}
	return 1;
}

//查表
int find()
{
	int i;
	int temp_lev=lev;
	for(i=top_of_table-1;i>=0;i--)
	{
		if(tbl[i].lev<=temp_lev)
		{
			temp_lev=tbl[i].lev;
			if(!strcmp(tbl[i].name,word))
				return i;			
		}				
	}
	return -1;
}

//生成pcode
int gen(int op,int in_lev,int x)
{
	if(top_of_pcode<MAXPCODE)
	{
		pcod[top_of_pcode].op=op;
		pcod[top_of_pcode].lev=in_lev;
		pcod[top_of_pcode].x=x;
		top_of_pcode++;
	}
	else
	{
		errorline[errornum]=linenum;
		error[errornum++]=19;
		return 0;
	}
	return 1;
}

int gen1(int op,int in_lev,double fx)
{
	if(top_of_pcode<MAXPCODE)
	{
		pcod[top_of_pcode].op=op;
		pcod[top_of_pcode].lev=in_lev;
		pcod[top_of_pcode].fx=fx;
		top_of_pcode++;
	}
	else
	{
		errorline[errornum]=linenum;
		error[errornum++]=19;
		return 0;
	}
	return 1;
}

//分析程序
int program()
{
	sym=getsym();
	if(block())//分程序成立
	{
		if(sym==PERIOD)//是.
		{
			return 1;
		}
		else
		{
			errorline[errornum]=linenum;
			error[errornum++]=6;//错误“缺少.”
			while(sym!=FILEOVER)//跳至文件结束
				sym=getsym();
			return 0;
		}
	}
	else
	{
		//errorline[errornum]=linenum;
		//error[errornum++]=6;//错误“分程序不全或错误”
		while(sym!=FILEOVER)//跳到文件末尾
			sym=getsym();
		return 0;
	}

}

int block()
{
	int block_error_flag=0;
	int block_error_flag1=0;
	int paratop,paranum=0;
//	sym=getsym();
	
	temp_linenum=linenum;
	//跳到预期的单词符号
	while(sym!=CONSTTK&&sym!=VARTK&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)
	{
		block_error_flag=1;
		block_error_flag1=1;
		sym=getsym();
	}
	if(block_error_flag1)
	{
		errorline[errornum]=temp_linenum;
		error[errornum++]=5;//错误“非法语法结构”
		block_error_flag1=0;
	}

	if(sym==CONSTTK)//分析常量说明
	{
		temp_linenum=linenum;
		if(!constdefine())
		{
			block_error_flag=1;
		}
		
		while(sym!=VARTK&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//跳到预期的单词符号
		{
			block_error_flag=1;
			block_error_flag1=1;
			sym=getsym();
		}
		if(block_error_flag1)
		{
			errorline[errornum]=temp_linenum;
			error[errornum++]=5;//错误“非法语法结构”
			block_error_flag1=0;
		}
	}


	if(sym==VARTK)//分析变量说明
	{
		if(!vardefine())
		{
			block_error_flag=1;
		}
		//sym=getsym();
		temp_linenum=linenum;
		//跳到预期的单词符号
		while(sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)
		{
			block_error_flag=1;
			block_error_flag1=1;
			sym=getsym();
		}
		if(block_error_flag1)
		{
			errorline[errornum]=temp_linenum;
			error[errornum++]=5;//错误“非法语法结构”
			block_error_flag1=0;
		}
	}

	num_of_var[top_of_numofvar++]=data_allocation_index;
	data_allocation_index=3;	

	while(sym==PROCETK||sym==FUNCTK)//分析程序或函数
	{	
		if(sym==PROCETK)//如果是procedure
		{
			func_flag[top_of_funcflag++]=0;
			if(!proc())
			{
				block_error_flag=1;
				
			}
	//		sym=getsym();
			temp_linenum=linenum;
			//跳到预期的单词符号
			while(sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)
			{
				block_error_flag=1;
				block_error_flag1=1;
				sym=getsym();
			}
			if(block_error_flag1)
			{
				errorline[errornum]=temp_linenum;
				error[errornum++]=5;//错误“非法语法结构”
				block_error_flag1=0;
			}
		}
		else if(sym==FUNCTK)//如果是function
		{
			func_flag[top_of_funcflag++]=1;
			func_assaign_flag=0;
			if(!func())
			{
				block_error_flag=1;
				
			}
			if(!func_assaign_flag)	
			{
				errorline[errornum]=linenum;
				error[errornum++]=34;
				block_error_flag1=0;
				if(!gen(STO,-3,0))
				{
					block_error_flag1=0;
				}
			}
			//sym=getsym();
			temp_linenum=linenum;
			//跳到预期的单词符号
			while(sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)
			{
				block_error_flag=1;
				block_error_flag1=1;
				sym=getsym();
			}
			if(block_error_flag1)
			{
				errorline[errornum]=temp_linenum;
				error[errornum++]=5;//错误“非法语法结构”
				block_error_flag1=0;
			}
		}
	}

	if(sym==BEGINTK)//分析符合语句说明
	{
		//当是主程序是填补目标代码第一条，否则无条件跳转到下一条
		if(lev==0)
		{
			pcod[0].op=JMP;
			pcod[0].lev=0;
			pcod[0].x=top_of_pcode;
		}
		else
		{
			if(!gen(JMP,0,top_of_pcode+1))
			{
				block_error_flag=1;
			}
			if(func_flag[--top_of_funcflag]==1)
			{
				if(!gen(INT,1,1))
				{
					block_error_flag1=0;
				}	
			}
		}
		//为变量分配空间
		if(!gen(INT,0,num_of_var[--top_of_numofvar]))
		{
			block_error_flag=1;
		}
		if(lev>0)
		{
			paratop=top_of_table-1;
			while(tbl[paratop].lev==lev)
			{
				paratop--;
			}
			paratop++;
			for(;paratop<top_of_table&&tbl[paratop].kind==PARA_KIND;paratop++)
			{
				if(!gen(LODADR,lev-tbl[paratop].lev,tbl[paratop].adr))
					return 0;
				if(!gen(STO,-2,tbl[paratop].type))
				{
					block_error_flag=1;
				}
			}
		}
		if(!compound())
		{
			block_error_flag=1;
			
		}
		temp_linenum=linenum;
		while(sym!=SEMICN&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK&&sym!=PERIOD&&sym!=FILEOVER)//跳到预期的单词符号
		{
			block_error_flag=1;
			block_error_flag1=1;
			sym=getsym();
		}
		if(block_error_flag1)
		{
			errorline[errornum]=temp_linenum;
			error[errornum++]=5;//错误“非法语法结构”
			block_error_flag1=0;
		}
		if(lev>0)
		{
			if(!gen(OPR,0,0))
			{
				block_error_flag=1;
			}
		}
	}
	if(block_error_flag)
		return 0;
	return 1;
}

//常量说明
int constdefine()
{
	int constdefine_error_flag=0;
	temp_linenum=linenum;
	sym=getsym();
	while(sym!=IDEN&&sym!=VARTK&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//跳过不可识别字符
	{
		sym=getsym();
		constdefine_error_flag=1;
	}
	if(constdefine_error_flag)
	{
		errorline[errornum]=temp_linenum;
		error[errornum++]=8;
	}
	while(sym==IDEN)
	{
		strcpy(name[top_of_table],word);//标识符存入name
		if((sym=getsym())==EQL)
		{
			sym=getsym();
			if(sym==PLUS)//判断是正数
			{
				sym=getsym();
				if(sym==INTCON)//正整数
				{
					if(!enter(lev,INT_TYPE,CONST_KIND))
						constdefine_error_flag=1;
					sym=getsym();
				}
				else if(sym==REALCON)//正实数
				{
					if(!enter(lev,REAL_TYPE,CONST_KIND))
						constdefine_error_flag=1;
					sym=getsym();
				}
				else//非法单词
				{
					constdefine_error_flag=1;
					errorline[errornum]=temp_linenum;
					error[errornum++]=10;
					while(sym!=COMMA&&sym!=SEMICN&&sym!=IDEN&&sym!=VARTK&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)
					{
						sym=getsym();
					}
				}
				//sym=getsym();
			}
			else if(sym==MINUS)//判断是负数
			{
				sym=getsym();
				if(sym==INTCON)
				{
					num=-num;
					if(!enter(lev,INT_TYPE,CONST_KIND))
						constdefine_error_flag=1;
					sym=getsym();
				}
				else if(sym==REALCON)
				{
					fnum=-fnum;
					if(!enter(lev,REAL_TYPE,CONST_KIND))
						constdefine_error_flag=1;
					sym=getsym();
				}
				else
				{
					constdefine_error_flag=1;
					errorline[errornum]=temp_linenum;
					error[errornum++]=10;
					while(sym!=COMMA&&sym!=SEMICN&&sym!=IDEN&&sym!=VARTK&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)
					{
						sym=getsym();
					}
				}
				//sym=getsym();
			}
			else if(sym==INTCON)//判断是无符号整数
			{
				if(!enter(lev,INT_TYPE,CONST_KIND))
						constdefine_error_flag=1;
				sym=getsym();
			}
			else if(sym==REALCON)//判断是无符号实数
			{
				if(!enter(lev,REAL_TYPE,CONST_KIND))
						constdefine_error_flag=1;
				sym=getsym();
			}
			else if(sym==CHARCON)//判断是字符常量
			{
				if(!enter(lev,CHAR_TYPE,CONST_KIND))
						constdefine_error_flag=1;
				sym=getsym();
			}
			else
			{
				constdefine_error_flag=1;
				errorline[errornum]=temp_linenum;
				error[errornum++]=10;
				while(sym!=COMMA&&sym!=SEMICN&&sym!=IDEN&&sym!=VARTK&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)
				{
					sym=getsym();
				}
			}
			
			if(sym==COMMA)
			{
				sym=getsym();
				if(sym!=IDEN)
				{
					while(sym!=IDEN&&sym!=VARTK&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//跳过不可识别字符
					{
						sym=getsym();
					}
					errorline[errornum]=temp_linenum;
					error[errornum++]=8;
				}
				continue;
			}
			if(sym==SEMICN)
			{
				sym=getsym();
				if(constdefine_error_flag)
					return 0;
				return 1;
			}
			errorline[errornum]=temp_linenum;
			error[errornum++]=7;
			while(sym!=IDEN&&sym!=VARTK&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//跳过不可识别字符
			{
				sym=getsym();
			}
			
		}
		else//缺少=
		{
			errorline[errornum]=temp_linenum;
			error[errornum++]=9;
			while(sym!=IDEN&&sym!=VARTK&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)
			{
				constdefine_error_flag=1;
				sym=getsym();
			}
		}
	}
//	errorline[errornum]=temp_linenum;
//	error[errornum++]=8;
	return 0;
}

//变量说明
int vardefine()
{
	int vardefine_error_flag=0;//标记变量声明是否正确
	int vardefine_temp_linenum;//记录变量声明开始行数
	vardefine_temp_linenum=linenum;
	temp_linenum=linenum;
	sym=getsym();
	while(sym!=IDEN&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//跳过不可识别字符
	{
		sym=getsym();
		vardefine_error_flag=1;
	}
	if(vardefine_error_flag)//缺少标识符
	{
		errorline[errornum]=temp_linenum;
		error[errornum++]=8;
	}
	while(sym==IDEN)//循环处理变量声明语句
	{
		if(!vardefinestatement())
		{
			vardefine_error_flag=1;	
		}
		//sym=getsym();
	}
	if(vardefine_error_flag)
	{
		return 0;
	}
	return 1;
}

//变量声明语句
int vardefinestatement()
{
	int vardefinestatement_error_flag=0;//标记变量声明语句错误
	char temp_name[MAXVARDEFINE][MAXWORD];//暂存变量
	int i=0,top_of_temp_name=0;
	temp_linenum=linenum;
	while(sym==IDEN)
	{
		strcpy(temp_name[top_of_temp_name++],word);//标识符存入temp_name
		sym=getsym();
		if(sym==COMMA)//遇见“，”，继续循环
		{
			sym=getsym();
			continue;
		}
		if(sym==COLON)//遇见“：”，跳出
		{
			sym=getsym();
			break;
		}
		else//缺少“，”或“：”
		{
			errorline[errornum]=temp_linenum;
			error[errornum++]=11;
			vardefinestatement_error_flag=1;
			while(sym!=IDEN&&sym!=SEMICN&&sym!=OFTK&&sym!=INTTK&&sym!=FLOATTK&&sym!=CHARTK&&sym!=ARRAYTK
				&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//跳过不可识别字符
			{
				sym=getsym();	
			}
		}
	}
	if(sym==INTTK)//integer类型
	{
		for(i=0;i<top_of_temp_name;i++)
		{
			strcpy(name[top_of_table],temp_name[i]);//标识符存入name
			if(!enter(lev,INT_TYPE,VAR_KIND))
				vardefinestatement_error_flag=1;
		}
		sym=getsym();
	}
	else if(sym==FLOATTK)//real类型
	{
		for(i=0;i<top_of_temp_name;i++)
		{
			strcpy(name[top_of_table],temp_name[i]);//标识符存入name
			if(!enter(lev,REAL_TYPE,VAR_KIND))
				vardefinestatement_error_flag=1;
		}
		sym=getsym();
	}
	else if(sym==CHARTK)//char类型
	{
		for(i=0;i<top_of_temp_name;i++)
		{
			strcpy(name[top_of_table],temp_name[i]);//标识符存入name
			if(!enter(lev,CHAR_TYPE,VAR_KIND))
				vardefinestatement_error_flag=1;
		}
		sym=getsym();
	}
	else if(sym==ARRAYTK)//数组类型
	{
		sym=getsym();
		if(sym==LBRACK)
		{
			sym=getsym();
		}
		else//缺少[
		{
			errorline[errornum]=temp_linenum;
			error[errornum++]=13;
			vardefinestatement_error_flag=1;
			while(sym!=IDEN&&sym!=INTCON&&sym==RBRACK&&sym!=SEMICN&&sym!=OFTK&&sym!=INTTK&&sym!=FLOATTK&&sym!=CHARTK&&sym!=ARRAYTK
			&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//跳过不可识别字符
			{
				sym=getsym();	
			}
		}
		if(sym==INTCON&&num>0)
		{
			arraylength=num;
			sym=getsym();
		}
		else//数组长度缺少
		{
			errorline[errornum]=temp_linenum;
			error[errornum++]=12;
			vardefinestatement_error_flag=1;
			while(sym!=IDEN&&sym!=RBRACK&&sym!=OFTK&&sym!=SEMICN&&sym!=INTTK&&sym!=FLOATTK&&sym!=CHARTK&&sym!=ARRAYTK
			&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//跳过不可识别字符
			{
				sym=getsym();	
			}
		}
		if(sym==RBRACK)
		{
			sym=getsym();
		}
		else//缺少]
		{
			errorline[errornum]=temp_linenum;
			error[errornum++]=13;
			vardefinestatement_error_flag=1;
			while(sym!=IDEN&&sym!=SEMICN&&sym!=OFTK&&sym!=INTTK&&sym!=FLOATTK&&sym!=CHARTK&&sym!=ARRAYTK
			&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//跳过不可识别字符
			{
				sym=getsym();	
			}
		}
		if(sym==OFTK)
		{
			sym=getsym();
		}
		else//缺少of
		{
			errorline[errornum]=temp_linenum;
			error[errornum++]=15;
			vardefinestatement_error_flag=1;
			while(sym!=IDEN&&sym!=SEMICN&&sym!=INTTK&&sym!=FLOATTK&&sym!=CHARTK&&sym!=ARRAYTK
			&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//跳过不可识别字符
			{
				sym=getsym();	
			}
		}
		//登陆符号表
		if(sym==INTTK)
		{
			for(i=0;i<top_of_temp_name;i++)
			{
				strcpy(name[top_of_table],temp_name[i]);//标识符存入name
				if(!enterarray(lev,INT_TYPE,ARRAY_KIND,arraylength))
					vardefinestatement_error_flag=1;
			}
			sym=getsym();
		}
		else if(sym==FLOATTK)
		{
			for(i=0;i<top_of_temp_name;i++)
			{
				strcpy(name[top_of_table],temp_name[i]);//标识符存入name
				if(!enterarray(lev,REAL_TYPE,ARRAY_KIND,arraylength))
					vardefinestatement_error_flag=1;
			}
			sym=getsym();
		}
		else if(sym==CHARTK)
		{
			for(i=0;i<top_of_temp_name;i++)
			{
				strcpy(name[top_of_table],temp_name[i]);//标识符存入name
				if(!enterarray(lev,CHAR_TYPE,ARRAY_KIND,arraylength))
					vardefinestatement_error_flag=1;
			}
			sym=getsym();
		}
		else
		{
			errorline[errornum]=temp_linenum;
			error[errornum++]=14;
			vardefinestatement_error_flag=1;
			while(sym!=IDEN&&sym!=SEMICN&&sym!=INTTK&&sym!=FLOATTK&&sym!=CHARTK&&sym!=ARRAYTK
			&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//跳过不可识别字符
			{
				sym=getsym();	
			}
		}			
	}
	else
	{
		errorline[errornum]=temp_linenum;
		error[errornum++]=14;
		vardefinestatement_error_flag=1;
		while(sym!=IDEN&&sym!=SEMICN
			&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//跳过不可识别字符
		{
			sym=getsym();	
		}
	}
	if(sym==SEMICN)
	{
		sym=getsym();
	}
	else
	{
		errorline[errornum]=temp_linenum;
		error[errornum++]=7;
		vardefinestatement_error_flag=1;
		while(sym!=IDEN&&sym!=SEMICN&&sym!=INTTK&&sym!=FLOATTK&&sym!=CHARTK&&sym!=ARRAYTK
			&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//跳过不可识别字符
		{
			sym=getsym();	
		}
	}
	if(vardefinestatement_error_flag)
		return 0;
	return 1;
}

//过程说明
int proc()
{
	int proc_error_flag=0;
	temp_linenum=linenum;
	sym=getsym();
	if(sym!=IDEN)//缺少过程名
	{
		errorline[errornum]=temp_linenum;
		error[errornum++]=21;
		return 0;
	}
	strcpy(name[top_of_table],word);//标识符存入temp_name
	if(!enter(lev,INT_TYPE,PROC_KIND))//过程名登陆符号表
			proc_error_flag=1;
	lev++;//层数加1
	sym=getsym();
	if(sym!=LPARENT)//缺少（
	{
		errorline[errornum]=temp_linenum;
		error[errornum++]=20;
		proc_error_flag=1;
		while(sym!=IDEN&&sym!=VARTK&&sym!=RPARENT&&sym!=SEMICN&&sym!=BEGINTK)//跳过不可识别字符
		{
			sym=getsym();	
		}
	}
	else
		sym=getsym();
	if(sym!=RPARENT)//参数表
	{
		if(!paradefine())
		{
			proc_error_flag=1;
			while(sym!=IDEN&&sym!=VARTK&&sym!=RPARENT&&sym!=SEMICN&&sym!=BEGINTK)//跳过不可识别字符
			{
				sym=getsym();	
			}
		}
		else
		{
			if(sym!=RPARENT)//缺少）
			{
				errorline[errornum]=temp_linenum;
				error[errornum++]=20;
				proc_error_flag=1;
				while(sym!=IDEN&&sym!=VARTK&&sym!=RPARENT&&sym!=SEMICN&&sym!=BEGINTK)//跳过不可识别字符
				{
					sym=getsym();	
				}	
			}
			else
			{
				sym=getsym();
			}
		}
	}
	else
		sym=getsym();
	if(sym!=SEMICN)
	{
		errorline[errornum]=temp_linenum;
		error[errornum++]=7;
		proc_error_flag=1;
		while(sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//跳过不可识别字符
		{
			sym=getsym();	
		}
	}
	else
		sym=getsym();
	if(!block())//处理分程序
	{
		proc_error_flag=1;
	}
	temp_linenum=linenum;
	if(sym!=SEMICN)
	{
		errorline[errornum]=linenum;
		error[errornum++]=7;
		proc_error_flag=1;
	}
	else
		sym=getsym();
	while(sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//跳到预期的单词符号
	{
		proc_error_flag=1;
		sym=getsym();
	}
	lev--;
	return 1;
}

//函数说明
int func()
{
	int func_error_flag=0;
	int funcname_index;
	temp_linenum=linenum;
	sym=getsym();
	if(sym!=IDEN)//缺少过程名
	{
		errorline[errornum]=temp_linenum;
		error[errornum++]=21;
		return 0;
	}
//	strcpy(func_name,word);//标识符存入func_name
	strcpy(name[top_of_table],word);//标识符存入name
	if(!enter(lev,INT_TYPE,FUNC_KIND))//过程名登陆符号表
		func_error_flag=1;
	funcname_index=top_of_table-1;
	sym=getsym();
	lev++;//层数加1
	if(sym!=LPARENT)//缺少（
	{
		errorline[errornum]=temp_linenum;
		error[errornum++]=20;
		func_error_flag=1;
		while(sym!=IDEN&&sym!=VARTK&&sym!=RPARENT&&sym!=SEMICN&&sym!=COLON&&sym!=INTTK&&sym!=CHARTK&&sym!=FLOATTK&&sym!=BEGINTK)//跳过不可识别字符
		{
			sym=getsym();	
		}
	}
	else
		sym=getsym();
	if(sym!=RPARENT)//参数表
	{
		if(!paradefine())
		{
			func_error_flag=1;
			while(sym!=IDEN&&sym!=VARTK&&sym!=RPARENT&&sym!=SEMICN&&sym!=BEGINTK)//跳过不可识别字符
			{
				sym=getsym();	
			}
		}
		else
		{
			if(sym!=RPARENT)//缺少）
			{
				errorline[errornum]=temp_linenum;
				error[errornum++]=20;
				func_error_flag=1;
				while(sym!=IDEN&&sym!=VARTK&&sym!=RPARENT&&sym!=SEMICN&&sym!=BEGINTK)//跳过不可识别字符
				{
					sym=getsym();	
				}	
			}
			else
			{
				sym=getsym();
			}
		}
	}
	else
		sym=getsym();
	if(sym!=COLON)
	{
		errorline[errornum]=temp_linenum;
		error[errornum++]=7;
		func_error_flag=1;
		while(sym!=SEMICN&&sym!=COLON&&sym!=INTTK&&sym!=CHARTK&&sym!=FLOATTK&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//跳过不可识别字符
		{
			sym=getsym();	
		}
	}
	else
		sym=getsym();
	if(sym==INTTK)
	{
		tbl[funcname_index].type=INT_TYPE;
		sym=getsym();
	}
	else if(sym==FLOATTK)
	{
		tbl[funcname_index].type=REAL_TYPE;
		sym=getsym();
	}
	else if(sym==CHARTK)
	{
		tbl[funcname_index].type=CHAR_TYPE;
		sym=getsym();
	}
	else
	{
		errorline[errornum]=temp_linenum;
		error[errornum++]=14;
		func_error_flag=1;
		while(sym!=SEMICN&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//跳过不可识别字符
		{
			sym=getsym();	
		}
	}
	if(sym!=SEMICN)
	{
		errorline[errornum]=temp_linenum;
		error[errornum++]=7;
		func_error_flag=1;
		while(sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//跳过不可识别字符
		{
			sym=getsym();	
		}
	}
	else
		sym=getsym();
	if(!block())//处理分程序
	{
		func_error_flag=1;
	}
	temp_linenum=linenum;
	if(sym!=SEMICN)
	{
		errorline[errornum]=linenum;
		error[errornum++]=7;
		func_error_flag=1;
	}
	else
		sym=getsym();
	while(sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//跳到预期的单词符号
	{
		func_error_flag=1;
		sym=getsym();
	}
	lev--;
	return 1;
}

//参数表
int paradefine()
{
	int paradefine_error_flag=0;//标记变量声明是否正确
	int paradefine_temp_linenum;//记录变量声明开始行数
	paradefine_temp_linenum=linenum;
	temp_linenum=linenum;
	//sym=getsym();
	while(sym!=IDEN&&sym!=VARTK&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//跳过不可识别字符
	{
		sym=getsym();
		paradefine_error_flag=1;
	}
	if(paradefine_error_flag)//缺少标识符或var
	{
		errorline[errornum]=temp_linenum;
		error[errornum++]=8;
	}
	while(sym==IDEN||sym==VARTK)//循环处理变量声明语句
	{
		if(!paradefinestatement())
		{
			paradefine_error_flag=1;	
		}
		//sym=getsym();
	}
	if(paradefine_error_flag)
	{
		errorline[errornum]=paradefine_temp_linenum;
		error[errornum++]=11;
	}
	return 1;
}

//形式参数段
int paradefinestatement()
{
	int paradefinestatement_error_flag=0;//标记变量声明语句错误
	char temp_name[MAXVARDEFINE][MAXWORD];//暂存变量
	int i=0,top_of_temp_name=0;
	temp_linenum=linenum;
	if(sym==VARTK)
	{
		sym=getsym();
	}
	while(sym==IDEN)
	{
		strcpy(temp_name[top_of_temp_name++],word);//标识符存入temp_name
		sym=getsym();
		if(sym==COMMA)//遇见“，”，继续循环
		{
			sym=getsym();
			continue;
		}
		if(sym==COLON)//遇见“：”，跳出
		{
			sym=getsym();
			break;
		}
		else//缺少“，”或“：”
		{
			errorline[errornum]=temp_linenum;
			error[errornum++]=7;
			paradefinestatement_error_flag=1;
			while(sym!=IDEN&&sym!=SEMICN&&sym!=OFTK&&sym!=INTTK&&sym!=FLOATTK&&sym!=CHARTK&&sym!=ARRAYTK
				&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//跳过不可识别字符
			{
				sym=getsym();	
			}
		}
	}
	if(sym==INTTK)//integer类型
	{
		for(i=0;i<top_of_temp_name;i++)
		{
			strcpy(name[top_of_table],temp_name[i]);//标识符存入name
			if(!enter(lev,INT_TYPE,PARA_KIND))
				paradefinestatement_error_flag=1;
		}
		sym=getsym();
	}
	else if(sym==FLOATTK)//real类型
	{
		for(i=0;i<top_of_temp_name;i++)
		{
			strcpy(name[top_of_table],temp_name[i]);//标识符存入name
			if(!enter(lev,REAL_TYPE,PARA_KIND))
				paradefinestatement_error_flag=1;
		}
		sym=getsym();
	}
	else if(sym==CHARTK)//char类型
	{
		for(i=0;i<top_of_temp_name;i++)
		{
			strcpy(name[top_of_table],temp_name[i]);//标识符存入name
			if(!enter(lev,CHAR_TYPE,PARA_KIND))
				paradefinestatement_error_flag=1;
		}
		sym=getsym();
	}
	else
	{
		errorline[errornum]=temp_linenum;
		error[errornum++]=14;
		paradefinestatement_error_flag=1;
		while(sym!=IDEN&&sym!=SEMICN
			&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//跳过不可识别字符
		{
			sym=getsym();	
		}
	}
	if(sym==SEMICN)
	{
		sym=getsym();
	}
	else if(sym==RPARENT)//遇见）结束
	{
		if(paradefinestatement_error_flag)
			return 0;
		return 1;
	}
	else
	{
		errorline[errornum]=temp_linenum;
		error[errornum++]=7;
		paradefinestatement_error_flag=1;
		while(sym!=IDEN&&sym!=SEMICN&&sym!=INTTK&&sym!=FLOATTK&&sym!=CHARTK&&sym!=ARRAYTK
			&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//跳过不可识别字符
		{
			sym=getsym();	
		}
	}
	if(paradefinestatement_error_flag)
		return 0;
	return 1;
}

//复合语句
int compound()
{
	int compound_error_flag=0;
	int compound_linenum=linenum;
	int compound_linenum1=linenum;

	sym=getsym();
	while(sym!=IDEN&&sym!=IFTK&&sym!=FORTK&&sym!=CASETK&&sym!=BEGINTK&&sym!=READTK
				&&sym!=WRITETK&&sym!=SEMICN&&sym!=ENDTK)//跳过不可识别字符
	{
		sym=getsym();
		compound_error_flag=1;
	}
	while(sym==IDEN||sym==IFTK||sym==FORTK||sym==CASETK||sym==BEGINTK||sym==READTK||sym==WRITETK||sym==SEMICN||sym==ENDTK)
	{
		compound_linenum=linenum;
		if(sym==ENDTK)
		{
			sym=getsym();
			if(compound_error_flag)
			{
				//errorline[errornum]=compound_linenum1;
				//error[errornum++]=18;
				return 0;
			}
			return 1;
		}
		if(sym==SEMICN)
		{
			sym=getsym();
			continue;
		}
		if(!statement())
		{
		//	errorline[errornum]=compound_linenum;
		//	error[errornum++]=18;
			compound_error_flag=1;
			while(sym!=SEMICN&&sym!=ENDTK)//跳过不可识别字符
			{
				sym=getsym();	
			}
		}
		if(sym!=ENDTK&&sym!=SEMICN)
		{
			errorline[errornum]=compound_linenum;
			error[errornum++]=7;
			compound_error_flag=1;
			while(sym!=IDEN&&sym!=IFTK&&sym!=FORTK&&sym!=CASETK&&sym!=BEGINTK&&sym!=READTK
				&&sym!=WRITETK&&sym!=SEMICN&&sym!=ENDTK&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//跳过不可识别字符
			{
				sym=getsym();
			}
		}
	}
	errorline[errornum]=compound_linenum1;
	error[errornum++]=18;
	return 0;
}

//处理语句
int statement()
{
	int find_return;
	if(sym==IDEN)
	{
		find_return=find();
		if(find_return==-1)
		{
			errorline[errornum]=linenum;
			error[errornum++]=22;
			while(sym!=SEMICN&&sym!=ENDTK)
			{
				sym=getsym();
			}
		//	sym=getsym();
			return 0;
		}
		else if(tbl[find_return].kind==PROC_KIND)
		{
			if(!proccall(find_return))
				return 0;
			if(!gen(CAL,lev-tbl[find_return].lev,tbl[find_return].adr))
			{
				return 0;
			}
			return 1;
		}
		else
		{
			return assignstatement(find_return);
		}
	}
	else if(sym==IFTK)
	{
		//sym=getsym();
		return ifstatement();
	}
	else if(sym==CASETK)
	{
		return casestatement();
	}
	else if(sym==BEGINTK)
	{
		return compound();
	}
	else if(sym==READTK)
	{
		return readstatement();
	}
	else if(sym==WRITETK)
	{
		return writestatement();
	}
	else if(sym==FORTK)
	{
		return forstatement();
	}

//	sym=getsym();
	return 0;
}

//过程调用
int proccall(int proc_name_index)
{
	int proccall_error=0,i,para_type,parameterListCount=0,xing_parameterListCount=0;
	sym=getsym();
	if(sym!=LPARENT)
	{
		errorline[errornum]=linenum;
		error[errornum++]=20;
		while(sym!=SEMICN&&sym!=ENDTK)
		{
			sym=getsym();
		}
		return 0;
	}
	sym=getsym();
	if(sym!=RPARENT)
	{
		for(i=proc_name_index+1;(i<top_of_table)&&(tbl[i].kind==PARA_KIND);i++)
		{
			if(sym!=RPARENT&&sym!=SEMICN)
			{
				para_type=expression();
				if(para_type==0)
				{
					errorline[errornum]=linenum;
					error[errornum++]=24;
					while(sym!=SEMICN&&sym!=ENDTK)
					{
						sym=getsym();
					}
					return 0;
				}
				if((tbl[i].type==INT_TYPE&&para_type==REAL_TYPE)||(tbl[i].type==CHAR_TYPE&&para_type==REAL_TYPE))
				{
					errorline[errornum]=linenum;
					error[errornum++]=23;
					while(sym!=SEMICN&&sym!=ENDTK)
					{
						sym=getsym();
					}
					return 0;
				}
				parameterListCount++;	
				if(sym==COMMA)
					sym=getsym();
				else
				{
					xing_parameterListCount++;
					break;
				}
			}
			xing_parameterListCount++;
		}
		if(xing_parameterListCount!=parameterListCount)
		{
			errorline[errornum]=linenum;
			error[errornum++]=24;
			while(sym!=SEMICN&&sym!=ENDTK)
			{
				sym=getsym();
			}
			return 0;
		}
	}
	if(sym!=RPARENT)
	{
		errorline[errornum]=linenum;
		error[errornum++]=24;
		while(sym!=SEMICN&&sym!=ENDTK)
		{
			sym=getsym();
		}
		return 0;
	}
	for(i=parameterListCount;i>0;i--)
	{
		if(!gen(STO,-1,0))
		{
			return 0;
		}
	}
	sym=getsym();
	return 1;
}

//函数调用
int funccall(int func_name_index)
{
	int funcall_error=0,i,para_type,parameterListCount=0,xing_parameterListCount=0;
	sym=getsym();
	if(sym!=LPARENT)
	{
		errorline[errornum]=linenum;
		error[errornum++]=20;
		while(sym!=SEMICN&&sym!=ENDTK)
		{
			sym=getsym();
		}
		return 0;
	}
	sym=getsym();
	if(sym!=RPARENT)
	{
		for(i=func_name_index+1;(i<top_of_table)&&(tbl[i].kind==PARA_KIND);i++)
		{
			if(sym!=RPARENT&&sym!=SEMICN)
			{
				para_type=expression();
				if(para_type==0)
				{
					errorline[errornum]=linenum;
					error[errornum++]=24;
					while(sym!=SEMICN&&sym!=ENDTK)
					{
						sym=getsym();
					}
					return 0;
				}
				if((tbl[i].type==INT_TYPE&&para_type==REAL_TYPE)||(tbl[i].type==CHAR_TYPE&&para_type==REAL_TYPE))
				{
					errorline[errornum]=linenum;
					error[errornum++]=23;
					while(sym!=SEMICN&&sym!=ENDTK)
					{
						sym=getsym();
					}
					return 0;
				}
				parameterListCount++;	
				if(sym==COMMA)
					sym=getsym();
				else
				{
					xing_parameterListCount++;
					break;
				}
			}
			xing_parameterListCount++;
		}
		if(xing_parameterListCount!=parameterListCount)
		{
			errorline[errornum]=linenum;
			error[errornum++]=24;
			while(sym!=SEMICN&&sym!=ENDTK)
			{
				sym=getsym();
			}
			return 0;
		}
	}
	if(sym!=RPARENT)
	{
		errorline[errornum]=linenum;
		error[errornum++]=24;
		while(sym!=SEMICN&&sym!=ENDTK)
		{
			sym=getsym();
		}
		return 0;
	}
	for(i=parameterListCount;i>0;i--)
	{
		if(!gen(STO,-1,0))
		{
			return 0;
		}
	}
	sym=getsym();
	return 1;
}
//赋值语句
int assignstatement(int iden_index)
{
	int iden_type=0;//标志是不是函数赋值语句
	int exp_type;

	if(tbl[iden_index].kind==VAR_KIND)
	{
		if(!gen(LODADR,lev-tbl[iden_index].lev,tbl[iden_index].adr))
			return 0;
		sym=getsym();
	}
	else if(tbl[iden_index].kind==ARRAY_KIND)
	{
		sym=getsym();
		if(!gen(LODADR,lev-tbl[iden_index].lev,tbl[iden_index].adr))
			return 0;
		if(sym!=LBRACK)
		{
			errorline[errornum]=linenum;
			error[errornum++]=13;
			while(sym!=SEMICN&&sym!=ENDTK)
			{
				sym=getsym();
			}
			return 0;
		}
		sym=getsym();
		if(!expression())
			return 0;
		if(!gen(OPR,tbl[iden_index].count,18))
			return 0;
		if(!gen(OPR,0,2))
			return 0;
		if(sym!=RBRACK)
		{
			errorline[errornum]=linenum;
			error[errornum++]=13;
			while(sym!=SEMICN&&sym!=ENDTK)
			{
				sym=getsym();
			}
			return 0;
		}
		sym=getsym();
	}
	else if(tbl[iden_index].kind==FUNC_KIND)
	{
		sym=getsym();
		iden_type=1;
	}
	else
	{
		errorline[errornum]=linenum;
		error[errornum++]=30;
		while(sym!=SEMICN&&sym!=ENDTK)
		{
			sym=getsym();
		}
		return 0;
	}
	if(sym!=ASSIGN)
	{
		errorline[errornum]=linenum;
		error[errornum++]=27;
		while(sym!=SEMICN&&sym!=ENDTK)
		{
			sym=getsym();
		}
		return 0;
	}
	sym=getsym();
	exp_type=expression();
	if(!exp_type)
		return 0;
	if(iden_type==0)
	{
		if(tbl[iden_index].type==REAL_TYPE)
		{
			if(!gen(STO,0,REAL_TYPE))
				return 0;
		}
		else if(tbl[iden_index].type==INT_TYPE)
		{
			if(exp_type==REAL_TYPE)
			{
				errorline[errornum]=linenum;
				error[errornum++]=35;
			}
			if(!gen(STO,0,INT_TYPE))
				return 0;
		}
		else if(tbl[iden_index].type==CHAR_TYPE)
		{
			if(exp_type==REAL_TYPE)
			{
				errorline[errornum]=linenum;
				error[errornum++]=35;
			}
			if(!gen(STO,0,CHAR_TYPE))
				return 0;
		}
	}
	else if(iden_type==1)
	{
		if(tbl[iden_index].type==REAL_TYPE)
		{
			if(!gen(STO,-4,REAL_TYPE))
				return 0;
		}
		else if(tbl[iden_index].type==INT_TYPE)
		{
			if(exp_type==REAL_TYPE)
			{
				errorline[errornum]=linenum;
				error[errornum++]=35;
			}
			if(!gen(STO,-4,INT_TYPE))
				return 0;
		}
		else if(tbl[iden_index].type==CHAR_TYPE)
		{
			if(exp_type==REAL_TYPE)
			{
				errorline[errornum]=linenum;
				error[errornum++]=35;
			}
			if(!gen(STO,-4,CHAR_TYPE))
				return 0;
		} 
		func_assaign_flag=1;
	}
	return 1;
}

//条件语句
int ifstatement()
{
	int a;
	int pcode_index1,pcode_index2;
	sym=getsym();
	if(!expression())
		return 0;
//	sym=getsym();
	if(sym!=LSS&&sym!=LEQ&&sym!=GRE&&sym!=GEQ&&sym!=EQL&&sym!=NEQ)
	{
		errorline[errornum]=linenum;
		error[errornum++]=28;
		while(sym!=SEMICN&&sym!=ENDTK)
		{
			sym=getsym();
		}
		return 0;
	}
	a=sym;
	sym=getsym();
	if(!expression())
		return 0;
	switch(a)
	{
		case LSS:
			if(!gen(OPR,0,9))
				return 0;
			break;
		case LEQ:
			if(!gen(OPR,0,10))
				return 0;
			break;
		case GRE:
			if(!gen(OPR,0,11))
				return 0;
			break;
		case GEQ:
			if(!gen(OPR,0,12))
				return 0;
			break;
		case EQL:
			if(!gen(OPR,0,7))
				return 0;
			break;
		case NEQ:
			if(!gen(OPR,0,8))
				return 0;
			break;
		default:
			return 0;
	}
	pcode_index1=top_of_pcode++;
	if(sym!=THENTK)
	{
		errorline[errornum]=linenum;
		error[errornum++]=29;
		while(sym!=SEMICN&&sym!=ENDTK)
		{
			sym=getsym();
		}
		return 0;
	}
	sym=getsym();
	if(sym!=ELSETK&&sym!=SEMICN&&sym!=ENDTK)
	{
		if(!statement())
		{
			return 0;	
		}
		
	}
	pcode_index2=top_of_pcode++;
	pcod[pcode_index1].op=JPC;
	pcod[pcode_index1].lev=0;
	pcod[pcode_index1].x=top_of_pcode;
	if(sym==ELSETK)
	{
		sym=getsym();
		if(sym!=SEMICN&&sym!=ENDTK)
		{
			if(!statement())
			{
				return 0;	
			}
		}
	}
	else if(sym!=SEMICN&&sym!=ENDTK)
	{
		errorline[errornum]=linenum;
		error[errornum++]=38;
		while(sym!=SEMICN&&sym!=ENDTK)
		{
			sym=getsym();
		}
		return 0;
	}
	pcod[pcode_index2].op=JMP;
	pcod[pcode_index2].lev=0;
	pcod[pcode_index2].x=top_of_pcode;	
	return 1;
}

//情况语句
int casestatement()
{
	int pcode_index1;
	int find_return,case_error_flag=0;
	int pcode_index[20],pcode_index2[20];
	int top_of_pcode_index=0,top_of_pcode_index2=0;
	int i;
	int exp_type;
	sym=getsym();
	exp_type=expression();
	if(!exp_type)
	{
		while(sym!=ENDTK)
		{
			sym=getsym();
		}
		sym=getsym();
		return 0;
	}
	else if(exp_type==REAL_TYPE)
	{
		errorline[errornum]=linenum;
		error[errornum++]=39;
	}
	pcode_index1=top_of_pcode;
	if(sym!=OFTK)
	{
		errorline[errornum]=linenum;
		error[errornum++]=15;
		while(sym!=IDEN&&sym!=INTCON&&sym!=CHARCON&&sym!=REALCON&&sym!=SEMICN&&sym!=ENDTK)
		{
			sym=getsym();
		}
		case_error_flag=1;
	}
	else
		sym=getsym();
	while(sym==IDEN||sym==INTCON||sym==CHARCON||sym==REALCON)
	{
		top_of_pcode_index=0;
		while(sym==IDEN||sym==INTCON||sym==CHARCON||sym==REALCON)
		{
			if(sym==IDEN)
			{
				find_return=find();
				if(find_return==-1)
				{
					errorline[errornum]=linenum;
					error[errornum++]=22;
					while(sym!=COMMA&&sym!=COLON&&sym!=SEMICN&&sym!=ENDTK)
					{
						sym=getsym();
					}
				}
			}
			if(sym==IDEN&&!(tbl[find_return].kind==CONST_KIND&&tbl[find_return].type!=REAL_TYPE)||sym==REALCON)	
			{
				errorline[errornum]=linenum;
				error[errornum++]=33;
				while(sym!=COMMA&&sym!=COLON&&sym!=SEMICN&&sym!=ENDTK)
				{
					sym=getsym();
				}
			}
			if(sym==IDEN&&tbl[find_return].kind==INT_TYPE)
			{
				if(!gen(LIT,INT_TYPE,tbl[find_return].uchar.num))
				{
					case_error_flag=1;
					while(sym!=COMMA&&sym!=COLON&&sym!=SEMICN&&sym!=ENDTK)
					{
						sym=getsym();
					}
				}
				sym=getsym();
			}
			else if(sym==IDEN&&tbl[find_return].kind==CHAR_TYPE)
			{
				if(!gen(LIT,CHAR_TYPE,tbl[find_return].uchar.ch))
				{
					case_error_flag=1;
					while(sym!=COMMA&&sym!=COLON&&sym!=SEMICN&&sym!=ENDTK)
					{
						sym=getsym();
					}
				}
				sym=getsym();
			}
			else if(sym==INTCON)
			{
				if(!gen(LIT,INT_TYPE,num))
				{
					case_error_flag=1;
					while(sym!=COMMA&&sym!=COLON&&sym!=SEMICN&&sym!=ENDTK)
					{
						sym=getsym();
					}
				}
				sym=getsym();
			}
			else if(sym==CHARCON)
			{
				if(!gen(LIT,CHAR_TYPE,symchar))
				{
					case_error_flag=1;
					while(sym!=COMMA&&sym!=COLON&&sym!=SEMICN&&sym!=ENDTK)
					{
						sym=getsym();
					}
				}
				sym=getsym();
			}

			if(!gen(OPR,0,15))
			{
				case_error_flag=1;
				while(sym!=COMMA&&sym!=COLON&&sym!=SEMICN&&sym!=ENDTK)
				{
					sym=getsym();
				}
			}

			if(sym==COLON)
			{
				pcode_index1=top_of_pcode;
				if(!gen(JPC,0,0))
				{
					case_error_flag=1;	
				}
				sym=getsym();
				for(i=0;i<top_of_pcode_index;i++)
				{
					pcod[pcode_index[i]].x=top_of_pcode;
				}
				break;
			}
			else if(sym==COMMA)
			{
				pcode_index[top_of_pcode_index++]=top_of_pcode;
				if(!gen(JPC,1,0))
				{
					case_error_flag=1;
					while(sym!=COMMA&&sym!=COLON&&sym!=SEMICN&&sym!=ENDTK)
					{
						sym=getsym();
					}
				}
				sym=getsym();
				continue;
			}
			else
			{
				errorline[errornum]=linenum;
				error[errornum++]=11;
				while(sym!=SEMICN&&sym!=ENDTK)
				{
					sym=getsym();
				}
				case_error_flag=1;
			}
		}
		if(sym!=SEMICN&&sym!=ENDTK)
		{
			if(!statement())
			{
				case_error_flag=1;
				while(sym!=SEMICN&&sym!=ENDTK)
				{
					sym=getsym();
				}
			}
			pcode_index2[top_of_pcode_index2++]=top_of_pcode;
			if(!gen(JMP,0,0))
			{
				case_error_flag=1;
				while(sym!=COMMA&&sym!=COLON&&sym!=SEMICN&&sym!=ENDTK)
				{
					sym=getsym();
				}
			}
		//	pcod[pcode_index1].lev=1;
			pcod[pcode_index1].x=top_of_pcode;
		}
		if(sym==SEMICN)
		{
			sym=getsym();
			continue;
		}
		else if(sym==ENDTK)
		{
			break;
		}
		else
		{
			errorline[errornum]=linenum;
			error[errornum++]=38;
			while(sym!=SEMICN&&sym!=ENDTK)
			{
				sym=getsym();
			}
			case_error_flag=1;
		}
	}
	if(sym==ENDTK)
	{
		sym=getsym();
		while((top_of_pcode_index2-1)>=0)
		{
			pcod[pcode_index2[--top_of_pcode_index2]].x=top_of_pcode;
		}
		if(case_error_flag)
			return 0;
		return 1;
	}
	else
	{
		errorline[errornum]=linenum;
		error[errornum++]=7;
		return 0;
	}
	if(case_error_flag)
		return 0;
}

//读语句
int readstatement()
{
	int find_return,read_error_flag=0;
	sym=getsym();
	if(sym!=LPARENT)
	{
		errorline[errornum]=linenum;
		error[errornum++]=20;
		while(sym!=SEMICN&&sym!=ENDTK)
		{
			sym=getsym();
		}
		return 0;
	}
	sym=getsym();
	while(sym==IDEN)
	{
		find_return=find();
		if(tbl[find_return].kind==VAR_KIND||tbl[find_return].kind==PARA_KIND)
		{
			if(!gen(LODADR,lev-tbl[find_return].lev,tbl[find_return].adr))
				return 0;
			if(!gen(RED,tbl[find_return].type,0))
				return 0;
		}
		else
		{
			errorline[errornum]=linenum;
			error[errornum++]=37;
			read_error_flag=1;
		}
		sym=getsym();
		if(sym==RPARENT)
		{
			break;
		}
		else if(sym==COMMA)
		{
			sym=getsym();
			continue;
		}
		else
		{
			errorline[errornum]=linenum;
			error[errornum++]=7;
			read_error_flag=1;
			while(sym!=IDEN&&sym!=RPARENT&&sym!=SEMICN&&sym!=ENDTK)
			{
				sym=getsym();
			}
		}
	}
	if(sym!=RPARENT)
	{
		errorline[errornum]=linenum;
		error[errornum++]=20;
		while(sym!=SEMICN&&sym!=ENDTK)
		{
			sym=getsym();
		}
		return 0;
	}
	else
		sym=getsym();
	if(read_error_flag)
		return 0;
	return 1;
}

//写语句
int writestatement()
{
	sym=getsym();
	if(sym!=LPARENT)
	{
		errorline[errornum]=linenum;
		error[errornum++]=20;
		while(sym!=SEMICN&&sym!=ENDTK)
		{
			sym=getsym();			
		}
		return 0;
	}
	sym=getsym();
	if(sym==STRCON)
	{
		strcpy(printstring[top_of_printstr++],symstring);
		if(!gen(WRT,0,(top_of_printstr-1)))
		{
			//errorline[errornum]=linenum;
			//error[errornum++]=20;
			while(sym!=SEMICN&&sym!=ENDTK)
			{
				sym=getsym();			
			}
			return 0;
		}
		sym=getsym();
		if(sym==COMMA)
		{
			sym=getsym();
			if(!expression())
			{
				//errorline[errornum]=linenum;
				//error[errornum++]=20;
				while(sym!=SEMICN&&sym!=ENDTK)
				{
					sym=getsym();			
				}
				return 0;
			}
			if(!gen(WRT,1,0))
			{
				while(sym!=SEMICN&&sym!=ENDTK)
				{
					sym=getsym();			
				}
				return 0;
			}
		//	sym=getsym();
		}
	}
	else
	{
		if(!expression())
		{
			//errorline[errornum]=linenum;
			//error[errornum++]=20;
			while(sym!=SEMICN&&sym!=ENDTK)
			{
				sym=getsym();			
			}
			return 0;
		}
		if(!gen(WRT,1,0))
		{
			while(sym!=SEMICN&&sym!=ENDTK)
			{
				sym=getsym();			
			}
			return 0;
		}
	}
	if(sym!=RPARENT)
	{
		errorline[errornum]=linenum;
		error[errornum++]=36;
		while(sym!=SEMICN&&sym!=ENDTK)
		{
			sym=getsym();			
		}
		return 0;
	}
	sym=getsym();
	return 1;
}

//for循环语句
int forstatement()
{
	int pcode_index1,pcode_index2;
	int find_return;
	int to_direction=0;
	sym=getsym();
	if(sym!=IDEN)
	{
		errorline[errornum]=linenum;
		error[errornum++]=8;
		while(sym!=SEMICN&&sym!=ENDTK)
		{
			sym=getsym();			
		}
		return 0;
	}
	find_return=find();
	if(find_return==-1)
	{
		errorline[errornum]=linenum;
		error[errornum++]=22;
		while(sym!=SEMICN&&sym!=ENDTK)
		{
			sym=getsym();			
		}
		return 0;
	}
	if(tbl[find_return].kind!=VAR_KIND&&tbl[find_return].kind!=PARA_KIND)
	{
		errorline[errornum]=linenum;
		error[errornum++]=30;
		while(sym!=SEMICN&&sym!=ENDTK)
		{
			sym=getsym();			
		}
		return 0;
	}
	if(!gen(LODADR,lev-tbl[find_return].lev,tbl[find_return].adr))
	{
		while(sym!=SEMICN&&sym!=ENDTK)
		{
			sym=getsym();			
		}
		return 0;
	}
	sym=getsym();
	if(sym!=ASSIGN)
	{
		errorline[errornum]=linenum;
		error[errornum++]=27;
		while(sym!=SEMICN&&sym!=ENDTK)
		{
			sym=getsym();			
		}
		return 0;
	}
	sym=getsym();
	if(!expression())
	{
		while(sym!=SEMICN&&sym!=ENDTK)
		{
			sym=getsym();			
		}
		return 0;
	}
	if(!gen(STO,0,tbl[find_return].type))
	{
		while(sym!=SEMICN&&sym!=ENDTK)
		{
			sym=getsym();			
		}
		return 0;
	}
	if(sym!=TOTK&&sym!=DOWNTOTK)
	{
		errorline[errornum]=linenum;
		error[errornum++]=31;
		while(sym!=SEMICN&&sym!=ENDTK)
		{
			sym=getsym();			
		}
		return 0;
	}
	if(sym==DOWNTOTK)
	{
		to_direction=1;
	}
	sym=getsym();
	if(!expression())
	{
		while(sym!=SEMICN&&sym!=ENDTK)
		{
			sym=getsym();			
		}
		return 0;
	}
	pcode_index1=top_of_pcode;
	if(!gen(LODADR,lev-tbl[find_return].lev,tbl[find_return].adr))
	{
		while(sym!=SEMICN&&sym!=ENDTK)
		{
			sym=getsym();			
		}
		return 0;
	}
	if(!gen(LOD,0,0))
	{
		while(sym!=SEMICN&&sym!=ENDTK)
		{
			sym=getsym();			
		}
		return 0;
	}
	if(!to_direction)
	{
		if(!gen(OPR,0,16))
		{
			while(sym!=SEMICN&&sym!=ENDTK)
			{
				sym=getsym();			
			}
			return 0;
		}
	}
	else
	{
		if(!gen(OPR,0,17))
		{
			while(sym!=SEMICN&&sym!=ENDTK)
			{
				sym=getsym();			
			}
			return 0;
		}
	}
	pcode_index2=top_of_pcode;
	if(!gen(JPC,1,0))
	{
		while(sym!=SEMICN&&sym!=ENDTK)
		{
			sym=getsym();			
		}
		return 0;
	}
	if(sym!=DOTK)
	{
		errorline[errornum]=linenum;
		error[errornum++]=32;
		while(sym!=SEMICN&&sym!=ENDTK)
		{
			sym=getsym();			
		}
		return 0;
	}
	sym=getsym();
	if(sym!=SEMICN&&sym!=ENDTK)
	{
		if(!statement())
		{
			while(sym!=SEMICN&&sym!=ENDTK)
			{
				sym=getsym();			
			}
			return 0;
		}
	}
	if(!gen(LODADR,lev-tbl[find_return].lev,tbl[find_return].adr))
	{
		while(sym!=SEMICN&&sym!=ENDTK)
		{
			sym=getsym();			
		}
		return 0;
	}
	if(!gen(LOD,0,0))
	{
		while(sym!=SEMICN&&sym!=ENDTK)
		{
			sym=getsym();			
		}
		return 0;
	}
	if(!to_direction)
	{
		if(!gen(OPR,0,13))
		{
			while(sym!=SEMICN&&sym!=ENDTK)
			{
				sym=getsym();			
			}
			return 0;
		}
	}
	else
	{
		if(!gen(OPR,0,14))
		{
			while(sym!=SEMICN&&sym!=ENDTK)
			{
				sym=getsym();			
			}
			return 0;
		}
	}
	if(!gen(LODADR,lev-tbl[find_return].lev,tbl[find_return].adr))
	{
		while(sym!=SEMICN&&sym!=ENDTK)
		{
			sym=getsym();			
		}
		return 0;
	}
	if(!gen(STO,1,tbl[find_return].type))
	{
		while(sym!=SEMICN&&sym!=ENDTK)
		{
			sym=getsym();			
		}
		return 0;
	}
	if(!gen(JMP,0,pcode_index1))
	{
		while(sym!=SEMICN&&sym!=ENDTK)
		{
			sym=getsym();			
		}
		return 0;
	}
	pcod[pcode_index2].x=top_of_pcode;	
	return 1;
}

//表达式
int expression()
{
	int pes=0;
	int i;
	int exp_type1,exp_type2;
	if(sym==PLUS||sym==MINUS)
	{
		if(sym==MINUS)
		{
			pes=1;// is pessimistic;
		}
		sym=getsym();
	}
	exp_type1=term();
	if(!exp_type1)
		return 0;
	if(pes)
	{
		if(!gen(OPR,0,1))//栈顶数值取负
			return 0;
	}

	while(sym==PLUS||sym==MINUS)
	{
		i=sym;
		sym=getsym();
		exp_type2=term();
		if(!exp_type2)
			return 0;
		else if(exp_type1==REAL_TYPE||exp_type2==REAL_TYPE)
			exp_type1=REAL_TYPE;
		else
			exp_type1=INT_TYPE;
		if(i==PLUS)
		{
			if(!gen(OPR,0,2))
				return 0;
		}
		else
		{
			if(!gen(OPR,0,3))
				return 0;
		}
	}
	return exp_type1;
}

//项
int term()
{
	int mul=1;
	int term_type1,term_type2;
	term_type1=factor();
	if(!term_type1)
		return 0;
	while(sym==MULT||sym==DIV)
	{
		mul=(sym==MULT)?1:0;
		sym=getsym();
		term_type2=factor();
		if(!term_type2)
			return 0;
		else if(term_type1==REAL_TYPE||term_type2==REAL_TYPE)
			term_type1=REAL_TYPE;
		else
			term_type1=INT_TYPE;
		if(mul)
		{
			if(!gen(OPR,0,4))
				return 0;
		}
		else
			if(!gen(OPR,0,5))
				return 0;
	}
	return term_type1;
}

//因子
int factor()
{
	int find_return=0;
	int factor_type,temp_type;
	if(sym==IDEN)
	{
		int find_return=find();
		if(find_return==-1)
		{
			errorline[errornum]=linenum;
		 	error[errornum++]=22;
			return 0;
		}
		if(tbl[find_return].kind==CONST_KIND)//常量
		{
			if(tbl[find_return].type==INT_TYPE)
			{
				if(!gen(LIT,INT_TYPE,tbl[find_return].uchar.num))
					return 0;
				factor_type=INT_TYPE;
			}
			if(tbl[find_return].type==CHAR_TYPE)
			{
				if(!gen(LIT,CHAR_TYPE,tbl[find_return].uchar.ch))
					return 0;
				factor_type=CHAR_TYPE;
			}
			if(tbl[find_return].type==REAL_TYPE){
				if(!gen1(LIT,REAL_TYPE,tbl[find_return].uchar.fnum))
					return 0;
				factor_type=REAL_TYPE;
			}
			sym=getsym();
		}
		else if(tbl[find_return].kind==VAR_KIND||tbl[find_return].kind==PARA_KIND)//变量
		{
			if(!gen(LODADR,lev-tbl[find_return].lev,tbl[find_return].adr))
			{
				return 0;
			}
			if(!gen(LOD,0,0))
				return 0;
			factor_type=tbl[find_return].type;
			sym=getsym();
		}
		else if(tbl[find_return].kind==FUNC_KIND)//函数调用语句
		{
			if(!funccall(find_return))
				return 0;
			if(!gen(CAL,lev-tbl[find_return].lev,tbl[find_return].adr))
			{
				return 0;
			}
			if(!gen(LOD,-1,0))
			{
				return 0;
			}
			factor_type=tbl[find_return].type;
			return 1;
		}
		else if(tbl[find_return].kind==ARRAY_KIND)//数组元素
		{
			factor_type=tbl[find_return].type;
			sym=getsym();
			if(!gen(LODADR,lev-tbl[find_return].lev,tbl[find_return].adr))
				return 0;
			if(sym==LBRACK)
			{
				sym=getsym();
				temp_type=expression();
				if(!gen(OPR,tbl[find_return].count,18))
					return 0;
				if(temp_type==0)
				{
					return 0;
				}
				else if(temp_type==REAL_TYPE)
				{
					errorline[errornum]=linenum;
		 			error[errornum++]=12;
					return 0;
				}
				else
				{
					if(sym!=RBRACK)
					{
						errorline[errornum]=linenum;
		 				error[errornum++]=13;
						return 0;
					}
					else
					{
						sym=getsym();
						if(!gen(OPR,0,2))
							return 0;
						if(!gen(LOD,0,0))
							return 0;					
					}
				}
			}
			else
			{
				errorline[errornum]=linenum;
		 		error[errornum++]=13;
				return 0;
			}
		}
		else
		{
			errorline[errornum]=linenum;
			error[errornum++]=25;
			return 0;
		}
		return factor_type;
	}
	else if(sym==LPARENT){
		sym=getsym();
		factor_type=expression();
		if(!factor_type)
			return 0;
		if(sym!=RPARENT){
			errorline[errornum]=linenum;
			error[errornum++]=20;
			return 0;
		}
		else
			sym=getsym();
		return factor_type;
	}
	else if(sym==INTCON)
	{
		if(!gen(LIT,INT_TYPE,num))
			return 0;
		sym=getsym();
		return INT_TYPE;
	}
	else if(sym==REALCON)
	{
		if(!gen1(LIT,REAL_TYPE,fnum))		
			return 0;
		sym=getsym();
		return REAL_TYPE;
	}
	else if(sym==CHARCON)
	{
		if(!gen(LIT,CHAR_TYPE,symchar))		
			return 0;
		sym=getsym();
		return CHAR_TYPE;
	}
	else{
		errorline[errornum]=linenum;
		error[errornum++]=26;
		return 0;
	}
	return 0;
}