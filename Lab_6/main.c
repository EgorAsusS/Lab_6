#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define n 2000

//Пример формирования стека
//Объявляем новый тип данных, который описывает узел стека 
struct nodeList {
	int var;
	double value;
	struct nodeList* next;
};

//Объявляем новый тип данных, который описывет стек
struct StekList {
	struct nodeList* Top;//Вершина стека(голова на начало списка)
	int size;//количество элементов в стеке
};

typedef struct nodeList Node;
typedef struct StekList Stek;

bool Push(Stek* st, double value, int var);
bool Pop(Stek* st, double* value, int* var);
bool ShowTop(Stek st, double* value, int* var);
bool isEmpty(Stek st);
void ClearStek(Stek st);

char* form_POLIS(char* input);
int calculate_POLIS(char* str, int* Tab, double* answer);
char* read_file(char* filepath, int** Tab);

int main() {
	int* Tab = NULL;
	char* output = NULL;
	double answer = 0;

	char* input = read_file("data.txt", &Tab);
	output = form_POLIS(input);
	int flag = calculate_POLIS(output, Tab, &answer);
	if (flag > 0) {
		if (input) {
			char* current = input;
			bool flag = 1;
			if ((97 <= *current && *current <= 122) || (65 <= *current && *current <= 90)) {
				while (*current && *current != '=') {
					current++;
				}
				if (*current) { 
					printf("%c = ", *input);
					flag = 0;
				}
			}
			if (flag == 1) {
				printf("answer = ");
			}
			printf("%.14lf\n", answer);
		}
	}
	else {
		printf("ERROR\n");
	}

	free(input);
	free(output);
	free(Tab);
	input = NULL;
	output = NULL;
	Tab = NULL;
	return 0;
}

char* read_file(char* filepath, int** Tab) {
	char* input = NULL;
	if (filepath && Tab) {
		*Tab = (int*)calloc(256, sizeof(int));
		if (*Tab) {
			(*Tab)[(unsigned char)'('] = 0;
			(*Tab)[(unsigned char)')'] = 1;
			(*Tab)[(unsigned char)'='] = 2;
			(*Tab)[(unsigned char)'+'] = 6;
			(*Tab)[(unsigned char)'-'] = 6;
			(*Tab)[(unsigned char)'*'] = 7;
			(*Tab)[(unsigned char)'/'] = 7;
			for (unsigned char i = 0; i < 10; i++) {
				(*Tab)[i + 48] = i;
			}
			FILE* file = fopen(filepath, "r");
			int flag = 1;
			char el = 0;
			int val = 0;
			size_t count = 0;
			if (file) {
				input = (char*)calloc(n, sizeof(char));
				if (input) {
					while (count < n - 1 && el != '\n' && flag > 0) {
						flag = fscanf(file, "%c", &el);
						if (flag > 0 && el != '\n') {
							input[count] = el;
							count++;
						}
					}
					while (flag > 0) {
						flag = fscanf(file, " %c = %d \n", &el, &val);
						if (flag > 1) {
							if ((97 <= el && el <= 122) || (65 <= el && el <= 90)) {
								(*Tab)[(unsigned char)el] = val;
							}
							else {
								flag = -3;
							}
						}
						else if (flag == 1){
							flag = -2; // Некорректное задание переменных
						}
					}
				}
				if (fclose(file)) {
					flag = -4; //
				}
				if (flag != -1) {
					free(input);
					free(*Tab);
					input = NULL;
					*Tab = NULL;
				}
			}
		}
	}
	return input;
}

char* form_POLIS(char* input) {
	char* output = NULL;
	if (input) {
		// Приоритеты операций
		int Tab[128] = { 0 };
		Tab[(unsigned char)'('] = 0;
		Tab[(unsigned char)')'] = 1;
		Tab[(unsigned char)'='] = 2;
		Tab[(unsigned char)'+'] = 6;
		Tab[(unsigned char)'-'] = 6;
		Tab[(unsigned char)'*'] = 7;
		Tab[(unsigned char)'/'] = 7;

		output = (char*)calloc(n, sizeof(char)); // Сколько буффер
		int flag = 1;
		char* ptr_str = 0;
		size_t count = 0;
		Stek st = { NULL, 0 };
		int cmp = 0;
		double value = 0;
		int var = 0;
		bool flag_op = 0; // Флаг на операнды на начало
		if (output) {
			ptr_str = input;
			cmp = 0;
			value = 0;
			var = 0;
			while (*ptr_str && flag > 0 && count < n - 1) {
				if ((97 <= *ptr_str && *ptr_str <= 122) || (65 <= *ptr_str && *ptr_str <= 90) || (48 <= *ptr_str && *ptr_str <= 57)) { // Операнды
					output[count] = *ptr_str;
					count++;
					if (flag_op == 0) {
						flag_op = 1;
					}
				}
				else if ((40 <= *ptr_str && *ptr_str <= 43) || *ptr_str == 45 || *ptr_str == 47 || *ptr_str == 61) { // Операции
					if (*ptr_str == 40 || *ptr_str == 61) {
						flag_op = 0;
					}
					else if (flag_op == 0) {
						if (*ptr_str == 45) {
							output[count] = '0';
							count++;
						}
						else if (*ptr_str == 43) {
							ptr_str++;
							continue;
						}
					}
					if (st.size) {
						if (*ptr_str == 61) {
							cmp = 9;
						}
						else {
							cmp = Tab[*ptr_str];
						}
						if (cmp) {
							ShowTop(st, &value, &var);
							if (cmp > Tab[var] && *ptr_str != 41) {
								if (!Push(&st, 0, *ptr_str)) {
									flag = 0; //ERROR
									//continue;
								}
							}
							else {
								while (flag > 0 && ShowTop(st, &value, &var) && cmp <= Tab[var]) {
									if (Pop(&st, &value, &var)) {
										output[count] = (char)var;
										count++;
									}
									else {
										flag = 0; // ERROR
										continue;
									}
								}
								if (flag) {
									if (*ptr_str != 41) { // Если скобки, то взаимно уничтожаются
										if (!Push(&st, 0, *ptr_str)) {
											flag = 0; //ERROR
											continue;
										}
									}
									else {
										if (!Pop(&st, &value, &var)) {
											flag = 0; // ERROR
											continue;
										}
									}
								}
							}
						}
						else {
							if (!Push(&st, 0, *ptr_str)) {
								flag = 0; //ERROR
								continue;
							}
						}
					}
					else {
						if (!Push(&st, 0, *ptr_str)) {
							flag = 0; //ERROR
							continue;
						}
					}
				}
				else if (*ptr_str != ' ') {
					flag = -3;
					continue;
				}
				ptr_str++;
			}
			while (flag > 0 && count < n - 1 && st.Top) {
				if (!Pop(&st, &value, &var)) {
					flag = 0; // ERROR
					continue;
				}
				if (var != 40) { // '(' пропуск
					output[count] = (char)var;
					count++;
				}
			}
			ClearStek(st);
			if (flag != 1) { // В случае ошибки
				free(output);
				output = NULL;
			}
		}
	}
	return output;
}

int calculate_POLIS(char* str, int* Tab, double* answer) {
	double a = 0;
	double b = 0;
	Stek st = { NULL, 0 };
	char* ptr_str = 0;
	int flag = -1;
	int var_a = 0;
	int var_b = 0;
	if (str && Tab && answer) {
		flag = 1;
		ptr_str = str;
		while (*ptr_str && flag > 0) {
			a = 0;
			b = 0;
			var_a = 0;
			var_b = 0;
			if ((97 <= *ptr_str && *ptr_str <= 122) || (65 <= *ptr_str && *ptr_str <= 90) || (48 <= *ptr_str && *ptr_str <= 57)) { // Операнды
				if (!Push(&st, 0, *ptr_str)) {
					flag = 0; //ERROR
					continue;
				}
			}
			else if (*ptr_str == 42 || *ptr_str == 43 || *ptr_str == 45 || *ptr_str == 47 || *ptr_str == 61) { // Операции
				if (!Pop(&st, &a, &var_a)) {
					flag = 0;
					continue;
				}
				if (var_a != -1) {
					a = Tab[var_a];
				}
				if (!Pop(&st, &b, &var_b)) {
					flag = 0;
					continue;
				}
				if (var_b != -1 && *ptr_str != 61) {
					b = Tab[var_b];
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
						flag = -2; // Error with / 0
					}
					break;
				case 61: // =
					if ((97 <= var_b && var_b <= 122) || (65 <= var_b && var_b <= 90)) {
						Tab[var_b] = a;
						b = a;
					}
					else {
						flag = -2; // const = variable
						continue;
					}
					break;
				}
				if(!Push(&st, b, -1)){
					flag = 0;
					continue;
				}
			}
			else if (*ptr_str && *ptr_str != ' ') {
				flag = -3; // Error
			}
			ptr_str++;
		}
		if (!Pop(&st, answer, &var_b)) {
			flag = 0;
		}
		if (st.size > 1 || (st.Top && st.Top->var != -1)) {
			flag = -4;
		}
	}
	ClearStek(st);
	return flag;
}

bool Push(Stek* st, double value, int var) {
	if (st) {
		Node* el = (Node*)malloc(sizeof(Node));
		if (el) {
			el->value = value;
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

bool Pop(Stek* st, double* value, int* var) {
	if (st && value && var) {
		Node* PtrIx = st->Top;
		if (PtrIx) {
			st->Top = st->Top->next; // Обновление головы
			*value = PtrIx->value;
			*var = PtrIx->var;
			free(PtrIx);
			PtrIx = NULL;
			st->size--;
			return true;
		}
	}
	return false;
}

bool ShowTop(Stek st, double* value, int* var) {
	if (value && var) {
		if (st.Top) {
			*value = st.Top->value;
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

void ClearStek(Stek st) {
	Node* PtrIx = NULL;
	//while (st) {
		//Node* PtrIx = st->Top;
	PtrIx = st.Top;
	if (PtrIx) {
		st.Top = st.Top->next; // Обновление головы
		//st->Top = st->Top->next; // Обновление головы
		free(PtrIx);
		PtrIx = NULL;
		//st->size--;
		st.size--;
	}
	//}
}