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

//	if (g->getTransducer()->Properties(kIDeterministic, true) == kIDeterministic) {
		if (g->getTransducer()->Properties(kAccessible, true) == kAccessible) {
			if (g->getTransducer()->Properties(kCoAccessible, true) == kCoAccessible) {
				return true;
			}
		}
//	}
	return false;
}

bool Checkups::is_validMutation(Graph* g) {

//	if (g->getTransducer()->Properties(kIDeterministic, true) == kIDeterministic) {
		if (g->getTransducer()->Properties(kAccessible, true) == kAccessible) {
			if (g->getTransducer()->Properties(kCoAccessible, true) == kCoAccessible) {
				return true;
			}
		}
//	}
	return false;
}

bool Checkups::checkMutation(Graph* g, list<list<IOpair>> TS) {

	bool detected = false;
	list<list<IOpair>> aux = TS;
	int size = aux.size();
	int i = 0;
	while (!detected && i < size) {
		detected = detected || checkMutation(g, aux.front());
		aux.pop_front();
		i++;
	}
	return detected;
}

bool Checkups::checkMutation(Graph* g, list<IOpair> T) {

	bool detected = true;
	StdMutableFst* transducer = g->getTransducer()->Copy(true);
	int state = transducer->Start();
//	int oldState = state;
	list<IOpair> aux = T;
	int size = aux.size();
	int i = 0;
	ArcIterator<StdMutableFst> arcIter(*transducer, state);
	while (detected && !arcIter.Done()) {
		if (to_string(arcIter.Value().ilabel) == aux.front().getInput()) {
			if (to_string(arcIter.Value().olabel) == aux.front().getOutput()) {
				detected = checkMutation(transducer, aux, arcIter.Value().nextstate, size, i+1);
			}
		}
		arcIter.Next();
	}
//
//	while (!detected && i < size) {
//		ArcIterator<StdMutableFst> arcIter(*transducer, state);
//		while (!detected && state == oldState && !arcIter.Done()) {
//			if (to_string(arcIter.Value().ilabel) == aux.front().getInput()) {
//				if (to_string(arcIter.Value().olabel) == aux.front().getOutput()) {
//					state = arcIter.Value().nextstate;
//					aux.pop_front();
//				}
//			}
//			if(state == oldState) {
//				arcIter.Next();
//			}
//			ticks++;
//		}
//		if(arcIter.Done()) {
//			detected = true;
//		} else {
//			oldState = state;
//		}
//		i++;
//	}
	delete transducer;
	return detected;
}

bool Checkups::checkMutation(StdMutableFst* transducer, list<IOpair> aux, int state, int size, int i) {

	bool detected = true;
	if (i < size) {
		aux.pop_front();
		ArcIterator<StdMutableFst> arcIter(*transducer, state);
		while (detected && !arcIter.Done()) {
			if (to_string(arcIter.Value().ilabel) == aux.front().getInput()) {
				if (to_string(arcIter.Value().olabel) == aux.front().getOutput()) {
					detected = checkMutation(transducer, aux, arcIter.Value().nextstate, size, i+1);
				}
			}
			arcIter.Next();
		}
	} else {
		detected = false;
	}
	return detected;
}

} /* namespace std */
