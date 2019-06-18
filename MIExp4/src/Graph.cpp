/*
 * Graph.cpp
 *
 *  Created on: 29 jul. 2017
 *      Author: colosu
 */

#include "Graph.h"

namespace fst {

Graph::Graph() {
	transducer = NULL;
	IOmap = NULL;
}

Graph::Graph(StdMutableFst* trans) {
	transducer = NULL;
	IOmap = NULL;
	setTransducer(trans);
}

Graph::~Graph() {
	if (transducer != NULL) {
		delete transducer;
	}
	transducer = NULL;
	if (IOmap != NULL) {
		delete IOmap;
	}
	IOmap = NULL;
}

StdMutableFst* Graph::getTransducer() {
	return transducer;
}

void Graph::setTransducer(StdMutableFst* trans) {
	if (transducer != NULL) {
		delete transducer;
	}
	transducer = trans;
	generateIOmap();
}

void Graph::setTransducer(StdMutableFst* trans, int &ticks) {
	if (transducer != NULL) {
		delete transducer;
	}
	transducer = trans;
	generateIOmap(ticks);
}

void Graph::setTransducer(StdMutableFst* trans, map<IOpair, int>* iomap) {
	if (transducer != NULL) {
		delete transducer;
	}
	transducer = trans;
	setIOmap(iomap);
}

map<IOpair, int>* Graph::getIOmap() {
	return IOmap;
}

void Graph::setIOmap(map<IOpair, int>* iomap) {
	if (IOmap != NULL) {
		delete IOmap;
	}
	IOmap = iomap;
}

void Graph::generateIOmap() {

	IOmap = new map<IOpair, int>();
	int state = transducer->Start();
	IOpair IO = IOpair();
	while (state < transducer->NumStates()) {
		for (ArcIterator<StdMutableFst> arcIter(*transducer, state); !arcIter.Done(); arcIter.Next()) {
			IO.setInput(to_string(arcIter.Value().ilabel));
			IO.setOutput(to_string(arcIter.Value().olabel));
			if(IOmap->find(IO) != IOmap->end()) {
				IOmap->at(IO)++;
			} else {
				IOmap->emplace(IO, 1);
			}
		}
		state++;
	}

}

void Graph::generateIOmap(int &ticks) {

	IOmap = new map<IOpair, int>();
	int state = transducer->Start();
	IOpair IO = IOpair();
	while (state < transducer->NumStates()) {
		for (ArcIterator<StdMutableFst> arcIter(*transducer, state); !arcIter.Done(); arcIter.Next()) {
			ticks++;
			IO.setInput(to_string(arcIter.Value().ilabel));
			IO.setOutput(to_string(arcIter.Value().olabel));
			if(IOmap->find(IO) != IOmap->end()) {
				IOmap->at(IO)++;
			} else {
				IOmap->emplace(IO, 1);
			}
		}
		state++;
	}

}

} /* namespace std */
