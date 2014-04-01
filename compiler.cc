#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include<string>
#include<iostream>

using namespace std;
#define MAX 1000

string sym;
int line,tablenum,tokenindex=0; 
string token[MAX];
int cc=0,ll=0;
char tokline[100];
FILE *stream;									//打开的文件
FILE *wstream;									//要写入的文件
char ch=' ';									//最后一个读的字符
int kk,varcount;
int firstblank=0;								//此行的开始空格数
int cur_cc=0;	                               //当前列数
char a[10];
string id;                                   //最后一个读的标识符
bool eof;										//文件末尾
const int al=10;
int stack[200];
int codecount=0,addcount=0;
int flag,eol,linecount=0;
int err_count=0;
int ep[100];                        //存放每个exit语句地址的盏，用于整个程序结束后赋给每个exit语句跳转地址
int epcount=0;
struct
{
	int s_add;     //当前循环判断条件开始的地址
	int c_add;     //continu语句生成指令的地址
}cp[MAX];          //用于continue和break语句的盏                 
int cpcount=-1;
int lev=0;
struct
{
	int e_add;
	int b_add;
}bp[MAX];
int bpcount=-1;
int lev1=0;
struct
{
	string s;
	int a;
}code[2000];

struct
{
	string name;
	int kind;
	int add;
	int val;
}table[MAX];

/************************函数声明*********************************/
string getsym();              
int getKind(string name);
int getAddress(string *name);
void getch();
void getToken();
void error(string x);
void program();
void declaration_list();
void statement_list();
void statement();
void if_stat();
void while_stat();
void write_stat();
void read_stat();
void compound_stat();
void expression_stat();
void expression();
void bool_expr();
void additive_expr();
void term();
void factor();
void gen(string s,int x);
void interpret();
int getValue(string name);
void for_stat();
void case_stat();
void dowhile_stat();
void repeat_stat();
void exit_stat();
void b_c_stat();
void show_code();
/************************************函数声明结束****************************/

void main()
{
   	int i,j;
	varcount=0;	
	cc=0;ll=0;ch=' ';
	kk=al-1;
	eof=false;
	if((fopen_s(&stream,"continueqiantao.txt","r"))==NULL)
		printf("输入文件打开失败！\n\n");
	else
		printf("输入文件打开成功！\n\n");
	/*if((errfile=fopen_s(&wstream,"testout.txt","w+"))!=0)
		printf("输出文件打开失败！\n\n");
	else
		printf("输出文件打开成功！\n\n");*/
	printf("SmallC程序源代码为：\n\n");
	while(eof==false)getToken();
	
	for(j=0;j<tablenum;j++)
	{
		if(table[j].kind==0)varcount++;
	}
	tokenindex=0;
	sym=getsym();
	program();
	if(flag==1&&err_count==0)
	{
		printf("SmallC 程序编译成功！！！\n");
		show_code();
		interpret();
	}
	else printf("由于存在上述错误，该SmallC程序无法编译运行！\n");
	if(stream)
		fclose(stream);
	if(wstream)
		fclose(wstream);
	while(1);
}

string getsym()
{
  return token[tokenindex++];
}
int getKind(string name)
{
	for(int i =0;i<tablenum;i++)
		if(table[i].name==name)return(table[i].kind);
	return(-1);
}

int getAddress(string name)
{
	for(int i =0;i<tablenum;i++)
		if(table[i].name==name)return(table[i].add);
	return(-1);
}

int getValue(string name)
{
	for(int i =0;i<tablenum;i++)
		if(table[i].name==name)return(table[i].val);
	return(-1);
}

int getExist(string name)
{
	for(int i =0;i<tablenum;i++)
		if(table[i].name==name)return(1);
	return(-1);
}

void getch()
{
	if(cur_cc==ll)
	{
		eol=1;
		if(feof(stream))
		{
			eof=true;
			printf("\n\n");
			//fprintf_s(wstream,"\n\n\n");
			return ;
		}
		ll=0;
		cc=0;
		if(fgets(tokline,100,stream)==NULL)
		{
			printf("fgets error\n");
			//fprintf_s(wstream,"fgets error\n");
		}
		firstblank=0;
		for(int m=0;m<100;m++)
		{	
			if(tokline[m]==' ')firstblank++;
			else break;
		}
		for(int m=0;m<100;m++)
		{
			if(tokline[m]=='\0'||tokline[m]==10)
			{
				ll++;break;
			}
			else if(tokline[m]=='/'&&tokline[m+1]=='/')
			{
				tokline[m]='\n';
				tokline[m+1]='\0';
			}
			else if(tokline[m]=='/'&&tokline[m+1]=='*')
			{
				int i=m;
				while(tokline[i]!='*'||tokline[i+1]!='/'){i++;}
				i=i+2;
				while(tokline[m]!='\0'&&tokline[m]!=10)
				{
					tokline[m]=tokline[i];
					m++;i++;ll++;
				}
				ll++;break;
			}
			else
				ll++;
		}
		printf("%s",&tokline);
		//fprintf_s(wstream,"%s",&tokline);
	}
	ch=tokline[firstblank + cc];
	cc++;
	cur_cc=firstblank+cc;
}

void getToken()
{
	int k;
	while(ch==' '||ch==9||ch==10)
	{
		if(eof==true)break;
        getch();
	}
	if(islower(ch)||isupper(ch))            /*islower函数，若是小写字母则返回非零值，否则返回零值*/
	{
		int m,n;
		char temp[10];
		k=0;
		do
		{	
			if(k<al)
			{
				a[k++]=ch;
			}
			if(tokline[cur_cc]==10||ch==10){getch();break;}
			getch();
		}while(islower(ch)||isdigit(ch));
	
		for(m=0;m<al;m++)
			temp[m]=a[m];
		
		for(m=0;temp[m]!='\0';m++)
			id=id+temp[m];
		token[tokenindex]=id;
		tokenindex=tokenindex+1;
		if(token[tokenindex-1]!="if"&&token[tokenindex-1]!="while"
			&&token[tokenindex-1]!="else"&&token[tokenindex-1]!="do"
			&&token[tokenindex-1]!="read"&&token[tokenindex-1]!="int"
			&&token[tokenindex-1]!="write"&&token[tokenindex-1]!="for"
			&&token[tokenindex-1]!="xor"&&token[tokenindex-1]!="odd"
			&&token[tokenindex-1]!="switch"&&token[tokenindex-1]!="case"
			&&token[tokenindex-1]!="break"&&token[tokenindex-1]!="do"
			&&token[tokenindex-1]!="repeat"&&token[tokenindex-1]!="exit"
			&&token[tokenindex-1]!="continue"&&token[tokenindex-1]!="and"
			&&token[tokenindex-1]!="or"&&token[tokenindex-1]!="not"
			&&token[tokenindex-1]!="until"&&getExist(id)==-1)
		{
			table[tablenum].name=id;
			table[tablenum].kind=0;
			tablenum=tablenum+1;
		}
		for(m=0;m<al;m++)
		{temp[m]='\0';a[m]='\0';}
		id="";
	}
	else if(isdigit(ch))
	{
		int num=0;
		k=0;
		do
		{	
			if(k<al)
			{
				num=num*10+(ch-'0');
				a[k++]=ch;
			}
			if(tokline[cur_cc]==10||ch==10){getch();break;}
			getch();
		}while(isdigit(ch));  /*isdigit函数，若是数字则返回非零值，否则返回零值*/
		
		for(int m=0;a[m]!='\0';m++)
			id=id+a[m];
		token[tokenindex]=id;
		tokenindex++;
		table[tablenum].name=id;
		table[tablenum].kind=1;
		table[tablenum].val=num;
		tablenum++;
		for(int m=0;m<al;m++)a[m]='\0';
		id="";
	}
	
	else if(ch=='*'){getch();token[tokenindex]="*";tokenindex++;}
	else if(ch=='{'){getch();token[tokenindex]="{";tokenindex++;}
	else if(ch=='}'){getch();token[tokenindex]="}";tokenindex++;}
	else if(ch=='('){getch();token[tokenindex]="(";tokenindex++;}
	else if(ch==')'){getch();token[tokenindex]=")";tokenindex++;}
	else if(ch==':'){getch();token[tokenindex]=":";tokenindex++;}
	else if(ch==';'){getch();token[tokenindex]=";";tokenindex++;}
	else if(ch==','){getch();token[tokenindex]=",";tokenindex++;}
	else if(ch=='%'){getch();token[tokenindex]="%";tokenindex++;}
	
	else if(ch=='+')
	{
        getch();
		if(ch=='+'){getch();token[tokenindex]="++";tokenindex++;}
		else {token[tokenindex]="+";tokenindex++;}
	}

	else if(ch=='-')
	{
        getch();
		if(ch=='-'){getch();token[tokenindex]="--";tokenindex++;}
		else {token[tokenindex]="-";tokenindex++;}
	}

	else if(ch=='/')
	{getch();token[tokenindex]="/";tokenindex++;}
	
	else if(ch=='>')
	{
        getch();
		if(ch=='='){getch();token[tokenindex]=">=";tokenindex++;}
		else {token[tokenindex]=">";tokenindex++;}
	}
	else if(ch=='<')
	{
        getch();
		if(ch=='='){getch();token[tokenindex]="<=";tokenindex++;}
		else {token[tokenindex]="<";tokenindex++;}
	}
	else if(ch=='!')
	{
        getch();
		if(ch=='='){getch();token[tokenindex]="!=";tokenindex++;}
		else {token[tokenindex]="!";tokenindex++;}
	}
	else if(ch=='=')
	{
        getch();
		if(ch=='='){getch();token[tokenindex]="==";tokenindex++;}
		else {token[tokenindex]="=";tokenindex++;}
	}
	else getch();
}

void error(string x)
{
	err_count=err_count+1;
	cout<<"第"<<err_count<<"个错误："<<x<<"\n"<<endl;
	
}

void program() 
{
	
	if(sym=="{")
	{
		sym=getsym();
	    gen("int",varcount);
		declaration_list();
		statement_list();
		if(sym=="}")
		{
			for(int i=0;ep[i]!=0;i++)code[ep[i]].a=codecount;
		    gen("jmp",0);
			flag=1;
		}
		else error("缺少右大括号！");
	}
	else error("缺少左大括号！");
}

void declaration_list()
{
	while(sym=="int")
	{
		sym=getsym();
		if(getKind(sym)==0)
		{
			sym=getsym();
			table[addcount].add=addcount++;
			if(sym==";")sym=getsym();else error("丢失分号");
		}
		else error("未定义变量！");
	}
}

void statement_list()
{
	while(sym=="if"||sym=="while"||sym=="read"
		||sym=="write"||sym=="{"||sym==";"||sym=="("||
		getKind(sym)==0||getKind(sym)==1||sym=="for"||
		sym=="switch"||sym=="do"||sym=="exit"||sym=="break"||
		sym=="continue"||sym=="repeat")
	{
		statement();
	}
}

void statement()
{
	if(sym=="if")if_stat();
	else if(sym=="while")while_stat();
	else if(sym=="write")write_stat();
	else if(sym=="read")read_stat();
	else if(sym=="{")compound_stat();
	else if(sym=="for")for_stat();
	else if(sym=="switch")case_stat();
	else if(sym=="do")dowhile_stat();
	else if(sym=="exit")exit_stat();
	else if(sym=="break"||sym=="continue")b_c_stat();
	else if(sym=="repeat")repeat_stat();
	else expression_stat();
}

void if_stat()
{
	if(sym=="if")
	{
		sym=getsym();
		if(sym=="(")
		{
			sym=getsym();
			expression();
			if(sym==")")
			{
				sym=getsym();
				int cx=codecount;
				gen("jpc",0);
				statement();
				code[cx].a=codecount;
				if(sym=="else")
				{
					code[cx].a++;
					sym=getsym();
					int cx1=codecount;
				    gen("jmp",0);
				    statement();
				    code[cx1].a=codecount;
				}
			}
			else error("if后面缺少右括号！");
		}
		else error("if后面缺少左括号！");
	}
}

void while_stat()
{
	if(sym=="while")
	{
		lev--;
		lev1--;
		sym=getsym();
		if(sym=="(")
		{
			int cx1=codecount;
            int t=codecount;
			sym=getsym();
			expression();
			int cx2=codecount;
			gen("jpc",0);
			
			if(sym==")")
			{
				sym=getsym();
				statement();
				gen("jmp",cx1);
				int cx3=codecount;
				code[cx2].a=cx3;
				int t1=codecount;
				while(bp[bpcount].e_add==lev&&cp[cpcount].c_add!=0)
				{
					code[bp[bpcount].b_add].a=t1;
					bp[bpcount].b_add=0;
					bp[bpcount].e_add=0;
					bpcount--;
				}
				lev1++;
				
				while(cp[cpcount].s_add==lev&&cp[cpcount].c_add!=0)
				{
					code[cp[cpcount].c_add].a=t;
					cp[cpcount].c_add=0;
					cp[cpcount].s_add=0;
					cpcount--;
				}
				lev++;

			}
			else error("while后面缺少右括号！");
		}
		else error("while后面缺少左括号！");
	}

}

void dowhile_stat()
{
	if(sym=="do")
	{
		lev--;
		lev1--;
		sym=getsym();
		int cx=codecount;
		statement();
		if(sym=="while")
		{
			sym=getsym();
			if(sym=="(")
			{
				int t=codecount;
				sym=getsym();
				expression();
				int cx1=codecount;
				gen("jpc",cx1+2);
				//int cx2=codecount
				gen("jmp",cx);
				//code[cx2].a=cx;

				if(sym==")")
				{
					sym=getsym();
					if(sym==";")
					{
						sym=getsym();
				        int t1=codecount;
				        while(bp[bpcount].e_add==lev&&cp[cpcount].c_add!=0)
				        {
					        code[bp[bpcount].b_add].a=t1;
					        bp[bpcount].b_add=0;
					        bp[bpcount].e_add=0;
					        bpcount--;
				        }
				        lev1++;

						while(cp[cpcount].s_add==lev&&cp[cpcount].c_add!=0)
				        {
					        code[cp[cpcount].c_add].a=t;
					        cp[cpcount].c_add=0;
					        cp[cpcount].s_add=0;
					        cpcount--;
				        }
						lev++;
					}
					else error("do while语句中的while判断条件后面缺少分号！");
				}
				else error("do while语句中的while后面缺少右括号！");
			}
			else error("do while语句中的while后面缺少左括号！");
		}
	}
}

void for_stat()
{
	if(sym=="for")
	{
		sym=getsym();
		if(sym=="(")
		{
			sym=getsym();
			expression();
			if(sym==";")
			{
				int cx1=codecount;
				sym=getsym();
				expression();
				if(sym==";")
				{
					int cx2=codecount;
			        gen("jpc",0);
					int cx5=codecount;
					gen("jmp",0);
					int cx7=codecount;
				    int t=codecount;
					sym=getsym();
					expression();
					gen("jmp",cx1);
					int cx3=codecount;
					code[cx5].a=cx3;
					if(sym==")")
					{
						sym=getsym();
						statement();
						gen("jmp",cx7);
						int cx6=codecount;
						code[cx2].a=cx6;
				        int t1=codecount;
				        while(bp[bpcount].e_add==lev&&cp[cpcount].c_add!=0)
				        {
					        code[bp[bpcount].b_add].a=t1;
					        bp[bpcount].b_add=0;
					        bp[bpcount].e_add=0;
					        bpcount--;
				        }
				        lev1++;

				        while(cp[cpcount].s_add==lev&&cp[cpcount].c_add!=0)
				        {
					        code[cp[cpcount].c_add].a=t;
					        cp[cpcount].c_add=0;
					        cp[cpcount].s_add=0;
					        cpcount--;
				        }
				        lev++;
					}
					else error("for后面缺少右括号！");
				}
				else error("for循环内部缺少分号！");
			}
			else error("for循环内部缺少分号！");
		}
		else error("for后面缺少左括号！");
	}
}

void repeat_stat()
{
	if(sym=="repeat")
	{
		lev--;
		sym=getsym();
		int cx=codecount;
		statement();
		if(sym=="until")
		{
			int t=codecount;
			sym=getsym();
			if(sym=="(")
			{
				sym=getsym();
				expression();
				//int cx1=codecount;
				gen("jpc",cx);
				//int cx2=codecount
				//gen("jmp",cx);
				//code[cx2].a=cx;

				if(sym==")")
				{
					sym=getsym();
					if(sym==";")
					{
						sym=getsym();
				        int t1=codecount;
				        while(bp[bpcount].e_add==lev&&cp[cpcount].c_add!=0)
				        {
					        code[bp[bpcount].b_add].a=t1;
					        bp[bpcount].b_add=0;
					        bp[bpcount].e_add=0;
					        bpcount--;
				        }
				        lev1++;

				        while(cp[cpcount].s_add==lev&&cp[cpcount].c_add!=0)
				        {
					        code[cp[cpcount].c_add].a=t;
					        cp[cpcount].c_add=0;
					        cp[cpcount].s_add=0;
					        cpcount--;
				        }
				        lev++;

					}
					else error("repeat语句中的until判断条件后面缺少分号！");
				}
				else error("repeat语句中的until后面缺少右括号！");
			}
			else error("repeat语句中的until后面缺少左括号！");
		}
	}

}

void case_stat()
{
	int i=0;
	int case_count[100];
	if(sym=="switch")
	{
		sym=getsym();
		if(sym=="(")
		{
			sym=getsym();
			expression();
			codecount--;
			string case_var=token[tokenindex-2];
			if(sym==")")
			{
				sym=getsym();
				if(sym=="{")
				{
					sym=getsym();
					while(sym=="case")
					{
						gen("lod",getAddress(case_var));
						sym=getsym();
						expression();
					    gen("opr",8);
						int cx1=codecount;
						gen("jpc",0);
						if(sym==":")
						{
							sym=getsym();
							statement_list();
							if(sym=="break")
							{
								sym=getsym();
								if(sym==";")
								{
									case_count[i]=codecount;i++;
									gen("jmp",0);
									int cx2=codecount;
									code[cx1].a=cx2;
									sym=getsym();
								}
								else error("break后面缺少分号！");
							}
							else error("case后面缺少break！");
						}
						else error("case后面缺少冒号！");
					}
					if(sym=="}")sym=getsym();else error("case语句整体缺少右大括号！");
					case_count[i]=0;
					for(int j=0;case_count[j]!=0;j++)code[case_count[j]].a=codecount;
				}
				else error("case语句整体缺少左大括号！");
			}
			else error("switch后面缺少右括号！");
		}
		else error("switch后面缺少左括号！");
	}
}

void exit_stat()
{
	if(sym=="exit")
	{
		sym=getsym();
		if(sym=="(")
		{
			sym=getsym();
			if(getKind(sym)==1)
			{
				if(sym=="1")
				{
					ep[epcount]=codecount;
					gen("jmp",0);
					epcount++;
					ep[epcount]=0;
					sym=getsym();
					if(sym==")")
					{
						sym=getsym();
						if(sym==";")sym=getsym();else error("exit语句结尾缺少分号！");
					}
					else error("exit后面缺少右括号！");
				}
				else if(sym=="0")
				{
					ep[epcount]=codecount;
					gen("jmp",0);
					epcount++;
					ep[epcount]=0;
					sym=getsym();
					if(sym==")")
					{
						sym=getsym();
						if(sym==";")sym=getsym();else error("exit语句结尾缺少分号！");
					}
					else error("exit后面缺少右括号！");
				}
				else error("exit语句中含有非法字符！");
			}
			else error("exit语句中含有非法字符！");
		}
		else error("exit后面缺少左括号！");
	}
}

void b_c_stat()
{
	if(sym=="break")
	{
		sym=getsym();
		if(sym==";")
		{
			bpcount++;
			bp[bpcount].b_add=codecount;
			bp[bpcount].e_add=lev1;
			gen("jmp",0);
			sym=getsym();

		}
		else error("循环中的break语句后面缺少分号！");
	}
	else if(sym=="continue")
	{
		sym=getsym();
		if(sym==";")
		{
			cpcount++;
			cp[cpcount].c_add=codecount;
			cp[cpcount].s_add=lev;
			gen("jmp",0);
			sym=getsym();

		}
		else error("循环中的break语句后面缺少分号！");
	}
}

void write_stat()
{
	if(sym=="write")
	{
	    sym=getsym();
		expression();
		gen("opr",15);
	}
}

void read_stat()
{
	if(sym=="read")
	{
		sym=getsym();
		if(getKind(sym)==0)
		{
			gen("opr",14);
			gen("sto",getAddress(sym));
			sym=getsym();
		}else error("read后面不是ID类型！");
	}
}

void compound_stat()
{
	if(sym=="{")
	{
		sym=getsym();
		statement_list();
		if(sym=="}")sym=getsym();else error("缺少复合语句右边大括号！");
	}
	else error("缺少复合语句左边大括号！");
}

void expression_stat()
{
	if(sym==";")sym=getsym();
	else 
	{
		expression();
		if(sym==";")sym=getsym();else error("缺少分号！");
	}
}

void expression()
{
	
	if(getKind(sym)==0)
	{
		//sym=getsym();
		string zz_var;
		if(token[tokenindex]=="==")
		{
			bool_expr();
			if(sym=="++"||sym=="--")
		    {
			    zz_var=token[tokenindex-2];
				if(sym=="++")
				{
					gen("lod",getAddress(zz_var));
					gen("opr",17);
				}
			    else
				{
					gen("lod",getAddress(zz_var));
					gen("opr",18);
				}
				gen("sto",getAddress(zz_var));
				sym=getsym();
		    }
		}
		else if(token[tokenindex]=="=")
		{
			string cur_var;
			cur_var=sym;
			sym=getsym();
			sym=getsym();
			bool_expr();
			gen("sto",getAddress(cur_var));
			if(sym=="++"||sym=="--")
		    {
			    zz_var=token[tokenindex-2];
				if(sym=="++")
				{
					gen("lod",getAddress(zz_var));
					gen("opr",17);
				}
			    else
				{
					gen("lod",getAddress(zz_var));
					gen("opr",18);
				}
				gen("sto",getAddress(zz_var));
				sym=getsym();
		    }
		}
		else 
		{
			bool_expr();
			if(sym=="++"||sym=="--")
		    {
			    zz_var=token[tokenindex-2];
				if(sym=="++")
				{
					gen("lod",getAddress(zz_var));
					gen("opr",17);
				}
			    else
				{
					gen("lod",getAddress(zz_var));
					gen("opr",18);
				}

				gen("sto",getAddress(zz_var));
				sym=getsym();
		    }
		}

	}
	else bool_expr();
}

void bool_expr()
{
	if(sym=="odd")
	{
		sym=getsym();
		additive_expr();
		gen("opr",6);
	}
	else
	{
		additive_expr();
	    if(sym==">"||sym=="<"||sym=="<="
		||sym==">="||sym=="=="||sym=="!="||sym=="xor")
		{
		     string temp=sym;
		     sym=getsym();
		     additive_expr();
		     if(temp==">")gen("opr",12);
		     else if(temp=="<")gen("opr",10);
		     else if(temp=="<=")gen("opr",13);
	     	 else if(temp==">=")gen("opr",11);
      		 else if(temp=="==")gen("opr",8);
		     else if(temp=="!=")gen("opr",9);
		     else if(temp=="xor")gen("opr",0);
	     }
		/*else if(sym=="++"||sym=="--")
		{
			sym=getsym();
			if(sym=="++")gen("opr",17);
			else gen("opr",18);
		}*/
	}
}

void additive_expr()
{
	term();
	while(sym=="+"||sym=="-")
	{
		string addop;
		addop=sym;
		sym=getsym();
		term();
		if(addop=="+")gen("opr",2);
		else if(addop=="-")gen("opr",3);
	}
}

void term()
{
	string mulop;
	factor();
	while(sym=="*"||sym=="/"||sym=="%")
	{
		mulop=sym;
		sym=getsym();
		factor();
		if(mulop=="*")gen("opr",4);
		else if(mulop=="%")gen("opr",16);
		else gen("opr",5);
	}
}

void factor()
{
	if(sym=="(")
	{
		sym=getsym();
		expression();
		if(sym==")")sym=getsym();else error("缺少右括号！");
	}
	else if(getKind(sym)==0||getKind(sym)==1)
	{
		if(getKind(sym)==1)
		{
			string s=sym;
			sym=getsym();
			gen("lit",getValue(s));
		}
		else if(getKind(sym)==0)
		{
			string s=sym;
			sym=getsym();
			gen("lod",getAddress(s));
		}

	}
	else error("缺少左括号！");
}

void gen(string s,int x)
{
	code[codecount].s=s;
	code[codecount].a=x;
	codecount++;
}

void interpret()
{
	int p,i,b,t;

	cout<<"开始执行上述SmallC程序...\n\n"<<endl;
	//fprintf_s(wstream,"Start SmallC\n");
	t=-1;
	b=0;
	p=0;
	//s[0]=0;
	
	do
	{
		int _a=code[p].a; //操作数或地址
		string s=code[p].s;
		if(s=="lit")i=0;
	    else if(s=="opr")i=1;
	    else if(s=="lod")i=2;
	    else if(s=="sto")i=3;
	    else if(s=="int")i=4;
	    else if(s=="jmp")i=5;
	    else if(s=="jpc")i=6;

		switch(i)
		{
		case 0:								//lit
			t=t+1;
			stack[t]=_a;
			break;
		case 1:								//opr
			switch(_a)
			{
			case 0:                        //异或
				t=t-1;
				stack[t]=(stack[t]^stack[t+1])?1:0;
				break;
			case 1:                         //取负
				stack[t]=-stack[t];
				break;
			case 2:                         //相加
				t=t-1;
				stack[t]=stack[t]+stack[t+1];
				break;
			case 3:                       //相减
				t=t-1;
				stack[t]=stack[t]-stack[t+1];
				break;
			case 4:                  //相乘
				t=t-1;
				stack[t]=stack[t]*stack[t+1];
				break;
			case 5:                 //相除
				t=t-1;
				stack[t]=stack[t]/stack[t+1];
				break;
			case 6:                   //判奇偶
				stack[t]=stack[t]%2;
				break;
			case 8:              //判相等，相等返回1，不相等返回0
				t=t-1;
				stack[t]=(stack[t]==stack[t+1])?1:0;
				break;
			case 9:              //判不相等，不相等返回1，相等返回0
				t=t-1;
				stack[t]=(stack[t]!=stack[t+1])?1:0;
				break;
			case 10:             //判小于，小于返回1，不小于返回0
				t=t-1;
				stack[t]=(stack[t]<stack[t+1])?1:0;
				break;
			case 11:            //判大于等于，大于等于返回1，不大于等于返回0
				t=t-1;
				stack[t]=(stack[t]>=stack[t+1])?1:0;
				break;
			case 12:             //判大于，大于返回1，不大于返回0
				t=t-1;
				stack[t]=(stack[t]>stack[t+1])?1:0;
				break;
			case 13:              //判小于等于，小于等于返回1，不小于等于返回0
				t=t-1;
				stack[t]=(stack[t]<=stack[t+1])?1:0;
				break;
			case 14:
			    int a;
				cout<<"请输入要读入的值:"<<endl;     //read第一步
				cin>>a;
		    	t=t+1;
				stack[t]=a;
				cout<<a<<"已经被放入运行栈栈顶\n"<<endl;
		    	break;
			case 15:                                //write
				cout<<"程序运行结果为:"<<stack[t]<<"\n"<<endl;
				t=t-1;
				break;
			case 16:                               //"%"运算符（mod）
				t=t-1;
				stack[t]=stack[t]%stack[t+1];
				break;
			case 17:                               //自增++
				stack[t]=stack[t]+1;
				break;
			case 18:                               //自减--
				stack[t]=stack[t]-1;
				break;

			}
			break;
		case 2:								//lod
			t=t+1;
			stack[t]=stack[_a];
			break;
		case 3:								//sto
			stack[_a]=stack[t];
			//printf("%d\n",stack[t]);
			//fprintf_s(wstream,"%d\n",stack[t]);
			t=t-1;
			break;
		case 4:								//int
			t=t+_a;
			break;
		case 5:								//jmp
			p=_a;
			break;
		case 6:								//jpc
			if(stack[t]==0)
			p=_a;
			t=t-1;
			break;
		}
		p++;
		if((i==5)||(i==6&&stack[t+1]==0))p--;
	}while(p!=0);
	//cout<<"程序执行结果为："<<stack[0]<<"\n"<<endl;
	cout<<"\nSmallC程序执行完毕！\n"<<endl;
	//fprintf_s(wstream,"End SmallC\n");
}

void show_code()
{
	cout<<"\n开始生成上述SmallC程序的机器指令代码...\n"<<endl;
	for(int i=0;i<codecount;i++)
	{
		cout<<i<<" "<<code[i].s<<" "<<code[i].a<<endl;
		//fprintf_s(wstream,"%s %d\n",code[i].s,code[i].a);
	}
	cout<<"\nSmallC程序的机器指令代码生成完毕！\n\n\n"<<endl;
}
