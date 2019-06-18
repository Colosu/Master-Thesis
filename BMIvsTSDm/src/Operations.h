/*
 * Operations.h
 *
 *  Created on: 29 jul. 2017
 *      Author: colosu
 */

#ifndef OPERATIONS_H_
#define OPERATIONS_H_

#include <semaphore.h>
#include <fst/fstlib.h>
#include <string>
#include <zlib.h>
#include "Graph.h"

namespace fst {

typedef struct {
	map<IOpair, int>* IOmap;
	list<IOpair> T1;
	list<IOpair> T2;
	sem_t* sem;
	double* MI;
} args;

void* SqueezinessAux(void * argum);

class Operations {
public:
	Operations();
	~Operations();
	void minimization(Graph* g);
	StdMutableFst* product(Graph* g1, Graph* g2);
	void GenerateRandomTestSuite(Graph* g, int size, list<list<IOpair>> &TS, bool repState, bool repTests);
	int GenerateTSDmTestSuite(Graph* g, int size, int pool, list<list<IOpair>> &TSMI, list<list<IOpair>> &TSTSDm);
	void MutualInformation(Graph* g, list<list<IOpair>> TS, double &MI);
	double MutualInformation(map<IOpair, int>* IOmap, list<IOpair> T1, list<IOpair> T2);

private:
	double MutualInformation(int n1, int n2, double s1, double s2, double mx, double P, double S2, bool eq);
	int GenerateRandomTest(Graph* g, int size, list<IOpair> &T, bool repState);
	bool repeated(list<IOpair> T, list<list<IOpair>> TS);
	bool is_on(int state, int states[], int size);
	string to_St(list<list<IOpair>> TS);
	string to_St(list<IOpair> TS);
};

} /* namespace std */

#endif /* OPERATIONS_H_ */
