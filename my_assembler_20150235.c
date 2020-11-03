/*
 * ȭ�ϸ� : my_assembler_00000000.c
 * ��  �� : �� ���α׷��� SIC/XE �ӽ��� ���� ������ Assembler ���α׷��� ���η�ƾ����,
 * �Էµ� ������ �ڵ� ��, ��ɾ �ش��ϴ� OPCODE�� ã�� ����Ѵ�.
 * ���� ������ ���Ǵ� ���ڿ� "00000000"���� �ڽ��� �й��� �����Ѵ�.
 */

 /*
  *
  * ���α׷��� ����� �����Ѵ�.
  *
  */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

  // ���ϸ��� "00000000"�� �ڽ��� �й����� ������ ��.
#include "my_assembler_20150235.h"


int add[MAX_LINES]; // �ּҸ� �����ϴ� ����
int code[MAX_LINES];// ������� ���� �ڵ� �����ϴ� ����

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

    if (assem_pass1() < 0)
    {
        printf("assem_pass1: �н�1 �������� �����Ͽ����ϴ�.  \n");
        return -1;
    }
    // make_opcode_output("output_00000000");

    make_symtab_output("symtab_20150235");
    make_literaltab_output("literaltab_20150235");
	make_symtab_output(NULL);			// NULL �Է� �� ���� Ȯ�ο� 
	make_literaltab_output(NULL);
    if (assem_pass2() < 0)
    {
        printf("assem_pass2: �н�2 �������� �����Ͽ����ϴ�.  \n");
        return -1;
    }
	make_objectcode_output(NULL);
	make_objectcode_output("output_20150235");

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

	if ((result = init_inst_file("inst.txt")) < 0)
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
	int errno;
    FILE* file = fopen(inst_file, "r");
    while (!feof(file)) {				// inst ���Ͽ��� ���پ� �Է� �޾Ƽ� inst_table�� ����
        inst_table[inst_index] = (inst*)malloc(sizeof(inst));
        fscanf(file, "%s %d %s %d", &inst_table[inst_index]->opName, &inst_table[inst_index]->format, &inst_table[inst_index]->opcode, &inst_table[inst_index]->numop);
        inst_index++;
    }
	return errno;
}

/* ----------------------------------------------------------------------------------
 * ���� : ����� �� �ҽ��ڵ带 �о� �ҽ��ڵ� ���̺�(input_data)�� �����ϴ� �Լ��̴�.
 * �Ű� : ������� �ҽ����ϸ�
 * ��ȯ : �������� = 0 , ���� < 0
 * ���� : ���δ����� �����Ѵ�.
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
        if (NULL == fgets(buff, 200, file)) // �ҽ��ڵ� ���� �޾ƿ�
            break;
        buff[strlen(buff) - 1] = '\0';
        if (buff[0] == '.')
            continue; 
        len = (int)strlen(buff);
        input_data[line_num] = (char*)malloc(sizeof(char) * len + 1);		// input_table�� �����ϱ� ���� ũ�� ���� �Ҵ�
        strcpy(input_data[line_num], buff);			// �Ҵ�� ������ �����Ͽ� �Է�                 
        line_num++;
    }
    return errno;
}

/* ----------------------------------------------------------------------------------
 * ���� : �ҽ� �ڵ带 �о�� ��ū������ �м��ϰ� ��ū ���̺��� �ۼ��ϴ� �Լ��̴�.
 *        �н� 1�� ���� ȣ��ȴ�.
 * �Ű� : �Ľ��� ���ϴ� ���ڿ�
 * ��ȯ : �������� = 0 , ���� < 0
 * ���� : my_assembler ���α׷������� ���δ����� ��ū �� ������Ʈ ������ �ϰ� �ִ�.
 * ----------------------------------------------------------------------------------
 */
int token_parsing(char *str)
{
	/* add your code here */
    int errno;
    token_table[token_line] = (token*)malloc(sizeof(token)); // token�� �Է��� ���ؼ� ���� �Ҵ�
    token_table[token_line]->nixbpe = 0x00; // �ʱ⿡�� ���� ���� �ʱ�ȭ
    for (int j = 0; j < MAX_OPERAND; j++)
        token_table[token_line]->operand[j] = NULL; //operand 3�� ���� �ʱ�ȭ
    if (str[0] == '\t' || str[0] == ' ') { // label�� ���� ���
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
    if (token_table[token_line]->operand[2] != NULL) {   //operand�� �������� ��� ������ �����ϱ� ���ؼ� ����
        token_table[token_line]->operand[0] = strtok(token_table[token_line]->operand[2], ",");
        token_table[token_line]->operand[1] = strtok(NULL, ",");            // ������ ������ NULL�� �����
        token_table[token_line]->operand[2] = strtok(NULL, "\n");
    }
    token_line++;
    return errno;
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
    if (str == NULL)
        return 0;
    if (str[0] == '+') {		// 4���Ŀ��� opcode�� Ȯ���� ����
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
static int assem_pass1(void)
{
    /* input_data�� ���ڿ��� ���پ� �Է� �޾Ƽ�
     * token_parsing()�� ȣ���Ͽ� token_unit�� ����
     */

    int errno;
    for (int i = 0; i < line_num; i++) {  //input �ϳ��� �Ѱ� token_table ����
        token_parsing(input_data[i]);
    }
    char buff[50];
    int ad = 0;		// token�� �ּ� ����
    int sa = 0;		// literal�� �ߺ��� ���� ���ؼ� ���
    locctr = 0;		// control section�� ������ ���ؼ� �ʱ�ȭ
    for (int i = 0; i < token_line; i++) {		// token_table�� �������� sym_table�� literal_table ����
        if (strcmp(token_table[i]->operator, "CSECT") == 0) { // CSECT��  control section�� �������� sym_table ������
            ad = 0;
            strcpy(sym_table[sym_line].symbol, "\n");	// ���� ����ϱ� ���ؼ� ���� �߰�
            sym_line++;
            strcpy(sym_table[sym_line].symbol, token_table[i]->label);	// ���� �ϴ� �Լ� �̸� sym_table�� ����
            sym_table[sym_line].addr = ad;
            add[i] = ad;
            sym_line++;
        }
        else if (strcmp(token_table[i]->operator, "LTORG") == 0 || strcmp(token_table[i]->operator, "END") == 0) { // literal_table ����
            char* str1, * str2;
            for (int j = locctr; j < i; j++) {
                if (token_table[j]->operand[0] == NULL)
                    continue;
                else if (token_table[j]->operand[0][0] == '=') { // token�� �ϳ��� Ȯ���Ͽ� operand�� =�� ����ִ��� Ȯ��
                    strcpy(buff, token_table[j]->operand[0]);
                    str1 = strtok(buff, "'");
                    str2 = strtok(NULL, "'");
                    for (int t = 0; t < literal_line; t++) {
                        if (strcmp(literal_table[t].literal, str2) == 0) {	// literal_table �ȿ� �̹� �ִ� ���̸� �Ѿ�� ���� Ȯ��
                            sa = 1;
                            break;
                        }
                    }
                    if (sa == 1) {	// literal_table�� �ִٸ� �Ѿ�� sa �ٽ� �ʱ�ȭ
                        sa = 0;
                        continue;
                    }
                    strcpy(literal_table[literal_line].literal, str2);	// �ߺ����� ������ literal ����
                    literal_table[literal_line].addr = ad;
                    if (strcmp(str1, "=X") == 0)	// �ּ��� ���� ����
                        ad++;
                    else
                        ad += strlen(literal_table[literal_line].literal);
                    add[i] = ad;		// �ּ� ���� add �迭�� ����
                    literal_line++;
                }
            }
            locctr = i + 1;
        }
        else if (token_table[i]->label == "") { // label ������ Ȯ�� �� ����
            if (search_opcode(token_table[i]->operator) >= 0) { // opcode�� �ִ��� Ȯ��
                if (token_table[i]->operator[0] == '+')
                    ad++;
                ad += inst_table[search_opcode(token_table[i]->operator)]->format;
                add[i] = ad;
            }
        }
        else {
            strcpy(sym_table[sym_line].symbol, token_table[i]->label);	// label�� �ִٸ� sym_table�� �߰�
            sym_table[sym_line].addr = ad;
            if (search_opcode(token_table[i]->operator) >= 0) {		//���� �ڵ� ������ �ּҸ� ���ؼ� ���
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
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ ��ɾ� ���� OPCODE�� ��ϵ� ǥ(���� 5��) �̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*        ���� ���� 5�������� ���̴� �Լ��̹Ƿ� ������ ������Ʈ������ ������ �ʴ´�.
* -----------------------------------------------------------------------------------
*/
// void make_opcode_output(char *file_name)
// {
// 	/* add your code here */

// }

/* ----------------------------------------------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ SYMBOL�� �ּҰ��� ����� TABLE�̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*
* -----------------------------------------------------------------------------------
*/
void make_symtab_output(char *file_name)
{
	/* add your code here */
    
    if (file_name == NULL) {	// ���� �̸� ���� null�� �־��� ��
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
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ LITERAL�� �ּҰ��� ����� TABLE�̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*
* -----------------------------------------------------------------------------------
*/
void make_literaltab_output(char *filen_ame)
{
	/* add your code here */
    if(filen_ame == NULL) { // ���� �̸� ���� null�� �־��� ��
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
* ���� : ����� �ڵ带 ���� �ڵ�� �ٲٱ� ���� �н�2 ������ �����ϴ� �Լ��̴�.
*		   �н� 2������ ���α׷��� ����� �ٲٴ� �۾��� ���� ������ ����ȴ�.
*		   ������ ���� �۾��� ����Ǿ� ����.
*		   1. ������ �ش� ����� ��ɾ ����� �ٲٴ� �۾��� �����Ѵ�.
* �Ű� : ����
* ��ȯ : �������� = 0, �����߻� = < 0
* ���� :
* -----------------------------------------------------------------------------------
*/
static int assem_pass2(void)
{
    int errno;
    locctr = 0; // control section�� ������ ���ؼ� �ʱ�ȭ
    char buff[50];
    int j = 0; // control section�� ���� �����ֱ� ����
    int slocctr = 0; // sym_table�� section�� ������ ���� ���
	for (int i = 0; i < token_line; i++) {
        if (strcmp(token_table[i]->operator, "START") == 0 || strcmp(token_table[i]->operator, "CSECT") == 0) {	// section ���� ���� �ڵ� �ʱ�ȭ
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
            if (op >= 0) {			// opcode�� ������ nixbpe �Է� 
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
                    else if (token_table[i]->operand[0][0] != '#' && inst_table[op]->numop != 0) // operand�� ���� �� ó��
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
						if (inst_table[op]->numop == 1) {		// operand�� 1���� ���
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
							code[i] = code[i] << 4;		// operand�� 2���� ���
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
					if (token_table[i]->operator[0] == '+') { // 4�����϶�
						code[i] = strtoul(inst_table[op]->opcode, NULL, 16) << 24; // opcode�� �Է�
						code[i] += token_table[i]->nixbpe << 20;	//nixbpe�� ����ŷ
						int f = 1;
						if (token_table[i]->operand[0][0] == '#') {		// immediate �ϰ��
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
						else if (token_table[i]->operand[0][0] == '@') { // indirect �ΰ��
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
						else if (token_table[i]->operand[0][0] == '=') { // literal�� ���
							strcpy(buff, token_table[i]->operand[0]);
							for (int y = slocctr; y < sym_line; y++) {
								if (strcmp(sym_table[y].symbol, "\n") == 0)
									break;
								if (strcmp(sym_table[y].symbol, str) == 0) {
									if (sym_table[y].addr < add[i])			// ���� ��ġ�� �ּҰ� ������ ����
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
						else {		// �Ϲ����� ���
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
					else {		// 3����
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
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ object code (������Ʈ 1��) �̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*
* -----------------------------------------------------------------------------------
*/
void make_objectcode_output(char* file_name)
{
	/* add your code here */
	if (file_name != NULL) {
		FILE* file = fopen(file_name, "w");
		locctr = 0; // control section üũ��
		int ad = 0; // head�� ���� üũ��
		int length = 1; // Text�� ���� üũ��
		int li = 0;	//head ���� ���� literal_table�� �ߺ� üũ ���� 
		int sli = 0; //text�� ���� ���� literal_table�� �ߺ� üũ ����
		char buff[50]; // string �ӽ������
		int j = 0; // locctr�� ��ȭ�� �� head ���� üũ�� ���� �ݺ��� ����
		int extr = 0; // EXTREF�� ��ġ ����
		for (int i = 0; i < token_line; i++) {
			int op = search_opcode(token_table[i]->operator);
			if (strcmp(token_table[i]->operator, "START") == 0 || strcmp(token_table[i]->operator, "CSECT") == 0) { //head ���
				ad = 0;
				length = 0;
				if (strcmp(token_table[i]->operator, "CSECT") == 0) {	// end ��� �� head ���
					if (length < 60)
						fprintf(file, "\n");
					for (int q = locctr; q < token_line; q++) {    // �ܺ� ���� ����� �� �ְ� modification ����
						if (strcmp(token_table[q]->operator, "CSECT") == 0 || strcmp(token_table[q]->operator, "LTORG") == 0)
							break;
						if (strcmp(token_table[q]->operator, "EXTREF") == 0 || strcmp(token_table[q]->operator, "EXTDEF") == 0)
							continue;
						if (strcmp(token_table[extr]->operand[0], token_table[q]->operand[0]) == 0)				// EXTREF�� operand ���� Ȯ���Ͽ� �� �� ������ ���
							fprintf(file, "M%06X05+%s\n", (add[q] - inst_table[search_opcode(token_table[q]->operator)]->format), token_table[extr]->operand[0]);
						else if (token_table[extr]->operand[1] == NULL)
							continue;
						else if (strcmp(token_table[extr]->operand[1], token_table[q]->operand[0]) == 0)
							fprintf(file, "M%06X05+%s\n", (add[q] - inst_table[search_opcode(token_table[q]->operator)]->format), token_table[extr]->operand[1]);
						else if (token_table[extr]->operand[2] == NULL)
							continue;
						else if (strcmp(token_table[extr]->operand[2], token_table[q]->operand[0]) == 0)
							fprintf(file, "M%06X05+%s\n", (add[q] - inst_table[search_opcode(token_table[q]->operator)]->format), token_table[extr]->operand[2]);
						else if (strcmp(token_table[q]->operator, "WORD") == 0)	// ������ ���� �Ҷ� ó��
							if (strcmp(token_table[q]->operand[0], "*") != 0) {
								strcpy(buff, token_table[q]->operand[0]);
								char* str1, * str2;
								str1 = strtok(buff, "-");
								str2 = strtok(NULL, "\n");
								fprintf(file, "M%06X06+%s\n", add[q] - 3, str1);
								fprintf(file, "M%06X06-%s\n", add[q] - 3, str2);
							}
					}
					if (locctr == 0)	// ó���� end recode�� �ּҰ��� �ʿ�
						fprintf(file, "E%06X\n\n", sym_table[locctr].addr);
					else fprintf(file, "E\n\n");
					length = 0;
					locctr = j + 1;
				}
				for (j = locctr; j < token_line; j++) {   // head�� �ѱ��� ���
					if (strcmp(token_table[j]->operator, "CSECT") == 0)
						break;
					else {
						if (strcmp(token_table[j]->operator, "LTORG") == 0 || strcmp(token_table[j]->operator, "END") == 0) {	// literal�� ���̵� �߰��ϱ� ���ؼ� ����
							char* str1, * str2;
							for (int k = locctr; k < j; k++) {
								if (token_table[k]->operand[0] == NULL)
									continue;
								else if (token_table[k]->operand[0][0] == '=') {	// �ҽ��ڵ� �ȿ� �����ϴ� literal�� �߰�
									strcpy(buff, token_table[k]->operand[0]);
									str1 = strtok(buff, "'");
									str2 = strtok(NULL, "'");
									for (int t = li; t < literal_line; t++) {
										if (strcmp(literal_table[t].literal, str2) == 0) {
											li++;						// �ߺ��Ǿ �߰� ���� �ʱ� ���� ����
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
								if (token_table[j]->operator[0] == '+')	// 4���� ���
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
							else if (strcmp(token_table[j]->operator, "RESB") == 0)		// ��� ���� ���� �߰�
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
				fprintf(file, "H%-6s%06X%06X\n", token_table[i]->label, sym_table[locctr].addr, ad); // head ���
				ad = 0;
			}
			else if (strcmp(token_table[i]->operator, "EXTDEF") == 0) {	// EXTDEF ���
				int opn = 0;
				while (token_table[i]->operand[opn]) { // operand�� ���� ���Ҷ����� ���
					for (int li = locctr; li < sym_line; li++) {
						if (strcmp(token_table[i]->operand[opn], sym_table[li].symbol) == 0) {	// sym_table���� �ּ� �޾ƿ���
							if (opn == 0) {
								fprintf(file, "D%-6s%06X", sym_table[li].symbol, sym_table[li].addr); // ó������ ���
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
			else if (strcmp(token_table[i]->operator, "EXTREF") == 0) {	// EXTREF ���
				extr = i;
				int opn = 0;
				while (token_table[i]->operand[opn] != NULL) {	// operand�� ���� ������ ���
					if (opn == 0)
						fprintf(file, "R%-6s", token_table[i]->operand[opn]);	// ó������ ���
					else
						fprintf(file, "%-6s", token_table[i]->operand[opn]);
					opn++;
				}
				fprintf(file, "\n");
			}
			else if (strcmp(token_table[i]->operator, "LTORG") == 0) {	// �ڵ忡 �����ϴ� literal ���
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
								if (t == 0) {		//0�϶��� ���
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
				if (length == 0) {		// text recode�� ���ο� ���� ���
					int l = 0;		// text�� ����
					int pleng = 0;	// text�� ����ǥ��
					for (pleng = i; pleng < token_line; pleng++)
					{
						if (strcmp(token_table[pleng]->operator, "BYTE") == 0) {	// literal�� ���� �ȵ��ִ� ���
							char* str, * str2;
							strcpy(buff, token_table[pleng]->operand[0]);
							str = strtok(buff, "'");
							str2 = strtok(NULL, "'");
							l += (int)(strlen(str2) / 2);
						}
						else if (strcmp(token_table[pleng]->operator, "WORD") == 0) {
							l += 3;
						}
						else if (strcmp(token_table[pleng]->operator, "END") == 0) {	// ���� literal ��� ���
							char* str1, * str2;
							int lix = 0;
							for (int el = locctr; el < token_line; el++) {
								if (lix >= literal_line)
									break;
								if (token_table[el]->operand[0][0] == '=') {		// ������ section�� ���� literal�� text recode ���̿� �߰�
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
										l += (int)(strlen(str2) / 2);	// BYTE �϶�
									else
										l += 3;	//WORD �϶�
								}
							}
						}
						else if (search_opcode(token_table[pleng]->operator) < 0)
							break;
						else if (l + inst_table[search_opcode(token_table[pleng]->operator)]->format <= 30) {	// opcode�� format Ȯ���Ͽ��� ���� �߰�
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
				if (op >= 0) {		// text ���� ���  opcode�� ���� �ҽ�
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
					if (strcmp(token_table[i]->operator, "BYTE") == 0) {	// BYTE �϶� �̾ �Է�
						char* str;
						strcpy(buff, token_table[i]->operand[0]);
						str = strtok(buff, "'");
						str = strtok(NULL, "'");
						fprintf(file, "%s", str);
						length += strlen(str);
					}
					else if (strcmp(token_table[i]->operator, "WORD") == 0) {	// WORD �϶� �̾ �Է�
						fprintf(file, "%06X", code[i]);
						length += 6;
					}
					else if (strcmp(token_table[i]->operator, "END") == 0) {	// END�� ������ literal table�� Ȯ���Ͽ� ���� literal ���
						char* str1, * str2;
						int lix = 0;
						for (int el = locctr; el < token_line; el++) {
							if (lix >= literal_line)
								break;
							if (token_table[el]->operand[0][0] == '=') {
								strcpy(buff, token_table[el]->operand[0]);
								str1 = strtok(buff, "'");
								str2 = strtok(NULL, "'");
								for (int t = lix; t < literal_line; t++) {		// literal ��Ͽ��� ã���� break
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
		if (length < 60) // text recode�� ������ ���̰� 60�� ���� �ʴ��� ���� �Է�
			fprintf(file, "\n");
		for (int q = locctr; q < token_line; q++) {	// ������ section�� modification ����
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
		fprintf(file, "E"); // ������ section�� end recode
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
			if (strcmp(token_table[i]->operator, "START") == 0 || strcmp(token_table[i]->operator, "CSECT") == 0) { //head ���
				ad = 0;
				length = 0;
				if (strcmp(token_table[i]->operator, "CSECT") == 0) {
					if (length < 60)
						printf("\n");
					for (int q = locctr; q < token_line; q++) {    // �ܺ� ���� ����� �� �ְ� modification ����
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
				for (j = locctr; j < token_line; j++) {   // head�� �ѱ��� ���
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
								if (t == 0) {		//0�϶��� ���
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