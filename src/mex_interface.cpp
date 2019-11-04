#include <iostream>
#include <stdio.h>
#include <string>
#include <stdexcept>

#include "arbitrary_torch.hpp"
#include "mex.h"

/** HEADER
 * 
 */
auto mat_to_tensor(mxArray*) -> torch::Tensor;
auto tensor_toMat(torch::Tensor) -> mxArray*;
auto mat_to_tensor_size_spec(const mxArray*, const mxArray*)->torch::Tensor;
auto tensor_to_mat_size_spec(torch::Tensor) -> std::array<mxArray*, 2>;

/**
 * @brief convert matrix to tensor by using a second input which specifies the size of the matirx
 *
 * @param matrix: flattend matrix from matlab in row major format
 * @param sizes: size specification of matrix before it was flattened in matlab
 */
auto mat_to_tensor_size_spec(const mxArray* matrix, const mxArray* sizes) -> torch::Tensor {

    if (!mxIsSingle(matrix)) {
        throw std::invalid_argument("Matrix must be single. Convert to single in MATLAB.");
    }
    if (!mxIsInt64(sizes)) {
        throw std::invalid_argument("Size specification must be int64t. Change in Matlab.");
    }

    // cumberson process of converting matlab array of size specification to torch readable size
    const int mat_ndim = mxGetNumberOfElements(sizes);
    int64_t* mat_sizes = mxGetInt64s(sizes);
    std::vector<int64_t> mat_sizes_(mat_sizes, mat_sizes + mat_ndim);
    torch::ArrayRef<int64_t> mat_size_t(mat_sizes_);

    // get elements of actual matrix and their (pseudo)-size
    const size_t* flattened_size = mxGetDimensions(matrix);
    const int flattend_ndim = 2;  // since MATLAB mxArray at least has 2D
    float* mat = mxGetSingles(matrix);

    std::vector<int64_t> mat_size_(flattened_size, flattened_size + flattend_ndim);
    torch::ArrayRef<int64_t> mat_size_target(mat_size_);

    // copy to tensor
    torch::Tensor tensor = torch::empty(mat_size_target, torch::kFloat);
    std::memcpy(tensor.data_ptr(), mat, (tensor.numel()) * sizeof(float));

    // Squeeze out additional dimension since MATLAB mxArray at least has 2D
    tensor.squeeze_();
    // Reshape as provided by sizes
    tensor = tensor.reshape(mat_size_t);
    return tensor;
}

auto tensor_to_mat_size_spec(torch::Tensor tensor) -> std::array<mxArray*, 2> {
    if (tensor.dtype() != torch::zeros(0, torch::kFloat).dtype()) {  // hacky :D
        throw std::invalid_argument("Tensor must be float (float32, Matlab single). Convert to float before.");
    }

    // store original tensor sizes
    const int tensor_ndim = tensor.ndimension();
    const auto tensor_sizes = tensor.sizes();

    // flatten tensor
    tensor = tensor.reshape(-1);
    const int flat_tensor_ndim = tensor.ndimension();
    const int tensor_numel = tensor.numel();
    const auto flat_tensor_sizes = tensor.sizes();

	size_t* flat_shapes = new size_t[flat_tensor_ndim];  // argh ...
    // const size_t flat_shapes[flat_tensor_ndim];
    for (int i=0; i < flat_tensor_ndim; i++) {
        flat_shapes[i] = flat_tensor_sizes[i];
    }

    // std::cout << "Number of dim: " << tensor_ndim << "\n";

    size_t shape_of_shapes[1];
    shape_of_shapes[0] = tensor_ndim;

    // copy the actual tensor into matrix
    mxArray* mat = mxCreateNumericArray(flat_tensor_ndim, flat_shapes, mxSINGLE_CLASS, mxREAL);
    std::memcpy(mxGetData(mat), tensor.data_ptr(), tensor_numel * sizeof(float));

	delete[] flat_shapes;

    // copy the size specification into matrix
    // std::cout << "Tensor sizes size: " << tensor_sizes.size() << "\n";
    mxArray* mat_size = mxCreateNumericArray(1, shape_of_shapes, mxINT64_CLASS, mxREAL);
    std::memcpy(mxGetData(mat_size), tensor_sizes.begin(), tensor_sizes.size() * sizeof(int64_t));

    std::array<mxArray*, 2> out;
    out[0] = mat;
    out[1] = mat_size;

    return out;
}


/**
 *  Entry point of Matlab.
 *  This assumes a single matrix input (rhs).
 */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    // Get the matrix
    torch::Tensor input = mat_to_tensor_size_spec(prhs[0], prhs[1]);
    
    // Pass it to some other CPP function
    torch::Tensor output = dummy_function(input);

    // Pass it back
    std::array<mxArray*, 2> output_vec = tensor_to_mat_size_spec(output);
    plhs[0] = output_vec[0];
    plhs[1] = output_vec[1];
    
    return;
}
