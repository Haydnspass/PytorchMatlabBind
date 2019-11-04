%% paths

addpath('src/matlab');
addpath('build');

%%

x = rand(100, 64, 64);

[xf, x_shape] = pseudo_col2row_major(x);
[outf, out_size] = mat_torch_interface(xf, x_shape);
out = pseudo_row2_col_major(outf, out_size);