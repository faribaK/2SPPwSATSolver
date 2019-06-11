# 2SPPwSATSolver
Extract the file from zip and click on the .sln file inside 2dSPPwSAT folder.
Run the program.
Upon running, it asks for a path to the input file containing a 2SPP problem.
Input file should be in the format below:
  number n of items
  width W for the strip 
  for each rectangle i (i=1,...,n): 
     index i, width $w_i$ and height $h_i$ of item i.
  lower bound (optional)
  upper bound (optional)
After execution is done, corresponding output file named as 'inputfilename_out.txt' can be found in IO/output_files folder.
Output file should be read as follows:
PROBLEM SIZE: clauses - #clauses literals - #literals
TIME TO SOLVE: time in sec
HEIGHT: minimum height found by the tool (optimal/ optimal within range/ )
for each rectangle i (i=1,...,n): 
    (x_i, y_i) coordinate of the lower left corner of rectangle i in found arrangement
