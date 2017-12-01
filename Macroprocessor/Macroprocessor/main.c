#include<stdio.h>
#include<string.h>

#define TRUE 1
#define FALSE 0

#define WORD_MAX 50 //�ܾ��� ����
#define MACRO_MAX 1000 //table�� ����
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
	char label[WORD_MAX];
	char opcode[WORD_MAX];
	char address[WORD_MAX];
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

//���ڿ��� ã�� ġȯ���ִ� �Լ�
char *replaceAll(char *s, const char *olds, const char *news);

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
}

void define(Line* macro, DEFTAB *def, NAMTAB *name, FILE *in) {
	int level;
	int index = 0;
	int index2 = 0;
	char buf[WORD_MAX];
	char arguments[WORD_MAX];
	char argumentSet[WORD_MAX][3];
	Line get;
	strcpy(name->name, macro->label);
	strcpy(def->prototype->name, macro->label);

	//&args1,&args2 �� ,�� �������� split
	strcpy(arguments, macro->address);
	char *ptr = strtok(arguments, ",");
	while (ptr != NULL) {
		strcpy(argumentSet[index2], ptr);
		printf("%s\n", arguments[index2]);
		index2++;
		ptr = strtok(NULL, ",");
	}

	level = 1;
	while (level > 0) {
		get = getLine(in, NULL);
		if (!is_comment_line(macro)) {

			//ó������ ������ ?1�� ġȯ, ?2�� ġȯ...
			for (int i = 0; i < index2; i++) {
				ptr = strstr(get.address, argumentSet[i]);
				sprintf(buf, "%s%d", "?", i + 1);
				if (ptr == NULL) {
					continue;
				}
				else {
					strcpy(get.address, replaceAll(get.address, argumentSet[i], buf));
				}
				printf("%s\n", get.address);
			}
			
			//���� line�� def�� ����
			def->line[index] = get;
			index++;
			if (strcmp(macro->opcode, "MACRO")) {
				level++;
				name->start = &get;
			}
			else if (strcmp(macro->opcode, "MEND")) {
				level--;
				def->last_line = index;
				name->end = &get;
			}
		}
	}
	name->end = &get;

}

Line getLine(FILE *in, Line *pline)
{
	Line line = { 0, };
	printf("hello getLine\n");
	int temp=0;
	if (expanding == TRUE) {
		fscanf(in, "%d\t", &temp);
		fscanf(in, "%s\t%s\t%s\t", line.address, line.label, line.opcode);
		printf("%s\t%s\t%s\n", line.address, line.label, line.opcode);
	}
	else {
		//printf("%s\n", pline);
		//�׽�Ʈ��
		return *pline;
	}
	
	return line;
}

char *replaceAll(char *s, const char *olds, const char *news) {
	char *result, *sr;
	size_t i, count = 0;
	size_t oldlen = strlen(olds); if (oldlen < 1) return s;
	size_t newlen = strlen(news);


	if (newlen != oldlen) {
		for (i = 0; s[i] != '\0';) {
			if (memcmp(&s[i], olds, oldlen) == 0) count++, i += oldlen;
			else i++;
		}
	}
	else i = strlen(s);


	result = (char *)malloc(i + 1 + count * (newlen - oldlen));
	if (result == NULL) return NULL;


	sr = result;
	while (*s) {
		if (memcmp(s, olds, oldlen) == 0) {
			memcpy(sr, news, newlen);
			sr += newlen;
			s += oldlen;
		}
		else *sr++ = *s++;
	}
	*sr = '\0';

	return result;
}
