#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <pthread.h>
#include <map>
#include <algorithm>
#include <math.h>
#include "Mapper.h"
using namespace std;
pthread_mutex_t mutex;
pthread_barrier_t barrier;

bool cmp(pair<string, long long>& a,
         pair<string, long long>& b)
{
    return a.second > b.second;
}
  
// Function to sort the map according
// to value in a (key-value) pairs
vector<pair<string, long long>> sortByFileSize(map<string, long long>& M)
{
  
    // Declare vector of pairs
    vector<pair<string, long long> > A;
  
    // Copy key-value pair from Map
    // to vector of pairs
    for (auto& it : M) {
        A.push_back(it);
    }
  
    // Sort using comparator function
    sort(A.begin(), A.end(), cmp);
    for (auto it = A.begin(); it != A.end(); ++it) {
    }
    return A;
}

int get_file_size(std::string filename) // path to file
{
    FILE *p_file = NULL;
    p_file = fopen(filename.c_str(),"rb");
    fseek(p_file,0,SEEK_END);
    int size = ftell(p_file);
    fclose(p_file);
    return size;
}

void readData(ifstream& file, queue<string>* q, map<string, long long> files, string filepath) {
    int number;
    string testfile;
    vector<string> v;
    if (file.is_open()) {
        file >> number;

        for (int i = 0; i < number; i++) {
            file >> testfile;
            v.push_back(testfile);
	    }
    } else {
        cout << "Can't open the file\n";
    }
    for (unsigned int i = 0; i < v.size(); i++) {
        ifstream file(v[i]);
        files[v[i]] = get_file_size(v[i]);
    }
    vector<pair<string, long long>> pairs = sortByFileSize(files);
    for (auto it = pairs.begin(); it != pairs.end(); ++it) {
        q->push(it->first);
    }
}

bool binarySearch(int lp, int rp, int value, int exp) {
    if (rp < lp) {
        return false;
    }
    int mid = (lp + rp) / 2;
    if (value < pow(mid, exp)) {
        return binarySearch(lp, mid, value, exp);
    } else if (value > pow(mid, exp)) {
        return binarySearch(mid, rp, value, exp);
    } else if (pow(mid, exp) == value) {
            return true;
    }
    return false;
}

vector<int> getExponents(int value, int exponents) {
    vector<int> v;
    for (int i = 2; i < exponents + 2; i++) {
        int aux = value;
        int index = 0;
        int base = 2;
        while (aux) {
            aux /= 2;
            index++;
        }
        int higherPower = index;
        int lowerPower = index - 1;
        int leftPointer = pow(base, static_cast<float>(lowerPower) / i) - 1;
        int rightPointer = pow(base, static_cast<float>(higherPower) / i) + 1;
        if (i < lowerPower) {
            for (int j = pow(base, static_cast<float>(lowerPower) / i) - 1;
                j < pow(base, static_cast<float>(higherPower) / i) + 1; j++) {
                if (pow(j, i) == value) {
                    v.push_back(i);
                }
            }
        } else if (value == 1) {
            v.push_back(i);
        }
        
    }
    return v;
}

void* mapFunc(void* arg) {
    Mapper* mapper = ((Mapper *) arg);
    string fileToProcess = "";
    while (!mapper->getFiles()->empty()) {
        pthread_mutex_lock(&mutex);
        if (!mapper->getFiles()->empty()) {
            fileToProcess = mapper->getFiles()->front();
            mapper->popFile();
        } else {
            break;
        }
        pthread_mutex_unlock(&mutex);
        ifstream fin(fileToProcess);
        unsigned int n;
        fin >> n;
        int value;
        for (unsigned int i = 0; i < n; i++) {
            fin >> value;
            vector<int> v = getExponents(value, mapper->getExponents());
            for (auto it : v) {
                mapper->values[it - 2].push_back(value);
            }
        }
        // read from file
        
    }
    pthread_barrier_wait(&barrier);
    pthread_exit(NULL);
}

void* reduceFunc(void* arg) {
    pair<vector<Mapper *>, int> pair1 = *((pair<vector<Mapper *>, int> *) arg);
    int id = pair1.second;
    vector<Mapper *> mappers = pair1.first;
    int noMappers = mappers[0]->getMax();
    pthread_barrier_wait(&barrier);
    pthread_mutex_lock(&mutex);
    int toReduce = id - noMappers;
    unordered_set<int> toReturn;
    for (auto mapper : mappers) {
        for (auto value : mapper->values[toReduce]) {
            toReturn.insert(value);
        }
    }
    cout << "Size of power " << toReduce + 2 << endl;
    cout << toReturn.size() << endl;
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        cout << "Run example:\n\t ./tema1 numberOfMappers numberOfReducers filename" << endl;
        exit(0);
    }
    int noMappers = atoi(argv[1]);
    int noReducers = atoi(argv[2]);
    const int NUM_THREADS = noMappers + noReducers;
    pthread_t threads[NUM_THREADS];
  	int r;
  	long id;
    long ids[NUM_THREADS];
    void *status;
    pthread_barrier_init(&barrier, NULL, NUM_THREADS);
    string filename = argv[3];
    ifstream file(filename, ifstream::in);
    queue<string> filesToRead = {};
    map<string, long long> files;
    vector<Mapper *> mappers;
    readData(file, &filesToRead, files, filename);
    vector<pair<vector<Mapper *>, int>> pairs(noReducers);
    for (id = 0; id < NUM_THREADS; id++) {
        ids[id] = id;
        if (id < noMappers) {
            Mapper* mapper = new Mapper(id, &filesToRead, noReducers, noMappers);
            r = pthread_create(&threads[id], NULL, mapFunc, mapper);
            mappers.push_back(mapper);
        } else if (id >= noMappers) {
            std::pair<vector<Mapper*>, int> pair1;
            pair1.first = mappers;
            pair1.second = ids[id];
            pairs[ids[id] - noMappers] = pair1;
            r = pthread_create(&threads[id], NULL, reduceFunc, &pairs[ids[id] - noMappers]);
        }

		if (r) {
	  		printf("Eroare la crearea thread-ului %ld\n", id);
	  		exit(-1);
		}
  	}

    for (id = 0; id < NUM_THREADS; id++) {
		r = pthread_join(threads[id], &status);

		if (r) {
	  		printf("Eroare la asteptarea thread-ului %ld\n", id);
	  		exit(-1);
		}
  	}

    // cout << "Values from exponent 2 " << mappers[0]->values[0].size() << endl;
    // for (auto it : mappers[0]->values[0]) {
    //     cout << it << " ";
    // }
    // cout << "\nValues from exponent 3 " << mappers[0]->values[1].size() << endl;
    // for (auto it : mappers[0]->values[1]) {
    //     cout << it << " ";
    // }
    // cout << "\nValues from exponent 4 " << mappers[0]->values[2].size() << endl;
    // for (auto it : mappers[0]->values[2]) {
    //     cout << it << " ";
    // }
    // cout << "\nValues from exponent 5 " << mappers[0]->values[3].size() << endl;
    // for (auto it : mappers[0]->values[3]) {
    //     cout << it << " ";
    // }
    // cout << endl;

    pthread_mutex_destroy(&mutex);
    pthread_barrier_destroy(&barrier);
  	pthread_exit(NULL);
}