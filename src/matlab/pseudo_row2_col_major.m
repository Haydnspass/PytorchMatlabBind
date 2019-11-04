function y = pseudo_row2_col_major(x_flat, x_size)
    y = permute(reshape(x_flat, fliplr(double(squeeze(x_size')))), numel(x_size):-1:1);
end

