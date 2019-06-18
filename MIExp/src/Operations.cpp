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
	list<list<IOpair>> aux2;
	MI = 0;
	int size1 = aux1.size();
	int size2 = 0;
//	list<IOpair> Ts = list<IOpair>();
//	list<IOpair> aux;
//
//	Ts.sort();
//	for (int i = 0; i < size1; i++) {
//		aux = aux1.front();
//		aux.sort();
//		Ts.merge(aux);
//		aux1.pop_front();
//	}
//
//	MI = MutualInformation(g->getIOmap(), Ts, Ts);

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
	list<int> naux4;
//	double s1;
//	double s2;
//	double S1;
//	double S2;
//	double P;

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

//	s1 = 0;
//	naux1 = n1;
//	naux2 = mx1;
//	for (long unsigned int i = 0; i < n1.size(); i++) {
////		s1 += (double)naux1.front() / (double)naux2.front();
//		s1 += 1 / (double)naux2.front();
//		naux1.pop_front();
//		naux2.pop_front();
//	}
//	naux1 = count1;
//	naux2 = mx;
//	for (long unsigned int i = 0; i < count1.size(); i++) {
////		s1 += (double)naux1.front() / (double)naux2.front();
//		s1 += 1 / (double)naux2.front();
//		naux1.pop_front();
//		naux2.pop_front();
//	}
//
//	s2 = 0;
//	naux1 = n2;
//	naux2 = mx2;
//	for (long unsigned int i = 0; i < n2.size(); i++) {
////		s2 += (double)naux1.front() / (double)naux2.front();
//		s2 += 1 / (double)naux2.front();
//		naux1.pop_front();
//		naux2.pop_front();
//	}
//	naux1 = count2;
//	naux2 = mx;
//	for (long unsigned int i = 0; i < count2.size(); i++) {
////		s2 += (double)naux1.front() / (double)naux2.front();
//		s2 += 1 / (double)naux2.front();
//		naux1.pop_front();
//		naux2.pop_front();
//	}
////	s1 = T1.size();
////	s2 = T2.size();
//
//	S1 = 0;
//	naux1 = n1;
//	naux3 = mx1;
//	for (long unsigned int i = 0; i < n1.size(); i++) {
//		naux2 = n2;
//		naux4 = mx2;
//		for (long unsigned int j = 0; j < n2.size(); j++) {
////			S1 += ((double)naux1.front()/(double)naux3.front()) * ((double)naux2.front()/(double)naux4.front());
////			S1 += (double)naux1.front() * (double)naux2.front();
//			S1 += (1/(double)naux3.front()) * (1/(double)naux4.front());
//			naux2.pop_front();
//			naux4.pop_front();
//		}
//		naux2 = count2;
//		naux4 = mx;
//		for (long unsigned int j = 0; j < count2.size(); j++) {
////			S1 += ((double)naux1.front()/(double)naux3.front()) * ((double)naux2.front()/(double)naux4.front());
////			S1 += (double)naux1.front() * (double)naux2.front();
//			S1 += (1/(double)naux3.front()) * (1/(double)naux4.front());
//			naux2.pop_front();
//			naux4.pop_front();
//		}
//		naux1.pop_front();
//		naux3.pop_front();
//	}
//
//	naux1 = count1;
//	naux3 = mx;
//	for (long unsigned int i = 0; i < count1.size(); i++) {
//		naux2 = n2;
//		naux4 = mx2;
//		for (long unsigned int j = 0; j < n2.size(); j++) {
////			S1 += ((double)naux1.front()/(double)naux3.front()) * ((double)naux2.front()/(double)naux4.front());
////			S1 += (double)naux1.front() * (double)naux2.front();
//			S1 += (1/(double)naux3.front()) * (1/(double)naux4.front());
//			naux2.pop_front();
//			naux4.pop_front();
//		}
//		naux2 = count2;
//		naux4 = mx;
//		for (long unsigned int j = 0; j < count2.size(); j++) {
//			if (j != i) {
////				S1 += ((double)naux1.front()/(double)naux3.front()) * ((double)naux2.front()/(double)naux4.front());
////				S1 += (double)naux1.front() * (double)naux2.front();
//				S1 += (1/(double)naux3.front()) * (1/(double)naux4.front());
//			}
//			naux2.pop_front();
//			naux4.pop_front();
//		}
//		naux1.pop_front();
//		naux3.pop_front();
//	}
//
//	S2 = 0;
//	naux1 = count1;
//	naux2 = count2;
//	naux3 = mx;
//	for (long unsigned int i = 0; i < count1.size(); i++) {
////		S2 += ((double)naux1.front()/(double)naux3.front()) * ((double)naux2.front()/(double)naux3.front()) * (double)naux3.front();
////		S2 += ((double)naux1.front()/(double)naux3.front()) * ((double)naux2.front()/(double)naux3.front()) * (1/(double)naux3.front());
////		S2 += (double)naux1.front() * (double)naux2.front() * (double)naux3.front();
////		S2 += (double)naux1.front() * (double)naux2.front() * (1/(double)naux3.front());
////		S2 += min((double)naux1.front(), (double)naux2.front());
////		S2 += (1/(double)naux3.front()) * (1/(double)naux3.front()) * (double)naux3.front();
////		S2 += (1/(double)naux3.front()) * (1/(double)naux3.front()) * (1/(double)naux3.front());
//		S2 += ((double)naux1.front()/(double)naux3.front()) * ((double)naux2.front()/(double)naux3.front());
//		naux1.pop_front();
//		naux2.pop_front();
//		naux3.pop_front();
//	}
//	S1 = S1/(s1*s2);
//	S2 = S2/(s1*s2);
//
//	P = 1 - S1;

	naux1 = count1;
	naux2 = count2;
	naux3 = mx;
	MI = 0;
	for (long unsigned int i = 0; i < count1.size(); i++) {
//		MI += MutualInformation(naux1.front(), naux2.front(), s1, s2, naux3.front(), P, S2, T1 == T2);
		MI += MutualInformation(naux1.front(), naux2.front(), 0, 0, naux3.front(), 0, 0, T1 == T2);
		naux1.pop_front();
		naux2.pop_front();
		naux3.pop_front();
	}
	return MI;
}

double Operations::MutualInformation(int n1, int n2, double s1, double s2, double mx, double P, double S2, bool eq) {

//	long double dist = ((double)n1/(mx*s1))*((double)n2/(mx*s2));
//	long double dist = ((double)1/(mx*s1))*((double)1/(mx*s2));
//	long double dist = ((double)n1/s1)*((double)n2/s2);
//	long double dist = ((double)1/mx)*((double)1/mx);

//	long double joint = (dist*mx*P)/S2;
//	long double joint = (dist*(P/mx))/S2;
//	long double joint = (dist*n1*n2*P)/S2;
//	long double joint = min(n1,n2)/S2;
//	long double joint = dist*mx;

	if (eq) {
		int sum = 0;
		for (int i = 1; i < n1; i++) {
			sum += i;
		}
//		return sum*joint*std::log2((joint/dist));
//		return abs(sum*joint*std::log2((joint/dist)));
		return sum*std::log2(mx+1)/mx;
	} else {
//		return n1*n2*joint*std::log2((joint/dist));
//		return abs(n1*n2*joint*std::log2((joint/dist)));
		return n1*n2*std::log2(mx+1)/mx;
	}
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
