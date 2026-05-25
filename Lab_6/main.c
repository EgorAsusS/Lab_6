#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//Пример формирования стека
//Объявляем новый тип данных, который описывает узел стека 
struct nodeList {
	bool var;
	int inf;
	struct nodeList* next;
};

//Объявляем новый тип данных, который описывет стек
struct StekList {
	struct nodeList* Top;//Вершина стека(голова на начало списка)
	int size;//количество элементов в стеке
};

typedef struct nodeList Node;
typedef struct StekList Stek;

bool Push(Stek* st, int value, bool var);
bool Pop(Stek* st, int* value, bool* var);
bool ShowTop(Stek st, int* value, bool* var);
bool isEmpty(Stek st);
void ClearStek(Stek* st);

char* form_POLIS(FILE* file, int* Tab);
bool calculate_POLIS(char* str, int* Tab);
int calculate_expression(char* filepath, int* Tab);

int main() {
	int Tab[256] = { 0 };
	int answer = calculate_expression("data.txt", Tab);
	if (answer != -1) {
		printf("%c = %d\n", (char)answer, Tab[answer]);
	}
	return 0;
}

int calculate_expression(char* filepath, int* Tab) {
	int answer = -1;
	if (filepath && Tab) {
		Tab[(unsigned char)'('] = 0;
		Tab[(unsigned char)')'] = 1;
		Tab[(unsigned char)'='] = 2;
		Tab[(unsigned char)'+'] = 6;
		Tab[(unsigned char)'-'] = 6;
		Tab[(unsigned char)'*'] = 7;
		Tab[(unsigned char)'/'] = 7;
		for (unsigned char i = 0; i < 10; i++) {
			Tab[i + 48] = i;
		}
		FILE* file = fopen(filepath, "r");
		char* str = NULL;
		char* vars = NULL;
		int flag = 1;
		char el = 0;
		long count = 0;
		if (file) {
			flag = fscanf(file, "%c", &el);
			count++;
			if (flag > 0 && (97 <= el && el <= 122) || (65 <= el && el <= 90)) {
				answer = (int)el;
				do {
					flag = fscanf(file, "%c", &el);
					count++;
				} while (flag > 0 && el == ' ');
				if (flag > 0 && el == 61) {
					if (fseek(file, -count, SEEK_CUR)) {
						flag = -2;
					}
					str = form_POLIS(file, Tab);
				}
			}
			if (str) {
				while (flag > 0) {
					count = 0;
					flag = fscanf(file, "%c", &el);
					count++;
					if (flag > 0 && (97 <= el && el <= 122) || (65 <= el && el <= 90)) {
						do {
							flag = fscanf(file, "%c", &el);
							count++;
						} while (flag > 0 && el == ' ');
						if (flag > 0 && el == 61) {
							if (fseek(file, -count, SEEK_CUR)) {
								flag = -2;
							}
							vars = form_POLIS(file, Tab);
							flag = calculate_POLIS(vars, Tab);
						}
					}
					else if (el != ' ' && el != '\n') {
						flag = -3;
					}
				}
			}
			flag = calculate_POLIS(str, Tab);
		}
	}
	return answer;
}

char* form_POLIS(FILE* file, int* Tab) {
	if (file && Tab) {
		int flag = 1;
		char* str = (char*)calloc(1000, sizeof(char)); // Сколько буффер
		char el = 1;
		size_t count = 0;
		Stek st = { NULL, 0 };
		int cmp = 0;
		int value = 0;
		bool var = 0;
		unsigned char flag_op = 0; // Флаг на операнды на начало
		if (str) {
			while (flag > 0 && el) {
				flag = fscanf(file, "%c", &el);
				if (flag > 0) {
					if ((97 <= el && el <= 122) || (65 <= el && el <= 90) || (48 <= el && el <= 57)) { // Операнды
						str[count] = el;
						count++;
						if (flag_op == 0) {
							flag_op = 1;
						}
					}
					else if ((40 <= el && el <= 43) || el == 45 || el == 47 || el == 61) { // Операции
						if (flag_op == 1) {
							flag_op = 0;
						}
						else if (flag_op != 2 && (el == 43 || el == 45)) {
							str[count] = '0';
							count++;
							flag_op = 2;
						}
						else {
							flag = -4; // Неверные операции
						}
						if (isEmpty(st)) {
							if (Push(&st, el, 0)) {
								flag = 0; //ERROR
							}
						}
						else {
							if (el == 61) {
								cmp = 9;
							}
							else {
								cmp = Tab[el];
							}
							if (cmp) {
								ShowTop(st, &value, &var);
								if (cmp > Tab[value]) {
									if (Push(&st, el, 0)) {
										flag = 0; //ERROR
									}
								}
								else {
									while (flag >= 0 && ShowTop(st, &value, &var) && cmp <= Tab[value]) {
										if (Pop(&st, &value, &var)) {
											str[count] = (char)value;
											count++;
										}
										else {
											flag = 0; // ERROR
										}
									}
									if (el != 41) { // Если скобки, то взаимно уничтожаются
										if (Push(&st, el, 0)) {
											flag = 0; //ERROR
										}
									}
									else {
										if (!Pop(&st, &value, &var)) {
											flag = 0; // ERROR
										}
									}
								}
							}
							else {
								if (Push(&st, el, 0)) {
									flag = 0; //ERROR
								}
							}
						}
					}
					else if (el != ' ') { // Выход по /n сделать
						flag = -3;
					}
				}
			}
			while (flag >= 0 && st.Top) {
				if (!Pop(&st, &value, &var)) {
					flag = 0; // ERROR
				}
				str[count] = (char)value;
				count++;
			}
			// Error with size of buffer
			str[count] = '\0';
		}
		return str;
	}
	return NULL;
}

bool calculate_POLIS(char* str, int* Tab) {
	int a = 0;
	int b = 0;
	Stek st = { NULL, 0 };
	char* ptr_str = 0;
	int flag = 1;
	bool var = 0;
	if (str && Tab) {
		ptr_str = str;
		while (*ptr_str) {
			var = 0;
			if ((97 <= *ptr_str && *ptr_str <= 122) || (65 <= *ptr_str && *ptr_str <= 90) || (48 <= *ptr_str && *ptr_str <= 57)) { // Операнды
				if (!Push(&st, *ptr_str, 1)) {
					flag = 0; //ERROR
					continue;
				}
			}
			else if (*ptr_str == 42 || *ptr_str == 43 || *ptr_str == 45 || *ptr_str == 47 || *ptr_str == 61) { // Операции
				if (!Pop(&st, &a, &var)) {
					flag = 0;
					continue;
				}
				if (var) {
					a = Tab[a];
				}
				if (!Pop(&st, &b, &var)) {
					flag = 0;
					continue;
				}
				if (var && *ptr_str != 61) {
					b = Tab[b];
				}
				switch (*ptr_str) {
				case 42: // *
					b *= a;
					break;
				case 43: // +
					b += a;
					break;
				case 45: // -
					b -= a;
					break;
				case 47: // /
					if (a) {
						b /= a;
					}
					else {
						flag = -4; // Error with / 0
					}
					break;
				case 61: // =
					if ((97 <= b && b <= 122) || (65 <= b && b <= 90)) {
						Tab[b] = a;
						b = Tab[b];
					}
					else {
						flag = -4; // const = variable
					}
					break;
				}
				if(Push(&st, b, 0)){
					flag = 0;
					continue;
				}
			}
			else if (*ptr_str && *ptr_str != ' ') {
				flag = -3; // Error
			}
			ptr_str++;
		}
		return true;
	}
	return false;
}

bool Push(Stek* st, int value, bool var) {
	if (st) {
		Node* el = (Node*)malloc(sizeof(Node));
		if (el) {
			el->inf = value;
			el->var = var;
			el->next = NULL;
			if (st->Top == NULL)
			{
				st->Top = el;
			}
			else
			{
				el->next = st->Top;
				st->Top = el;
			}
			st->size++;
			return true;
		}
	}
	return false;
}

bool Pop(Stek* st, int* value, bool* var) {
	if (st && value && var) {
		Node* PtrIx = st->Top;
		if (PtrIx) {
			st->Top = st->Top->next; // Обновление головы
			*value = PtrIx->inf;
			*var = PtrIx->var;
			free(PtrIx);
			PtrIx = NULL;
			st->size--;
		}
		return true;
	}
	return false;
}

bool ShowTop(Stek st, int* value, bool* var) {
	if (value) {
		if (st.Top) {
			*value = st.Top->inf;
			*var = st.Top->var;
			return true;
		}
	}
	return false;
}

bool isEmpty(Stek st) {
	if (st.Top == NULL)
		return true;
	return false;
}

void ClearStek(Stek* st) {
	Node* PtrIx = NULL;
	while (st) {
		Node* PtrIx = st->Top;
		if (PtrIx) {
			st->Top = st->Top->next; // Обновление головы
			free(PtrIx);
			PtrIx = NULL;
			st->size--;
		}
	}
}