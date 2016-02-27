#include"compiler.h"

extern FILE *FIN;  //�ļ�����ָ��
extern FILE *FOUT;	//�ļ�����ָ��
extern char fp[20];		//�ļ���
extern int cifafenxi;

int linenum=1;//������
int errornum=1;//������
int errorline[MAXERROR];//�����к�
int error[MAXERROR];//������
struct table tbl[MAXTABLE];//���ű�
int top_of_table=0;//���ű�ջ��
int initial_table_index;//��ʼ���ű�����
struct pcode pcod[MAXPCODE];//�м����
int top_of_pcode=1;//�м����ջ��
int initial_pcode_index;//��ʼ�м��������
struct runstack run[MAXSTACK];//����ջ
int top_of_runstack=0;//����ջջ��
int num_of_var[MAXTABLE];//��¼�����������
int top_of_numofvar=0;//num_of_varջ��
struct runstack parastack[MAXVARDEFINE];//��¼����ֵ
int top_of_parastack=0;//parastackջ��
struct runstack funcstack[MAXVARDEFINE];//��¼��������ֵ
int top_of_funcstack=-1;//funcstackջ��
int func_flag[MAXVARDEFINE];//��¼�Ƿ�Ϊ����
int top_of_funcflag=0;
char printstring[100][MAXSTRING];//���������ַ���
int top_of_printstr=0;
int staticlink[MAXSTACK];//��̬��������ϲ���̵Ļ���ַ
int top_of_sl=0;
int dynamiclink[MAXSTACK];//��̬���ӣ���ŵ��ú����Ļ���ַ
int top_of_dl=0;
int return_adr[MAXPCODE];//���ص�ַ,��ŷ��ص�ַ
int top_of_ra=0;



int lev=0;//��ǰ���
int temp_linenum;//��ʱ����
char name[MAXTABLE][MAXWORD];		//��ʶ������
int arraylength;//���鳤��
int data_allocation_index=3;//���ݷ�������
int func_assaign_flag=0;//��Ǻ������Ƿ��к�����ֵ���
int base=0;//��ַ
int p=0;//Ŀ���������
int t=0;//����ջ��
char print_string[1000]="";

char word[MAXWORD]; //��token
int sym;	//��ʷ�����
int num;	//������
double fnum;	//��ʵ��
char symchar;	//���ַ�
char symstring[MAXSTRING];	//���ַ���

char *Keyword[]={	"const","integer","real","char",				//�ؼ���
	"var","array","of","if","then","else",
	"switch","case", "for","to","downto","do",
	"procedure","function",
	"read","write","begin","end","odd","while"
};

char *symtype[]={	"","IDEN","INTCON","REALCON","CHARCON","STRCON",  //�ʷ��������ʹ��
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

char* err_msg[] =					//���������ʾ��Ϣ
{
/*0*/ "",
/*1*/ "��ʶ�������ֹ�����������ǰ��0",
/*2*/ "С����ǰȱ������",
/*3*/ "�ַ��Ƿ���ȱ��\'",
/*4*/ "�ַ����Ƿ��������ȱ��\"",
/*5*/ "�Ƿ����ʻ�Ƿ��﷨�ṹ",
/*6*/ "ȱ��.",
/*7*/ "ȱ��,��;",
/*8*/ "ȱ�ٱ�ʶ��",
/*9*/ "�˴�Ӧ��'='",
/*10*/ "��������Ƿ�",
/*11*/ "ȱ��,��:",
/*12*/ "ȱ�����鳤�Ȼ��±�����鳤���±����",
/*13*/ "ȱ�١�[����]��",
/*14*/ "ȱ������",
/*15*/ "ȱ��of",
/*16*/ "��ʶ���ظ�����",
/*17*/ "���ű�Խ��",
/*18*/ "����������䴦�����",
/*19*/ "��������Ŀ�����������",
/*20*/ "ȱ�١������򡰣���",
/*21*/ "ȱ�ٹ�����������",
/*22*/ "δ����ı�ʶ��",
/*23*/ "ʵ�����β����Ͳ�ƥ��",
/*24*/ "�������ݴ�����β���ʵ�γ��Ȳ�ƥ���ȱ�١�����",
/*25*/ "δʶ���ʶ��",
/*26*/ "ȱ�ٻ�δʶ�������",
/*27*/ "ȱ��:=",
/*28*/ "ȱ�������жϷ���",
/*29*/ "ȱ��then",
/*30*/ "�����Ͳ��ܱ���ֵ",
/*31*/ "ȱ��to��downto",
/*32*/ "ȱ��do",
/*33*/  "case�������������ʹ���",
/*34*/  "������ȱ�ٺ�����ֵ���",
/*35*/  "ʵ�Ͳ��ܸ������κ��ַ���",
/*36*/  "ȱ�١������򡰣�����Ƿ�����ṹ",
/*37*/  "�˱�ʶ�����Ͳ��ɶ�",
/*38*/  "ȱ�٣���end",
/*39*/  "case����ʽ����Ϊʵ��",
};

//�ʷ�����
int getsym()
{
	char ch;
	int i=0,j=0,m=0,flag=0;

	ch=fgetc(FIN);
	if(ch==EOF)
		return FILEOVER;
	//�����ո�
	while(isspace(ch))
	{
		putc(ch,FOUT);
		if(ch=='\n')
		{
			linenum++;//�����س������кż�1
			fprintf(FOUT,"%d: ",linenum);
		}
		ch=fgetc(FIN);
		if(ch==EOF)
			return FILEOVER;
	}
	
	word[i++]=ch;


	//�ж��Ƿ�Ϊ��ʶ��
	if(isalpha(ch))
	{
		putc(ch,FOUT);
		ch=fgetc(FIN);
		while(isalpha(ch)||isdigit(ch))
		{
			word[i++]=ch;
			//�жϱ�ʶ���Ƿ������������ض�
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
			if(!(strcmp(Keyword[m],word)))	//��ȡ���ַ������϶�key word���ж�
			{
				if(m==23)//����while,ֻΪ�ʷ����� �ķ�����
				{
				//	printf("%d %s %s\n",cifafenxi++,symtype[WHILETK],word);
					return WHILETK;//������Ӧ��keyword�Ķ�Ӧ����
				}
			//	printf("%d %s %s\n",cifafenxi++,symtype[m+6],word);
				return m+6;//������Ӧ��keyword�Ķ�Ӧ����
			}
		}
	//	printf("%d %s %s\n",cifafenxi++,symtype[IDEN],word);
		return IDEN;
	}


	//�����޷����������޷���ʵ��
	if(isdigit(ch))
	{
		putc(ch,FOUT);
		ch=getc(FIN);
		while(isdigit(ch))
		{
			word[i++]=ch;
			if(i==MAXWORD)//���ȴ������ֵ���ضϣ�����������
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
		if(ch=='.')//�ж��Ƿ�Ϊ�޷���ʵ��
		{
			word[i++]=ch;
			putc(ch,FOUT);
			ch=getc(FIN);
			if(!isdigit(ch))//С����������֣���������
			{
				errorline[errornum]=linenum;
				error[errornum++]=2;
			}
			while(isdigit(ch))
			{
				word[i++]=ch;
				if(i==MAXWORD)//���ȴ������ֵ���ضϣ�����������
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
			return REALCON;//����Ϊ�޷���ʵ��
		}
		word[i]='\0';
		ungetc(ch,FIN);
		num=atoi(word);
	//	printf("%d %s %s\n",cifafenxi++,symtype[INTCON],word);
		return INTCON;//����Ϊ�޷�������
	}


	//�����ַ�����
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
				return CHARCON;//����Ϊ�ַ�����
			}
			else//ȱ��'
			{
				errorline[errornum]=linenum;
				error[errornum++]=3;
				while(!isspace(ch))//���������ַ�ֱ���հ׷�
				{
					putc(ch,FOUT);
					ch=fgetc(FIN);
				}
				ungetc(ch,FIN);
				return CHARCON;//ɾȥ
			}
		}
		else//�ַ������Ƿ�
		{
			errorline[errornum]=linenum;
			error[errornum++]=3;
			while(!isspace(ch))//���������ַ�ֱ���հ׷�
			{
				putc(ch,FOUT);
				ch=fgetc(FIN);
			}
			ungetc(ch,FIN);
			return CHARCON;//ɾȥ
		}
	}


	//�����ַ�������
	if(ch=='\"')
	{
		i--;
		putc(ch,FOUT);
		ch=fgetc(FIN);
		while(ch>=32&&ch<=126&&ch!=34)
		{
			word[i++]=ch;
			if(i==MAXSTRING)//���ȴ����ַ������ֵ���ضϣ�����������
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
		else//ȱ��"���ַ����ַ��Ƿ�
		{
			errorline[errornum]=linenum;
			error[errornum++]=4;
			while(!isspace(ch))//���������ַ�ֱ���հ׷�
			{
				putc(ch,FOUT);
				ch=fgetc(FIN);
			}
			ungetc(ch,FIN);
			return STRCON;//ɾȥ
		}
	}


	//����<  <=  <>
	if(ch=='<')
	{
		putc(ch,FOUT);
		ch=fgetc(FIN);
		if(ch=='=')//�ж�<=
		{
			putc(ch,FOUT);
			word[i++]=ch;
			word[i]='\0';
//			printf("%d %s %s\n",cifafenxi++,symtype[LEQ],word);
			return LEQ;
		}
		if(ch=='>')//�ж�<>
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
		return LSS;//����Ϊ<
	}


	//����>  >=
	if(ch=='>')
	{
		putc(ch,FOUT);
		ch=fgetc(FIN);
		if(ch=='=')//�ж�>=
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
		return GRE;//����Ϊ>
	}


	//����:  :=
	if(ch==':')
	{
		putc(ch,FOUT);
		ch=fgetc(FIN);
		if(ch=='=')//�ж�:=
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
		return COLON;//����Ϊ:
	}

	//����+
	if(ch=='+')
	{
		putc(ch,FOUT);
		word[i]='\0';
//		printf("%d %s %s\n",cifafenxi++,symtype[PLUS],word);
		return PLUS;//����Ϊ+
	}


	//����-
	if(ch=='-')
	{
		putc(ch,FOUT);
		word[i]='\0';
//		printf("%d %s %s\n",cifafenxi++,symtype[MINU],word);
		return MINUS;//����Ϊ-
	}


	//����*
	if(ch=='*')
	{
		putc(ch,FOUT);
		word[i]='\0';
//		printf("%d %s %s\n",cifafenxi++,symtype[MULT],word);
		return MULT;//����Ϊ
	}


	//����/
	if(ch=='/')
	{
		putc(ch,FOUT);
		word[i]='\0';
//		printf("%d %s %s\n",cifafenxi++,symtype[DIV],word);
		return DIV;//����Ϊ
	}


	//����=
	if(ch=='=')
	{
		putc(ch,FOUT);
		word[i]='\0';
//		printf("%d %s %s\n",cifafenxi++,symtype[EQL],word);
		return EQL;//����Ϊ=
	}


	//����;
	if(ch==';')
	{
		putc(ch,FOUT);
		word[i]='\0';
//		printf("%d %s %s\n",cifafenxi++,symtype[SEMICN],word);
		return SEMICN;//����Ϊ;
	}


	//����,
	if(ch==',')
	{
		putc(ch,FOUT);
		word[i]='\0';
//		printf("%d %s %s\n",cifafenxi++,symtype[COMMA],word);
		return COMMA;//����Ϊ,
	}


	//����.
	if(ch=='.')
	{
		putc(ch,FOUT);
		word[i]='\0';
//		printf("%d %s %s\n",cifafenxi++,symtype[PERIOD],word);
		return PERIOD;//����Ϊ.
	}


	//����(
	if(ch=='(')
	{
		putc(ch,FOUT);
		word[i]='\0';
//		printf("%d %s %s\n",cifafenxi++,symtype[LPARENT],word);
		return LPARENT;//����Ϊ(
	}


	//����)
	if(ch==')')
	{
		putc(ch,FOUT);
		word[i]='\0';
//		printf("%d %s %s\n",cifafenxi++,symtype[RPARENT],word);
		return RPARENT;//����Ϊ)
	}


	//����[
	if(ch=='[')
	{
		putc(ch,FOUT);
		word[i]='\0';
//		printf("%d %s %s\n",cifafenxi++,symtype[LBRACK],word);
		return LBRACK;//����Ϊ[
	}


	//����]
	if(ch==']')
	{
		putc(ch,FOUT);
		word[i]='\0';
//		printf("%d %s %s\n",cifafenxi++,symtype[RBRACK],word);
		return RBRACK;//����Ϊ]
	}



	//����{
	if(ch=='{')
	{
		putc(ch,FOUT);
		word[i]='\0';
//		printf("%d %s %s\n",cifafenxi++,symtype[LBRACE],word);
		return LBRACE;//����Ϊ{
	}


	//����}
	if(ch=='}')
	{
		putc(ch,FOUT);
		word[i]='\0';
//		printf("%d %s %s\n",cifafenxi++,symtype[RBRACE],word);
		return RBRACE;//����Ϊ}
	}
	if(ch==EOF)
		return FILEOVER;
	//δʶ�𵥴ʷ���
	errorline[errornum]=linenum;
	error[errornum++]=5;
	return 0;
}
