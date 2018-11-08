#include <iomanip>
#include <iostream>
#include <vector>

#include <nlopt.hpp>

int count = 0;

typedef struct {
	double a, b;
} my_constraint_data;

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

int main(int argc, char *argv[]) {
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
	system("pause");
}