# Implementation notes

- The solver implements the weak-form 1D linear finite element method.
- Element stiffness is `k*A/h * [[1,-1],[-1,1]]`.
- Consistent source load is `q*A*h/2 * [1,1]`.
- Both Dirichlet temperatures are imposed by elimination.
- Midpoint error uses the FE interpolated midpoint, not a nodal-only comparison.
- The CSV outputs contain nodal values. Extend them to include midpoint/error columns
  if desired for the final evidence.
- This starter is not yet the complete assignment. Do not claim geometry,
  parallelism, hardware, or backpropagation are complete until implemented and tested.
