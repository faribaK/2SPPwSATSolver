#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "solver.h"



int W, N, lb, ub;//width fo the strip, number of rectangles, lower bound, upper bound
int *wr, *hr;//width and height of each rectangle; dimension = N
int optimal_height;
int nvars = 0;
int **x, **y, **lr, **ud, *xsize, *ysize;
int *ph, phsize;
//optimization 
bool domainReduc=0, bigRect=0, sameRect=0;
bool output_for_table = 0;

bool createProblemFromfile(FILE* fp) {
	fscanf(fp, "%d", &N);
	fscanf(fp, "%d", &W);
	
	wr = (int *)malloc((N + 1) * sizeof(int));//>0
	hr = (int *)malloc((N + 1) * sizeof(int));//>0

	for (int i = 1; i <= N; i++) {
		int k;
		fscanf(fp, "%d", &k);
		fscanf(fp, "%d", &wr[i]);
		if (wr[i] > W) {
			printf("\nINVALID PROBLEM: ");
			printf("\n width of %d-th rectangle is greater than width of the strip\n", i);
			return false;
		}
		fscanf(fp, "%d", &hr[i]);
	}
	fscanf(fp, "%d", &lb);
	if (lb == EOF)
	{
		int area = 0;
		for (int i = 1; i <= N; i++)
			area += wr[i] * hr[i];
		lb = lower_bound(area);
		ub = upper_bound();
	}
	else 
		fscanf(fp, "%d", &ub);
	if (ub == EOF)
	{
		int area = 0;
		for (int i = 1; i <= N; i++)
			area += wr[i] * hr[i];
		lb = lower_bound(area);
		ub = upper_bound();
	}
	return true;
}

//creates a hard coded 2SPP problem
void createProblem() {
	W = 4;
	N = 4;//>0
	wr = (int *)malloc((N + 1) * sizeof(int));//>0
	hr = (int *)malloc((N + 1) * sizeof(int));//>0
	/*wr[1] = 1; wr[2] = 1; wr[3] = 2; wr[4] = 1;
	hr[1] = 2; hr[2] = 2; hr[3] = 1; hr[4] = 1;*/
	wr[1] = 4; wr[2] = 4; wr[3] = 4; wr[4] = 4;
	hr[1] = 2; hr[2] = 2; hr[3] = 1; hr[4] = 1;
	int area = 0;
	for (int i = 1; i <= N; i++)
		area += wr[i] * hr[i];
	lb = 4;// lower_bound(area);
	ub = 13;
}

int lower_bound(int area){
	int h = (area + W - 1) / W;
	return h;
}

void mark_position(int i, int x0, int y0, int **b, int bsize) {
	int w, h;
	w = wr[i]; h = hr[i];
	for (int y = y0; y <= y0 + h - 1; y++) {
		for (int x = x0; x <= x0 + w - 1; x++) {
			b[y][x] = i;
		}
	}
	/*my($i, $x0, $y0, $b) = @_;
	my($h, $w) = @{$rect[$i] };
	foreach my $y($y0 ..$y0 + $h - 1) {
	foreach my $x($x0 ..$x0 + $w - 1) {
	$b->[$y][$x] = $i;
	}
	}*/
}

int is_available(int i, int x0, int y0, int **b, int bsize) {
	int w, h;
	w = wr[i]; h = hr[i];
	if (x0 + w > W) {
		return 0;
	}
	for (int y = y0; y <= y0 + h - 1; y++) {
		for (int x = x0; x <= x0 + w - 1; x++) {
			if (b[y][x]) {
				return 0;
			}
		}
	}
	return 1;
}

void find_position(int i, int **b, int* x, int* y, int bsize){
	*(x) = -1; *(y) = -1;
	for (int y1 = 0; y1 <= bsize-1; y1++) {
		for (int x1 = 0; x1 <= W - 1; x1++) {
			if (is_available(i, x1, y1, b, bsize)) {
				*(x) = x1; *(y) = y1;
				return;
			}
		}
	}
}

int upper_bound() {
	int total_height = 0;
	for (int i = 1; i <= N; i++)
		total_height += hr[i];
	int **b;
	b = (int**)malloc((total_height+1)*sizeof(int*));
	for (int i = 0; i <= total_height; i++) {
		b[i] = (int*)malloc((W+1)*sizeof(int));
		for (int j = 0; j <= W; j++)
			b[i][j] = 0;
	}
	int H=0;
	for (int i = 1; i <= N; i++) {
		int x, y;
		find_position(i, b, &x, &y, total_height);
		mark_position(i, x, y, b, total_height);
		int temp_H = y + hr[i];
		if(temp_H>H)
			H = temp_H;
	}
	for (int i = 0; i <= total_height; i++) {
		free(b[i]);
	}
	free(b);
	return H;
}

void copy_value(int *source, int* dest, int size) {
	for (int i = 0; i < size; i++) {
		dest[i] = source[i];
	}
}

void init_zero(int *arr, int size) {
	for (int i = 0; i < size; i++) {
		arr[i] =0;
	}
}

int create_initial_vars() {

	//boolean variables for rectangles within domains
	nvars = 0; 
	x = (int**)malloc((N + 1) * sizeof(int*));
	y = (int**)malloc((N + 1) * sizeof(int*));
	xsize = (int*)malloc((N + 1) * sizeof(int));
	ysize = (int*)malloc((N + 1) * sizeof(int));
	int max_width=-1, second_max_width=-1, max_wi=0, second_max_wi=0;
	int max_height = -1, second_max_height = -1, max_hi = 0, second_max_hi = 0;
	/*for (int i = 1; i <= N; i++) {
		int x_width = (W - wr[i]);
		if (x_width > max_width) {
			max_width = x_width;
			max_wi = i;
		}
		else if (x_width <= max_width && x_width > second_max_width) {
			second_max_width = x_width;
			second_max_wi = i;
		}
		int y_width = (ub - hr[i]);
		if (y_width > max_height) {
			max_height = y_width;
			max_hi = i;
		}
		else if (y_width <= max_height && y_width > second_max_height) {
			second_max_height = y_width;
			second_max_hi = i;
		}
	}*/
	for (int i = 1; i <= N; i++) {
		/*int size_x, size_y;
		if (i != max_wi) 
			size_x = max_width+1;
		else 
			size_x = second_max_width+1;
		if (i != max_hi) 
			size_y = max_height+1;
		else 
			size_y = second_max_height=1;*/
		int size_x = W-1+1, size_y=ub-1+1;
		x[i] = (int*)malloc(size_x * sizeof(int));
		xsize[i] = size_x;
		for (int e = 0; e < xsize[i]; e++) {
			if (e <= (W - wr[i])) {
				x[i][e] = ++nvars;
			}
			else
				x[i][e] = 0;
		}
		y[i] = (int*)malloc(size_y * sizeof(int));
		ysize[i] = size_y;
		for (int f = 0; f < ysize[i]; f++) {
			if (f <= (ub - hr[i])) {
				y[i][f] = ++nvars;
			}
			else
				y[i][f] = 0;
		}
	}

	//for (int i = 1; i <= N; i++) {
	//	for (int j = i + 1; j <= N; j++) {
	//		for (int e = 0; e < (W - wr[i]); e++) {
	//			if (x[j][e + wr[i]] == 0) //check var created after initialization but not assigned index [not used yet]
	//				x[j][e + wr[i]] = ++nvars;
	//		}
	//		for (int f = 0; f < (ub - hr[i]); f++) {
	//			//check var not created during initialization; resize and create var
	//			if (y[j][f + hr[i]] == 0) //check var created after initialization but not assigned index [not used yet]
	//				y[j][f + hr[i]] = ++nvars;
	//		}
	//		for (int e = 0; e < (W - wr[j]); e++) {
	//			if (x[i][e + wr[j]] == 0)
	//				x[i][e + wr[j]] = ++nvars;
	//		}
	//		for (int f = 0; f < (ub - hr[j]); f++) {
	//			//check var not created during initialization; resize and create var
	//			if (y[i][f + hr[j]] == 0) //check var created after initialization but not assigned index [not used yet]
	//				y[i][f + hr[j]] = ++nvars;
	//		}
	//	}
	//}

	lr = (int**)malloc((N + 1) * sizeof(int*));
	ud = (int**)malloc((N + 1) * sizeof(int*));
	for (int i = 0; i <= N; i++) {
		lr[i] = (int*)malloc((N + 1) * sizeof(int));
		ud[i] = (int*)malloc((N + 1) * sizeof(int));
		for (int j = 0; j <= N; j++) {
			lr[i][j] = 0;//assign index only if used as boolean variable
			ud[i][j] = 0;
			if (i != 0 && j != 0 && i != j) {
				lr[i][j] = ++nvars; //rectangle goes from1 to N and for each pair ri, rj -> i!=j
				ud[i][j] = ++nvars;
			}
		}
	}
	
}

lbool add_one_clause(int* literals, solver *s, int numlits) {
	veci lits;
	veci_new(&lits);
	lit* begin;
	veci_resize(&lits, 0);

	for (int i = 0; i < numlits; i++) {
		if (literals[i] < 0) {
			veci_push(&lits, lit_neg(toLit(abs(literals[i]) - 1)));
		}
		else {
			veci_push(&lits, toLit(abs(literals[i]) - 1));
		}
	}

	begin = veci_begin(&lits);
	int* end = begin + veci_size(&lits);
	if (!solver_addclause(s, begin, end)) {
		veci_delete(&lits);
		return l_False;
	}
	veci_delete(&lits);
	return l_True;
}

lbool OPPtoCNF(solver *s) {
	//dummy clause for x0
	for (int i = 1; i <= N; i++) {
		for (int e = 0; e <= (W - wr[i]); e++) {
			if (e == (W - wr[i])) {
				int literals[1];
				literals[0] = x[i][e];
				if (add_one_clause(literals, s, 1) == l_False)
					return l_False;
			}
			else {
				int literals[2];
				literals[0] = -x[i][e]; literals[1] = x[i][e + 1];
				if (add_one_clause(literals, s, 2) == l_False)
					return l_False;
			}
		}
		for (int f = 0; f <= (ub - hr[i]); f++) {
			if ( f == (ub - hr[i])) {
				int literals[1];
				literals[0] = y[i][f];
				if (add_one_clause(literals, s, 1) == l_False)
					return l_False;
			}
			else {
				int literals[2];
				literals[0] = -y[i][f]; literals[1] = y[i][f + 1];
				if (add_one_clause(literals, s, 2) == l_False)
					return l_False;
			}
		}
	}
	for (int i = 1; i <= N; i++) {
		for (int j = i + 1; j <= N; j++) {
			bool sidebyside = true, topbottom = true; //big rectangle pair
			bool samedim = false;
			if (bigRect) {
				if (wr[i] + wr[j] > W)
					sidebyside = false;
				if (hr[i] + hr[j] > ub)
					topbottom = false;
			}
			if (sameRect) {
				if (wr[i] == wr[j] && hr[i] == hr[j])
					samedim = true;
			}
			if (!sidebyside && !topbottom) continue; //in this case, ub should be high enough to accomodate both
			int literals[4];
			literals[0] = lr[i][j]; literals[1] = lr[j][i];
			literals[2] = ud[i][j]; literals[3] = ud[j][i];
			//lr[i][j]  V lr[j][i] V ud[i][j] V ud[j][i]
			if (!sidebyside) {
				literals[0] = ud[i][j]; literals[1] = ud[j][i];
				if (add_one_clause(literals, s, 2) == l_False)
					return l_False;
			}
			else if (!topbottom) {
				if (add_one_clause(literals, s, 2) == l_False)
					return l_False;
			}
			else if (samedim) {//set lr[j][i] to false=>remove literal lr[j][i]
				literals[1] = literals[2]; literals[2] = literals[3];
				if (add_one_clause(literals, s, 3) == l_False)
					return l_False;
			}
			else {
				if (add_one_clause(literals, s, 4) == l_False)
					return l_False;
			}

			//~lr[i][j] clauses and ~lr[j][i] clause
			if (sidebyside) {
				for (int e = -1; e < (W - wr[i]); e++) {
					/*if (x[j][e + wr[i]] == 0)
						x[j][e + wr[i]] = ++nvars;*/
						//if ((W - wr[i]) == 0)break;
					int literals[3];
					literals[0] = -lr[i][j]; literals[1] = x[i][e];
					literals[2] = -x[j][e + wr[i]];
					if (e == -1) {
						int swap = literals[1];
						literals[1] = literals[2];
						literals[2] = swap;
						if (add_one_clause(literals, s, 2) == l_False)
							return l_False;
					}
					else if ((e + 1) == (W - wr[i])) {
						if (add_one_clause(literals, s, 2) == l_False)
							return l_False;
					}
					else if (xsize[j] == (e + 1 + wr[i])) {
						if (add_one_clause(literals, s, 2) == l_False)
							return l_False;
						else
							break;
					}
					else if (x[j][e + 1 + wr[i]] == 0) {
						if (add_one_clause(literals, s, 2) == l_False)
							return l_False;
						else
							break;
					}
					else {
						if (add_one_clause(literals, s, 3) == l_False)
							return l_False;
					}
				}
				if (!samedim) {//donot add any clase with ~lr[j][i]
					for (int e = -1; e < (W - wr[j]); e++) {
						//if ((W - wr[j]) == 0)break;
						/*if (x[i][e + wr[j]] == 0)
						x[i][e + wr[j]] = ++nvars;*/
						int literals[3];
						literals[0] = -lr[j][i]; literals[1] = x[j][e];
						literals[2] = -x[i][e + wr[j]];
						if (e == -1) {
							int swap = literals[1];
							literals[1] = literals[2];
							literals[2] = swap;
							if (add_one_clause(literals, s, 2) == l_False)
								return l_False;
						}
						else if ((e + 1) == (W - wr[i])) {
							if (add_one_clause(literals, s, 2) == l_False)
								return l_False;
						}
						else if (xsize[i] == (e + 1 + wr[j])) {
							if (add_one_clause(literals, s, 2) == l_False)
								return l_False;
							else
								break;
						}
						else if (x[i][e + 1 + wr[j]] == 0) {
							if (add_one_clause(literals, s, 2) == l_False)
								return l_False;
							else break;
						}
						else {
							if (add_one_clause(literals, s, 3) == l_False)
								return l_False;
						}
					}
				}
				else
				{
					literals[0] = lr[i][j]; literals[1] = -ud[j][i];
					if (add_one_clause(literals, s, 2) == l_False)
						return l_False;
				}
			}
			//~ud[i][j] clauses and ~ud[j][i] clause
			if (topbottom) {
				for (int f = -1; f < (ub - hr[i]); f++) {
					//if ((ub - hr[i]) == 0)break;
					/*if (y[j][f + hr[i]] == 0)
						y[j][f + hr[i]] = ++nvars;*/
					int literals[3];
					literals[0] = -ud[i][j]; literals[1] = y[i][f];
					literals[2] = -y[j][f + hr[i]];
					if (f == -1) {
						int swap = literals[1];
						literals[1] = literals[2];
						literals[2] = swap;
						if (add_one_clause(literals, s, 2) == l_False)
							return l_False;
					}
					else if ((f + 1) == (ub - hr[i])) {
						if (add_one_clause(literals, s, 2) == l_False)
							return l_False;
					}
					else if (ysize[j] == (f + 1 + hr[i])) {
						if (add_one_clause(literals, s, 2) == l_False)
							return l_False;
						else
							break;
					}
					else if (y[j][f + 1 + hr[i]] == 0) {
						if (add_one_clause(literals, s, 2) == l_False)
							return l_False;
						else break;
					}
					else {
						if (add_one_clause(literals, s, 3) == l_False)
							return l_False;
					}
				}
				for (int f = -1; f < (ub - hr[j]); f++) {
					//if ((ub - hr[j]) == 0)break;
					/*if (y[i][f + hr[j]] == 0)
					y[i][f + hr[j]] = ++nvars;*/
					int literals[3];
					literals[0] = -ud[j][i]; literals[1] = y[j][f];
					literals[2] = -y[i][f + hr[j]];
					if (f == -1) {
						int swap = literals[1];
						literals[1] = literals[2];
						literals[2] = swap;
						if (add_one_clause(literals, s, 2) == l_False)
							return l_False;
					}
					else if ((f + 1) == (ub - hr[i])) {
						if (add_one_clause(literals, s, 2) == l_False)
							return l_False;
					}
					else if (ysize[i] == (f + 1 + hr[j])) {
						if (add_one_clause(literals, s, 2) == l_False)
							return l_False;
						else
							break;
					}
					else if (y[i][f + 1 + hr[j]] == 0) {
						if (add_one_clause(literals, s, 2) == l_False)
							return l_False;
						else break;
					}
					else {
						if (add_one_clause(literals, s, 3) == l_False)
							return l_False;
					}
				}
			}
		}
	}
	return solver_simplify(s);
}

lbool SPPEXTtoCNF(solver *s) {
	//ph[0] => ph[lb]; ph[phsize-1] => ph[ub]
	phsize = ub - lb + 1;
	ph = (int*)malloc((phsize) * sizeof(int));
	for (int i = 0; i <= phsize - 1; i++)
		ph[i] = ++nvars;
	for (int i = 0; i <= phsize-1; i++) {
		if (i == phsize - 1) {
			int literals[1];
			literals[0] = ph[i];
			if (add_one_clause(literals, s, 1) == l_False)
				return l_False;
		}
		else {
			int literals[2];
			literals[0] = -ph[i]; literals[1] = ph[i + 1];
			if (add_one_clause(literals, s, 2) == l_False)
				return l_False;
		}
	}
	for (int i = 1; i <= N; i++) {
		for (int o = lb; o <= ub; o++) {
			int literals[2];
			literals[0] = -ph[o-lb]; literals[1] = y[i][o-hr[i]];
			if (add_one_clause(literals, s, 2) == l_False)
				return l_False;
		}
	}
	return solver_simplify(s);
}

int clear_SPP() {
	nvars = 0;
	for (int i = 0; i <= N; i++) {
		if (i > 0) {
			free(x[i]); free(y[i]);
		}
		free(lr[i]); free(ud[i]);
	}
	free(x); free(y); free(lr); free(ud);
	free(xsize); free(ysize);
	free(ph);
}

int clear_Problem() {
	free(wr); free(hr);
}





