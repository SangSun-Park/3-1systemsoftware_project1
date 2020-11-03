/*
 * 화일명 : my_assembler_00000000.c
 * 설  명 : 이 프로그램은 SIC/XE 머신을 위한 간단한 Assembler 프로그램의 메인루틴으로,
 * 입력된 파일의 코드 중, 명령어에 해당하는 OPCODE를 찾아 출력한다.
 * 파일 내에서 사용되는 문자열 "00000000"에는 자신의 학번을 기입한다.
 */

 /*
  *
  * 프로그램의 헤더를 정의한다.
  *
  */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

  // 파일명의 "00000000"은 자신의 학번으로 변경할 것.
#include "my_assembler_20150235.h"


int add[MAX_LINES]; // 주소를 저장하는 변수
int code[MAX_LINES];// 출력해줄 기계어 코드 저장하는 변수

/* ----------------------------------------------------------------------------------
 * 설명 : 사용자로 부터 어셈블리 파일을 받아서 명령어의 OPCODE를 찾아 출력한다.
 * 매계 : 실행 파일, 어셈블리 파일
 * 반환 : 성공 = 0, 실패 = < 0
 * 주의 : 현재 어셈블리 프로그램의 리스트 파일을 생성하는 루틴은 만들지 않았다.
 *		   또한 중간파일을 생성하지 않는다.
 * ----------------------------------------------------------------------------------
 */
int main(int args, char *arg[])
{
	if (init_my_assembler() < 0)
	{
        printf("init_my_assembler: 프로그램 초기화에 실패 했습니다.\n");
        return -1;
    }

    if (assem_pass1() < 0)
    {
        printf("assem_pass1: 패스1 과정에서 실패하였습니다.  \n");
        return -1;
    }
    // make_opcode_output("output_00000000");

    make_symtab_output("symtab_20150235");
    make_literaltab_output("literaltab_20150235");
	make_symtab_output(NULL);			// NULL 입력 시 동작 확인용 
	make_literaltab_output(NULL);
    if (assem_pass2() < 0)
    {
        printf("assem_pass2: 패스2 과정에서 실패하였습니다.  \n");
        return -1;
    }
	make_objectcode_output(NULL);
	make_objectcode_output("output_20150235");

	return 0;
} 

/* ----------------------------------------------------------------------------------
 * 설명 : 프로그램 초기화를 위한 자료구조 생성 및 파일을 읽는 함수이다.
 * 매계 : 없음
 * 반환 : 정상종료 = 0 , 에러 발생 = -1
 * 주의 : 각각의 명령어 테이블을 내부에 선언하지 않고 관리를 용이하게 하기
 *		   위해서 파일 단위로 관리하여 프로그램 초기화를 통해 정보를 읽어 올 수 있도록
 *		   구현하였다.
 * ----------------------------------------------------------------------------------
 */
int init_my_assembler(void)
{
	int result;

	if ((result = init_inst_file("inst.txt")) < 0)
		return -1;
	if ((result = init_input_file("input.txt")) < 0)
		return -1;
	return result;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 머신을 위한 기계 코드목록 파일을 읽어 기계어 목록 테이블(inst_table)을
 *        생성하는 함수이다.
 * 매계 : 기계어 목록 파일
 * 반환 : 정상종료 = 0 , 에러 < 0
 * 주의 : 기계어 목록파일 형식은 자유롭게 구현한다. 예시는 다음과 같다.
 *
 *	===============================================================================
 *		   | 이름 | 형식 | 기계어 코드 | 오퍼랜드의 갯수 | NULL|
 *	===============================================================================
 *
 * ----------------------------------------------------------------------------------
 */
int init_inst_file(char *inst_file)
{
	int errno;
    FILE* file = fopen(inst_file, "r");
    while (!feof(file)) {				// inst 파일에서 한줄씩 입력 받아서 inst_table을 생성
        inst_table[inst_index] = (inst*)malloc(sizeof(inst));
        fscanf(file, "%s %d %s %d", &inst_table[inst_index]->opName, &inst_table[inst_index]->format, &inst_table[inst_index]->opcode, &inst_table[inst_index]->numop);
        inst_index++;
    }
	return errno;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 어셈블리 할 소스코드를 읽어 소스코드 테이블(input_data)를 생성하는 함수이다.
 * 매계 : 어셈블리할 소스파일명
 * 반환 : 정상종료 = 0 , 에러 < 0
 * 주의 : 라인단위로 저장한다.
 *
 * ----------------------------------------------------------------------------------
 */
int init_input_file(char *input_file)
{
    FILE* file = fopen(input_file, "r");
    int errno;
    char buff[200]; 
    int len;
    while (!feof(file)) {
        if (NULL == fgets(buff, 200, file)) // 소스코드 한줄 받아옴
            break;
        buff[strlen(buff) - 1] = '\0';
        if (buff[0] == '.')
            continue; 
        len = (int)strlen(buff);
        input_data[line_num] = (char*)malloc(sizeof(char) * len + 1);		// input_table을 생성하기 위해 크기 동적 할당
        strcpy(input_data[line_num], buff);			// 할당된 공간에 복사하여 입력                 
        line_num++;
    }
    return errno;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 소스 코드를 읽어와 토큰단위로 분석하고 토큰 테이블을 작성하는 함수이다.
 *        패스 1로 부터 호출된다.
 * 매계 : 파싱을 원하는 문자열
 * 반환 : 정상종료 = 0 , 에러 < 0
 * 주의 : my_assembler 프로그램에서는 라인단위로 토큰 및 오브젝트 관리를 하고 있다.
 * ----------------------------------------------------------------------------------
 */
int token_parsing(char *str)
{
	/* add your code here */
    int errno;
    token_table[token_line] = (token*)malloc(sizeof(token)); // token에 입력을 위해서 동적 할당
    token_table[token_line]->nixbpe = 0x00; // 초기에는 값이 없어 초기화
    for (int j = 0; j < MAX_OPERAND; j++)
        token_table[token_line]->operand[j] = NULL; //operand 3개 전부 초기화
    if (str[0] == '\t' || str[0] == ' ') { // label이 없는 경우
        token_table[token_line]->label = "";
        token_table[token_line]->operator = strtok(str, "\t");
        token_table[token_line]->operand[2] = strtok(NULL, "\t");
        token_table[token_line]->comment = strtok(NULL, "\n");
    }
    else {
        token_table[token_line]->label = strtok(str, "\t");
        token_table[token_line]->operator = strtok(NULL, "\t");
        token_table[token_line]->operand[2] = strtok(NULL, "\t");
        token_table[token_line]->comment = strtok(NULL, "\n");
    }
    if (token_table[token_line]->operand[2] != NULL) {   //operand가 여려개일 경우 나누어 저장하기 위해서 동작
        token_table[token_line]->operand[0] = strtok(token_table[token_line]->operand[2], ",");
        token_table[token_line]->operand[1] = strtok(NULL, ",");            // 내용이 없으면 NULL로 저장됨
        token_table[token_line]->operand[2] = strtok(NULL, "\n");
    }
    token_line++;
    return errno;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 입력 문자열이 기계어 코드인지를 검사하는 함수이다.
 * 매계 : 토큰 단위로 구분된 문자열
 * 반환 : 정상종료 = 기계어 테이블 인덱스, 에러 < 0
 * 주의 :
 *
 * ----------------------------------------------------------------------------------
 */
int search_opcode(char *str)
{
    if (str == NULL)
        return 0;
    if (str[0] == '+') {		// 4형식에도 opcode를 확인을 위해
        str = str + 1;
    }
    for (int i = 0; i < inst_index; i++) { 
        if (strcmp(inst_table[i]->opName, str) == 0) { 
            return i;
        }
    }
    return -1;
}

/* ----------------------------------------------------------------------------------
* 설명 : 어셈블리 코드를 위한 패스1과정을 수행하는 함수이다.
*		   패스1에서는..
*		   1. 프로그램 소스를 스캔하여 해당하는 토큰단위로 분리하여 프로그램 라인별 토큰
*		   테이블을 생성한다.
*
* 매계 : 없음
* 반환 : 정상 종료 = 0 , 에러 = < 0
* 주의 : 현재 초기 버전에서는 에러에 대한 검사를 하지 않고 넘어간 상태이다.
*	  따라서 에러에 대한 검사 루틴을 추가해야 한다.
*
* -----------------------------------------------------------------------------------
*/
static int assem_pass1(void)
{
    /* input_data의 문자열을 한줄씩 입력 받아서
     * token_parsing()을 호출하여 token_unit에 저장
     */

    int errno;
    for (int i = 0; i < line_num; i++) {  //input 하나씩 넘겨 token_table 생성
        token_parsing(input_data[i]);
    }
    char buff[50];
    int ad = 0;		// token의 주소 저장
    int sa = 0;		// literal의 중복을 막기 위해서 사용
    locctr = 0;		// control section을 나누기 위해서 초기화
    for (int i = 0; i < token_line; i++) {		// token_table을 기준으로 sym_table과 literal_table 생성
        if (strcmp(token_table[i]->operator, "CSECT") == 0) { // CSECT를  control section을 기준으로 sym_table 나누기
            ad = 0;
            strcpy(sym_table[sym_line].symbol, "\n");	// 나눠 사용하기 위해서 개행 추가
            sym_line++;
            strcpy(sym_table[sym_line].symbol, token_table[i]->label);	// 시작 하는 함수 이름 sym_table에 저장
            sym_table[sym_line].addr = ad;
            add[i] = ad;
            sym_line++;
        }
        else if (strcmp(token_table[i]->operator, "LTORG") == 0 || strcmp(token_table[i]->operator, "END") == 0) { // literal_table 저장
            char* str1, * str2;
            for (int j = locctr; j < i; j++) {
                if (token_table[j]->operand[0] == NULL)
                    continue;
                else if (token_table[j]->operand[0][0] == '=') { // token을 하나씩 확인하여 operand에 =이 들어있는지 확인
                    strcpy(buff, token_table[j]->operand[0]);
                    str1 = strtok(buff, "'");
                    str2 = strtok(NULL, "'");
                    for (int t = 0; t < literal_line; t++) {
                        if (strcmp(literal_table[t].literal, str2) == 0) {	// literal_table 안에 이미 있는 값이면 넘어가기 위해 확인
                            sa = 1;
                            break;
                        }
                    }
                    if (sa == 1) {	// literal_table에 있다면 넘어가고 sa 다시 초기화
                        sa = 0;
                        continue;
                    }
                    strcpy(literal_table[literal_line].literal, str2);	// 중복되지 않으니 literal 저장
                    literal_table[literal_line].addr = ad;
                    if (strcmp(str1, "=X") == 0)	// 주소의 값을 증가
                        ad++;
                    else
                        ad += strlen(literal_table[literal_line].literal);
                    add[i] = ad;		// 주소 값을 add 배열에 저장
                    literal_line++;
                }
            }
            locctr = i + 1;
        }
        else if (token_table[i]->label == "") { // label 없는지 확인 후 동작
            if (search_opcode(token_table[i]->operator) >= 0) { // opcode가 있는지 확인
                if (token_table[i]->operator[0] == '+')
                    ad++;
                ad += inst_table[search_opcode(token_table[i]->operator)]->format;
                add[i] = ad;
            }
        }
        else {
            strcpy(sym_table[sym_line].symbol, token_table[i]->label);	// label이 있다면 sym_table에 추가
            sym_table[sym_line].addr = ad;
            if (search_opcode(token_table[i]->operator) >= 0) {		//다음 코드 라인의 주소를 위해서 계산
                if (token_table[i]->operator[0] == '+')
                    ad++;
                ad += inst_table[search_opcode(token_table[i]->operator)]->format;
            }
            else if (strcmp(token_table[i]->operator, "RESB") == 0)
                ad += (int)atoi(token_table[i]->operand[0]);
            else if (strcmp(token_table[i]->operator, "RESW") == 0)
                ad += (int)atoi(token_table[i]->operand[0]) * 3;
            else if (strcmp(token_table[i]->operator, "BYTE") == 0)
                ad += (int)sizeof(token_table[i]->operand[0]) - 3;
            else if (strcmp(token_table[i]->operator, "WORD") == 0)
                ad += 3;
            else if (strcmp(token_table[i]->operator, "EQU") == 0)
                if (strcmp(token_table[i]->operand[0], "*") != 0) {
                    strcpy(buff, token_table[i]->operand[0]);
                    char* str1, * str2;
                    str1 = strtok(buff, "-");
                    str2 = strtok(NULL, "\n");
                    int q = 0, p = 0;
                    for (int j = 0; j < sym_line; j++) {
                        if (strcmp(sym_table[j].symbol, str1) == 0)
                            q = sym_table[j].addr;
                        else if (strcmp(sym_table[j].symbol, str2) == 0)
                            p = sym_table[j].addr;
                    }
                    sym_table[sym_line].addr = q - p;
                }
            add[i] = ad;
            sym_line++;
        }
    }
    strcpy(sym_table[sym_line].symbol, "\n");
    sym_line++;
    return errno;
}

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 명령어 옆에 OPCODE가 기록된 표(과제 5번) 이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*        또한 과제 5번에서만 쓰이는 함수이므로 이후의 프로젝트에서는 사용되지 않는다.
* -----------------------------------------------------------------------------------
*/
// void make_opcode_output(char *file_name)
// {
// 	/* add your code here */

// }

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 SYMBOL별 주소값이 저장된 TABLE이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/
void make_symtab_output(char *file_name)
{
	/* add your code here */
    
    if (file_name == NULL) {	// 파일 이름 값을 null로 주었을 때
        for (int i = 0; i < sym_line; i++) {
            if (strcmp(sym_table[i].symbol, "\n") == 0) {
                printf("\n");
                continue;
            }
            printf("%s\t%X\n", sym_table[i].symbol, sym_table[i].addr);
        }
    }
    else {
        FILE* file = fopen(file_name, "w");
        for (int i = 0; i < sym_line; i++) {
            if (strcmp(sym_table[i].symbol, "\n") == 0) {
                fprintf(file, "\n");
                continue;
            }
            fprintf(file, "%s\t%X\n", sym_table[i].symbol, sym_table[i].addr);
        }
        fclose(file);
    }
}

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 LITERAL별 주소값이 저장된 TABLE이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/
void make_literaltab_output(char *filen_ame)
{
	/* add your code here */
    if(filen_ame == NULL) { // 파일 이름 값을 null로 주었을 때
        for (int i = 0; i < literal_line; i++) { 
            printf("%s\t%X\n", literal_table[i].literal, literal_table[i].addr);
        }
		printf("\n");
    }
    else {
        FILE* file = fopen(filen_ame, "w");
        for (int i = 0; i < literal_line; i++) {
            fprintf(file, "%s\t%X\n", literal_table[i].literal, literal_table[i].addr);
        }
		fprintf(file, "\n");
        fclose(file);
    }
}

/* ----------------------------------------------------------------------------------
* 설명 : 어셈블리 코드를 기계어 코드로 바꾸기 위한 패스2 과정을 수행하는 함수이다.
*		   패스 2에서는 프로그램을 기계어로 바꾸는 작업은 라인 단위로 수행된다.
*		   다음과 같은 작업이 수행되어 진다.
*		   1. 실제로 해당 어셈블리 명령어를 기계어로 바꾸는 작업을 수행한다.
* 매계 : 없음
* 반환 : 정상종료 = 0, 에러발생 = < 0
* 주의 :
* -----------------------------------------------------------------------------------
*/
static int assem_pass2(void)
{
    int errno;
    locctr = 0; // control section을 나누기 위해서 초기화
    char buff[50];
    int j = 0; // control section을 변경 시켜주기 위해
    int slocctr = 0; // sym_table의 section을 나누기 위해 사용
	for (int i = 0; i < token_line; i++) {
        if (strcmp(token_table[i]->operator, "START") == 0 || strcmp(token_table[i]->operator, "CSECT") == 0) {	// section 별로 기계어 코드 초기화
            if (strcmp(token_table[i]->operator, "CSECT") == 0) {
				locctr = j + 1;
				int y = 0;
				for (y = slocctr; strcmp(sym_table[y].symbol, "\n") != 0; y++) {}
				slocctr = y + 1;
			}
			for (j = locctr; j < token_line; j++) {
				if (strcmp(token_table[j]->operator, "CSECT") == 0)
					break;
			}
		}
		else {   
            int op = search_opcode(token_table[i]->operator);
            if (op >= 0) {			// opcode가 있으면 nixbpe 입력 
				if (inst_table[op]->format == 2 || inst_table[op]->format == 1) { 
				}
                else {
                    if (token_table[i]->operand[0][0] == '@') //n = 1, i = 0
                        token_table[i]->nixbpe |= 1 << 5;
                    else if (token_table[i]->operand[0][0] == '#') // n = 0, i = 1
                        token_table[i]->nixbpe |= 1 << 4;
                    else token_table[i]->nixbpe |= 3 << 4; // n = 1, i = 1

                    if (token_table[i]->operator[0] == '+') // e = 1 
                        token_table[i]->nixbpe |= 1;
                    else if (token_table[i]->operand[0][0] != '#' && inst_table[op]->numop != 0) // operand가 없는 것 처리
                        token_table[i]->nixbpe |= 1 << 1; // p = 1
                    if (token_table[i]->operand[1] != NULL)
                        if (strcmp(token_table[i]->operand[1], "X") == 0) //x = 1
                            token_table[i]->nixbpe |= 1 << 3;
                }
			}
			if (op >= 0) {
				if (inst_table[op]->format == 1) {
					code[i] = strtoul(inst_table[op]->opcode, NULL, 16);
				}
				else if (inst_table[op]->format == 2) {
                    code[i] = strtoul(inst_table[op]->opcode, NULL, 16);
					for (int y = 0; y < inst_table[op]->numop; y++) {
						if (inst_table[op]->numop == 1) {		// operand가 1개인 경우
							code[i] = code[i] << 8;
							if (strcmp(token_table[i]->operand[y], "A") == 0)
								code[i] |= 0;
							else if (strcmp(token_table[i]->operand[y], "S") == 0)
								code[i] |= 4 << 4;
							else if (strcmp(token_table[i]->operand[y], "T") == 0)
								code[i] |= 5 << 4;
							else if (strcmp(token_table[i]->operand[y], "X") == 0)
								code[i] |= 1 << 4;
						}
						else {
							code[i] = code[i] << 4;		// operand가 2개인 경우
							if (strcmp(token_table[i]->operand[y], "A") == 0)
								code[i] |= 0;
							else if (strcmp(token_table[i]->operand[y], "S") == 0)
								code[i] |= 4;
							else if (strcmp(token_table[i]->operand[y], "T") == 0)
								code[i] |= 5;
							else if (strcmp(token_table[i]->operand[y], "X") == 0)
								code[i] |= 1;
						}
					}
				}
				else if (inst_table[op]->format >= 3) {
					char* str = 0;
					if (token_table[i]->operator[0] == '+') { // 4형식일때
						code[i] = strtoul(inst_table[op]->opcode, NULL, 16) << 24; // opcode를 입력
						code[i] += token_table[i]->nixbpe << 20;	//nixbpe를 마스킹
						int f = 1;
						if (token_table[i]->operand[0][0] == '#') {		// immediate 일경우
							strcpy(buff, token_table[i]->operand[0]);
							str = strtok(buff, "\n");
							str += 1;
							for (int y = slocctr; y < sym_line; y++) {
								if (strcmp(sym_table[y].symbol, "\n") == 0)
									break;
								if (strcmp(sym_table[y].symbol, str) == 0) {
									code[i] += sym_table[y].addr;
									f = 0;
									break;
								}
							}
							if (f)
								for (int y = 0; y < literal_line; y++) {
									if (strcmp(literal_table[y].literal, str) == 0) {
										code[i] += literal_table[y].addr;
										f = 0;
										break;
									}
								}
							if(f) code[i] += atoi(str);
						}
						else if (token_table[i]->operand[0][0] == '@') { // indirect 인경우
							strcpy(buff, token_table[i]->operand[0]);
							str = strtok(buff, "\n");
							str += 1;
							for (int y = slocctr; y < sym_line; y++) {
								if (strcmp(sym_table[y].symbol, "\n") == 0)
									break;
								if (strcmp(sym_table[y].symbol, str) == 0) {
									code[i] += sym_table[y].addr;
									f = 0;
									break;
								}
							}
							if (f)
								for (int y = 0; y < literal_line; y++) {
									if (strcmp(literal_table[y].literal, str) == 0) {
										code[i] += literal_table[y].addr;
										f = 0;
										break;
									}
								}
						}
						else if (token_table[i]->operand[0][0] == '=') { // literal일 경우
							strcpy(buff, token_table[i]->operand[0]);
							for (int y = slocctr; y < sym_line; y++) {
								if (strcmp(sym_table[y].symbol, "\n") == 0)
									break;
								if (strcmp(sym_table[y].symbol, str) == 0) {
									if (sym_table[y].addr < add[i])			// 참조 위치의 주소가 작을때 적용
										code[i] += 4096;
									code[i] += sym_table[y].addr - add[i];
									f = 0;
									break;
								}
							}
							if (f)
								for (int y = 0; y < literal_line; y++) {
									if (strcmp(literal_table[y].literal, str) == 0) {
										if (literal_table[y].addr < add[i])
											code[i] += 4096;
										code[i] += literal_table[y].addr - add[i];
										break;
									}
								}
						}
						else {		// 일반적인 경우
							strcpy(buff, token_table[i]->operand[0]);
							str = strtok(buff, "\n");
							for (int y = slocctr; y < sym_line; y++) {
								if (strcmp(sym_table[y].symbol, "\n") == 0)
									break;
								if (strcmp(sym_table[y].symbol, str) == 0) {
									if (sym_table[y].addr < add[i])
										code[i] += 4096;
									code[i] += sym_table[y].addr - add[i];
									f = 0;
									break;
								}
							}
							if (f)
								for (int y = 0; y < literal_line; y++) {
									if (strcmp(literal_table[y].literal, str) == 0) {
										if (literal_table[y].addr < add[i])
											code[i] += 4096;
										code[i] += literal_table[y].addr - add[i];
										break;
									}
								}
						}
					}
					else {		// 3형식
						code[i] = strtoul(inst_table[op]->opcode, NULL, 16) << 16;
						code[i] += token_table[i]->nixbpe << 12;
						int f = 1;
						if (token_table[i]->operand[0][0] == '#') {
							strcpy(buff, token_table[i]->operand[0]);
							str = strtok(buff, "\n");
							str += 1;
							for (int y = slocctr; y < sym_line; y++) {
								if (strcmp(sym_table[y].symbol, "\n") == 0)
									break;
								if (strcmp(sym_table[y].symbol, str) == 0) {
									code[i] += sym_table[y].addr;
									f = 0;
									break;
								}
							}
							if (f)
								for (int y = 0; y < literal_line; y++) {
									if (strcmp(literal_table[y].literal, str) == 0) {
										code[i] += literal_table[y].addr;
										f = 0;
										break;
									}
								}
							if (f) code[i] += atoi(str);
						}
						else if (token_table[i]->operand[0][0] == '@') {
							strcpy(buff, token_table[i]->operand[0]);
							str = strtok(buff, "\n");
							str += 1;
							for (int y = slocctr; y < sym_line; y++) {
								if (strcmp(sym_table[y].symbol, "\n") == 0)
									break;
								if (strcmp(sym_table[y].symbol, str) == 0) {
                                    for(int k = locctr; k < token_line; k++)
                                        if (strcmp(sym_table[y].symbol, token_table[k]->label) == 0) {
                                            if (strcmp(token_table[k]->operator, "RESB") == 0 || strcmp(token_table[k]->operator, "RESW") == 0) {
                                                code[i] += 0;
                                            }
                                            else {
                                                code[i] += code[k];
                                            }
                                            f = 0;
                                            break;
                                        }
                                    if (f) {
                                        code[i] += sym_table[y].addr;
                                        f = 0;
                                    }
									break;
								}
							}
							if (f)
								for (int y = 0; y < literal_line; y++) {
									if (strcmp(literal_table[y].literal, str) == 0) {
										code[i] += (int)literal_table[y].literal;
										f = 0;
										break;
									}
								}
						}
						else if (token_table[i]->operand[0][0] == '=') {
							strcpy(buff, token_table[i]->operand[0]);
							str = strtok(buff, "'");
							str = strtok(NULL, "'");
							for (int y = slocctr; y < sym_line; y++) {
								if (strcmp(sym_table[y].symbol, "\n") == 0)
									break;
								if (strcmp(sym_table[y].symbol, str) == 0) {
									if (sym_table[y].addr < add[i])
										code[i] += 4096;
									code[i] += sym_table[y].addr - add[i];
									f = 0;
									break;
								}
							}
							if (f)
								for (int y = 0; y < literal_line; y++) {
									if (strcmp(literal_table[y].literal, str) == 0) {
										if (literal_table[y].addr < add[i])
											code[i] += 4096;
										code[i] += literal_table[y].addr - add[i];
										break;
									}
								}
						}
						else {
							strcpy(buff, token_table[i]->operand[0]);
							str = strtok(buff, "\n");
							for (int y = slocctr; y < sym_line; y++) {
								if (strcmp(sym_table[y].symbol, "\n") == 0)
									break;
								if (strcmp(sym_table[y].symbol, str) == 0) {
									if (sym_table[y].addr < add[i])
										code[i] += 4096;
									code[i] += sym_table[y].addr - add[i];
									f = 0;
									break;
								}
							}
							if (f)
								for (int y = 0; y < literal_line; y++) {
									if (strcmp(literal_table[y].literal, str) == 0) {
										if (literal_table[y].addr < add[i])
											code[i] += 4096;
										code[i] += literal_table[y].addr - add[i];
										break;
									}
								}
						}
					}
				}
			}
		}
	}
    return errno;
}

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 object code (프로젝트 1번) 이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/
void make_objectcode_output(char* file_name)
{
	/* add your code here */
	if (file_name != NULL) {
		FILE* file = fopen(file_name, "w");
		locctr = 0; // control section 체크용
		int ad = 0; // head의 길이 체크용
		int length = 1; // Text의 길이 체크용
		int li = 0;	//head 길이 계산시 literal_table의 중복 체크 방지 
		int sli = 0; //text의 길이 계산시 literal_table의 중복 체크 방지
		char buff[50]; // string 임시저장용
		int j = 0; // locctr의 변화를 및 head 길이 체크를 위한 반복문 변수
		int extr = 0; // EXTREF의 위치 저장
		for (int i = 0; i < token_line; i++) {
			int op = search_opcode(token_table[i]->operator);
			if (strcmp(token_table[i]->operator, "START") == 0 || strcmp(token_table[i]->operator, "CSECT") == 0) { //head 출력
				ad = 0;
				length = 0;
				if (strcmp(token_table[i]->operator, "CSECT") == 0) {	// end 출력 후 head 출력
					if (length < 60)
						fprintf(file, "\n");
					for (int q = locctr; q < token_line; q++) {    // 외부 참조 사용할 수 있게 modification 생성
						if (strcmp(token_table[q]->operator, "CSECT") == 0 || strcmp(token_table[q]->operator, "LTORG") == 0)
							break;
						if (strcmp(token_table[q]->operator, "EXTREF") == 0 || strcmp(token_table[q]->operator, "EXTDEF") == 0)
							continue;
						if (strcmp(token_table[extr]->operand[0], token_table[q]->operand[0]) == 0)				// EXTREF의 operand 내용 확인하여 비교 후 있으면 출력
							fprintf(file, "M%06X05+%s\n", (add[q] - inst_table[search_opcode(token_table[q]->operator)]->format), token_table[extr]->operand[0]);
						else if (token_table[extr]->operand[1] == NULL)
							continue;
						else if (strcmp(token_table[extr]->operand[1], token_table[q]->operand[0]) == 0)
							fprintf(file, "M%06X05+%s\n", (add[q] - inst_table[search_opcode(token_table[q]->operator)]->format), token_table[extr]->operand[1]);
						else if (token_table[extr]->operand[2] == NULL)
							continue;
						else if (strcmp(token_table[extr]->operand[2], token_table[q]->operand[0]) == 0)
							fprintf(file, "M%06X05+%s\n", (add[q] - inst_table[search_opcode(token_table[q]->operator)]->format), token_table[extr]->operand[2]);
						else if (strcmp(token_table[q]->operator, "WORD") == 0)	// 수식이 존재 할때 처리
							if (strcmp(token_table[q]->operand[0], "*") != 0) {
								strcpy(buff, token_table[q]->operand[0]);
								char* str1, * str2;
								str1 = strtok(buff, "-");
								str2 = strtok(NULL, "\n");
								fprintf(file, "M%06X06+%s\n", add[q] - 3, str1);
								fprintf(file, "M%06X06-%s\n", add[q] - 3, str2);
							}
					}
					if (locctr == 0)	// 처음의 end recode만 주소값이 필요
						fprintf(file, "E%06X\n\n", sym_table[locctr].addr);
					else fprintf(file, "E\n\n");
					length = 0;
					locctr = j + 1;
				}
				for (j = locctr; j < token_line; j++) {   // head의 총길이 출력
					if (strcmp(token_table[j]->operator, "CSECT") == 0)
						break;
					else {
						if (strcmp(token_table[j]->operator, "LTORG") == 0 || strcmp(token_table[j]->operator, "END") == 0) {	// literal의 길이도 추가하기 위해서 동작
							char* str1, * str2;
							for (int k = locctr; k < j; k++) {
								if (token_table[k]->operand[0] == NULL)
									continue;
								else if (token_table[k]->operand[0][0] == '=') {	// 소스코드 안에 존재하는 literal만 추가
									strcpy(buff, token_table[k]->operand[0]);
									str1 = strtok(buff, "'");
									str2 = strtok(NULL, "'");
									for (int t = li; t < literal_line; t++) {
										if (strcmp(literal_table[t].literal, str2) == 0) {
											li++;						// 중복되어서 추가 되지 않기 위해 증가
											if (strcmp(str1, "=X") == 0)
												ad += (int)strlen(str2) / 2;
											else
												ad += 3;
											break;
										}
									}
								}
							}
						}
						else if (token_table[j]->label == "") {
							if (search_opcode(token_table[j]->operator) >= 0) {
								if (token_table[j]->operator[0] == '+')	// 4형식 경우
									ad++;
								ad += inst_table[search_opcode(token_table[j]->operator)]->format;
							}
						}
						else {
							if (search_opcode(token_table[j]->operator) >= 0) {
								if (token_table[j]->operator[0] == '+')
									ad++;
								ad += inst_table[search_opcode(token_table[j]->operator)]->format;
							}
							else if (strcmp(token_table[j]->operator, "RESB") == 0)		// 경우 별로 길이 추가
								ad += (int)atoi(token_table[j]->operand[0]);
							else if (strcmp(token_table[j]->operator, "RESW") == 0)
								ad += (int)atoi(token_table[j]->operand[0]) * 3;
							else if (strcmp(token_table[j]->operator, "BYTE") == 0)
								ad += (int)sizeof(token_table[j]->operand[0]) - 3;
							else if (strcmp(token_table[j]->operator, "WORD") == 0)
								ad += 3;
						}
					}
				}
				fprintf(file, "H%-6s%06X%06X\n", token_table[i]->label, sym_table[locctr].addr, ad); // head 출력
				ad = 0;
			}
			else if (strcmp(token_table[i]->operator, "EXTDEF") == 0) {	// EXTDEF 출력
				int opn = 0;
				while (token_table[i]->operand[opn]) { // operand가 존재 안할때까지 출력
					for (int li = locctr; li < sym_line; li++) {
						if (strcmp(token_table[i]->operand[opn], sym_table[li].symbol) == 0) {	// sym_table에서 주소 받아오기
							if (opn == 0) {
								fprintf(file, "D%-6s%06X", sym_table[li].symbol, sym_table[li].addr); // 처음에만 출력
							}
							else
								fprintf(file, "%-6s%06X", sym_table[li].symbol, sym_table[li].addr);
							break;
						}
					}
					opn++;
				}
				fprintf(file, "\n");
			}
			else if (strcmp(token_table[i]->operator, "EXTREF") == 0) {	// EXTREF 출력
				extr = i;
				int opn = 0;
				while (token_table[i]->operand[opn] != NULL) {	// operand가 없을 때까지 출력
					if (opn == 0)
						fprintf(file, "R%-6s", token_table[i]->operand[opn]);	// 처음에만 출력
					else
						fprintf(file, "%-6s", token_table[i]->operand[opn]);
					opn++;
				}
				fprintf(file, "\n");
			}
			else if (strcmp(token_table[i]->operator, "LTORG") == 0) {	// 코드에 존재하는 literal 출력
				char* str1, * str2;
				for (int k = locctr; k < i; k++) {
					if (token_table[k]->operand[0] == NULL)
						continue;
					else if (token_table[k]->operand[0][0] == '=') {
						strcpy(buff, token_table[k]->operand[0]);
						str1 = strtok(buff, "'");
						str2 = strtok(NULL, "'");
						for (int t = sli; t < literal_line; t++) {
							if (strcmp(literal_table[t].literal, str2) == 0) {
								sli++;
								if (t == 0) {		//0일때만 출력
									if (strcmp(str1, "=X") == 0)	
										fprintf(file, "\nT%06X%02X", (add[i] - strlen(str2)), strlen(str2));
									else
										fprintf(file, "\nT%06X%02X", (add[i] - strlen(str2)), strlen(str2));
									for (int z = 0; z < (int)strlen(str2); z++)
										fprintf(file, "%X", str2[z]);
									break;
								}
								else {
									if (strcmp(str1, "=X") == 0)
										fprintf(file, "%06X%02X", (add[i] - strlen(str2)), strlen(str2));
									else
										fprintf(file, "%06X%02X", (add[i] - strlen(str2)), strlen(str2));
									for (int z = 0; z < (int)strlen(str2); z++)
										fprintf(file, "%X", str2[z]);
									break;
								}
							}
						}
					}
				}
			}
			else {
				if (length == 0) {		// text recode의 새로운 시작 출력
					int l = 0;		// text의 길이
					int pleng = 0;	// text의 시작표시
					for (pleng = i; pleng < token_line; pleng++)
					{
						if (strcmp(token_table[pleng]->operator, "BYTE") == 0) {	// literal에 저장 안되있는 경우
							char* str, * str2;
							strcpy(buff, token_table[pleng]->operand[0]);
							str = strtok(buff, "'");
							str2 = strtok(NULL, "'");
							l += (int)(strlen(str2) / 2);
						}
						else if (strcmp(token_table[pleng]->operator, "WORD") == 0) {
							l += 3;
						}
						else if (strcmp(token_table[pleng]->operator, "END") == 0) {	// 남은 literal 모두 출력
							char* str1, * str2;
							int lix = 0;
							for (int el = locctr; el < token_line; el++) {
								if (lix >= literal_line)
									break;
								if (token_table[el]->operand[0][0] == '=') {		// 마지막 section의 남은 literal을 text recode 길이에 추가
									strcpy(buff, token_table[el]->operand[0]);
									str1 = strtok(buff, "'");
									str2 = strtok(NULL, "'");
									for (int t = lix; t < literal_line; t++) {
										if (strcmp(literal_table[t].literal, str2) == 0) {
											lix = t + 1;
											break;
										}
									}
									if (strcmp(str1, "=X") == 0)
										l += (int)(strlen(str2) / 2);	// BYTE 일때
									else
										l += 3;	//WORD 일때
								}
							}
						}
						else if (search_opcode(token_table[pleng]->operator) < 0)
							break;
						else if (l + inst_table[search_opcode(token_table[pleng]->operator)]->format <= 30) {	// opcode의 format 확인하여서 길이 추가
							if (token_table[pleng]->operator[0] == '+')
								l++;
							l += inst_table[search_opcode(token_table[pleng]->operator)]->format;
						}
						else if (strcmp(token_table[i]->operator, "RESB") == 0 || strcmp(token_table[i]->operator, "RESW") == 0) {
							continue;
						}
						else
							break;
					}
					pleng -= 1;
					fprintf(file, "T%06X%02X", (add[i] - inst_table[search_opcode(token_table[i]->operator)]->format), l);
				}
				if (op >= 0) {		// text 본문 출력  opcode가 존재 할시
					if (inst_table[op]->format == 1) {		
						fprintf(file, "%X", code[i]);
						length += 2;
					}
					else if (inst_table[op]->format == 2) {
						fprintf(file, "%4X", code[i]);
						length += 4;
					}
					else if (token_table[i]->operator[0] == '+') {
						fprintf(file, "%08X", code[i]);
						length += 8;
					}
					else {
						fprintf(file, "%06X", code[i]);
						length += 6;
					}
					if (search_opcode(token_table[i + 1]->operator) >= 0)
						if (length + inst_table[search_opcode(token_table[i + 1]->operator)]->format > 60) {
							fprintf(file, "\n");
							length = 0;
						}
				}
				else {
					if (strcmp(token_table[i]->operator, "BYTE") == 0) {	// BYTE 일때 이어서 입력
						char* str;
						strcpy(buff, token_table[i]->operand[0]);
						str = strtok(buff, "'");
						str = strtok(NULL, "'");
						fprintf(file, "%s", str);
						length += strlen(str);
					}
					else if (strcmp(token_table[i]->operator, "WORD") == 0) {	// WORD 일때 이어서 입력
						fprintf(file, "%06X", code[i]);
						length += 6;
					}
					else if (strcmp(token_table[i]->operator, "END") == 0) {	// END로 끝나면 literal table을 확인하여 남은 literal 출력
						char* str1, * str2;
						int lix = 0;
						for (int el = locctr; el < token_line; el++) {
							if (lix >= literal_line)
								break;
							if (token_table[el]->operand[0][0] == '=') {
								strcpy(buff, token_table[el]->operand[0]);
								str1 = strtok(buff, "'");
								str2 = strtok(NULL, "'");
								for (int t = lix; t < literal_line; t++) {		// literal 목록에서 찾으면 break
									if (strcmp(literal_table[t].literal, str2) == 0) {
										lix = t + 1;
										break;
									}
								}
								if (strcmp(str1, "=X") == 0) {
									fprintf(file, "%s", str2);
									length += (int)(strlen(str2) / 2);
								}
								else {
									fprintf(file, "%06X", code[i]);
									length += 3;
								}
							}
						}
					}
				}

			}

		}
		if (length < 60) // text recode가 끝나면 길이가 60이 되지 않더라도 개행 입력
			fprintf(file, "\n");
		for (int q = locctr; q < token_line; q++) {	// 마지막 section의 modification 생성
			if (strcmp(token_table[q]->operator, "CSECT") == 0 || strcmp(token_table[q]->operator, "LTORG") == 0)
				break;
			if (strcmp(token_table[q]->operator, "EXTREF") == 0 || strcmp(token_table[q]->operator, "EXTDEF") == 0)
				continue;
			if (strcmp(token_table[extr]->operand[0], token_table[q]->operand[0]) == 0)
				fprintf(file, "M%06X05+%s\n", (add[q] - inst_table[search_opcode(token_table[q]->operator)]->format), token_table[extr]->operand[0]);
			else if (token_table[extr]->operand[1] == NULL)
				continue;
			else if (strcmp(token_table[extr]->operand[1], token_table[q]->operand[0]) == 0)
				fprintf(file, "M%06X05+%s\n", (add[q] - inst_table[search_opcode(token_table[q]->operator)]->format), token_table[extr]->operand[1]);
			else if (token_table[extr]->operand[2] == NULL)
				continue;
			else if (strcmp(token_table[extr]->operand[2], token_table[q]->operand[0]) == 0)
				fprintf(file, "M%06X05+%s\n", (add[q] - inst_table[search_opcode(token_table[q]->operator)]->format), token_table[extr]->operand[2]);
		}
		fprintf(file, "E"); // 마지막 section의 end recode
		fclose(file);
	}
	else {
		locctr = 0;
		int ad = 0;
		int sa = 0;
		int length = 1;
		int li = 0;
		int sli = 0;
		char buff[50];
		int j = 0;
		int extr = 0;
		for (int i = 0; i < token_line; i++) {
			int op = search_opcode(token_table[i]->operator);
			if (strcmp(token_table[i]->operator, "START") == 0 || strcmp(token_table[i]->operator, "CSECT") == 0) { //head 출력
				ad = 0;
				length = 0;
				if (strcmp(token_table[i]->operator, "CSECT") == 0) {
					if (length < 60)
						printf("\n");
					for (int q = locctr; q < token_line; q++) {    // 외부 참조 사용할 수 있게 modification 생성
						if (strcmp(token_table[q]->operator, "CSECT") == 0 || strcmp(token_table[q]->operator, "LTORG") == 0)
							break;
						if (strcmp(token_table[q]->operator, "EXTREF") == 0 || strcmp(token_table[q]->operator, "EXTDEF") == 0)
							continue;
						if (strcmp(token_table[extr]->operand[0], token_table[q]->operand[0]) == 0)
							printf("M%06X05+%s\n", (add[q] - inst_table[search_opcode(token_table[q]->operator)]->format), token_table[extr]->operand[0]);
						else if (token_table[extr]->operand[1] == NULL)
							continue;
						else if (strcmp(token_table[extr]->operand[1], token_table[q]->operand[0]) == 0)
							printf("M%06X05+%s\n", (add[q] - inst_table[search_opcode(token_table[q]->operator)]->format), token_table[extr]->operand[1]);
						else if (token_table[extr]->operand[2] == NULL)
							continue;
						else if (strcmp(token_table[extr]->operand[2], token_table[q]->operand[0]) == 0)
							printf("M%06X05+%s\n", (add[q] - inst_table[search_opcode(token_table[q]->operator)]->format), token_table[extr]->operand[2]);
						else if (strcmp(token_table[q]->operator, "WORD") == 0)
							if (strcmp(token_table[q]->operand[0], "*") != 0) {
								strcpy(buff, token_table[q]->operand[0]);
								char* str1, * str2;
								str1 = strtok(buff, "-");
								str2 = strtok(NULL, "\n");
								printf("M%06X06+%s\n", add[q] - 3, str1);
								printf("M%06X06-%s\n", add[q] - 3, str2);
							}
					}
					if (locctr == 0)
						printf("E%06X\n\n", sym_table[locctr].addr);
					else printf("E\n\n");
					length = 0;
					locctr = j + 1;
				}
				for (j = locctr; j < token_line; j++) {   // head의 총길이 출력
					if (strcmp(token_table[j]->operator, "CSECT") == 0)
						break;
					else {
						if (strcmp(token_table[j]->operator, "LTORG") == 0 || strcmp(token_table[j]->operator, "END") == 0) {
							char* str1, * str2;
							for (int k = locctr; k < j; k++) {
								if (token_table[k]->operand[0] == NULL)
									continue;
								else if (token_table[k]->operand[0][0] == '=') {
									strcpy(buff, token_table[k]->operand[0]);
									str1 = strtok(buff, "'");
									str2 = strtok(NULL, "'");
									for (int t = li; t < literal_line; t++) {
										if (strcmp(literal_table[t].literal, str2) == 0) {
											li++;
											if (strcmp(str1, "=X") == 0)
												ad += (int)strlen(str2) / 2;
											else
												ad += 3;
											break;
										}
									}
								}
							}
						}
						else if (token_table[j]->label == "") {
							if (search_opcode(token_table[j]->operator) >= 0) {
								if (token_table[j]->operator[0] == '+')
									ad++;
								ad += inst_table[search_opcode(token_table[j]->operator)]->format;
							}
						}
						else {
							if (search_opcode(token_table[j]->operator) >= 0) {
								if (token_table[j]->operator[0] == '+')
									ad++;
								ad += inst_table[search_opcode(token_table[j]->operator)]->format;
							}
							else if (strcmp(token_table[j]->operator, "RESB") == 0)
								ad += (int)atoi(token_table[j]->operand[0]);
							else if (strcmp(token_table[j]->operator, "RESW") == 0)
								ad += (int)atoi(token_table[j]->operand[0]) * 3;
							else if (strcmp(token_table[j]->operator, "BYTE") == 0)
								ad += (int)sizeof(token_table[j]->operand[0]) - 3;
							else if (strcmp(token_table[j]->operator, "WORD") == 0)
								ad += 3;
						}
					}
				}
				printf("H%-6s%06X%06X\n", token_table[i]->label, sym_table[locctr].addr, ad);
				ad = 0;
			}
			else if (strcmp(token_table[i]->operator, "EXTDEF") == 0) {
				int opn = 0;
				while (token_table[i]->operand[opn]) {
					for (int li = locctr; li < sym_line; li++) {
						if (strcmp(token_table[i]->operand[opn], sym_table[li].symbol) == 0) {
							if (opn == 0) {
								printf("D%-6s%06X", sym_table[li].symbol, sym_table[li].addr);
							}
							else
								printf("%-6s%06X", sym_table[li].symbol, sym_table[li].addr);
							break;
						}
					}
					opn++;
				}
				printf("\n");
			}
			else if (strcmp(token_table[i]->operator, "EXTREF") == 0) {
				extr = i;
				int opn = 0;
				while (token_table[i]->operand[opn] != NULL) {
					if (opn == 0)
						printf("R%-6s", token_table[i]->operand[opn]);
					else
						printf("%-6s", token_table[i]->operand[opn]);
					opn++;
				}
				printf("\n");
			}
			else if (strcmp(token_table[i]->operator, "LTORG") == 0) {
				char* str1, * str2;
				for (int k = locctr; k < i; k++) {
					if (token_table[k]->operand[0] == NULL)
						continue;
					else if (token_table[k]->operand[0][0] == '=') {
						strcpy(buff, token_table[k]->operand[0]);
						str1 = strtok(buff, "'");
						str2 = strtok(NULL, "'");
						for (int t = sli; t < literal_line; t++) {
							if (strcmp(literal_table[t].literal, str2) == 0) {
								sli++;
								if (t == 0) {		//0일때만 출력
									if (strcmp(str1, "=X") == 0)
										printf("\nT%06X%02X", (add[i] - strlen(str2)), strlen(str2));
									else
										printf("\nT%06X%02X", (add[i] - strlen(str2)), strlen(str2));
									for (int z = 0; z < (int)strlen(str2); z++)
										printf("%X", str2[z]);
									break;
								}
								else {
									if (strcmp(str1, "=X") == 0)
										printf("%06X%02X", (add[i] - strlen(str2)), strlen(str2));
									else
										printf("%06X%02X", (add[i] - strlen(str2)), strlen(str2));
									for (int z = 0; z < (int)strlen(str2); z++)
										printf("%X", str2[z]);
									break;
								}
							}
						}
					}
				}
			}
			else {
				if (length == 0) {
					int l = 0;
					int pleng = 0;
					for (pleng = i; pleng < token_line; pleng++)
					{
						if (strcmp(token_table[pleng]->operator, "BYTE") == 0) {
							char* str, * str2;
							strcpy(buff, token_table[pleng]->operand[0]);
							str = strtok(buff, "'");
							str2 = strtok(NULL, "'");
							l += (int)(strlen(str2) / 2);
						}
						else if (strcmp(token_table[pleng]->operator, "WORD") == 0) {
							l += 3;
						}
						else if (strcmp(token_table[pleng]->operator, "END") == 0) {
							char* str1, * str2;
							int lix = 0;
							for (int el = locctr; el < token_line; el++) {
								if (lix >= literal_line)
									break;
								if (token_table[el]->operand[0][0] == '=') {
									strcpy(buff, token_table[el]->operand[0]);
									str1 = strtok(buff, "'");
									str2 = strtok(NULL, "'");
									for (int t = lix; t < literal_line; t++) {
										if (strcmp(literal_table[t].literal, str2) == 0) {
											lix = t + 1;
											break;
										}
									}
									if (strcmp(str1, "=X") == 0)
										l += (int)(strlen(str2) / 2);
									else
										l += 3;
								}
							}
						}
						else if (search_opcode(token_table[pleng]->operator) < 0)
							break;
						else if (l + inst_table[search_opcode(token_table[pleng]->operator)]->format <= 30) {
							if (token_table[pleng]->operator[0] == '+')
								l++;
							l += inst_table[search_opcode(token_table[pleng]->operator)]->format;
						}
						else if (strcmp(token_table[i]->operator, "RESB") == 0 || strcmp(token_table[i]->operator, "RESW") == 0) {
							continue;
						}
						else
							break;
					}
					pleng -= 1;
					printf("T%06X%02X", (add[i] - inst_table[search_opcode(token_table[i]->operator)]->format), l);
				}
				if (op >= 0) {
					if (inst_table[op]->format == 1) {
						printf("%X", code[i]);
						length += 2;
					}
					else if (inst_table[op]->format == 2) {
						printf("%4X", code[i]);
						length += 4;
					}
					else if (token_table[i]->operator[0] == '+') {
						printf("%08X", code[i]);
						length += 8;
					}
					else {
						printf("%06X", code[i]);
						length += 6;
					}
					if (search_opcode(token_table[i + 1]->operator) >= 0)
						if (length + inst_table[search_opcode(token_table[i + 1]->operator)]->format > 60) {
							printf("\n");
							length = 0;
						}
				}
				else {
					if (strcmp(token_table[i]->operator, "BYTE") == 0) {
						char* str;
						strcpy(buff, token_table[i]->operand[0]);
						str = strtok(buff, "'");
						str = strtok(NULL, "'");
						printf("%s", str);
						length += strlen(str);
					}
					else if (strcmp(token_table[i]->operator, "WORD") == 0) {
						printf("%06X", code[i]);
						length += 6;
					}
					else if (strcmp(token_table[i]->operator, "END") == 0) {
						char* str1, * str2;
						int lix = 0;
						for (int el = locctr; el < token_line; el++) {
							if (lix >= literal_line)
								break;
							if (token_table[el]->operand[0][0] == '=') {
								strcpy(buff, token_table[el]->operand[0]);
								str1 = strtok(buff, "'");
								str2 = strtok(NULL, "'");
								for (int t = lix; t < literal_line; t++) {
									if (strcmp(literal_table[t].literal, str2) == 0) {
										lix = t + 1;
										break;
									}
								}
								if (strcmp(str1, "=X") == 0) {
									printf("%s", str2);
									length += (int)(strlen(str2) / 2);
								}
								else {
									printf("%06X", code[i]);
									length += 3;
								}
							}
						}
					}
				}

			}

		}
		if (length < 60)
			printf("\n");
		for (int q = locctr; q < token_line; q++) {
			if (strcmp(token_table[q]->operator, "CSECT") == 0 || strcmp(token_table[q]->operator, "LTORG") == 0)
				break;
			if (strcmp(token_table[q]->operator, "EXTREF") == 0 || strcmp(token_table[q]->operator, "EXTDEF") == 0)
				continue;
			if (strcmp(token_table[extr]->operand[0], token_table[q]->operand[0]) == 0)
				printf("M%06X05+%s\n", (add[q] - inst_table[search_opcode(token_table[q]->operator)]->format), token_table[extr]->operand[0]);
			else if (token_table[extr]->operand[1] == NULL)
				continue;
			else if (strcmp(token_table[extr]->operand[1], token_table[q]->operand[0]) == 0)
				printf("M%06X05+%s\n", (add[q] - inst_table[search_opcode(token_table[q]->operator)]->format), token_table[extr]->operand[1]);
			else if (token_table[extr]->operand[2] == NULL)
				continue;
			else if (strcmp(token_table[extr]->operand[2], token_table[q]->operand[0]) == 0)
				printf("M%06X05+%s\n", (add[q] - inst_table[search_opcode(token_table[q]->operator)]->format), token_table[extr]->operand[2]);
		}
		printf("E\n");
	}
}