#pragma once
#include <vector>
#include <string>

namespace thermal {
struct Node { double x{}; double temperature{}; };
struct Result {
    int elements{};
    std::vector<Node> nodes;
    double max_midpoint_error{};
};

Result solve_uniform_rod(int elements,
                         double L = 1.0,
                         double A = 1e-4,
                         double k = 10.0,
                         double q = 10000.0,
                         double left_temperature = 100.0,
                         double right_temperature = 20.0);

double exact_temperature(double x, double L = 1.0,
                         double k = 10.0, double q = 10000.0,
                         double left_temperature = 100.0,
                         double right_temperature = 20.0);

bool write_csv(const Result& result, const std::string& path);
}
