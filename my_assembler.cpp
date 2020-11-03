/*
* 화일명 : my_assembler.c
* 설  명 : 이 프로그램은 SIC/XE 머신을 위한 간단한 Assembler 프로그램의 메인루틴으로,
* 입력된 파일의 코드 중, 명령어에 해당하는 OPCODE를 찾아 출력한다.
*
*/

/*
*
* 프로그램의 헤더를 정의한다.
*
*/
#pragma once
#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include<ctype.h>
#include "my_assembler.h"

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


	if (assem_pass1() < 0) {
		printf("assem_pass1: 패스1 과정에서 실패하였습니다.  \n"); //패스1 안에서 패스2를 호출( 파일 따로따로 ) 
		return -1;
	}
	
	//make_objectcode_output("output.txt");
	

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

	if ((result = init_inst_file("inst.data")) < 0)
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
	FILE * file;
	int errno;
	file = fopen(inst_file, "r");

	for (int i = 0; i < MAX_INST; ++i)
	{
		inst_table[i] = (inst *)malloc(sizeof(inst));
		if (3 != fscanf(file, "%s %d %s", inst_table[i]->operator_, &inst_table[i]->format, &inst_table[i]->opcode))
			break;
		else
			inst_index++;

	}

	/* add your code here */

	return errno;
}

/* ----------------------------------------------------------------------------------
* 설명 : 어셈블리 할 소스코드를 읽어오는 함수이다.
* 매계 : 어셈블리할 소스파일명
* 반환 : 정상종료 = 0 , 에러 < 0
* 주의 :
*
* ----------------------------------------------------------------------------------
*/
int init_input_file(char *input_file)
{
	FILE * file;
	int errno;
	file = fopen(input_file, "r");
	char buff[300]; //한줄을 최대한 담을만한 버퍼
	int len; //버퍼가 꽉찰만큼의 길이가 아니면 메모리를 아끼기 위해서 딱 들어온 길이만큼의 공간을 할당해야한다
			 //그 길이를 저장하기위한 변수 len
	for (int i = 0; i < MAX_LINES; ++i)
	{
		if (NULL == fgets(buff, 300, file))
			break;
		if (buff[0] == '\n' || buff[0] == '.') //아무것도 없는 개행문자거나 주석라인이면 안받음
		{
			--i;
			continue;
		}
		/*if (EOF == fgetc(file))
		{
			input_data[i] = (char *)malloc(strlen(buff) + 1);
			strcpy(input_data[i], buff);
			++line_num;
			break;
		}*/
		buff[strlen(buff) - 1] = '\0';
		len = strlen(buff); //버퍼에 한줄 받아온 길이를 리턴받아
		input_data[i] = (char *)malloc((sizeof(char)*len + 1)); //그 길이보다 1더한만큼 공간을 할당한 후
		strcpy(input_data[i], buff); //복사해준다.

		++line_num; //총 라인넘버 증가!
	}

	/* add your code here */

	return errno;
}

/* ----------------------------------------------------------------------------------
* 설명 : 소스 코드를 읽어와 토큰단위로 분석하고 토큰 테이블을 작성하는 함수이다.
*        패스 1로 부터 호출된다.
* 매계 : 소스코드의 라인번호
* 반환 : 정상종료 = 0 , 에러 < 0
* 주의 : my_assembler 프로그램에서는 라인단위로 토큰 및 오브젝트 관리를 하고 있다.
* ----------------------------------------------------------------------------------
*/
int token_parsing(int index)
{
	char check;

	char buff[4][50] = { NULL, };


	token_table[index] = (token *)malloc(sizeof(token)); //그줄에 해당하는 토큰구조체 생성
	token_table[index]->objectcode = -1;
	//초기화해주는과정//
	token_table[index]->current_locate = -1;
	token_table[index]->pc = -1;
	token_table[index]->label = NULL; //디폴트
	for (int i = 0; i < MAX_OPERAND; ++i) //디폴트
		token_table[index]->operand[i] = NULL;
	token_table[index]->operator_ = NULL;
	token_table[index]->comment = NULL; //디폴트
	//////////////////////
	sscanf(input_data[index], "%c", &check); //한 줄의 맨 앞 한글자를 받아본다.

	if ((check == '\t') || (check == ' '))// label이 없는경우
		sscanf(input_data[index], "%s %s %s",
			buff[1], buff[2], buff[3]);
	else
		sscanf(input_data[index], "%s %s %s %s", // label이 있는경우
			buff[0], buff[1], buff[2], buff[3]);
	if (buff[0][0] != NULL)

	{
		token_table[index]->label =
			(char *)malloc(sizeof(buff[0] + 1));
		sprintf(token_table[index]->label, buff[0]);
	}

	token_table[index]->operator_ =
		(char *)malloc(sizeof(buff[1] + 1));
	sprintf(token_table[index]->operator_, buff[1]);


	if (buff[2][0] != NULL)
	{
		char *ptr = strtok(buff[2], ",");      // " " 공백 문자를 기준으로 문자열을 자름, 포인터 반환
		int i = 0;
		while (ptr != NULL)               // 자른 문자열이 나오지 않을 때까지 반복
		{
			char *tmp = (char *)malloc(strlen(ptr) + 1);         // 자른 문자열 출력
			strcpy(tmp, ptr);
			token_table[index]->operand[i++] = tmp;
			ptr = strtok(NULL, ",");      // 다음 문자열을 잘라서 포인터를 반환
			
		}
		
	}
	if (buff[3][0] != NULL)
	{
		token_table[index]->comment =
			(char *)malloc(strlen(buff[3]) + 1);
		sprintf(token_table[index]->comment, "%s", buff[3]);
	}
	return 1;/* add your code here */

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
	int found_index = -1;
	
	for (int i = 0; i < inst_index; ++i)
	{
		if (strcmp(str, inst_table[i]->operator_) == 0) //OPTAB에 있으면
		{
			found_index = i;
			break;
		}
	}
	
	return found_index;/* add your code here */

}

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 명령어 옆에 OPCODE가 기록된 표(과제 4번) 이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*        또한 과제 4번에서만 쓰이는 함수이므로 이후의 프로젝트에서는 사용되지 않는다.
* -----------------------------------------------------------------------------------
*/
void make_opcode_output(int start,int finish,int sym_line)
{
	token *tmp;
	for (int i = start; i <= finish; ++i)
	{

		tmp = token_table[i];
		if (strcmp(tmp->operator_, "LTORG") == 0|| strcmp(tmp->operator_, "END") == 0)
		{
			if (strcmp(tmp->operator_, "LTORG") == 0)
			{
				printf("\t\t%6s\n", tmp->label);
				for (int k = 0; k <sym_line; ++k)
				{
					if (sym_table[k].symbol[0] == '=')
						printf("%04x\t*\6s", sym_table[k].addr,sym_table[k].symbol);
				}
			}
			continue;
		}
		if (tmp->current_locate != -1)
			printf("%04x\t", tmp->current_locate);
		else
			printf("\t");
		if (tmp->label != NULL)
			printf("%6s\t", tmp->label);
		else
			printf("\t");
		printf("%6s\t", tmp->operator_);
	
		for (int j = 0; j < MAX_OPERAND; ++j)
		{
			
			if (token_table[i]->operand[j] != NULL)
				printf("%s", token_table[i]->operand[j]);
			if (token_table[i]->operand[j + 1] != NULL && j<2)
				printf(",");
		}
		printf("\t");
		if (token_table[i]->objectcode != -1)
			printf("%x", token_table[i]->objectcode);//,byte_calculator(token_table[i]->objectcode));
		printf("\n");
	}
}
/* --------------------------------------------------------------------------------*
* ------------------------- 추후 프로젝트에서 사용할 함수 --------------------------*
* --------------------------------------------------------------------------------*/


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
int startaddress; //프로그램 시작주소
int section_start_line = 0; //각 개별 파일의 시작라인을 순간순간 담는다.
int sym_line = 0; //심볼테이블의 원소 개수
static int assem_pass1(void)
{
	for (int i = 0; i < line_num; ++i) //토큰파싱
	{
		token_parsing(i);
	}
	int cut_line_index = 0; //현재 파일의 시작라인
	token *tmp;
	tmp = token_table[0];
	if (strcmp(tmp->operator_, "START") == 0) //START면
	{
		sscanf(tmp->operand[0], "%x", &startaddress);
		locctr = startaddress;
		tmp->current_locate = locctr;
	}
	int locator_up_value;
	for (int i = 1; i < line_num; ++i) //1번째줄부터
	{
		tmp = token_table[i];
		if (strcmp(tmp->operator_, "CSECT") == 0) //새로운파일
		{
			
			
			if(assem_pass2(section_start_line, i - 1,locctr)==-1)
				return -1; //이전까지 pass2실행
			csect(i, tmp); //초기화작업들을 모아둔 함수
			continue;
		}
		if(strcmp(tmp->operator_, "END") == 0)
		{
			ltorg(i, section_start_line);
			if (assem_pass2(section_start_line, i - 1,locctr) == -1)
				return -1; //이전까지 pass2실행
			csect(i, tmp); //초기화작업들을 모아둔 함수
			continue;
		}
		if (strcmp(tmp->operator_, "EQU") == 0)//로케이터는 변함이없음, 오로지 SYMTAB검사만
		{
			locator_up_value = 0;
			for (int j = 0; j < sym_line; ++j)
			{
				if (strcmp(tmp->label, sym_table[j].symbol) == 0) //이미 있는지 싹다뒤져봄
				{
					printf("EQU레이블중복\n");
					return -1;
				}
			}
			strcpy(sym_table[sym_line].symbol, tmp->label); //SYMTAB에 추가
			sym_table[sym_line].addr = equate_check(tmp->operand[0]);
			tmp->current_locate = sym_table[sym_line].addr;
			//printf("%s %x\n", sym_table[sym_line].symbol, sym_table[sym_line].addr);
			++sym_line;

		}
		else if (tmp->label != NULL)
		{
			for (int j = 0; j < sym_line; ++j)
			{

				if (strcmp(tmp->label, sym_table[j].symbol) == 0) //이미 있는지 싹다뒤져봄
				{
					printf("중복된 레이블명");
					return -1;
				}
			}
			strcpy(sym_table[sym_line].symbol, tmp->label); //SYMTAB에 추가
			sym_table[sym_line].addr = locctr;
			tmp->current_locate = locctr;

			//printf("%s %x\n", sym_table[sym_line].symbol, sym_table[sym_line].addr);
			++sym_line;
		}
	
		int point = search_opcode(tmp->operator_);
		if (tmp->operator_[0] == '+')
		{
			tmp->current_locate = locctr;
			locator_up_value = 4;
			

		}
		else if (point != -1)
		{
			tmp->current_locate = locctr;
			locator_up_value = inst_table[point]->format;
			
		}
		else if (strcmp(tmp->operator_, "WORD") == 0)
		{
			tmp->current_locate = locctr;
			locator_up_value = 3;
			tmp->objectcode = 0; 
		}
		else if (strcmp(tmp->operator_, "RESW") == 0)
		{
			tmp->current_locate = locctr;
			locator_up_value = 3 * atoi(tmp->operand[0]);
		}
		else if (strcmp(tmp->operator_, "RESB") == 0)
		{
			tmp->current_locate = locctr;
			locator_up_value = atoi(tmp->operand[0]);
		}
		else if (strcmp(tmp->operator_, "BYTE") == 0)
		{
			tmp->current_locate = locctr;
			locator_up_value = calc_size(tmp->operand[0]);
			char buffff[10] = { NULL, };
			strncpy(buffff, tmp->operand[0] + 2, locator_up_value*2);
			tmp->objectcode=strtoul(buffff, NULL, 16);
			
		}
		else if (strcmp(tmp->operator_, "EXTDEF") == 0 || strcmp(tmp->operator_, "EXTREF") == 0)
		{
			locator_up_value = 0;
		}
		else if (strcmp(tmp->operator_, "LTORG")==0)
		{
			ltorg(i, section_start_line);
			locator_up_value = 0;
		}
		locctr += locator_up_value;
		token_table[i]->pc = locctr;
		
		
		


	}
	return 0;

}
void csect(int i,token *tmp)
{
	section_start_line = i;
	locctr = 0;
	tmp->current_locate = locctr;
	
	for (int k = 0; k < sym_line; ++k)
	{
		sym_table[k].symbol[0] = NULL; //심볼테이블 다 지워버림

	}
	sym_line = 0;
	for (int k = section_start_line + 1; k < line_num; ++k) //라벨 넣어주는과정..
	{
		if (strcmp(token_table[k]->operator_, "EXTREF") == 0
			|| strcmp(token_table[k]->operator_, "EXTDEF") == 0)
			continue;
		else
		{
			token_table[k]->label = tmp->label;
			break;
		}
	}
}
void ltorg(int i,int section_start_line )
{
	for (int m = section_start_line + 1; m < i; ++m)
	{

		if (token_table[m]->operand[0][0] == '=') //=으로 시작하는게 있으면
		{

			int constantcheck = 0;
			for (int j = 0; j < sym_line; ++j)
			{
				if (strcmp(token_table[m]->operand[0], sym_table[j].symbol) == 0) //이미 있는지 싹다뒤져봄
				{
					constantcheck = 1; //상수중복은 괜찮음 , 추가만 안하면됨
					break;
				}
			}
			if (constantcheck != 1)
			{
				strcpy(sym_table[sym_line].symbol, token_table[m]->operand[0]); //SYMTAB에 추가
				sym_table[sym_line].addr = locctr;

				//printf("%s %x\n", sym_table[sym_line].symbol, sym_table[sym_line].addr);
				++sym_line;
				locctr += calc_size(token_table[m]->operand[0] + 1);
			}


		}

	}

}
//EQU 의 OPERAND값으로 
int equate_check(char *str)
{

	if (str[0] == '*')
		return locctr;
	else//일단 빼기만 구현
	{
		int tmp1,tmp2;
		char *ptr = strtok(str, "-");      // "-" 공백 문자를 기준으로 문자열을 자름, 포인터 반환
		int i = 0;
		if (ptr != NULL)
		{
			for (int i = 0; i < sym_line; ++i)
			{
				if (strcmp(sym_table[i].symbol, ptr) == 0) //심볼테이블에서 값을 찾아줌
				{
					tmp1 = sym_table[i].addr;
					break;
				}
			}
		}
		ptr = strtok(NULL, "-");      // 다음 문자열을 잘라서 포인터를 반환
		if (ptr != NULL)
		{
			for (int i = 0; i < sym_line; ++i)
			{
				if (strcmp(sym_table[i].symbol, ptr) == 0) //심볼테이블에서 값을 찾아줌
				{
					tmp2 = sym_table[i].addr;
					break;
				}
			}
		}
		return tmp1-tmp2;
	}


}
// X나 C로 시작하는 상수의 사이즈를 계산해서 리턴해주는 함수
int calc_size(char *str)
{
	float size = 0;
	if (str[0] == '=')
		str += 1;
	if (str[0] == 'C')
		for (int i = 1; i < strlen(str); ++i)
		{
			if (str[i] != 39)
				size++;
		}
	else
		for (int i = 1; i < strlen(str); ++i)
		{
			if (str[i] != 39)
				size+=0.5;
		}


	
	return size;
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
int file_cnt = 0;
#define MAXFILE 5
int start_indexof_each_file[MAXFILE];
int finish_indexof_each_file[MAXFILE];

static int assem_pass2(int start, int finish,int len)
{
	start_indexof_each_file[file_cnt]=start;
	finish_indexof_each_file[file_cnt] = finish;
	file_cnt++;
	
	//모든 라인의 locate와 object code를 생성하는 과정
	token *extref=NULL;
	char buff[50];
	int objectcode;
	int opcode;
	int address;
	char nixbpe;
	token *tmp;
	for (int i = start; i <= finish; ++i)
	{
		objectcode = 0; //일단 0으로세팅
		tmp = token_table[i];
		if (strcmp(tmp->operator_, "EXTREF") == 0) //EXTREF의 operand가 저장되어있는 token unit을 통으로 저장해놓음
		{
			extref = tmp;
		}
		int searched_index;
		if (tmp->operator_[0] == '+')
			searched_index = search_opcode(tmp->operator_ + 1);
		else
			searched_index = search_opcode(tmp->operator_);
		if (searched_index != -1) //inst_table에서 찾았으면
		{
			address=0;
			objectcode = 0;
			nixbpe = 0;
			opcode = 0;
			sscanf(inst_table[searched_index]->opcode, "%x", &opcode);
			if (inst_table[searched_index]->format != 2) //3,4형식의 경우
			{
				if (tmp->operand[0][0] == '#') //immediate
					nixbpe = nixbpe | 1 << 4;
				else if (tmp->operand[0][0] == '@') //indirect
					nixbpe = nixbpe | 1 << 5;
				else
					nixbpe = nixbpe | 3 << 4;


				if (tmp->operand[1] != NULL && strcmp(tmp->operand[1], "X") == 0) //X레지참조
					nixbpe = nixbpe | 1 << 3;
				//if BASE구현
				nixbpe = nixbpe | 1 << 1; //pc relative 로 무조건일단 세팅
				if (tmp->operand[0][0] == '#'|| (strcmp(tmp->operator_, "RSUB") == 0)) //pc부분 다시 지워줌
				{
					nixbpe = nixbpe & (~(1 << 1));
					
				}
				if (strcmp(tmp->operator_, "RSUB") == 0)
				{
					objectcode = opcode << 16 | nixbpe << 12;
					tmp->objectcode = objectcode;

					continue;
				}
				/////ADDRESS구하는과정////////
				
				address=0;
				int symbol_point;
				int valid_label_check_flag=-0;
				char *str_tmp;
							
				if (tmp->operand[0][0] == '#' || tmp->operand[0][0] == '@')
				{
					str_tmp = tmp->operand[0] + 1;

				}
				else
					str_tmp = tmp->operand[0];
				symbol_point=search_symbol_table(str_tmp);
				if (symbol_point != -1) //심탭에 있으면
				{
					address = sym_table[symbol_point].addr-tmp->pc;
					valid_label_check_flag = 1;
				}
				else//일단 심탭에없으면
				{
					if (str_tmp[0] >= '0'&&str_tmp[0] <= '9') //#뒤가 숫자면
					{
						sscanf(str_tmp, "%d", &address);
						valid_label_check_flag = 1;
						
					}
					else 
					{

						for (int p = 0; p < MAX_OPERAND; ++p)
						{
							
							if (extref->operand[p]== NULL)
								break;
							
							if (strcmp(tmp->operand[0], extref->operand[p]) == 0) // EXTREF이면
								valid_label_check_flag = 1;
						}
					}
				}
				if (valid_label_check_flag == 0) //심탭에도없고 EXTREF도 아닌경우
				{
					printf("심탭에도없고 EXTREF도 아닌에러발생\n");
					return -1; //에러
				}
				//////앞의코드만드는과정///////
				if (tmp->operator_[0] == '+') //4형식일경우
				{
					nixbpe = nixbpe | 1;
					nixbpe = nixbpe & 0x39;
					//address가 음수일경우를 대비해서 수정
					address = address & 0x000FFFFF;
				}
				else
				{
					address = address & 0x00000FFF;
				}
				opcode = opcode << 4;
				objectcode = opcode | nixbpe;
				if (tmp->operator_[0] == '+')
				{
					objectcode = objectcode << 5 * 4;
				}
				else
				{
					objectcode = objectcode << 3 * 4;
					
				}

			}
			else
			{

				opcode = opcode << 8;
				objectcode = objectcode | opcode;
				char reg_num1 = 0, reg_num2 = 0;
				char reg_part = 0;
				reg_num1 = search_num_of_register(tmp->operand[0][0]);
				reg_part = reg_part | reg_num1;
				reg_part = reg_part << 4;
				if (tmp->operand[1] != NULL) //2형식에 operand 두개일 경우
				{
					reg_num2 = search_num_of_register(tmp->operand[1][0]);
					reg_part = reg_part | reg_num2;
				}

				objectcode = objectcode | reg_part;
			}
			objectcode = objectcode | address;
			
			tmp->objectcode = objectcode;


		}
		
	}
	//파일 한섹션을 만들었으니깐
	//출력도하고
	make_opcode_output(start,finish,sym_line);
	make_objectcode_output(len, start, finish, "output.txt");

	return 0;/* add your code here */

}
int search_symbol_table(char *str)
{
	int index = -1;
	for (int i = 0; i < sym_line; ++i)
		if (strcmp(str, sym_table[i].symbol) == 0)
		{
			index = i;
			break;
		}	
	return index;
}
int search_num_of_register(char c)
{
	for (int t = 0; t < sizeof(register_number); ++t) //레지스터 번호를 찾아주는 함수
	{
		if (register_number[t] == c)
		{
			return t;
		}
	}
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
void make_objectcode_output(int program_len, int start, int finish, char *filename)
{
	int one_programlength;
	FILE *fp = fopen(filename, "a+");
	fputc('H', fp); //헤더
	fprintf(fp, "%-6s", token_table[start]->label);
	fprintf(fp, "%6s", "000000");
	fprintf(fp, "%06x\n", program_len);
	token *tmp;
	int code_start_line=start+1;
	for (int i = start;i<start+3; ++i) //최대 두번
	{
		tmp = token_table[i];
		if (strcmp(tmp->operator_, "EXTDEF")==0)
		{
			fputc('D', fp);
			for (int j = 0; j < MAX_OPERAND; ++j)
			{
				if (tmp->operand[j] == NULL)
					break;
				
				fprintf(fp, "%6s", tmp->operand[j]);
				
				fprintf(fp, "%06x", sym_table[search_symbol_table(tmp->operand[j])].addr);
					
			}
			fputc('\n', fp);
			code_start_line++;
			
		}
		if (strcmp(tmp->operator_, "EXTREF") == 0)
		{
			fputc('R', fp);
			for (int j = 0; j < MAX_OPERAND; ++j)
			{
				if (tmp->operand[j] == NULL)
					break;

				fprintf(fp, "%6s", tmp->operand[j]);
								

			}
			fputc('\n', fp);
			code_start_line++;
		}
	} //R,D records
	
	int i = code_start_line;
	
	int textlength=0;
	while (i <= finish && strcmp(token_table[i]->operator_, "LTORG") != 0)
	{
		if (token_table[i]->objectcode != -1) //ob내용이있으면
		{
			fputc('T', fp);
			fprintf(fp, "%06x", token_table[i]->current_locate);
			fprintf(fp, "%2s", "00"); //일단 길이 00으로 써둠
			if (token_table[i]->objectcode == 0)
			{
				fprintf(fp, "%06x", token_table[i]->objectcode);
				textlength += 6;
			}
			else
			{
				fprintf(fp, "%x", token_table[i]->objectcode);
				textlength += byte_calculator(token_table[i]->objectcode);
			}
			
			while (1)
			{
				if (token_table[i + 1]->objectcode == -1 || ((textlength + byte_calculator(token_table[i + 1]->objectcode)) > 30))
				{
					
					fprintf(fp, "%x", textlength);
					fputc('\n', fp);
					textlength = 0;
					break;
				}
				else
				{
					++i;
					if (token_table[i]->objectcode == 0)
					{
						fprintf(fp, "%06x", token_table[i]->objectcode);
						textlength += 6;
					}
					else
					{
						fprintf(fp, "%x", token_table[i]->objectcode);
						textlength += byte_calculator(token_table[i]->objectcode);
					}
					
				}
			}
		}
		++i;
	}
	if (strcmp(token_table[i]->operator_, "LTORG") == 0||strcmp(token_table[i]->operator_,"END")==0)
	{
		fputc('T', fp);
		fprintf(fp, "%06x", token_table[i]->current_locate);
		fprintf(fp, "%2s", "00"); //일단 길이 00으로 써둠
		for (int k = 0; k < sym_line; ++k)
		{
			//
		}
	}
	fputc('\n',fp);
	fputc('\n', fp);
}
int byte_calculator(int x)
{
	int check = 0x000000FF;
	if ((check << 24 & x) != 0)
		return 4;
	else if ((check << 16 & x) != 0)
		return 3;
	else return 2;

}