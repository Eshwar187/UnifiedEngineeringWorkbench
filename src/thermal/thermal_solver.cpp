#include "thermal/thermal_solver.hpp"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <vector>
#include <algorithm>

namespace thermal {
double exact_temperature(double x, double L, double k, double q,
                         double TL, double TR) {
    return TL + (TR - TL) * x / L + (q / (2.0 * k)) * x * (L - x);
}

// Dense Gaussian elimination with partial pivoting. Adequate for the
// assignment's small 1D meshes; no external numerical library is needed.
static std::vector<double> solve_linear(std::vector<std::vector<double>> M,
                                        std::vector<double> rhs) {
    const int n = static_cast<int>(rhs.size());
    for (int col = 0; col < n; ++col) {
        int pivot = col;
        for (int r = col + 1; r < n; ++r)
            if (std::abs(M[r][col]) > std::abs(M[pivot][col])) pivot = r;
        if (std::abs(M[pivot][col]) < 1e-14)
            throw std::runtime_error("Singular stiffness matrix");
        std::swap(M[pivot], M[col]);
        std::swap(rhs[pivot], rhs[col]);

        const double d = M[col][col];
        for (int c = col; c < n; ++c) M[col][c] /= d;
        rhs[col] /= d;

        for (int r = 0; r < n; ++r) {
            if (r == col) continue;
            const double factor = M[r][col];
            for (int c = col; c < n; ++c) M[r][c] -= factor * M[col][c];
            rhs[r] -= factor * rhs[col];
        }
    }
    return rhs;
}

Result solve_uniform_rod(int N, double L, double A, double k, double q,
                         double TL, double TR) {
    if (N <= 0 || L <= 0 || A <= 0 || k <= 0)
        throw std::invalid_argument("N, L, A and k must be positive");
    const int n = N + 1;
    const double h = L / N;
    std::vector<std::vector<double>> K(n, std::vector<double>(n, 0.0));
    std::vector<double> F(n, 0.0);

    // Element stiffness: (k*A/h) [[1,-1],[-1,1]]
    // Consistent heat-generation load: (q*A*h/2) [1,1]
    for (int e = 0; e < N; ++e) {
        const double ke = k * A / h;
        const double fe = q * A * h / 2.0;
        K[e][e] += ke;       K[e][e+1] -= ke;
        K[e+1][e] -= ke;     K[e+1][e+1] += ke;
        F[e] += fe;          F[e+1] += fe;
    }

    // Apply prescribed temperatures by elimination, preserving RHS.
    auto apply_dirichlet = [&](int node, double value) {
        for (int r = 0; r < n; ++r) {
            if (r == node) continue;
            F[r] -= K[r][node] * value;
            K[r][node] = 0.0;
        }
        for (int c = 0; c < n; ++c) K[node][c] = 0.0;
        K[node][node] = 1.0;
        F[node] = value;
    };
    apply_dirichlet(0, TL);
    apply_dirichlet(n - 1, TR);

    const auto T = solve_linear(std::move(K), std::move(F));
    Result out;
    out.elements = N;
    out.nodes.reserve(n);
    for (int i = 0; i < n; ++i) out.nodes.push_back({i * h, T[i]});

    // Evaluate FE linear interpolation at element midpoints.
    for (int e = 0; e < N; ++e) {
        const double xmid = (e + 0.5) * h;
        const double femid = 0.5 * (T[e] + T[e+1]);
        const double err = std::abs(femid - exact_temperature(xmid, L, k, q, TL, TR));
        out.max_midpoint_error = std::max(out.max_midpoint_error, err);
    }
    return out;
}

bool write_csv(const Result& result, const std::string& path) {
    std::ofstream f(path);
    if (!f) return false;
    f << "x_m,temperature_C\n" << std::setprecision(12);
    for (const auto& node : result.nodes) f << node.x << ',' << node.temperature << '\n';
    return static_cast<bool>(f);
}
}
