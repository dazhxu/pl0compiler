
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

#define MAXWORD 20 //token��󳤶�
#define MAXSTRING  30 //�ַ�����󳤶�
#define MAXVARDEFINE 20//ÿ���������������
#define MAXERROR 100	//��������
#define MAXTABLE 500    //�����ű���Ŀ
#define MAXPCODE 1000		//����м��������
#define MAXSTACK 200		//�������ջ�ռ�
#define MAXLEV	10	//�����ò���

#define CONST_KIND 1    //�������ͣ����ű��ã�
#define VAR_KIND 2    //�������ͣ����ű��ã�
#define ARRAY_KIND 3    //����������ͣ����ű��ã�
#define PROC_KIND 4    //�������ͣ����ű��ã�
#define FUNC_KIND 5    //�������ͣ����ű��ã�
#define PARA_KIND 6    //�β����ͣ����ű��ã�


#define INT_TYPE 1		//��������
#define REAL_TYPE 2		//ʵ������
#define CHAR_TYPE 3		//�ַ�����


#define IDEN 1    //��ʶ��
#define INTCON 2  //���ͳ���
#define REALCON 3 //ʵ�ͳ���
#define CHARCON 4 //�ַ�����
#define STRCON 5  //�ַ�������

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
#define FILEOVER 53//�ļ�����

#define LIT 0//ȡ����a��������ջջ��
#define OPR 1//ִ�����㣬a��ʾִ�к�������
#define LODADR 2//�ѵ�ַ�浽ջ��
#define LOD 3//ȡ��������Ե�ַΪa����β�Ϊl����������ջջ��
#define STO 4//������ջջ�����ݴ����������Ե�ַΪa����β�Ϊl��
#define CAL 5//���ù��̣����ָ���ַΪa����β�Ϊl��
#define INT 6//����ջջ��ָ������a
#define JMP 7//������ת�Ƶ�a
#define JPC 8//����ת�Ƶ�ָ���ַa
#define RED 9//�����ݲ������������Ե�ַa,��β�Ϊl��
#define WRT 10//��ջ���������


struct pcode{							//�м���� 
	int op,lev,x;
	double fx;
};

union Cha{
	int num;
	double fnum;
	char ch;
};
struct table{					//���ű�
	char *name ;
	int type,lev,adr,kind;//adr �溯����Ӧ��pcodeָ�����ʼ��ַ
	int count;//���鳤��
	union Cha uchar;
};

struct runstack{//����ջ���ݽṹ
	int x;
	double fx;
	int kind;
};

int getsym();	//�ʷ�����
void error_msg(int i);//������
int enter(int in_lev,int type,int kind);//��½���ű�
int enterarray(int in_lev,int type,int kind,int size);//�����½���ű�
int test();//����ʶ���Ƿ��ظ�����
int find();//���
int gen(int op,int in_lev,int x);//����Ŀ�����
int gen1(int op,int in_lev,double fx);//����Ŀ�����
int program();	//����
int block();	//�ֳ���
int constdefine();//����˵��
int vardefine();//����˵��
int vardefinestatement();//�����������
int proc();//����˵��
int func();//����˵��
int paradefine();//������
int paradefinestatement();//��ʽ������
int compound();//�������
int statement();//���
int proccall(int proc_name_index);//���̵���
int funccall(int func_name_index);//��������
int assignstatement(int iden_index);//��ֵ���
int ifstatement();//�������
int casestatement();//������
int readstatement();//�����
int writestatement();//д���
int forstatement();//forѭ�����
int expression();//���ʽ
int term();//��
int factor();//��
int basefind(int l);//���Ϊl���������Ļ�ַ
int interpret();//����ִ��

