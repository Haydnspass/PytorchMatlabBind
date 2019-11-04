function [y, x_shape] = pseudo_col2row_major(x)

    dim_ix = ndims(x):-1:1;
    x_shape = int64(size(x));
    y = permute(x, dim_ix);
    y = single(y(:));
    
end

