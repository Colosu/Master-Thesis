/*
 * Checkups.cpp
 *
 *  Created on: 29 jul. 2017
 *      Author: colosu
 */

#include <fst/fstlib.h>
#include "Graph.h"
#include "Checkups.h"
#include "Operations.h"
#include <unistd.h>
#include <time.h>

namespace fst {

Checkups::Checkups() {

}

Checkups::~Checkups() {

}

bool Checkups::are_equivalent(Graph* g1, Graph* g2) {

	if (Equivalent(*(g1->getTransducer()->Copy()), *(g2->getTransducer()->Copy()))) {
		cout << "are equal" << endl;
		return true;
	} else {
		return false;
	}
}

bool Checkups::is_valid(Graph* g) {

	if (g->getTransducer()->Properties(kIDeterministic, true) == kIDeterministic) {
		if (g->getTransducer()->Properties(kAccessible, true) == kAccessible) {
			if (g->getTransducer()->Properties(kCoAccessible, true) == kCoAccessible) {
				return true;
			}
		}
	}
	return false;
}

bool Checkups::is_validMutation(Graph* g) {

	if (g->getTransducer()->Properties(kIDeterministic, true) == kIDeterministic) {
		//if (g->getTransducer()->Properties(kCoAccessible, true) == kCoAccessible) {
			return true;
		//}
	}
	return false;
}

bool Checkups::checkMutation(Graph* g, list<list<IOpair>> TS, int &ticks) {

	bool detected = false;
	list<list<IOpair>> aux = TS;
	int size = aux.size();
	int i = 0;
	while (!detected && i < size) {
		detected = detected || checkMutation(g, aux.front(), ticks);
		aux.pop_front();
		i++;
	}
	return detected;
}

bool Checkups::checkMutation(Graph* g, list<IOpair> T, int &ticks) {

	bool detected = false;
	StdMutableFst* transducer = g->getTransducer()->Copy(true);
	int state = transducer->Start();
	int oldState = state;
	list<IOpair> aux = T;
	int size = aux.size();
	int i = 0;
	while (!detected && i < size) {
		ArcIterator<StdMutableFst> arcIter(*transducer, state);
		while (!detected && state == oldState && !arcIter.Done()) {
			if (to_string(arcIter.Value().ilabel) == aux.front().getInput()) {
				if (to_string(arcIter.Value().olabel) == aux.front().getOutput()) {
					state = arcIter.Value().nextstate;
					aux.pop_front();
				}
			}
			if(state == oldState) {
				arcIter.Next();
			}
			ticks++;
		}
		if(arcIter.Done()) {
			detected = true;
		} else {
			oldState = state;
		}
		i++;
	}
	delete transducer;
	return detected;
}

} /* namespace std */
