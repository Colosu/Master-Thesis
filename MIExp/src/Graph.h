/*
 * Graph.h
 *
 *  Created on: 29 jul. 2017
 *      Author: colosu
 */

#ifndef GRAPH_H_
#define GRAPH_H_

#include "IOpair.h"
#include <fst/fstlib.h>

namespace fst {

class Graph {
public:
	Graph();
	Graph(StdMutableFst* trans);
	~Graph();
	StdMutableFst* getTransducer();
	void setTransducer(StdMutableFst* trans);
	void setTransducer(StdMutableFst* trans, int &ticks);
	void setTransducer(StdMutableFst* trans, map<IOpair, int>* iomap);
	map<IOpair, int>* getIOmap();
	void setIOmap(map<IOpair, int>* iomap);

private:
	void generateIOmap();
	void generateIOmap(int &ticks);
	StdMutableFst* transducer;
	map<IOpair,int>* IOmap;

};

} /* namespace std */

#endif /* GRAPH_H_ */
