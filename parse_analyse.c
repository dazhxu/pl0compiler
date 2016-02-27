#include"compiler.h"

extern FILE *FIN;  //�ļ�����ָ��
extern FILE *FOUT;	//�ļ�����ָ��
extern char fp[20];		//�ļ���
extern int cifafenxi;

extern int linenum;//������
extern int errornum;//������
extern int errorline[MAXERROR];//�����к�
extern int error[MAXERROR];//������
extern struct table tbl[MAXTABLE];//���ű�
extern int top_of_table;//���ű�ջ��
extern int initial_table_index;//��ʼ���ű�����
extern struct pcode pcod[MAXPCODE];//�м����
extern int top_of_pcode;//�м����ջ��
extern int initial_pcode_index;//��ʼ�м��������
extern struct runstack run[MAXSTACK];//����ջ
extern int top_of_runstack;//����ջջ��
extern int num_of_var[MAXTABLE];//��¼�����������
extern int top_of_numofvar;//num_of_varջ��
extern struct runstack parastack[MAXVARDEFINE];//��¼����ֵ
extern int top_of_parastack;//parastackջ��
extern struct runstack funcstack[MAXVARDEFINE];//��¼��������ֵ
extern int top_of_funcstack;//funcstackջ��
extern int func_flag[MAXVARDEFINE];//��¼�Ƿ�Ϊ����
extern int top_of_funcflag;
extern char printstring[100][MAXSTRING];//���������ַ���
extern int top_of_printstr;

extern int lev;//��ǰ��
extern int temp_linenum;//��¼��ʱ����
extern char name[MAXTABLE][MAXWORD];		//��ʶ������
extern int arraylength;//���鳤��
extern int data_allocation_index;//���ݷ�������
extern int func_assaign_flag;//��Ǻ������Ƿ��к�����ֵ���
extern int base;//��ַ
extern int p;//Ŀ���������
extern int t;//����ջ��

extern char word[MAXWORD]; //��token
extern int sym;	//��ʷ�����
extern int num;	//������
extern double fnum;	//��ʵ��
extern char symchar;	//���ַ�
extern char symstring[MAXSTRING];	//���ַ���
extern char print_string[1000];

extern char *Keyword[];//�ؼ���
extern char *symtype[];//�ʷ�����
extern char *operation[];//Ŀ��������
extern char *err_msg[];//��������

//��ӡ������Ϣ����ʽΪ��line(1)	error7:		ȱ��.
void error_msg(int i)
{
	fprintf(FOUT,"line(%2d)\terror%d:\t\t%s\n",errorline[i],error[i],err_msg[error[i]]);
}

//��½���ű�
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
		if(kind==CONST_KIND)//����ǳ���������ֵ������ű���
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

//�����½���ű�
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

//��ǰ�����Ƿ�Ϸ� 
int test()
{
	int i;
	for(i=top_of_table-1;i>=0;i--){
		if(tbl[i].lev<lev)//���ű���Ĳ���С�ڵ�ǰ�㣬���غϷ�
			return 1;
		else{
			if(!strcmp(tbl[i].name,name[top_of_table]))//�����ظ�����
				return 0;
		}
	}
	return 1;
}

//���
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

//����pcode
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

//��������
int program()
{
	sym=getsym();
	if(block())//�ֳ������
	{
		if(sym==PERIOD)//��.
		{
			return 1;
		}
		else
		{
			errorline[errornum]=linenum;
			error[errornum++]=6;//����ȱ��.��
			while(sym!=FILEOVER)//�����ļ�����
				sym=getsym();
			return 0;
		}
	}
	else
	{
		//errorline[errornum]=linenum;
		//error[errornum++]=6;//���󡰷ֳ���ȫ�����
		while(sym!=FILEOVER)//�����ļ�ĩβ
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
	//����Ԥ�ڵĵ��ʷ���
	while(sym!=CONSTTK&&sym!=VARTK&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)
	{
		block_error_flag=1;
		block_error_flag1=1;
		sym=getsym();
	}
	if(block_error_flag1)
	{
		errorline[errornum]=temp_linenum;
		error[errornum++]=5;//���󡰷Ƿ��﷨�ṹ��
		block_error_flag1=0;
	}

	if(sym==CONSTTK)//��������˵��
	{
		temp_linenum=linenum;
		if(!constdefine())
		{
			block_error_flag=1;
		}
		
		while(sym!=VARTK&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//����Ԥ�ڵĵ��ʷ���
		{
			block_error_flag=1;
			block_error_flag1=1;
			sym=getsym();
		}
		if(block_error_flag1)
		{
			errorline[errornum]=temp_linenum;
			error[errornum++]=5;//���󡰷Ƿ��﷨�ṹ��
			block_error_flag1=0;
		}
	}


	if(sym==VARTK)//��������˵��
	{
		if(!vardefine())
		{
			block_error_flag=1;
		}
		//sym=getsym();
		temp_linenum=linenum;
		//����Ԥ�ڵĵ��ʷ���
		while(sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)
		{
			block_error_flag=1;
			block_error_flag1=1;
			sym=getsym();
		}
		if(block_error_flag1)
		{
			errorline[errornum]=temp_linenum;
			error[errornum++]=5;//���󡰷Ƿ��﷨�ṹ��
			block_error_flag1=0;
		}
	}

	num_of_var[top_of_numofvar++]=data_allocation_index;
	data_allocation_index=3;	

	while(sym==PROCETK||sym==FUNCTK)//�����������
	{	
		if(sym==PROCETK)//�����procedure
		{
			func_flag[top_of_funcflag++]=0;
			if(!proc())
			{
				block_error_flag=1;
				
			}
	//		sym=getsym();
			temp_linenum=linenum;
			//����Ԥ�ڵĵ��ʷ���
			while(sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)
			{
				block_error_flag=1;
				block_error_flag1=1;
				sym=getsym();
			}
			if(block_error_flag1)
			{
				errorline[errornum]=temp_linenum;
				error[errornum++]=5;//���󡰷Ƿ��﷨�ṹ��
				block_error_flag1=0;
			}
		}
		else if(sym==FUNCTK)//�����function
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
			//����Ԥ�ڵĵ��ʷ���
			while(sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)
			{
				block_error_flag=1;
				block_error_flag1=1;
				sym=getsym();
			}
			if(block_error_flag1)
			{
				errorline[errornum]=temp_linenum;
				error[errornum++]=5;//���󡰷Ƿ��﷨�ṹ��
				block_error_flag1=0;
			}
		}
	}

	if(sym==BEGINTK)//�����������˵��
	{
		//�������������Ŀ������һ����������������ת����һ��
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
		//Ϊ��������ռ�
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
		while(sym!=SEMICN&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK&&sym!=PERIOD&&sym!=FILEOVER)//����Ԥ�ڵĵ��ʷ���
		{
			block_error_flag=1;
			block_error_flag1=1;
			sym=getsym();
		}
		if(block_error_flag1)
		{
			errorline[errornum]=temp_linenum;
			error[errornum++]=5;//���󡰷Ƿ��﷨�ṹ��
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

//����˵��
int constdefine()
{
	int constdefine_error_flag=0;
	temp_linenum=linenum;
	sym=getsym();
	while(sym!=IDEN&&sym!=VARTK&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//��������ʶ���ַ�
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
		strcpy(name[top_of_table],word);//��ʶ������name
		if((sym=getsym())==EQL)
		{
			sym=getsym();
			if(sym==PLUS)//�ж�������
			{
				sym=getsym();
				if(sym==INTCON)//������
				{
					if(!enter(lev,INT_TYPE,CONST_KIND))
						constdefine_error_flag=1;
					sym=getsym();
				}
				else if(sym==REALCON)//��ʵ��
				{
					if(!enter(lev,REAL_TYPE,CONST_KIND))
						constdefine_error_flag=1;
					sym=getsym();
				}
				else//�Ƿ�����
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
			else if(sym==MINUS)//�ж��Ǹ���
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
			else if(sym==INTCON)//�ж����޷�������
			{
				if(!enter(lev,INT_TYPE,CONST_KIND))
						constdefine_error_flag=1;
				sym=getsym();
			}
			else if(sym==REALCON)//�ж����޷���ʵ��
			{
				if(!enter(lev,REAL_TYPE,CONST_KIND))
						constdefine_error_flag=1;
				sym=getsym();
			}
			else if(sym==CHARCON)//�ж����ַ�����
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
					while(sym!=IDEN&&sym!=VARTK&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//��������ʶ���ַ�
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
			while(sym!=IDEN&&sym!=VARTK&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//��������ʶ���ַ�
			{
				sym=getsym();
			}
			
		}
		else//ȱ��=
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

//����˵��
int vardefine()
{
	int vardefine_error_flag=0;//��Ǳ��������Ƿ���ȷ
	int vardefine_temp_linenum;//��¼����������ʼ����
	vardefine_temp_linenum=linenum;
	temp_linenum=linenum;
	sym=getsym();
	while(sym!=IDEN&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//��������ʶ���ַ�
	{
		sym=getsym();
		vardefine_error_flag=1;
	}
	if(vardefine_error_flag)//ȱ�ٱ�ʶ��
	{
		errorline[errornum]=temp_linenum;
		error[errornum++]=8;
	}
	while(sym==IDEN)//ѭ����������������
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

//�����������
int vardefinestatement()
{
	int vardefinestatement_error_flag=0;//��Ǳ�������������
	char temp_name[MAXVARDEFINE][MAXWORD];//�ݴ����
	int i=0,top_of_temp_name=0;
	temp_linenum=linenum;
	while(sym==IDEN)
	{
		strcpy(temp_name[top_of_temp_name++],word);//��ʶ������temp_name
		sym=getsym();
		if(sym==COMMA)//����������������ѭ��
		{
			sym=getsym();
			continue;
		}
		if(sym==COLON)//����������������
		{
			sym=getsym();
			break;
		}
		else//ȱ�١������򡰣���
		{
			errorline[errornum]=temp_linenum;
			error[errornum++]=11;
			vardefinestatement_error_flag=1;
			while(sym!=IDEN&&sym!=SEMICN&&sym!=OFTK&&sym!=INTTK&&sym!=FLOATTK&&sym!=CHARTK&&sym!=ARRAYTK
				&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//��������ʶ���ַ�
			{
				sym=getsym();	
			}
		}
	}
	if(sym==INTTK)//integer����
	{
		for(i=0;i<top_of_temp_name;i++)
		{
			strcpy(name[top_of_table],temp_name[i]);//��ʶ������name
			if(!enter(lev,INT_TYPE,VAR_KIND))
				vardefinestatement_error_flag=1;
		}
		sym=getsym();
	}
	else if(sym==FLOATTK)//real����
	{
		for(i=0;i<top_of_temp_name;i++)
		{
			strcpy(name[top_of_table],temp_name[i]);//��ʶ������name
			if(!enter(lev,REAL_TYPE,VAR_KIND))
				vardefinestatement_error_flag=1;
		}
		sym=getsym();
	}
	else if(sym==CHARTK)//char����
	{
		for(i=0;i<top_of_temp_name;i++)
		{
			strcpy(name[top_of_table],temp_name[i]);//��ʶ������name
			if(!enter(lev,CHAR_TYPE,VAR_KIND))
				vardefinestatement_error_flag=1;
		}
		sym=getsym();
	}
	else if(sym==ARRAYTK)//��������
	{
		sym=getsym();
		if(sym==LBRACK)
		{
			sym=getsym();
		}
		else//ȱ��[
		{
			errorline[errornum]=temp_linenum;
			error[errornum++]=13;
			vardefinestatement_error_flag=1;
			while(sym!=IDEN&&sym!=INTCON&&sym==RBRACK&&sym!=SEMICN&&sym!=OFTK&&sym!=INTTK&&sym!=FLOATTK&&sym!=CHARTK&&sym!=ARRAYTK
			&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//��������ʶ���ַ�
			{
				sym=getsym();	
			}
		}
		if(sym==INTCON&&num>0)
		{
			arraylength=num;
			sym=getsym();
		}
		else//���鳤��ȱ��
		{
			errorline[errornum]=temp_linenum;
			error[errornum++]=12;
			vardefinestatement_error_flag=1;
			while(sym!=IDEN&&sym!=RBRACK&&sym!=OFTK&&sym!=SEMICN&&sym!=INTTK&&sym!=FLOATTK&&sym!=CHARTK&&sym!=ARRAYTK
			&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//��������ʶ���ַ�
			{
				sym=getsym();	
			}
		}
		if(sym==RBRACK)
		{
			sym=getsym();
		}
		else//ȱ��]
		{
			errorline[errornum]=temp_linenum;
			error[errornum++]=13;
			vardefinestatement_error_flag=1;
			while(sym!=IDEN&&sym!=SEMICN&&sym!=OFTK&&sym!=INTTK&&sym!=FLOATTK&&sym!=CHARTK&&sym!=ARRAYTK
			&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//��������ʶ���ַ�
			{
				sym=getsym();	
			}
		}
		if(sym==OFTK)
		{
			sym=getsym();
		}
		else//ȱ��of
		{
			errorline[errornum]=temp_linenum;
			error[errornum++]=15;
			vardefinestatement_error_flag=1;
			while(sym!=IDEN&&sym!=SEMICN&&sym!=INTTK&&sym!=FLOATTK&&sym!=CHARTK&&sym!=ARRAYTK
			&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//��������ʶ���ַ�
			{
				sym=getsym();	
			}
		}
		//��½���ű�
		if(sym==INTTK)
		{
			for(i=0;i<top_of_temp_name;i++)
			{
				strcpy(name[top_of_table],temp_name[i]);//��ʶ������name
				if(!enterarray(lev,INT_TYPE,ARRAY_KIND,arraylength))
					vardefinestatement_error_flag=1;
			}
			sym=getsym();
		}
		else if(sym==FLOATTK)
		{
			for(i=0;i<top_of_temp_name;i++)
			{
				strcpy(name[top_of_table],temp_name[i]);//��ʶ������name
				if(!enterarray(lev,REAL_TYPE,ARRAY_KIND,arraylength))
					vardefinestatement_error_flag=1;
			}
			sym=getsym();
		}
		else if(sym==CHARTK)
		{
			for(i=0;i<top_of_temp_name;i++)
			{
				strcpy(name[top_of_table],temp_name[i]);//��ʶ������name
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
			&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//��������ʶ���ַ�
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
			&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//��������ʶ���ַ�
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
			&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//��������ʶ���ַ�
		{
			sym=getsym();	
		}
	}
	if(vardefinestatement_error_flag)
		return 0;
	return 1;
}

//����˵��
int proc()
{
	int proc_error_flag=0;
	temp_linenum=linenum;
	sym=getsym();
	if(sym!=IDEN)//ȱ�ٹ�����
	{
		errorline[errornum]=temp_linenum;
		error[errornum++]=21;
		return 0;
	}
	strcpy(name[top_of_table],word);//��ʶ������temp_name
	if(!enter(lev,INT_TYPE,PROC_KIND))//��������½���ű�
			proc_error_flag=1;
	lev++;//������1
	sym=getsym();
	if(sym!=LPARENT)//ȱ�٣�
	{
		errorline[errornum]=temp_linenum;
		error[errornum++]=20;
		proc_error_flag=1;
		while(sym!=IDEN&&sym!=VARTK&&sym!=RPARENT&&sym!=SEMICN&&sym!=BEGINTK)//��������ʶ���ַ�
		{
			sym=getsym();	
		}
	}
	else
		sym=getsym();
	if(sym!=RPARENT)//������
	{
		if(!paradefine())
		{
			proc_error_flag=1;
			while(sym!=IDEN&&sym!=VARTK&&sym!=RPARENT&&sym!=SEMICN&&sym!=BEGINTK)//��������ʶ���ַ�
			{
				sym=getsym();	
			}
		}
		else
		{
			if(sym!=RPARENT)//ȱ�٣�
			{
				errorline[errornum]=temp_linenum;
				error[errornum++]=20;
				proc_error_flag=1;
				while(sym!=IDEN&&sym!=VARTK&&sym!=RPARENT&&sym!=SEMICN&&sym!=BEGINTK)//��������ʶ���ַ�
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
		while(sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//��������ʶ���ַ�
		{
			sym=getsym();	
		}
	}
	else
		sym=getsym();
	if(!block())//����ֳ���
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
	while(sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//����Ԥ�ڵĵ��ʷ���
	{
		proc_error_flag=1;
		sym=getsym();
	}
	lev--;
	return 1;
}

//����˵��
int func()
{
	int func_error_flag=0;
	int funcname_index;
	temp_linenum=linenum;
	sym=getsym();
	if(sym!=IDEN)//ȱ�ٹ�����
	{
		errorline[errornum]=temp_linenum;
		error[errornum++]=21;
		return 0;
	}
//	strcpy(func_name,word);//��ʶ������func_name
	strcpy(name[top_of_table],word);//��ʶ������name
	if(!enter(lev,INT_TYPE,FUNC_KIND))//��������½���ű�
		func_error_flag=1;
	funcname_index=top_of_table-1;
	sym=getsym();
	lev++;//������1
	if(sym!=LPARENT)//ȱ�٣�
	{
		errorline[errornum]=temp_linenum;
		error[errornum++]=20;
		func_error_flag=1;
		while(sym!=IDEN&&sym!=VARTK&&sym!=RPARENT&&sym!=SEMICN&&sym!=COLON&&sym!=INTTK&&sym!=CHARTK&&sym!=FLOATTK&&sym!=BEGINTK)//��������ʶ���ַ�
		{
			sym=getsym();	
		}
	}
	else
		sym=getsym();
	if(sym!=RPARENT)//������
	{
		if(!paradefine())
		{
			func_error_flag=1;
			while(sym!=IDEN&&sym!=VARTK&&sym!=RPARENT&&sym!=SEMICN&&sym!=BEGINTK)//��������ʶ���ַ�
			{
				sym=getsym();	
			}
		}
		else
		{
			if(sym!=RPARENT)//ȱ�٣�
			{
				errorline[errornum]=temp_linenum;
				error[errornum++]=20;
				func_error_flag=1;
				while(sym!=IDEN&&sym!=VARTK&&sym!=RPARENT&&sym!=SEMICN&&sym!=BEGINTK)//��������ʶ���ַ�
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
		while(sym!=SEMICN&&sym!=COLON&&sym!=INTTK&&sym!=CHARTK&&sym!=FLOATTK&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//��������ʶ���ַ�
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
		while(sym!=SEMICN&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//��������ʶ���ַ�
		{
			sym=getsym();	
		}
	}
	if(sym!=SEMICN)
	{
		errorline[errornum]=temp_linenum;
		error[errornum++]=7;
		func_error_flag=1;
		while(sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//��������ʶ���ַ�
		{
			sym=getsym();	
		}
	}
	else
		sym=getsym();
	if(!block())//����ֳ���
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
	while(sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//����Ԥ�ڵĵ��ʷ���
	{
		func_error_flag=1;
		sym=getsym();
	}
	lev--;
	return 1;
}

//������
int paradefine()
{
	int paradefine_error_flag=0;//��Ǳ��������Ƿ���ȷ
	int paradefine_temp_linenum;//��¼����������ʼ����
	paradefine_temp_linenum=linenum;
	temp_linenum=linenum;
	//sym=getsym();
	while(sym!=IDEN&&sym!=VARTK&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//��������ʶ���ַ�
	{
		sym=getsym();
		paradefine_error_flag=1;
	}
	if(paradefine_error_flag)//ȱ�ٱ�ʶ����var
	{
		errorline[errornum]=temp_linenum;
		error[errornum++]=8;
	}
	while(sym==IDEN||sym==VARTK)//ѭ����������������
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

//��ʽ������
int paradefinestatement()
{
	int paradefinestatement_error_flag=0;//��Ǳ�������������
	char temp_name[MAXVARDEFINE][MAXWORD];//�ݴ����
	int i=0,top_of_temp_name=0;
	temp_linenum=linenum;
	if(sym==VARTK)
	{
		sym=getsym();
	}
	while(sym==IDEN)
	{
		strcpy(temp_name[top_of_temp_name++],word);//��ʶ������temp_name
		sym=getsym();
		if(sym==COMMA)//����������������ѭ��
		{
			sym=getsym();
			continue;
		}
		if(sym==COLON)//����������������
		{
			sym=getsym();
			break;
		}
		else//ȱ�١������򡰣���
		{
			errorline[errornum]=temp_linenum;
			error[errornum++]=7;
			paradefinestatement_error_flag=1;
			while(sym!=IDEN&&sym!=SEMICN&&sym!=OFTK&&sym!=INTTK&&sym!=FLOATTK&&sym!=CHARTK&&sym!=ARRAYTK
				&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//��������ʶ���ַ�
			{
				sym=getsym();	
			}
		}
	}
	if(sym==INTTK)//integer����
	{
		for(i=0;i<top_of_temp_name;i++)
		{
			strcpy(name[top_of_table],temp_name[i]);//��ʶ������name
			if(!enter(lev,INT_TYPE,PARA_KIND))
				paradefinestatement_error_flag=1;
		}
		sym=getsym();
	}
	else if(sym==FLOATTK)//real����
	{
		for(i=0;i<top_of_temp_name;i++)
		{
			strcpy(name[top_of_table],temp_name[i]);//��ʶ������name
			if(!enter(lev,REAL_TYPE,PARA_KIND))
				paradefinestatement_error_flag=1;
		}
		sym=getsym();
	}
	else if(sym==CHARTK)//char����
	{
		for(i=0;i<top_of_temp_name;i++)
		{
			strcpy(name[top_of_table],temp_name[i]);//��ʶ������name
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
			&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//��������ʶ���ַ�
		{
			sym=getsym();	
		}
	}
	if(sym==SEMICN)
	{
		sym=getsym();
	}
	else if(sym==RPARENT)//����������
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
			&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//��������ʶ���ַ�
		{
			sym=getsym();	
		}
	}
	if(paradefinestatement_error_flag)
		return 0;
	return 1;
}

//�������
int compound()
{
	int compound_error_flag=0;
	int compound_linenum=linenum;
	int compound_linenum1=linenum;

	sym=getsym();
	while(sym!=IDEN&&sym!=IFTK&&sym!=FORTK&&sym!=CASETK&&sym!=BEGINTK&&sym!=READTK
				&&sym!=WRITETK&&sym!=SEMICN&&sym!=ENDTK)//��������ʶ���ַ�
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
			while(sym!=SEMICN&&sym!=ENDTK)//��������ʶ���ַ�
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
				&&sym!=WRITETK&&sym!=SEMICN&&sym!=ENDTK&&sym!=PROCETK&&sym!=FUNCTK&&sym!=BEGINTK)//��������ʶ���ַ�
			{
				sym=getsym();
			}
		}
	}
	errorline[errornum]=compound_linenum1;
	error[errornum++]=18;
	return 0;
}

//�������
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

//���̵���
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

//��������
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
//��ֵ���
int assignstatement(int iden_index)
{
	int iden_type=0;//��־�ǲ��Ǻ�����ֵ���
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

//�������
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

//������
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

//�����
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

//д���
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

//forѭ�����
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

//���ʽ
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
		if(!gen(OPR,0,1))//ջ����ֵȡ��
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

//��
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

//����
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
		if(tbl[find_return].kind==CONST_KIND)//����
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
		else if(tbl[find_return].kind==VAR_KIND||tbl[find_return].kind==PARA_KIND)//����
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
		else if(tbl[find_return].kind==FUNC_KIND)//�����������
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
		else if(tbl[find_return].kind==ARRAY_KIND)//����Ԫ��
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