#include <iostream>
#include <stdio.h>

#include "arbitrary_torch.hpp"

auto dummy_function(torch::Tensor tensor_input) -> torch::Tensor {

    torch::Tensor calc_out = tensor_input * 2;
    return calc_out;
}