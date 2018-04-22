#include "interpreter.h"

int width, height;
char* code[MAX_LINE_NUM];

int ip[2];
char direction;

struct IntStack {
	int contents[MAX_STACK_SIZE];
	int head;
};
intStack stacks[MAX_STACK_NUM];
int registers[MAX_STACK_NUM];
bool isReg[MAX_STACK_NUM];
int head;

bool isSQuoteOn, isDQuoteOn;

int interval;
bool isIntInput;

void errorHandle(int type);

void commandExecute(int argc, char* argv[]);
void init();
FILE* readFile();
void readCode(FILE* path);
void refresh();
void start();

void push(int n);
int pop();
void pushStack(intStack stack);
intStack popStack();

char codeAt(int x, int y);


void handleInstruction(char symbol);
void move();
void turn(char symbol);
void mirror(char symbol);
void randomize();
void skip();
void cond();
void jump();
void pushChar(char symbol);
void plus();
void minus();
void mult();
void divide();
void modulo();
void equals();
void greater();
void less();
void sQuote();
void dQuote();
void dup();
void delete();
void swap2();
void swap3();
void shiftL();
void shiftR();
void reverse();
void length();
void call();
void ret();
void outputChar();
void outputNum();
void read();
void reg();
void getCode();
void putCode();
void terminate(int status);

void printInfo();
void printStatus();

int main(int argc, char* argv[]) {
	puts("=-------------------------------------=\n"
		" *- ><> Language Interpreter on C99 -* \n"
		" \\ *---=== Producted By  snw ===---* / \n"
		" *--*---*----- 2018.4.21 -----*---*--* \n"
		"=-----*-------------------------*-----=\n");
	init();
	puts("Initialized successfully.\n");

	//commandExecute(argc, argv);

	readCode(readFile());
	puts("The code has been read successfully.\n");
	printInfo();

	puts("\nPlease set time interval (>=0, /ms):");
	scanf("%d%*c", &interval);

	char ans = 'N';
	fflush(stdin);
	puts("\nDo you want to toggle the integer inputing on? [Y/N]");
	scanf("%c", &ans);
	if (ans == 'Y' || ans == 'y') isIntInput = True;

	puts("\n---------  Console Start  ---------");

	start();
	return 0;
}

void errorHandle(int type) {
	switch (type) {
	case ERR_DIV_ZERO:
	case ERR_INVALID_INST:
	case ERR_STACK_UNDERFLOW:
	case ERR_STACKS_UNDERFLOW:
	case ERR_INVALID_JUMP:
		printf("%s\n", "something smells fishy...");
		break;
	case ERR_STACK_OVERFLOW:
		printf("Runtime Error: Stack Overflow\nThe maximum of a stack size should be under %d.\n", MAX_STACK_SIZE);
		break;
	case ERR_STACKS_OVERFLOW:
		printf("Runtime Error: Stacks Overflow\nThe maximum of the stack number should be under %d.\n", MAX_STACK_NUM);
		break;
	case ERR_LINE_2_LONG:
		printf("Interpreting Error: Too Long a Line\nThe maximum of a line length should be under %d.\n", MAX_LINE_LENGTH);
		break;
	case ERR_LINE_2_MANY:
		printf("Interpreting Error: Too Many Lines\nThe line number should be under %d.\n", MAX_LINE_NUM);
		break;
	case ERR_MEMORY_LEAK:
		printf("%s", "System Error: Space Not Enough\n");
		break;
	case ERR_FILE_IO:
		printf("%s", "Interpreting Error: Failed To Open The File\n");
		break;
	case ERR_FLAG:
		printf("%s", "Command Error: Failed To Interpret the Flags\n");
		break;
	}
	terminate(1);
}

void push(int n) {
	intStack* stack = stacks + head - 1;
	if (stack->head >= MAX_STACK_SIZE) {
		errorHandle(ERR_STACK_OVERFLOW);
	}
	stack->contents[stack->head++] = n;
}

int pop() {
	intStack* stack = stacks + head - 1;
	if (stack->head <= 0) {
		errorHandle(ERR_STACK_UNDERFLOW);
	}
	return stack->contents[--stack->head];
}

void pushStack(intStack stack) {
	if (head >= MAX_STACK_NUM) {
		errorHandle(ERR_STACKS_OVERFLOW);
	}
	stacks[head++] = stack;
	isReg[head - 1] = False;
}

intStack popStack() {
	if (head <= 0) {
		errorHandle(ERR_STACKS_UNDERFLOW);
	}
	return stacks[--head];
}

char codeAt(int x, int y) {
	if (x >= height || x < 0) return ' ';
	if (y >= strlen(code[x]) || y < 0) return ' ';
	return code[x][y];
}

//TODO
void commandExecute(int argc, char * argv[]) {
	if (argc > 1) {
		if (argv[1] != "-t" || argc < 3) errorHandle(ERR_FLAG);
		sscanf(argv[2], "%d", &interval);
		if (interval < 0) errorHandle(ERR_FLAG);
	}
}

void init() {
	ip[0] = 0;
	ip[1] = 0;
	direction = RIGHT;
	head = 0;
	isSQuoteOn = False;
	isDQuoteOn = False;
	intStack first;
	first.head = 0;
	pushStack(first);
	interval = 0;
	isIntInput = False;
}

FILE * readFile() {
	char path[128] = "";
	puts("Please input the file path: ");
	fflush(stdin);
	scanf("%s", path);
	FILE* fp = fopen(path, "r");
	if (fp == NULL) errorHandle(ERR_FILE_IO);
	return fp;
}

void readCode(FILE * fp) {
	height = 0;
	width = 0;
	char line[MAX_LINE_LENGTH + 1] = "";
	while (fgets(line, MAX_LINE_LENGTH + 1, fp) != NULL && height <= MAX_LINE_NUM) {
		if (!feof(fp)) {
			if (line[strlen(line) - 1] != '\n') errorHandle(ERR_LINE_2_LONG);
			line[strlen(line) - 1] = 0;
		}
		else {
			if (strlen(line) > MAX_LINE_LENGTH) errorHandle(ERR_LINE_2_LONG);
		}
		code[height] = malloc(sizeof(line));
		if (code[height] == NULL) errorHandle(ERR_MEMORY_LEAK);
		strcpy(code[height++], line);
		width = max(width, strlen(line));
	}
	if (height > MAX_LINE_NUM) errorHandle(ERR_LINE_2_MANY);
	fclose(fp);
}

void refresh() {
	if (_DEBUG)	printStatus();
	handleInstruction(codeAt(ip[0], ip[1]));
	move();
	Sleep(interval);
}

void start() {
	while (True) {
		refresh();
	}
}

//TODO quick oob handle
void move() {
	switch (direction) {
	case LEFT:
		ip[1]--;
		if (ip[1] < 0) {
			ip[1] = width - 1;
		}
		break;
	case RIGHT:
		ip[1]++;
		if (ip[1] >= width) {
			ip[1] = 0;
		}
		break;
	case UP:
		ip[0]--;
		if (ip[0] < 0) {
			ip[0] = height - 1;
		}
		break;
	case DOWN:
		ip[0]++;
		if (ip[0] >= height) {
			ip[0] = 0;
		}
		break;
	}
}

void handleInstruction(char symbol) {
	if (isSQuoteOn) {
		if (symbol == '\'') {
			sQuote();
		}
		else {
			push(symbol);
		}
		return;
	}
	if (isDQuoteOn) {
		if (symbol == '\"') {
			dQuote();
		}
		else {
			push(symbol);
		}
		return;
	}
	switch (symbol) {
	case ' ':
		break;
	case '>':
	case '<':
	case '^':
	case 'v':
		turn(symbol);
		break;
	case '/':
	case '\\':
	case '_':
	case '|':
	case '#':
		mirror(symbol);
		break;
	case 'x':
		randomize();
		break;
	case '!':
		skip();
		break;
	case '?':
		cond();
		break;
	case '.':
		jump();
		break;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
		pushChar(symbol);
		break;
	case '+':
		plus();
		break;
	case '-':
		minus();
		break;
	case '*':
		mult();
		break;
	case ',':
		divide();
		break;
	case '%':
		modulo();
		break;
	case '=':
		equals();
		break;
	case ')':
		greater();
		break;
	case '(':
		less();
		break;
	case '\'':
		sQuote();
		break;
	case '\"':
		dQuote();
		break;
	case ':':
		dup();
		break;
	case '~':
		delete();
		break;
	case '$':
		swap2();
		break;
	case '@':
		swap3();
		break;
	case '}':
		shiftR();
		break;
	case '{':
		shiftL();
		break;
	case 'r':
		reverse();
		break;
	case 'l':
		length();
		break;
	case '[':
		call();
		break;
	case ']':
		ret();
		break;
	case 'o':
		outputChar();
		break;
	case 'n':
		outputNum();
		break;
	case 'i':
		read();
		break;
	case '&':
		reg();
		break;
	case 'g':
		getCode();
		break;
	case 'p':
		putCode();
		break;
	case ';':
		terminate(0);
		break;
	default:
		errorHandle(ERR_INVALID_INST);
	}
}

void turn(char symbol) {
	switch (symbol) {
	case '>':
		direction = RIGHT;
		break;
	case '<':
		direction = LEFT;
		break;
	case '^':
		direction = UP;
		break;
	case 'v':
		direction = DOWN;
		break;
	}
}

void mirror(char symbol) {
	switch (symbol) {
	case '/':
		direction = 3 & (~direction);
		break;
	case '\\':
		direction = 2 ^ direction;
		break;
	case '_':
		direction = (direction >> 1) ^ direction;
		break;
	case '|':
		direction = !(direction >> 1) ^ direction;
		break;
	case '#':
		direction = 1 ^ direction;
		break;
	}
}

void randomize() {
	direction = random(4);
}

void skip() {
	move();
}

void cond() {
	bool exp = pop();
	if (!exp) {
		move();
	}
}

void jump() {
	int x = pop(), y = pop();
	if (x < 0 || y < 0) errorHandle(ERR_INVALID_INST);
	ip[0] = x;
	ip[1] = y;
}

void pushChar(char symbol) {
	if (symbol <= '9') {
		push(symbol - '0');
	}
	else {
		push(symbol - 'a' + 10);
	}
}

void plus() {
	push(pop() + pop());
}

void minus() {
	push(-pop() + pop());
}

void mult() {
	push(pop() * pop());
}

void divide() {
	int x = pop();
	push(pop() / x);
}

void modulo() {
	int x = pop();
	push(pop() % x);
}

void equals() {
	push(pop() == pop());
}

void greater() {
	push(pop() < pop());
}

void less() {
	push(pop() > pop());
}

void sQuote() {
	isSQuoteOn = !isSQuoteOn;
}

void dQuote() {
	isDQuoteOn = !isDQuoteOn;
}

void dup() {
	int x = pop();
	push(x);
	push(x);
}

void delete() {
	pop();
}
void swap2() {
	int x = pop(), y = pop();
	push(x);
	push(y);
}

void swap3() {
	int x = pop(), y = pop(), z = pop();
	push(x);
	push(z);
	push(y);
}

void shiftL() {
	intStack* stack = stacks + head - 1;
	if (stack->head <= 0) return;
	int tmp = stack->contents[0];
	for (int i = 1; i < stack->head; i++) {
		stack->contents[i - 1] = stack->contents[i];
	}
	stack->contents[stack->head - 1] = tmp;
}

void shiftR() {
	intStack* stack = stacks + head - 1;
	if (stack->head <= 0) return;
	int tmp = stack->contents[stack->head - 1];
	for (int i = 1; i < stack->head; i++) {
		stack->contents[i] = stack->contents[i - 1];
	}
	stack->contents[0] = tmp;
}

void reverse() {
	intStack oldStack = currentStack;
	intStack *stack = stacks + head - 1;
	stack->head = 0;
	while (oldStack.head > 0) {
		stack->contents[stack->head++] = oldStack.contents[--oldStack.head];
	}
}

void length() {
	push(currentStack.head);
}

void call() {
	int x = pop();
	intStack newStack;
	newStack.head = x;
	for (int i = x - 1; i >= 0; i--) {
		newStack.contents[i] = pop();
	}
	pushStack(newStack);
}

void ret() {
	intStack stack = popStack();
	for (int i = 0; i < stack.head; i++) {
		push(stack.contents[i]);
	}
}

void outputChar() {
	printf("%c", pop());
}

void outputNum() {
	printf("%d", pop());
}

void read() {
	int v;
	fflush(stdin);
	scanf(isIntInput ? "%d%*c" : "%c%*c", &v);
	push(v);
}

void reg() {
	if (isReg[head - 1]) {
		push(registers[head - 1]);
	}
	else {
		registers[head - 1] = pop();
	}
	isReg[head - 1] = !isReg[head - 1];
}

//Note: empty cells are equal to ' '
//      different from the wiki definition
void getCode() {
	int x = pop(), y = pop();
	push(codeAt(x, y));
}

//Note: a negative position will cause an error
//      the height and the width may be changed after
//      putting a symbol on a point out of current
//      bounds of the code
void putCode() {
	int x = pop(), y = pop(), v = pop();
	if (x < 0 || y < 0) errorHandle(ERR_INVALID_INST);
	if (y >= MAX_LINE_LENGTH) errorHandle(ERR_LINE_2_LONG);
	if (x >= MAX_LINE_NUM) errorHandle(ERR_LINE_2_MANY);
	if (x >= height) {
		for (; height <= x; height++) {
			code[height] = "";
		}
	}
	if (y >= strlen(code[x])) {
		for (int i = strlen(code[x]); i <= y; i++) {
			strcat(code[x], " ");
		}
		width = max(y + 1, width);
	}
	code[x][y] = v;
}

void terminate(int status) {
	switch (status) {
	case 0:
		puts("\n---------   Output Over   ---------\n");
		puts("The program terminated successfully.");
		break;
	default:
		puts("The program terminated with some problems.");
	}
	puts("Thank you for using.");
	exit(status);
}

void printInfo() {
	puts("CODE INFORMATION:");
	printf("HEIGHT:%3d\n", height);
	printf("WIDTH :%3d\n", width);
}

void printStatus() {
	puts("STATUS:");
	printf("IP: (%d, %d) \"%c\"\n", ip[1], ip[0], codeAt(ip[0], ip[1]));
	static char* directionName[4] = { "LEFT","RIGHT","UP","DOWN" };
	printf("DIRECTION: %s\n", directionName[direction]);
	puts("STACKS:");
	for (int i = 0; i < head; i++) {
		printf("%d: %s", i + 1, "[");
		for (int j = 0; j < stacks[i].head; j++) {
			printf("%s%d", j ? ", " : "", stacks[i].contents[j]);
		}
		printf("]");
		if (isReg[i]) {
			printf(" REG: [%d]", registers[i]);
		}
		puts("");
	}
	puts("");
}
