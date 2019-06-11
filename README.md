# 2SPPwSATSolver
1. Extract the file from zip and click on the .sln file inside 2dSPPwSAT folder.
2. Run the program.
3. Upon running, it asks for a path to the input file containing a 2SPP problem.
4. Input file should be in the format below:
5.   number n of items
6.   width W for the strip 
7.   for each rectangle i (i=1,...,n): 
8.      index i, width w_i and height h_i of item i.
9.   lower bound (optional)
10.  upper bound (optional)
11. After execution is done, corresponding output file named as 'inputfilename_out.txt' can be found in IO/output_files folder.
12. Output file should be read as follows:
13.  PROBLEM SIZE: clauses - #clauses literals - #literals
14.  TIME TO SOLVE: time in sec
15.  HEIGHT: minimum height found by the tool (optimal/ optimal within range/ )
16.  for each rectangle i (i=1,...,n): 
17.     (x_i, y_i) coordinate of the lower left corner of rectangle i in found arrangement
