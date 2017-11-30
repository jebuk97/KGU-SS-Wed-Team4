#include<stdio.h>
#include<string.h>

#define TRUE 1
#define FALSE 0

#define WORD_MAX 10 //�ܾ��� ����
#define MACRO_MAX 20 //table�� ����
#define MAX 1000 //DEFTAB�� line �迭�� ����

//�Ű������� �� �Ű������� �ش�Ǵ� ���ڸ� ������ table 
typedef struct ARGTAB {
	char parameter[WORD_MAX];
	char argument[WORD_MAX];
}ARGTAB;

//��ũ���� ���Ǹ� �����ϴ� ����ü
typedef struct prototype {
	char name[WORD_MAX];
	ARGTAB argtab[5];
}PROTOTYPE;

//������ �����ϴ� ����ü
typedef struct line {
	char label[MACRO_MAX];
	char opcode[MACRO_MAX];
	char address[MACRO_MAX];
}Line;

//��ũ���� ���ǿ� ��ü�� �����ϴ� ������ ������ table
typedef struct DEFTAB
{
	int last_line;  //line �迭�� ����ִ� �κ��� ����Ŵ
	PROTOTYPE prototype[MACRO_MAX];
	Line line[MAX];
}DEFTAB;

//��ũ���� �̸��� �� ��ũ���� �����ٰ� ������ ��ġ�� ������ table
typedef struct NAMTAB {
	char name[WORD_MAX];
	Line *start, *end;
}NAMTAB;


int expanding;// TRUE, FALSE���� �����ϴ� ���Ǻ���
int top = 0;//PROTOTYPE, NAMTAB�� ���� ä������ ���� �迭�� ��ġ�� ����Ų��.

			//DEFTAB�� NAMTAB�� ������ �ٸ� �Լ��� ȣ���ϴ� �Լ�, �������ִ� �ڷᱸ���� �ϴ� �������
void macro_processor(FILE *in, FILE *out);

//expanding�� ���� ���� DEFTAB���� ���ڿ��� �аų� input�� FILE�� �о���� �Լ�
Line getLine(FILE *in, Line *pline);

//getline���� �о�� ���ڿ��� expand�� define �Լ��� ȣ���ϰų� expand�� �ҽ��� �ۼ��ϴ� �Լ�
void processline(FILE *in, FILE *out, DEFTAB *def, NAMTAB *name, Line *line);

//NAMTAB�� start���� end������ ���ڿ��� �о�� expand�� �ҽ��� �ۼ��ϴ� �Լ�
void expand(FILE *in, FILE *out, DEFTAB *def, NAMTAB *name, Line *line);

//macro�κ��� NAMTAB�� DEFTAB�� �̸��� ���Ǹ� set�ϰ� ���پ� �о� DEFTAB�� ����, NAMTAB�� ���۰� ���� ��ġ�� �����Ѵ�
void define(char *macro, DEFTAB *def, NAMTAB *name, FILE *in);

//�ּ����� �ƴ��� �˻��ϴ� �Լ�
int is_comment_line(char *buf);

//line�� opcode�� �־��� ���ڿ��� ������ ���ϴ� �Լ�
int compare_opcode(Line *line, char* buf);

Line *getlinePtr = NULL;

void main() {
	FILE *input, *output;
	fopen_s(&input, "INPUT.txt", "r");
	fopen_s(&output, "OUTPUT.txt", "w");
	if (input == NULL || output == NULL) {
		printf("���Ͽ��� ����");
		return 1;
	}
	macro_processor(input, output);
}

void macro_processor(FILE *in, FILE *out) {
	DEFTAB definetable;
	NAMTAB nametable[MACRO_MAX];
	getLine(in, &definetable);
	getLine(in, &definetable);
}

Line getLine(FILE *in, Line *pline)
{
	Line line = { 0, };
	printf("hello\n");
	int temp=0;
	if (expanding == 1) {
		fscanf(in, "%d\t", &temp);
		fscanf(in, "%s\t%s\t%s\t", line.address, line.label, line.opcode);
		printf("%s\t%s\t%s\n", line.address, line.label, line.opcode);
	}
	else {
		return *pline;
	}
	

	return line;
}