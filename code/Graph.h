#pragma once

class Graph {
public:
	Graph();
	~Graph();

	void setValue(int x, float y);
	void setValues(int x, float* ys, int dim);
};
