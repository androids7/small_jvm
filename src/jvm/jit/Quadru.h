/*
 * Quadru.h
 *
 *  Created on: Oct 27, 2013
 *      Author: inti
 */

#ifndef QUADRU_H_
#define QUADRU_H_

#include "op_quadruplus.h"

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <set>

#include <boost/graph/adjacency_list.hpp>
#include <boost/tuple/tuple.hpp>

namespace jit {
enum value_scope {Constant, Local, Field, Temporal, Useless, Register, Label};
enum value_type {Integer, Boolean, ObjRef,ArrRef};

struct jit_value {
	value_type type;
	value_scope scope;
	union {
		int constant; // used with constants or local variables
	} value;
	std::string toString() const {
		if (scope == Constant) {
			return std::to_string(value.constant);
		}
		else if (scope == Useless)
			return "_";
		else if (scope == Temporal) {
			return "T" + std::to_string(value.constant);
		}
		else if (scope == Local) {
			return "L" + std::to_string(value.constant);
		}
		else if (scope == Label) {
			return "LA" + std::to_string(value.constant);
		}
		return "Wrong";
	}
} ;

extern jit_value  useless_value;

struct Quadr {
	OP_QUAD op;
	jit_value op1;
	jit_value op2;
	jit_value res;
	int label; // -1 if no label is required
	friend std::ostream& operator<< (std::ostream &out, Quadr &q);
};

struct BasicBlock {
	std::vector<Quadr> q;
};

//Define the graph using those classes
typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, BasicBlock* > ControlFlowGraph;
//Some typedefs for simplicity
typedef boost::graph_traits<ControlFlowGraph>::vertex_descriptor vertex_t;
typedef boost::graph_traits<ControlFlowGraph>::edge_descriptor edge_t;

struct Routine {
	unsigned countOfParameters;
	ControlFlowGraph g;
	std::vector<Quadr> q;
	int last_temp;
	std::set<int> freeTmp;

	Routine(unsigned countOfParameters);

	Routine(Routine&& r) {
		g = r.g; r.g = {};
		countOfParameters = r.countOfParameters;
		q = r.q; r.q = {};
		last_temp = r.last_temp;
		freeTmp = r.freeTmp; r.freeTmp = {};
	}

	Routine() {
		last_temp = 0;
		countOfParameters = 0;
	}

	/**
	 * Arithmetic operations
	 */
	jit_value jit_binary_operation(OP_QUAD op, jit_value op1, jit_value op2);

	/**
	 * Used to emit regular quadruplos
	 */
	jit_value jit_regular_operation(OP_QUAD op, jit_value op1, jit_value op2, value_type result_type);
	void jit_regular_operation(OP_QUAD op, jit_value op1, jit_value op2, jit_value resultRef);

	/**
	 * Methods
	 */
	void jit_return_int(jit_value r);

	/**
	 * Assignments
	 */
	void jit_assign_local(jit_value local, jit_value v);

	/**
	 * Utils
	 */
	int getTempId();


	void buildControlFlowGraph();

	/**
	 * Debug
	 */
	void print();
};

/**
 * Functions to deal with jit values
 */
jit_value jit_constant(int c);
jit_value jit_local_field(int index, value_type type);
jit_value jit_label(int pos);


}
#endif /* QUADRU_H_ */