/*
* ȭ�ϸ� : my_assembler.c
* ��  �� : �� ���α׷��� SIC/XE �ӽ��� ���� ������ Assembler ���α׷��� ���η�ƾ����,
* �Էµ� ������ �ڵ� ��, ��ɾ �ش��ϴ� OPCODE�� ã�� ����Ѵ�.
*
*/

/*
*
* ���α׷��� ����� �����Ѵ�.
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
* ���� : ����ڷ� ���� ����� ������ �޾Ƽ� ��ɾ��� OPCODE�� ã�� ����Ѵ�.
* �Ű� : ���� ����, ����� ����
* ��ȯ : ���� = 0, ���� = < 0
* ���� : ���� ����� ���α׷��� ����Ʈ ������ �����ϴ� ��ƾ�� ������ �ʾҴ�.
*		   ���� �߰������� �������� �ʴ´�.
* ----------------------------------------------------------------------------------
*/
int main(int args, char *arg[])
{
	if (init_my_assembler() < 0)
	{
		printf("init_my_assembler: ���α׷� �ʱ�ȭ�� ���� �߽��ϴ�.\n");
		return -1;
	}


	if (assem_pass1() < 0) {
		printf("assem_pass1: �н�1 �������� �����Ͽ����ϴ�.  \n"); //�н�1 �ȿ��� �н�2�� ȣ��( ���� ���ε��� ) 
		return -1;
	}
	
	//make_objectcode_output("output.txt");
	

	return 0;
}

/* ----------------------------------------------------------------------------------
* ���� : ���α׷� �ʱ�ȭ�� ���� �ڷᱸ�� ���� �� ������ �д� �Լ��̴�.
* �Ű� : ����
* ��ȯ : �������� = 0 , ���� �߻� = -1
* ���� : ������ ��ɾ� ���̺��� ���ο� �������� �ʰ� ������ �����ϰ� �ϱ�
*		   ���ؼ� ���� ������ �����Ͽ� ���α׷� �ʱ�ȭ�� ���� ������ �о� �� �� �ֵ���
*		   �����Ͽ���.
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
* ���� : �ӽ��� ���� ��� �ڵ��� ������ �о� ���� ��� ���̺�(inst_table)��
*        �����ϴ� �Լ��̴�.
* �Ű� : ���� ��� ����
* ��ȯ : �������� = 0 , ���� < 0
* ���� : ���� ������� ������ �����Ӱ� �����Ѵ�. ���ô� ������ ����.
*
*	===============================================================================
*		   | �̸� | ���� | ���� �ڵ� | ���۷����� ���� | NULL|
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
* ���� : ����� �� �ҽ��ڵ带 �о���� �Լ��̴�.
* �Ű� : ������� �ҽ����ϸ�
* ��ȯ : �������� = 0 , ���� < 0
* ���� :
*
* ----------------------------------------------------------------------------------
*/
int init_input_file(char *input_file)
{
	FILE * file;
	int errno;
	file = fopen(input_file, "r");
	char buff[300]; //������ �ִ��� �������� ����
	int len; //���۰� ������ŭ�� ���̰� �ƴϸ� �޸𸮸� �Ƴ��� ���ؼ� �� ���� ���̸�ŭ�� ������ �Ҵ��ؾ��Ѵ�
			 //�� ���̸� �����ϱ����� ���� len
	for (int i = 0; i < MAX_LINES; ++i)
	{
		if (NULL == fgets(buff, 300, file))
			break;
		if (buff[0] == '\n' || buff[0] == '.') //�ƹ��͵� ���� ���๮�ڰų� �ּ������̸� �ȹ���
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
		len = strlen(buff); //���ۿ� ���� �޾ƿ� ���̸� ���Ϲ޾�
		input_data[i] = (char *)malloc((sizeof(char)*len + 1)); //�� ���̺��� 1���Ѹ�ŭ ������ �Ҵ��� ��
		strcpy(input_data[i], buff); //�������ش�.

		++line_num; //�� ���γѹ� ����!
	}

	/* add your code here */

	return errno;
}

/* ----------------------------------------------------------------------------------
* ���� : �ҽ� �ڵ带 �о�� ��ū������ �м��ϰ� ��ū ���̺��� �ۼ��ϴ� �Լ��̴�.
*        �н� 1�� ���� ȣ��ȴ�.
* �Ű� : �ҽ��ڵ��� ���ι�ȣ
* ��ȯ : �������� = 0 , ���� < 0
* ���� : my_assembler ���α׷������� ���δ����� ��ū �� ������Ʈ ������ �ϰ� �ִ�.
* ----------------------------------------------------------------------------------
*/
int token_parsing(int index)
{
	char check;

	char buff[4][50] = { NULL, };


	token_table[index] = (token *)malloc(sizeof(token)); //���ٿ� �ش��ϴ� ��ū����ü ����
	token_table[index]->objectcode = -1;
	//�ʱ�ȭ���ִ°���//
	token_table[index]->current_locate = -1;
	token_table[index]->pc = -1;
	token_table[index]->label = NULL; //����Ʈ
	for (int i = 0; i < MAX_OPERAND; ++i) //����Ʈ
		token_table[index]->operand[i] = NULL;
	token_table[index]->operator_ = NULL;
	token_table[index]->comment = NULL; //����Ʈ
	//////////////////////
	sscanf(input_data[index], "%c", &check); //�� ���� �� �� �ѱ��ڸ� �޾ƺ���.

	if ((check == '\t') || (check == ' '))// label�� ���°��
		sscanf(input_data[index], "%s %s %s",
			buff[1], buff[2], buff[3]);
	else
		sscanf(input_data[index], "%s %s %s %s", // label�� �ִ°��
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
		char *ptr = strtok(buff[2], ",");      // " " ���� ���ڸ� �������� ���ڿ��� �ڸ�, ������ ��ȯ
		int i = 0;
		while (ptr != NULL)               // �ڸ� ���ڿ��� ������ ���� ������ �ݺ�
		{
			char *tmp = (char *)malloc(strlen(ptr) + 1);         // �ڸ� ���ڿ� ���
			strcpy(tmp, ptr);
			token_table[index]->operand[i++] = tmp;
			ptr = strtok(NULL, ",");      // ���� ���ڿ��� �߶� �����͸� ��ȯ
			
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
* ���� : �Է� ���ڿ��� ���� �ڵ������� �˻��ϴ� �Լ��̴�.
* �Ű� : ��ū ������ ���е� ���ڿ�
* ��ȯ : �������� = ���� ���̺� �ε���, ���� < 0
* ���� :
*
* ----------------------------------------------------------------------------------
*/
int search_opcode(char *str)
{
	int found_index = -1;
	
	for (int i = 0; i < inst_index; ++i)
	{
		if (strcmp(str, inst_table[i]->operator_) == 0) //OPTAB�� ������
		{
			found_index = i;
			break;
		}
	}
	
	return found_index;/* add your code here */

}

/* ----------------------------------------------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ ��ɾ� ���� OPCODE�� ��ϵ� ǥ(���� 4��) �̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*        ���� ���� 4�������� ���̴� �Լ��̹Ƿ� ������ ������Ʈ������ ������ �ʴ´�.
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
* ------------------------- ���� ������Ʈ���� ����� �Լ� --------------------------*
* --------------------------------------------------------------------------------*/


/* ----------------------------------------------------------------------------------
* ���� : ����� �ڵ带 ���� �н�1������ �����ϴ� �Լ��̴�.
*		   �н�1������..
*		   1. ���α׷� �ҽ��� ��ĵ�Ͽ� �ش��ϴ� ��ū������ �и��Ͽ� ���α׷� ���κ� ��ū
*		   ���̺��� �����Ѵ�.
*
* �Ű� : ����
* ��ȯ : ���� ���� = 0 , ���� = < 0
* ���� : ���� �ʱ� ���������� ������ ���� �˻縦 ���� �ʰ� �Ѿ �����̴�.
*	  ���� ������ ���� �˻� ��ƾ�� �߰��ؾ� �Ѵ�.
*
* -----------------------------------------------------------------------------------
*/
int startaddress; //���α׷� �����ּ�
int section_start_line = 0; //�� ���� ������ ���۶����� �������� ��´�.
int sym_line = 0; //�ɺ����̺��� ���� ����
static int assem_pass1(void)
{
	for (int i = 0; i < line_num; ++i) //��ū�Ľ�
	{
		token_parsing(i);
	}
	int cut_line_index = 0; //���� ������ ���۶���
	token *tmp;
	tmp = token_table[0];
	if (strcmp(tmp->operator_, "START") == 0) //START��
	{
		sscanf(tmp->operand[0], "%x", &startaddress);
		locctr = startaddress;
		tmp->current_locate = locctr;
	}
	int locator_up_value;
	for (int i = 1; i < line_num; ++i) //1��°�ٺ���
	{
		tmp = token_table[i];
		if (strcmp(tmp->operator_, "CSECT") == 0) //���ο�����
		{
			
			
			if(assem_pass2(section_start_line, i - 1,locctr)==-1)
				return -1; //�������� pass2����
			csect(i, tmp); //�ʱ�ȭ�۾����� ��Ƶ� �Լ�
			continue;
		}
		if(strcmp(tmp->operator_, "END") == 0)
		{
			ltorg(i, section_start_line);
			if (assem_pass2(section_start_line, i - 1,locctr) == -1)
				return -1; //�������� pass2����
			csect(i, tmp); //�ʱ�ȭ�۾����� ��Ƶ� �Լ�
			continue;
		}
		if (strcmp(tmp->operator_, "EQU") == 0)//�������ʹ� �����̾���, ������ SYMTAB�˻縸
		{
			locator_up_value = 0;
			for (int j = 0; j < sym_line; ++j)
			{
				if (strcmp(tmp->label, sym_table[j].symbol) == 0) //�̹� �ִ��� �ϴٵ�����
				{
					printf("EQU���̺��ߺ�\n");
					return -1;
				}
			}
			strcpy(sym_table[sym_line].symbol, tmp->label); //SYMTAB�� �߰�
			sym_table[sym_line].addr = equate_check(tmp->operand[0]);
			tmp->current_locate = sym_table[sym_line].addr;
			//printf("%s %x\n", sym_table[sym_line].symbol, sym_table[sym_line].addr);
			++sym_line;

		}
		else if (tmp->label != NULL)
		{
			for (int j = 0; j < sym_line; ++j)
			{

				if (strcmp(tmp->label, sym_table[j].symbol) == 0) //�̹� �ִ��� �ϴٵ�����
				{
					printf("�ߺ��� ���̺��");
					return -1;
				}
			}
			strcpy(sym_table[sym_line].symbol, tmp->label); //SYMTAB�� �߰�
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
		sym_table[k].symbol[0] = NULL; //�ɺ����̺� �� ��������

	}
	sym_line = 0;
	for (int k = section_start_line + 1; k < line_num; ++k) //�� �־��ִ°���..
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

		if (token_table[m]->operand[0][0] == '=') //=���� �����ϴ°� ������
		{

			int constantcheck = 0;
			for (int j = 0; j < sym_line; ++j)
			{
				if (strcmp(token_table[m]->operand[0], sym_table[j].symbol) == 0) //�̹� �ִ��� �ϴٵ�����
				{
					constantcheck = 1; //����ߺ��� ������ , �߰��� ���ϸ��
					break;
				}
			}
			if (constantcheck != 1)
			{
				strcpy(sym_table[sym_line].symbol, token_table[m]->operand[0]); //SYMTAB�� �߰�
				sym_table[sym_line].addr = locctr;

				//printf("%s %x\n", sym_table[sym_line].symbol, sym_table[sym_line].addr);
				++sym_line;
				locctr += calc_size(token_table[m]->operand[0] + 1);
			}


		}

	}

}
//EQU �� OPERAND������ 
int equate_check(char *str)
{

	if (str[0] == '*')
		return locctr;
	else//�ϴ� ���⸸ ����
	{
		int tmp1,tmp2;
		char *ptr = strtok(str, "-");      // "-" ���� ���ڸ� �������� ���ڿ��� �ڸ�, ������ ��ȯ
		int i = 0;
		if (ptr != NULL)
		{
			for (int i = 0; i < sym_line; ++i)
			{
				if (strcmp(sym_table[i].symbol, ptr) == 0) //�ɺ����̺��� ���� ã����
				{
					tmp1 = sym_table[i].addr;
					break;
				}
			}
		}
		ptr = strtok(NULL, "-");      // ���� ���ڿ��� �߶� �����͸� ��ȯ
		if (ptr != NULL)
		{
			for (int i = 0; i < sym_line; ++i)
			{
				if (strcmp(sym_table[i].symbol, ptr) == 0) //�ɺ����̺��� ���� ã����
				{
					tmp2 = sym_table[i].addr;
					break;
				}
			}
		}
		return tmp1-tmp2;
	}


}
// X�� C�� �����ϴ� ����� ����� ����ؼ� �������ִ� �Լ�
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
* ���� : ����� �ڵ带 ���� �ڵ�� �ٲٱ� ���� �н�2 ������ �����ϴ� �Լ��̴�.
*		   �н� 2������ ���α׷��� ����� �ٲٴ� �۾��� ���� ������ ����ȴ�.
*		   ������ ���� �۾��� ����Ǿ� ����.
*		   1. ������ �ش� ����� ��ɾ ����� �ٲٴ� �۾��� �����Ѵ�.
* �Ű� : ����
* ��ȯ : �������� = 0, �����߻� = < 0
* ���� :
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
	
	//��� ������ locate�� object code�� �����ϴ� ����
	token *extref=NULL;
	char buff[50];
	int objectcode;
	int opcode;
	int address;
	char nixbpe;
	token *tmp;
	for (int i = start; i <= finish; ++i)
	{
		objectcode = 0; //�ϴ� 0���μ���
		tmp = token_table[i];
		if (strcmp(tmp->operator_, "EXTREF") == 0) //EXTREF�� operand�� ����Ǿ��ִ� token unit�� ������ �����س���
		{
			extref = tmp;
		}
		int searched_index;
		if (tmp->operator_[0] == '+')
			searched_index = search_opcode(tmp->operator_ + 1);
		else
			searched_index = search_opcode(tmp->operator_);
		if (searched_index != -1) //inst_table���� ã������
		{
			address=0;
			objectcode = 0;
			nixbpe = 0;
			opcode = 0;
			sscanf(inst_table[searched_index]->opcode, "%x", &opcode);
			if (inst_table[searched_index]->format != 2) //3,4������ ���
			{
				if (tmp->operand[0][0] == '#') //immediate
					nixbpe = nixbpe | 1 << 4;
				else if (tmp->operand[0][0] == '@') //indirect
					nixbpe = nixbpe | 1 << 5;
				else
					nixbpe = nixbpe | 3 << 4;


				if (tmp->operand[1] != NULL && strcmp(tmp->operand[1], "X") == 0) //X��������
					nixbpe = nixbpe | 1 << 3;
				//if BASE����
				nixbpe = nixbpe | 1 << 1; //pc relative �� �������ϴ� ����
				if (tmp->operand[0][0] == '#'|| (strcmp(tmp->operator_, "RSUB") == 0)) //pc�κ� �ٽ� ������
				{
					nixbpe = nixbpe & (~(1 << 1));
					
				}
				if (strcmp(tmp->operator_, "RSUB") == 0)
				{
					objectcode = opcode << 16 | nixbpe << 12;
					tmp->objectcode = objectcode;

					continue;
				}
				/////ADDRESS���ϴ°���////////
				
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
				if (symbol_point != -1) //���ǿ� ������
				{
					address = sym_table[symbol_point].addr-tmp->pc;
					valid_label_check_flag = 1;
				}
				else//�ϴ� ���ǿ�������
				{
					if (str_tmp[0] >= '0'&&str_tmp[0] <= '9') //#�ڰ� ���ڸ�
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
							
							if (strcmp(tmp->operand[0], extref->operand[p]) == 0) // EXTREF�̸�
								valid_label_check_flag = 1;
						}
					}
				}
				if (valid_label_check_flag == 0) //���ǿ������� EXTREF�� �ƴѰ��
				{
					printf("���ǿ������� EXTREF�� �ƴѿ����߻�\n");
					return -1; //����
				}
				//////�����ڵ常��°���///////
				if (tmp->operator_[0] == '+') //4�����ϰ��
				{
					nixbpe = nixbpe | 1;
					nixbpe = nixbpe & 0x39;
					//address�� �����ϰ�츦 ����ؼ� ����
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
				if (tmp->operand[1] != NULL) //2���Ŀ� operand �ΰ��� ���
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
	//���� �Ѽ����� ��������ϱ�
	//��µ��ϰ�
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
	for (int t = 0; t < sizeof(register_number); ++t) //�������� ��ȣ�� ã���ִ� �Լ�
	{
		if (register_number[t] == c)
		{
			return t;
		}
	}
}
/* ----------------------------------------------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ object code (������Ʈ 1��) �̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*
* -----------------------------------------------------------------------------------
*/
void make_objectcode_output(int program_len, int start, int finish, char *filename)
{
	int one_programlength;
	FILE *fp = fopen(filename, "a+");
	fputc('H', fp); //���
	fprintf(fp, "%-6s", token_table[start]->label);
	fprintf(fp, "%6s", "000000");
	fprintf(fp, "%06x\n", program_len);
	token *tmp;
	int code_start_line=start+1;
	for (int i = start;i<start+3; ++i) //�ִ� �ι�
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
		if (token_table[i]->objectcode != -1) //ob������������
		{
			fputc('T', fp);
			fprintf(fp, "%06x", token_table[i]->current_locate);
			fprintf(fp, "%2s", "00"); //�ϴ� ���� 00���� ���
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
		fprintf(fp, "%2s", "00"); //�ϴ� ���� 00���� ���
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