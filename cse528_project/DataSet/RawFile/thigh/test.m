fid = fopen('0.raw', 'r');
X = fread(fid);
X = uint8(X);
% X = reshape(X, [4,4,4]);
fclose(fid);

fid = fopen('1.raw', 'r');
Y = fread(fid);
Y = uint8(Y);
Y = reshape(Y, [4,4,4]);
fclose(fid);

clear fid;
clear ans;