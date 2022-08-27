#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>

typedef enum { false, true } bool;

typedef struct Node {
	unsigned char *str;		// Указатель на место в памяти, где хранится строка с элементом (без нулевого символа)
	int32_t len;			// Его "длина" - сколько в нем символов
	uint16_t code;			// Номер элемента (его "код", который мы выводим в выходной файл)
} Node;

uint32_t getTableSize(int32_t maxOutLenght);

int32_t isPowerOfTwo(int32_t num);

int32_t areElementsEqual(unsigned char* str, int32_t strLen, Node tableElement);

void outCode(unsigned char* outByte, int32_t* outByteLenght, int32_t curOutLenght, uint16_t elementCode, FILE* outFile);

uint32_t hash(unsigned char* str, int32_t len, uint32_t tableSize);

Node* createTable(uint32_t tableSize);

void addElement(unsigned char* str, int32_t len, uint32_t hashValue, Node* table);

void baseTableFill(Node* table, uint32_t tableSize);

void clearTable(Node* table, uint32_t tableSize);

void printTable(Node* table, uint32_t tableSize);

bool search(unsigned char* buf, int32_t bufLen, Node* table, uint32_t tableSize, uint32_t* hashValue);

int encode(FILE* inFile, FILE* outFile, int32_t maxOutLenght);