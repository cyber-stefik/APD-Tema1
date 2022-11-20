#ifndef TEMA1_MAPPER_H
#define TEMA1_MAPPER_H
#include <string>
#include <queue>
#include <unordered_set>
#include <vector>
#include <sstream>
#include <iostream>
using namespace std;

class Mapper {
protected:
    queue<string>* filesToRead;
    int id;
    int exponents;
    int maxMappers;
    
public:
    vector<vector<int>> values;
    Mapper(int id, queue<string>* filesToRead, int exponents, int maxMappers){
        vector<vector<int>> values(exponents);
        this->id = id;
        this->filesToRead = filesToRead;
        this->values = values;
        this->exponents = exponents;
        this->maxMappers = maxMappers;
    };
    virtual ~Mapper();
    int getId();
    vector<vector<int>> getValues();
    queue<string>* getFiles();
    void popFile();
    int getExponents();
    void setExponents(int exponents);
    int getMax();
    std::string toString() {
        std::stringstream files;
        std::stringstream values;
        
        while (!this->filesToRead->empty()) {
            files << filesToRead->front() << ' ';
            filesToRead->pop();
        }
        files << '\n';
        return "Thread id: " + 
            to_string(id) + '\n' +
            "Files to read: " +
            files.str() +
            "Values: " +
            values.str() + "\n";
    }
};

#endif //TEMA1_MAPPER_H
