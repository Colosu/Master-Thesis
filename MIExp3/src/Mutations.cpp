/*
 * Mutations.cpp
 *
 *  Created on: 29 jul. 2017
 *      Author: colosu
 */

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fst/fstlib.h>
#include "Graph.h"
#include "Mutations.h"
#include "Checkups.h"

namespace fst {

Mutations::Mutations() {

	std::srand(time(NULL));
}

Mutations::~Mutations() {

}

Graph* Mutations::mutateState(Graph* g) {

	Graph* m = new Graph();
	StdMutableFst* mut = g->getTransducer()->Copy();
	map<IOpair, int>* iomap = new map<IOpair, int>();
	*iomap = *g->getIOmap();
	StdArc arc;
	int state = rand() % (mut->NumStates() - 1);

	while (mut->NumArcs(state) == 0) {
		state = rand() % (mut->NumStates() - 1);
	}

	MutableArcIterator<StdMutableFst>* arcIter = new MutableArcIterator<StdMutableFst>(mut, state);
	arcIter->Seek(rand() % mut->NumArcs(state));
	arc = arcIter->Value();

	int obj = rand() % mut->NumStates();
	while (obj == arc.nextstate || obj == state) {
		obj = rand() % mut->NumStates();
	}

	arc.nextstate = obj;
	arcIter->SetValue(arc);
	m->setTransducer(mut, iomap);
	delete arcIter;

	return m;
}

} /* namespace std */
