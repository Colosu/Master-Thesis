/*
 * IOpair.cpp
 *
 *  Created on: 12 sept. 2018
 *      Author: colosu
 */

#include "IOpair.h"

namespace fst {

IOpair::IOpair() {
}

IOpair::IOpair(string in, string out) {
	input = in;
	output = out;
}

IOpair::~IOpair() {
}

const string& IOpair::getInput() {
	return input;
}

void IOpair::setInput(const string& input) {
	this->input = input;
}

const string& IOpair::getOutput() {
	return output;
}

void IOpair::setOutput(const string& output) {
	this->output = output;
}

bool IOpair::equal(IOpair* other) {
	return this == other;
}

bool operator==(const IOpair& first, const IOpair& second) {
	if (strcmp(first.input.c_str(), second.input.c_str()) == 0) {
		if (strcmp(first.output.c_str(), second.output.c_str()) == 0) {
			return true;
		}
	}
	return false;
}

bool operator<(const IOpair& first, const IOpair& second) {
	if (strcmp(first.input.c_str(), second.input.c_str()) < 0) {
		return true;
	} else if (strcmp(first.input.c_str(), second.input.c_str()) == 0) {
		if (strcmp(first.output.c_str(), second.output.c_str()) < 0) {
			return true;
		}
	}
	return false;
}

} /* namespace fst */
