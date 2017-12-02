#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define _CRT_SECURE_NO_WARNINGS

#define TRUE 1
#define FALSE 0

#define WORD_MAX 50 //단어의 길이
#define MACRO_MAX 1000 //table의 개수
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
	char label[WORD_MAX];
	char opcode[WORD_MAX];
	char address[WORD_MAX];
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

//DEFTAB과 NAMTAB을 가지고 다른 함수를 호출하는 함수
void macro_processor(FILE *in, FILE *out);

//expanding의 값에 따라 DEFTAB에서 문자열을 읽거나 input된 FILE을 읽어오는 함수
Line getLine(FILE *in, PROTOTYPE *p, Line *pline);

//getline에서 읽어온 문자열로 expand나 define 함수를 호출하거나 expand된 소스를 작성하는 함수
void processline(FILE *in, FILE *out, DEFTAB *def, NAMTAB *name, Line *line);

//NAMTAB의 start부터 end까지의 문자열을 읽어와 expand된 소스를 작성하는 함수
void expand(FILE *in, FILE *out, DEFTAB *def, NAMTAB *name, Line *line);

//macro로부터 NAMTAB과 DEFTAB에 이름과 정의를 set하고 한줄씩 읽어 DEFTAB에 저장, NAMTAB에 시작과 끝의 위치를 저장한다
void define(Line *macro, DEFTAB *def, NAMTAB *name, FILE *in);

//주석인지 아닌지 검사하는 함수
int is_comment_line(Line *buf);

//line의 opcode가 주어진 문자열과 같은지 비교하는 함수
int compare_opcode(Line *line, char* buf);

Line *getlinePtr = NULL;

//문자열을 찾아 치환해주는 함수
char *replaceAll(char *s, const char *olds, const char *news);

int main() {
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
	expanding = FALSE;
	Line buf;
	while (strcmp(buf.opcode, "END")) {
		buf = getLine(in, NULL, NULL);
		//processline(in, out, &definetable, &nametable[top], &buf);
	}
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

	//&args1,&args2 를 ,를 기준으로 split
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
		get = getLine(in, NULL, NULL);
		if (!is_comment_line(macro)) {

			//처음것이 나오면 ?1로 치환, ?2로 치환...
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

			//받은 line을 def에 저장
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

Line getLine(FILE *in, PROTOTYPE *p, Line *pline)
{
	Line line;
	char buf[50];
	char *ptr = NULL;
	int temp = 0;
	if (expanding == TRUE) {
		line = *pline;
		replaceAll(line.address, "?1", p->argtab[0].argument);
		replaceAll(line.address, "?2", p->argtab[1].argument);
		replaceAll(line.address, "?3", p->argtab[2].argument);
		printf("%s\t%s\t%s\n", line.address, line.label, line.opcode);
	}
	else {
		fgets(buf, 50, in);
		if (buf[0] == '\t') {
			for (int i = 48; i >= 0; i--)
				buf[i + 1] = buf[i];
			buf[0] = ' ';
		}
		ptr = strtok(buf, "\t");
		strcpy(line.label, ptr);
		if (!strcmp(ptr, ".")) {
			strcpy(line.opcode, "");
			strcpy(line.address, "");
			return line;
		}
		ptr = strtok(NULL, "\t");
		strcpy(line.opcode, ptr);
		if (!strcmp(ptr, "MEND\n")) {
			strcpy(line.address, "");
			return line;
		}
		ptr = strtok(NULL, "\t");
		strcpy(line.address, ptr);
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

int is_comment_line(Line *buf) {
	if (buf->label[0] == '.')
		return 1;
	return 0;
}

int compare_opcode(Line *line, char* buf) {
	if (!strcmp(line->opcode, buf))
		return 1;
	return 0;
}