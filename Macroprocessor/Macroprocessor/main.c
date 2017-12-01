#include<stdio.h>
#include<string.h>

#define TRUE 1
#define FALSE 0

#define WORD_MAX 10 //단어의 길이
#define MACRO_MAX 20 //table의 개수
#define MAX 1000 //DEFTAB의 line 배열의 길이

//매개변수와 그 매개변수에 해당되는 인자를 가지는 table 
typedef struct ARGTAB {
	char parameter[WORD_MAX];
	char argument[WORD_MAX];
}ARGTAB;

//매크로의 정의를 저장하는 구조체
typedef struct prototype {
	char name[WORD_MAX];
	ARGTAB argtab[5];
}PROTOTYPE;

//한줄을 저장하는 구조체
typedef struct line {
	char label[MACRO_MAX];
	char opcode[MACRO_MAX];
	char address[MACRO_MAX];
}Line;

//매크로의 정의와 몸체를 구성하는 문장을 가지는 table
typedef struct DEFTAB
{
	int last_line;  //line 배열의 비어있는 부분을 가리킴
	PROTOTYPE prototype[MACRO_MAX];
	Line line[MAX];
}DEFTAB;

//매크로의 이름과 그 매크로의 시작줄과 끝줄의 위치를 가지는 table
typedef struct NAMTAB {
	char name[WORD_MAX];
	Line *start, *end;
}NAMTAB;


int expanding;// TRUE, FALSE만을 저장하는 조건변수
int top = 0;//PROTOTYPE, NAMTAB의 아직 채워지지 않은 배열의 위치를 가리킨다.

			//DEFTAB과 NAMTAB을 가지고 다른 함수를 호출하는 함수, 가지고있는 자료구조만 일단 적어놓음
void macro_processor(FILE *in, FILE *out);

//expanding의 값에 따라 DEFTAB에서 문자열을 읽거나 input된 FILE을 읽어오는 함수
Line getLine(FILE *in, Line *pline);

//getline에서 읽어온 문자열로 expand나 define 함수를 호출하거나 expand된 소스를 작성하는 함수
void processline(FILE *in, FILE *out, DEFTAB *def, NAMTAB *name, Line *line);

//NAMTAB의 start부터 end까지의 문자열을 읽어와 expand된 소스를 작성하는 함수
void expand(FILE *in, FILE *out, DEFTAB *def, NAMTAB *name, Line *line);

//macro로부터 NAMTAB과 DEFTAB에 이름과 정의를 set하고 한줄씩 읽어 DEFTAB에 저장, NAMTAB에 시작과 끝의 위치를 저장한다
void define(char *macro, DEFTAB *def, NAMTAB *name, FILE *in);

//주석인지 아닌지 검사하는 함수
int is_comment_line(char *buf);

//line의 opcode가 주어진 문자열과 같은지 비교하는 함수
int compare_opcode(Line *line, char* buf);

Line *getlinePtr = NULL;

void main() {
	FILE *input, *output;
	fopen_s(&input, "INPUT.txt", "r");
	fopen_s(&output, "OUTPUT.txt", "w");
	if (input == NULL || output == NULL) {
		printf("파일열기 실패");
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
	char arguments[MAX];
	Line get;
	strcpy(name->name, macro->label);
	strcpy(def->prototype->name, macro->label);

	//&args1,&args2 를 ,를 기준으로 split
	strcpy(arguments, macro->address);
	char *ptr = strtok(arguments, ",");
	while (ptr != NULL) {
		printf("%s\n", ptr);
		ptr = strtok(NULL, ",");
	}

	level = 1;
	while (level > 0) {
		get = getLine(in, NULL);
		if (!is_comment_line(macro)) {
			//처음것이 나오면 ?1로 저장, ?2로 저장... 부분 추가해야함 (substitute positional notation for parameters)
			//받은 line을 def에 저장
			def->line[index] = get;
			if (strcmp(macro->opcode, "MACRO")) {
				level++;
				name->start = &get;
			}
			else if (strcmp(macro->opcode, "MEND")) {
				level--;
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
		//테스트용
		return *pline;
	}
	
	return line;
}
