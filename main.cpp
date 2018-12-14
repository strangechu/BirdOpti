#include <iomanip>
#include <iostream>
#include <vector>

#include <nlopt.hpp>
#include "FileManager.h"
#include "Vector3.h"

int count = 0;

typedef struct {
	double a, b;
} my_constraint_data;


typedef struct {
	std::vector<Vector3> v;
} v_data;

double myfunc(const std::vector<double> &x, std::vector<double> &grad, void *my_func_data) // x[0]:x1 x[1]:x2
{
	count++;
	if (!grad.empty()) {
		grad[0] = 0.0;
		grad[1] = 0.5 / sqrt(x[1]);
	}
	return sqrt(x[1]); // objective value: sqrt(x2)
}

double myconstraint(const std::vector<double> &x, std::vector<double> &grad, void *data)
{
	my_constraint_data *d = reinterpret_cast<my_constraint_data*>(data);
	double a = d->a, b = d->b;
	if (!grad.empty()) {
		grad[0] = 3 * a * (a*x[0] + b) * (a*x[0] + b);
		grad[1] = -1.0;
	}
	return ((a*x[0] + b) * (a*x[0] + b) * (a*x[0] + b) - x[1]);
}

double opti_func(const std::vector<double> &x, std::vector<double> &grad, void *data) 
{
	count++;

	v_data *d = reinterpret_cast<v_data*>(data);
	std::vector<Vector3> v = d->v;

	if (!grad.empty()) {
		//
	}

	double sum = 0;
	std::vector<Vector3> p;
	for (int i = 0; i < v.size(); i++) {
		p.push_back(v[i] * x[0]);
	}
	
	for (int i = 0; i < p.size(); i++) {
		//p.push_back(v[i] * x[0]);
	}

	return p[0].length() + p[1].length() + p[2].length() + p[3].length() + p[4].length();
}

int main(int argc, char *argv[]) {
	/*
	//nlopt::opt opt(nlopt::LD_MMA, 2); // algorithm and dimensionality
	nlopt::opt opt(nlopt::LN_COBYLA, 2); // algorithm and dimensionality
	std::vector<double> lb(2);
	lb[0] = -HUGE_VAL; lb[1] = 0;
	opt.set_lower_bounds(lb);
	opt.set_min_objective(myfunc, NULL); // specify objective function to minimizing
	my_constraint_data data[2] = { { 2,0 },{ -1,1 } };
	opt.add_inequality_constraint(myconstraint, &data[0], 1e-8);
	opt.add_inequality_constraint(myconstraint, &data[1], 1e-8);
	opt.set_xtol_rel(1e-4);
	std::vector<double> x(2);
	x[0] = 1.234; x[1] = 5.678;
	double minf;

	try {
		nlopt::result result = opt.optimize(x, minf); // perform optimization
		std::cout << "found minimun after " << count << " evaluations" << std::endl;
		std::cout << "found minimum at f(" << x[0] << "," << x[1] << ") = "
			<< std::setprecision(10) << minf << std::endl;
	}
	catch (std::exception &e) {
		std::cout << "nlopt failed: " << e.what() << std::endl;
	}
	*/

	FileManager* fm = FileManager::getInstance();
	fm->LoadTrackFile("trace01_turn.csv");
	fm->LoadTrackFile("trace02_turn.csv");
	fm->LoadTrackFile("trace03_turn.csv");
	fm->LoadTrackFile("trace04_turn.csv");
	fm->LoadTrackFile("trace05_turn.csv");

	nlopt::opt opt(nlopt::LN_COBYLA, 5);
	std::vector<double> lb = { 10, 10, 10, 10, 10 };
	std::vector<double> ub = { 30, 30, 30, 30, 30 };
	opt.set_lower_bounds(lb);
	opt.set_upper_bounds(ub);

	v_data data;
	data.v.push_back(Vector3(1.0, 1.0, 1.0));
	data.v.push_back(Vector3(1.0, 0.8, -1.0));
	data.v.push_back(Vector3(-1.0, 1.0, 0.8));
	data.v.push_back(Vector3(-1.0, -0.5, 1.0));
	data.v.push_back(Vector3(1.0, -1.0, -0.5));

	opt.set_min_objective(opti_func, &data);
	opt.set_xtol_rel(1e-4);
	std::vector<double> guess = { 20, 20, 20, 20, 20 };
	double min;

	try {
		nlopt::result result = opt.optimize(guess, min); // perform optimization
		std::cout << "found minimun after " << count << " evaluations" << std::endl;
		std::cout << "found minimum at f(" << guess[0] << "," << guess[1] << "," << guess[2] << "," << guess[3] << "," << guess[4] << ") = "
			<< std::setprecision(10) << min << std::endl;
	}
	catch (std::exception &e) {
		std::cout << "nlopt failed: " << e.what() << std::endl;
	}

	system("pause");
}