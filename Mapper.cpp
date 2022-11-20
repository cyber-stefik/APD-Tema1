#include "Mapper.h"


int Mapper::getId() {
    return this->id;
}
vector<vector<int>> Mapper::getValues() {
    return this->values;
}
queue<string>* Mapper::getFiles() {
    return this->filesToRead;
}

void Mapper::popFile() {
	this->filesToRead->pop();
}

int Mapper::getExponents() {
    return this->exponents;
}

void Mapper::setExponents(int exponents) {
    this->exponents = exponents;
}

int Mapper::getMax() {
    return this->maxMappers;
}

Mapper::~Mapper() {
	std::cout << "Destroyed object\n";
}
