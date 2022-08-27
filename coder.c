#include "coder.h"

// Текущий размер таблицы (заполненность)
int32_t curTableSize = 0;

// Рассчет размера таблицы
uint32_t getTableSize(int32_t maxOutLenght) {
	// 2^n + 2^(n-1) + 1
	return (1 << maxOutLenght) + (1 << (maxOutLenght - 1));
}

// Возвращает true, если число является степенью двойки
int32_t isPowerOfTwo(int32_t num) {
	return num && !(num & (num - 1));
}

// Проверяет на совпадение двух элементов
int32_t areElementsEqual(unsigned char* str, int32_t strLen, Node tableElement) {
	if (strLen == tableElement.len) {
		for (int i = 0; i < strLen; i++) {
			if (str[i] != tableElement.str[i]) 
				return false;
		}
		return true;
	}
	else 
		return false;
}

// Выводит в выходной файл буферный байт
void outCode(unsigned char* outByte, int32_t* outByteLenght, int32_t curOutLenght, uint16_t elementCode, FILE* outFile) {

	char* elementCodeBytes = &elementCode;
	int32_t curElementChunk = 0;
	int32_t outBitsAmount = 0;

	elementCodeBytes[1] = elementCodeBytes[1] << (16 - curOutLenght);

	
	for (curElementChunk = 1; curElementChunk >= 0; curElementChunk--) {
		if (curElementChunk == 1) outBitsAmount = curOutLenght - 8;
		else outBitsAmount = 8;

		for (int32_t i = 0; i < outBitsAmount; i++) {

			if ((elementCodeBytes[curElementChunk] & 0b10000000) == 0b10000000) {
				*outByte = *outByte << 1;
				(*outByte)++;
			}
			else {
				*outByte = *outByte << 1;
			}
			elementCodeBytes[curElementChunk] = elementCodeBytes[curElementChunk] << 1;

			(*outByteLenght)++;
			if (*outByteLenght >= 8) {
				fwrite(outByte, 1, 1, outFile);
				*outByteLenght = 0;
				*outByte = 0;
			}
		}
	}
}

// Принимает строку; его длину ; размер таблицы
uint32_t hash(unsigned char* str, int32_t len, uint32_t tableSize) {
	uint32_t hashValue = 0;
	uint32_t seed = 28657; //28657 26231 10711

	for (int i = 0; i < len; i++) {
		hashValue = (hashValue * seed) + (uint32_t)str[i];
	}

	return hashValue % tableSize;
}

// Создание таблицы размера tableSize
Node* createTable(uint32_t tableSize) {
	Node* table = (Node*)calloc(tableSize ,sizeof(Node));
	if (table == NULL) {
		printf("Error!\n Can't allocate memory for Hash Table!");
		exit(1);
	}

	return table;
}

void addElement(unsigned char* str, int32_t len, uint32_t hashValue, Node* table) {
	table[hashValue].str = (unsigned char*)malloc(len * sizeof(char));
	if (table[hashValue].str == NULL) { 
		printf("Error!\n Can't allocate memory for element in Hash Table!"); 
		exit(1); 
	}
	memcpy(table[hashValue].str, str, len);
	table[hashValue].len = len;
	table[hashValue].code = curTableSize;
	curTableSize++;
}

// Заполняет хеш-таблицу всеми char элементами
void baseTableFill(Node* table, uint32_t tableSize) {
	int32_t hashValue = 0;
	unsigned char* str = NULL;

	for (int i = 0; i <= UCHAR_MAX; i++) {
		str = (unsigned char*)malloc(sizeof(char));
		if (str == NULL) { 
			printf("Error!\n Can't allocate memory for element in Hash Table!"); 
			exit(1); 
		}
		str[0] = i;

		hashValue = hash(str, 1, tableSize);
		// т.к. в случае просто с символами функция не дает коллизий, заполняем таблицу без проверки.
		table[hashValue].str = str;
		table[hashValue].len = 1;
		table[hashValue].code = curTableSize;
		curTableSize++;
	}
}

// Полностью очищает всю таблицу
void clearTable(Node* table, uint32_t tableSize) {
	for (uint32_t i = 0; i < tableSize; i++) {
		free(table[i].str);
		table[i].str = NULL;
		table[i].len = 0;
		table[i].code = 0;
	}
}

// Вывод ненулевых элементов таблицы
void printTable(Node* table, uint32_t tableSize) {
	for (uint32_t i = 0; i < tableSize; i++) {
		if (table[i].str != NULL) {
			printf("%d) CODE: %d; LEN: %d; STRpointer: %p;STR:", i, table[i].code, table[i].len, table[i].str);
			for (int j = 0; j < table[i].len; j++) {
				printf("%c", table[i].str[j]);
			}
			printf("\n");
		}
	}
}

// Поиск в таблице
bool search(unsigned char* buf, int32_t bufLen, Node* table, uint32_t tableSize, uint32_t* hashValue) {
	while (true) {
		// Нету в таблице
		if (table[*hashValue].str == NULL) {
			return false;
		}

		// Есть в таблице
		else if (areElementsEqual(buf, bufLen, table[*hashValue]) == true) {
			return true;
		}
		
		// Линейное пробирование
		else {
			*hashValue = (*hashValue + 1) % tableSize;
		}
	}
}

int encode(FILE* inFile, FILE* outFile, int32_t maxOutLenght) {

	unsigned char inByte = '\0';
	unsigned char* buf = (unsigned char*)malloc(sizeof(char) * (1 << maxOutLenght));
	if (buf == NULL) {
		printf("Buffer memory allocation error.");
		exit(1);
	}
	int32_t bufLen = 0;

	unsigned char outByte = 0;
	int32_t OutByteLenght = 0;
	int32_t curOutLenght = 8;	// Размер выводимого элемента в битах. Изначально равен 8, как только превысим лимит, увеличиваем.

	Node* table = NULL;
	uint32_t hashValue = 0;
	uint32_t prevHashValue = 0;
	uint32_t tableSize = getTableSize(maxOutLenght);

	bool found = false;

	table = createTable(tableSize);
	baseTableFill(table, tableSize);

	while (fread(&inByte, 1, 1, inFile)){
		buf[bufLen] = inByte;
		bufLen++;

		prevHashValue = hashValue;
		hashValue = hash(buf, bufLen, tableSize);
		found = search(buf, bufLen, table, tableSize, &hashValue);

		if (found) continue;

		else {
			addElement(buf, bufLen, hashValue, table);

			// Вывод кода элемента
			outCode(&outByte, &OutByteLenght, curOutLenght, table[prevHashValue].code, outFile);

			if (isPowerOfTwo(curTableSize-1) == true)
				curOutLenght++;

			// Если таблица переполнилась, начинаем заполнение с САМОГО начала.
			if (curOutLenght > maxOutLenght) {
				//printTable(table, tableSize);
				clearTable(table, tableSize);
				curOutLenght = 8;
				curTableSize = 0;
				baseTableFill(table, tableSize);
				//printf("Table updated\n");
			}

			// Заготовка для следующего
			bufLen = 0;
			buf[bufLen] = inByte;
			bufLen++;
			hashValue = hash(buf, bufLen, tableSize);
		}
	}

	if (bufLen == 0) {
		printf("Input file is empty!\n");
		exit(1);
	}

	outCode(&outByte, &OutByteLenght, curOutLenght, table[hashValue].code, outFile);

	// Выводим оставшееся в outByte
	outByte = outByte << (8 - OutByteLenght);
	fwrite(&outByte, 1, 1, outFile);

	// Очистка таблицы
	clearTable(table, tableSize);
	free(table);

	free(buf);
	return 0;
}