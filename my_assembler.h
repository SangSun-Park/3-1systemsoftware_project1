/*
* my_assembler �Լ��� ���� ���� ���� �� ��ũ�θ� ��� �ִ� ��� �����̴�.
*
*/
#define MAX_INST 256
#define MAX_LINES 5000
#define MAX_OPERAND 3

/*
* instruction ��� ���Ϸ� ���� ������ �޾ƿͼ� �����ϴ� ����ü �����̴�.
* ������ ������ instruction set�� ��Ŀ� ���� ���� �����ϵ�
* ���� ���� �ϳ��� instruction�� �����Ѵ�.
*/
struct inst_unit {
	char operator_[6];
	int format;
	char opcode[2];
	//���Ŀ� ��� �� �߰��ؾ���
};
typedef struct inst_unit inst;
inst *inst_table[MAX_INST];
int inst_index;

/*
* ����� �� �ҽ��ڵ带 �Է¹޴� ���̺��̴�. ���� ������ ������ �� �ִ�.
*/
char *input_data[MAX_LINES];
static int line_num;

int label_num;
char register_number[10] = { 'A','X',NULL,NULL,'S','T' };
/*
* ����� �� �ҽ��ڵ带 ��ū������ �����ϱ� ���� ����ü �����̴�.
* operator�� renaming�� ����Ѵ�.
* nixbpe�� 8bit �� ���� 6���� bit�� �̿��Ͽ� n,i,x,b,p,e�� ǥ���Ѵ�.
*/
struct token_unit {
	int current_locate;
	int pc;
	char *label;
	char *operator_;
	char *operand[MAX_OPERAND];
	char *comment;
	int objectcode;
	char nixbpe; // ���� ������Ʈ���� ���ȴ�.
	
};

typedef struct token_unit token;
token *token_table[MAX_LINES];
static int token_line;

/*
* �ɺ��� �����ϴ� ����ü�̴�.
* �ɺ� ���̺��� �ɺ� �̸�, �ɺ��� ��ġ�� �����ȴ�.
*/
struct symbol_unit {
	char symbol[10];
	int addr;
};

typedef struct symbol_unit symbol;
symbol sym_table[MAX_LINES];

static int locctr;
//--------------

static char *input_file;
static char *output_file;
int init_my_assembler(void);
int init_inst_file(char *inst_file);
int init_input_file(char *input_file);
int search_opcode(char *str);
void make_opcode_output(int,int,int);

/* ���� ������Ʈ���� ����ϰ� �Ǵ� �Լ�*/
static int assem_pass1(void);
static int assem_pass2(int,int, int);
void make_objectcode_output(int, int,int,char *filename);
int calc_size(char *str);
int equate_check(char *str);
int search_num_of_register(char c);
int search_symbol_table(char *str);
void csect(int i, token *tmp);
void ltorg(int i, int section_start_line);
FILE *output = fopen("output.txt", "w");
int byte_calculator(int x);