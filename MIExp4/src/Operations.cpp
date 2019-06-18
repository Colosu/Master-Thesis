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
#include "Graph.h"
#include "Operations.h"

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

void Operations::GenerateTestSuite(Graph* g, int size, list<list<IOpair>> &TS, bool repState, bool repTests) {

	int length = 0;
	int tam  = 0;
	list<IOpair> T;
	while (length < size) {
		T = list<IOpair>();
		do {
			T = list<IOpair>();
			tam = GenerateTest(g, size - length, T, repState);
		} while (!repTests && repeated(T, TS));
		TS.push_back(T);
		length += tam;
	}
}

void Operations::GenerateTestSuite(list<list<IOpair>> &TS, int ver) {
	IOpair IO;
	list<IOpair> T;
	if (ver == 0) {
		T = list<IOpair>();
		IO = IOpair("97", "120");
		T.push_back(IO);
		IO = IOpair("98", "122");
		T.push_back(IO);
		IO = IOpair("99", "119");
		T.push_back(IO);
		IO = IOpair("100", "121");
		T.push_back(IO);
		IO = IOpair("101", "122");
		T.push_back(IO);
		IO = IOpair("102", "122");
		T.push_back(IO);
		TS.push_back(T);
	} else if (ver == 1) {
		T = list<IOpair>();
		IO = IOpair("97", "120");
		T.push_back(IO);
		IO = IOpair("98", "122");
		T.push_back(IO);
		IO = IOpair("105", "122");
		T.push_back(IO);
		TS.push_back(T);
		T = list<IOpair>();
		IO = IOpair("97", "120");
		T.push_back(IO);
		IO = IOpair("106", "117");
		T.push_back(IO);
		IO = IOpair("105", "122");
		T.push_back(IO);
		TS.push_back(T);
	} else if (ver == 2) {
		T = list<IOpair>();
		IO = IOpair("97", "120");
		T.push_back(IO);
		IO = IOpair("98", "122");
		T.push_back(IO);
		IO = IOpair("105", "122");
		T.push_back(IO);
		IO = IOpair("97", "120");
		T.push_back(IO);
		IO = IOpair("106", "117");
		T.push_back(IO);
		IO = IOpair("105", "122");
		T.push_back(IO);
		TS.push_back(T);
	} else if (ver == 3) {
		T = list<IOpair>();
		IO = IOpair("97", "120");
		T.push_back(IO);
		IO = IOpair("98", "122");
		T.push_back(IO);
		IO = IOpair("98", "122");
		T.push_back(IO);
		TS.push_back(T);
		T = list<IOpair>();
		IO = IOpair("97", "120");
		T.push_back(IO);
		IO = IOpair("106", "117");
		T.push_back(IO);
		IO = IOpair("105", "122");
		T.push_back(IO);
		TS.push_back(T);
	}
}

void Operations::MutualInformation(Graph* g, list<list<IOpair>> TS, double &MI) {

	list<list<IOpair>> aux1 = TS;
	MI = 0;
	int size1 = aux1.size();
	int size2 = 0;
	for (int i = 0; i < size1; i++) {
		list<list<IOpair>> aux2 = aux1;
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
	list<IOpair> aux1 = T1;
	list<IOpair> aux3 = T2;
	list<IOpair> aux2;
	int size1 = aux1.size();
	int size2 = 0;
	for (int i = 0; i < size1; i++) {
		aux2 = aux3; //Error garrafal!!! aux2=aux1
		if (T1 == T2) {
			aux2.pop_front();
			aux3.pop_front();
		}
		size2 = aux2.size();
		for (int j = 0; j < size2; j++) {
			if (aux1.front() == aux2.front()) {
				MI += MutualInformation(IOmap, aux1.front());
			}
			aux2.pop_front();
		}
		aux1.pop_front();
	}
	return MI;
}

double Operations::MutualInformation(map<IOpair, int>* IOmap, IOpair T) {

	double reps = IOmap->at(T);

	//	return std::log2(reps)/reps;
		return std::log2(reps+1)/reps;
	//	return std::log2(reps+2)/(reps+2);
}

int Operations::GenerateTest(Graph* g, int size, list<IOpair> &T, bool repState) {

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

} /* namespace std */
