/*
 * Operations.cpp
 *
 *  Created on: 29 jul. 2017
 *      Author: colosu
 */

#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <pthread.h>
#include <semaphore.h>
#include <fst/fstlib.h>
#include <iostream>     // std::cout
#include <algorithm>    // std::shuffle
#include "Graph.h"
#include "Operations.h"
#include "zlc/zlibcomplete.hpp"
#include <cstring>

namespace fst {

Operations::Operations() {

}

Operations::~Operations() {

}

void Operations::minimization(Graph* g) {
	Minimize(g->getTransducer());
}

StdMutableFst* Operations::product(Graph* g1, Graph* g2) {
	StdMutableFst* a1 = g1->getTransducer()->Copy();
	StdMutableFst* a2 = g2->getTransducer()->Copy();
	//StdMutableFst* prod = Times(a1, a2);
	delete a1;
	delete a2;
	return NULL; //prod;
}

void Operations::GenerateRandomTestSuite(Graph* g, int size, list<list<IOpair>> &TS, bool repState, bool repTests) {

	int length = 0;
	int tam  = 0;
	list<IOpair> T;
	while (length < size) {
		T = list<IOpair>();
		do {
			T = list<IOpair>();
			tam = GenerateRandomTest(g, size - length, T, repState);
		} while (!repTests && repeated(T, TS));
		TS.push_back(T);
		length += tam;
	}
}

int Operations::GenerateTSDmTestSuite(Graph* g, int size, int pool, list<list<IOpair>> &TSMI, list<list<IOpair>> &TSTSDm) {

//	sem_t sem;
	int genSize = 100;
	int genSize2 = pool;
	list<IOpair> TG[genSize];
	string STG[genSize];
	double C[genSize];
	double MI[genSize];
	zlibcomplete::ZLibCompressor* zlib = new zlibcomplete::ZLibCompressor();
	int result = 0;
	string orig = "";
	string comp = "";
	int k = 0;
	int max = 0;
	int min = 10000;
	int pos = 0;
	args argum[genSize2];
//	pthread_t th[genSize2];

//	sem_init(&sem, 0, 1);

	//Generate TSDm test suite

	for (int i = 0; i < genSize; i++) {
		//Initialize Pool
		TG[i] = list<IOpair>();
		GenerateRandomTest(g, size, TG[i], true);
		C[i] = 0;
		result += TG[i].size();
		STG[i] = to_St(TG[i]);
	}

	k = 0;
	while (k < genSize && result > size) {
		for (int i = 0; i < genSize; i++) {
			if (C[i] != -1) {
				orig = "";
				for (int j = 0; j < genSize; j++) {
					if (j != i && C[j] != -1) {
						orig += STG[j];
					}
				}
				comp = zlib->compress(orig);
				C[i] = comp.size();
			}
		}
		max = 0;
		pos = 0;
		for (int i = 0; i < genSize; i++) {
			if (C[i] > max) {
				max = C[i];
				pos = i;
			}
		}
		C[pos] = -1;
		result -= TG[pos].size();
		k++;
	}

	for (int i = 0; i < genSize; i++) {
		if (C[i] > 0) {
			TSTSDm.push_back(TG[i]);
		}
	}

	if (result < size) {
		result += TG[pos].size();
		TSTSDm.push_back(TG[pos]);
	}

	//Generate MI test suite

	result = 0;
	for (int i = 0; i < genSize2; i++) {
		//Initialize Pool
		MI[i] = 0;
		result += TG[i].size();
	}

	k = 0;
	while (k < genSize2 && result > size) {
		for (int i = 0; i < genSize2; i++) {
			if (MI[i] != -1) {
				MI[i] = 0;
				for (int j = 0; j < genSize2; j++) {
					if (j != i && MI[j] != -1) {
						for (int l = j; l < genSize2; l++) {
							if (l != i && MI[l] != -1) {
								MI[i] += MutualInformation(g->getIOmap(), TG[j], TG[l]);
//								argum[l].IOmap = g->getIOmap();
//								argum[l].T1 = TG[j];
//								argum[l].T2 = TG[l];
//								argum[l].sem = &sem;
//								argum[l].MI = &MI[i];
							}
						}
//						for (int l = j; l < genSize2; l++) {
//							if (l != i && MI[l] != -1) {
//								pthread_create(&th[l], NULL, PMutualInformation, (void *)&argum[l]);
//							}
//						}
//						for (int l = j; l < genSize2; l++) {
//							if (l != i && MI[l] != -1) {
//								pthread_join(th[l], NULL);
//							}
//						}
					}
				}
			}
		}
		min = 10000;
		pos = 0;
		for (int i = 0; i < genSize2; i++) {
			if (MI[i] < min && MI[i] != -1) {
				min = MI[i];
				pos = i;
			}
		}
		MI[pos] = -1;
		result -= TG[pos].size();
		k++;
	}

	for (int i = 0; i < genSize2; i++) {
		if (MI[i] > 0) {
			TSMI.push_back(TG[i]);
		}
	}

	if (result < size) {
		result += TG[pos].size();
		TSMI.push_back(TG[pos]);
	}

//	sem_close(&sem);
	delete zlib;

	return result;
}

void Operations::MutualInformation(Graph* g, list<list<IOpair>> TS, double &MI) {

	list<list<IOpair>> aux1 = TS;
	list<list<IOpair>> aux2;
	MI = 0;
	int size1 = aux1.size();
	int size2 = 0;
	for (int i = 0; i < size1; i++) {
		aux2 = aux1;
//		aux2.pop_front();  //Switch for computing MI also on the own test.
		size2 = aux2.size();
		for (int j = 0; j < size2; j++) { //Otro error garrafal!!! j=i+1
			MI += MutualInformation(g->getIOmap(), aux1.front(), aux2.front());
			aux2.pop_front();
		}
		aux1.pop_front();
	}
}

double Operations::MutualInformation(map<IOpair, int>* IOmap, list<IOpair> T1, list<IOpair> T2) {

	double MI = 0;
	list<IOpair> aux1(T1);
	list<IOpair> aux2(T2);
	IOpair aux;
	int size1 = aux1.size();
	int size2 = aux2.size();
	int c = 0;
	int c1 = 0;
	int c2 = 0;
	list<int> n1 = list<int>();
	list<int> n2 = list<int>();
	list<int> mx1 = list<int>();
	list<int> mx2 = list<int>();
	list<int> count1 = list<int>();
	list<int> count2 = list<int>();
	list<int> mx = list<int>();
	list<int> naux1;
	list<int> naux2;
	list<int> naux3;

	aux1.sort();
	aux2.sort();

	while (size1 > 0 && size2 > 0) {
		c = 0;
		if (aux1.front() > aux2.front()) {
			aux = aux2.front();
			mx2.push_front(IOmap->at(aux));
			while(aux2.front() == aux) {
				c++;
				aux2.pop_front();
			}
			size2 -= c;
			n2.push_front(c);
		} else  if (aux1.front() < aux2.front()) {
			aux = aux1.front();
			mx1.push_front(IOmap->at(aux));
			while(aux1.front() == aux) {
				c++;
				aux1.pop_front();
			}
			size1 -= c;
			n1.push_front(c);
		} else {
			aux = aux1.front();
			mx.push_front(IOmap->at(aux));
			while(aux1.front() == aux && aux2.front() == aux) {
				c++;
				aux2.pop_front();
				aux1.pop_front();
			}
			c1 = c;
			c2 = c;
			while (aux1.front() == aux) {
				c1++;
				aux1.pop_front();
			}
			while (aux2.front() == aux) {
				c2++;
				aux2.pop_front();
			}
			size1 -= c1;
			size2 -= c2;
			count1.push_front(c1);
			count2.push_front(c2);
		}
	}

	while (size1 > 0) {
		c = 0;
		aux = aux1.front();
		mx1.push_front(IOmap->at(aux));
		while(aux1.front() == aux) {
			c++;
			aux1.pop_front();
		}
		size1 -= c;
		n1.push_front(c);
	}

	while (size2 > 0) {
		c = 0;
		aux = aux2.front();
		mx2.push_front(IOmap->at(aux));
		while(aux2.front() == aux) {
			c++;
			aux2.pop_front();
		}
		size2 -= c;
		n2.push_front(c);
	}

	naux1 = count1;
	naux2 = count2;
	naux3 = mx;
	MI = 0;
	for (long unsigned int i = 0; i < count1.size(); i++) {
		MI += MutualInformation(naux1.front(), naux2.front(), 0, 0, naux3.front(), 0, 0, T1 == T2);
		naux1.pop_front();
		naux2.pop_front();
		naux3.pop_front();
	}
	return MI;
}

double Operations::MutualInformation(int n1, int n2, double s1, double s2, double mx, double P, double S2, bool eq) {

	if (eq) {
		int sum = 0;
		for (int i = 1; i < n1; i++) {
			sum += i;
		}
		return sum*std::log2(mx+1)/mx;
	} else {
		return n1*n2*std::log2(mx+1)/mx;
	}
}

int Operations::GenerateRandomTest(Graph* g, int size, list<IOpair> &T, bool repState) {

	T = list<IOpair>();
	StdMutableFst* transducer = g->getTransducer()->Copy(true);
	IOpair IO = IOpair();
	int length = 0;
	int state = transducer->Start();
	int states[size];
	int stsize = 0;
	while (length < size && transducer->NumArcs(state) != 0 && (repState || !is_on(state, states, stsize))) {
		ArcIterator<StdMutableFst> arcIter(*transducer, state);
		arcIter.Seek(rand()%transducer->NumArcs(state));
		IO = IOpair();
		IO.setInput(to_string(arcIter.Value().ilabel));
		IO.setOutput(to_string(arcIter.Value().olabel));
		T.push_back(IO);
		length++;
		states[stsize] = state;
		stsize++;
		state = arcIter.Value().nextstate;
	}
	delete transducer;
	return length;
}

bool Operations::repeated(list<IOpair> T, list<list<IOpair>> TS) {

	bool repeat = false;
	list<list<IOpair>> aux = TS;
	int size = aux.size();
	int i = 0;
	while(!repeat && i < size) {
		if(aux.front() == T) {
			repeat = true;
		}
		aux.pop_front();
		i++;
	}
	return repeat;
}

bool Operations::is_on(int state, int states[], int size) {
	bool result = false;
	int i = 0;
	while (i < size && !result) {
		if (state == states[i]) {
			result = true;
		}
		i++;
	}
	return result;
}

string Operations::to_St(list<list<IOpair>> TS) {
	string result = "";
	list<list<IOpair>> aux1 = TS;
	int size1 = aux1.size();
	for (int i = 0; i < size1; i++) {
		result += to_St(aux1.front());
		aux1.pop_front();
	}
	return result;
}

string Operations::to_St(list<IOpair> T) {
	string result = "";
	list<IOpair> aux1 = T;
	int size1 = aux1.size();
	for (int i = 0; i < size1; i++) {
		result += char(stoi(aux1.front().getInput()));
		result += char(stoi(aux1.front().getOutput()));
		aux1.pop_front();
	}
	return result;
}

} /* namespace std */
