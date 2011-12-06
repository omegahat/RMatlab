% Run matlab
%  matlab -nojvm -nosplash -nodisplay 
% and then give the command 
%   basic

initializeR({'RMatlab' '--silent' '--vanilla'})
callR('source', '../tests/converterTests.R')


% Conversion of scalars from matlab and matrix to R.
o = callR('matrix', 0, 2, 3)

% Tests vector conversion to R and matrix from R.
o = callR('matrix', 1:6, 3, 2)

% 
o = callR('matrix', {'a' 'b' 'c' 'd' 'e' 'f'}, 3, 2)


% Numeric
x = [ 1 2; 3 4]
callR('Identity', x)
callR('Identity', 1, 2)
callR('Identity', [1], [2])
callR('Identity', [1 2], [2])

% Logical
callR('Identity', true, false)

% Logical matrix.
x = [true true; false true]
callR('Identity', x)


% Complex
callR('Identity', 1 + i)

callR('Identity', [1 + i  2 + 2i])

callR('Identity', [1 + i  2 + 2i; 3 + 3i 4 + 4i])

callR('Identity', ones(4, 3, 2) + i)


% Characters

d = 'abcdefghijklmnopqrstuvwxyzABCD'
callR('Identity', d)
callR('Identity', reshape(d, [3, 10]))

callR('Identity', reshape(d, [2, 3, 5]))

x = char('a', 'b', 'cd', 'efg')
callR('Identity', x)
z = reshape(x, [2 2 3])
callR('Identity', z)

% callR('Identity', repmat('xyz', [4 3 2 2]))
% Gives dimensions in R of c(4, 9, 2, 2)
% size(repmat(feval('char', 'abc'), [3 2 4]))
% size(repmat(feval('char', 'abcd'), [3 2 4]))



% Struct
i = imfinfo('/home/duncan/rggobi.png')
callR('Identity', i)

% uint8

o = repmat(feval('uint8', 1), [2 3])
z = callR('Identity', o)

o = repmat(feval('uint32', 1), [2 3])
z = callR('Identity', o)

% reshape not defined for uint64,etc. 


img = imread('/home/duncan/rggobi.png');
o = callR('assign', 'bob', img)

s = img(1:5, 1:6, 1)
o = callR('Identity', s) 


%###############################################


% Complex numbers.
o = callR('complex')


% Call R which calls Matlab to compute a simple 
% random number.
o = callR('.MatlabMexCall', 'rand', 1, 1)


% Convert an S3 class object to   
b = callR('s3Test')
b.email


% Test a Matlab struct. 
x.a = 1
x.b = {'abc' 'def_initely'}
x.c = 'wxyz'

callR('Identity', x)

% Matrices
x = rand(3, 5)
callR('Identity', x)

callR('Identity', x, [1 2; 3 4])



% Fix
callR('Identity', char( 'a',  'cd'))
callR('Identity', char( 'a',  'cd',  'efgh'))

%  This creates a list rather than character vector.
callR('Identity', { 'a' 'cd' 'efg'})

callR('Identity', { 1 2 3})

callR('Identity', { true true false})

% But this keeps it as a list.
callR('Identity', { 'a' 'cd' 'efg', 1})




o = callR('l')
o = callR('l', false)


o = callR('nas')

o = callR('naStrings')


