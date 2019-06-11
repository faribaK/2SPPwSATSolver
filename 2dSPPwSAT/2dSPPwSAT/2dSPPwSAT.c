// 2dSPPwSAT.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdio.h>
#include <string.h> 
#include <stdlib.h>
#include <time.h>
#include "SPPProblem.h"
//#include <unistd.h>
//#include <signal.h>
//#include <zlib.h>
//#include <sys/time.h>
//#include <sys/resource.h>

//=================================================================================================
// Helpers:


// Reads an input stream to end-of-file and returns the result as a 'char*' terminated by '\0'
// (dynamic allocation in case 'in' is standard input).
//
//char* readFile(FILE *  in)
//{
//	char*   data = malloc(65536);
//	int     cap = 65536;
//	int     size = 0;
//
//	while (!feof(in)) {
//		if (size == cap) {
//			cap *= 2;
//			data = realloc(data, cap);
//		}
//		size += fread(&data[size], 1, 65536, in);
//	}
//	data = realloc(data, size + 1);
//	data[size] = '\0';
//
//	return data;
//}

//static inline double cpuTime(void) {
//    struct rusage ru;
//    getrusage(RUSAGE_SELF, &ru);
//    return (double)ru.ru_utime.tv_sec + (double)ru.ru_utime.tv_usec / 1000000; }


//=================================================================================================
// DIMACS Parser:


static inline void skipWhitespace(char** in) {
	while ((**in >= 9 && **in <= 13) || **in == 32)
		(*in)++;
}

static inline void skipLine(char** in) {
	for (;;) {
		if (**in == 0) return;
		if (**in == '\n') { (*in)++; return; }
		(*in)++;
	}
}

static inline int parseInt(char** in) {
	int     val = 0;
	int    _neg = 0;
	skipWhitespace(in);
	if (**in == '-') _neg = 1, (*in)++;
	else if (**in == '+') (*in)++;
	if (**in < '0' || **in > '9') fprintf(stderr, "PARSE ERROR! Unexpected char: %c\n", **in), exit(1);
	while (**in >= '0' && **in <= '9')
		val = val * 10 + (**in - '0'),
		(*in)++;
	return _neg ? -val : val;
}

static void readClause(char** in, solver* s, veci* lits) {
	int parsed_lit, var;
	veci_resize(lits, 0);
	for (;;) {
		parsed_lit = parseInt(in);
		if (parsed_lit == 0) break;
		var = abs(parsed_lit) - 1;
		veci_push(lits, (parsed_lit > 0 ? toLit(var) : lit_neg(toLit(var))));
	}
}

static lbool parse_DIMACS_main(char* in, solver* s) {
	veci lits;
	veci_new(&lits);

	for (;;) {
		skipWhitespace(&in);
		if (*in == 0)
			break;
		else if (*in == 'c' || *in == 'p')
			skipLine(&in);
		else {
			lit* begin;
			readClause(&in, s, &lits);
			begin = veci_begin(&lits);
			if (!solver_addclause(s, begin, begin + veci_size(&lits))) {
				veci_delete(&lits);
				return l_False;
			}
		}
	}
	veci_delete(&lits);
	return solver_simplify(s);
}


// Inserts problem into solver. Returns FALSE upon immediate conflict.
//
//static lbool parse_DIMACS(FILE * in, solver* s) {
//	char* text = readFile(in);
//	lbool ret = parse_DIMACS_main(text, s);
//	free(text);
//	return ret;
//}


//=================================================================================================

static lbool create_CNF(solver *s)
{
	int      n_vars = 5;
	//int    n_clauses = *(int*)(header+12);

	
	veci lits;
	veci_new(&lits);

	lit* begin;
	
	veci_resize(&lits, 0);
	//c[0] = toLit(0); c[1] = toLit(-5); c[2] = toLit(4);
	
	veci_push(&lits, toLit(abs(1)-1));
	veci_push(&lits, lit_neg(toLit(abs(-5)-1)));
	veci_push(&lits, toLit(abs(4)-1));

	begin = veci_begin(&lits);
	int* end = begin + veci_size(&lits);
	if (!solver_addclause(s, begin, end)) {
		veci_delete(&lits);
		return l_False;
	}

	//c[0] = toLit(-1); c[1] = toLit(5); c[2] = toLit(3); c[2] = toLit(4);
	veci_resize(&lits, 0);
	veci_push(&lits, lit_neg(toLit(abs(-1)-1)));
	veci_push(&lits, toLit(abs(5)-1));
	veci_push(&lits, lit_neg(toLit(abs(3)-1)));
	veci_push(&lits, toLit(abs(4)-1));

	begin = veci_begin(&lits);
	if (!solver_addclause(s, begin, begin + veci_size(&lits))) {
		veci_delete(&lits);
		return l_False;
	}

	//c[0] = toLit(-3); c[1] = toLit(-4);
	veci_resize(&lits, 0);
	veci_push(&lits, lit_neg(toLit(abs(-3)-1)));
	veci_push(&lits, lit_neg(toLit(abs(-4)-1)));

	begin = veci_begin(&lits);
	if (!solver_addclause(s, begin, begin + veci_size(&lits))) {
		veci_delete(&lits);
		return l_False;
	}

	/*veci_resize(&lits, 0);
	veci_push(&lits, toLit(abs(1) - 1));
	

	begin = veci_begin(&lits);
	if (!solver_addclause(s, begin, begin + veci_size(&lits))) {
		veci_delete(&lits);
		return l_False;
	}

	veci_resize(&lits, 0);
	veci_push(&lits, toLit(abs(2) - 1));

	begin = veci_begin(&lits);
	if (!solver_addclause(s, begin, begin + veci_size(&lits))) {
		veci_delete(&lits);
		return l_False;
	}*/

	veci_resize(&lits, 0);
	veci_push(&lits, toLit(abs(4) - 1));
	

	begin = veci_begin(&lits);
	if (!solver_addclause(s, begin, begin + veci_size(&lits))) {
		veci_delete(&lits);
		return l_False;
	}

	veci_resize(&lits, 0);
	veci_push(&lits, toLit(abs(5) - 1));

	begin = veci_begin(&lits);
	if (!solver_addclause(s, begin, begin + veci_size(&lits))) {
		veci_delete(&lits);
		return l_False;
	}

	veci_delete(&lits);
	return solver_simplify(s);

}

//=================================================================================================

void printStats(stats* stats, int cpu_time)
{
	double Time = (float)(cpu_time) / (float)(CLOCKS_PER_SEC);
	printf("restarts          : %12d\n", stats->starts);
	printf("conflicts         : %12.0f           (%9.0f / sec      )\n", (double)stats->conflicts, (double)stats->conflicts / Time);
	printf("decisions         : %12.0f           (%9.0f / sec      )\n", (double)stats->decisions, (double)stats->decisions / Time);
	printf("propagations      : %12.0f           (%9.0f / sec      )\n", (double)stats->propagations, (double)stats->propagations / Time);
	printf("inspects          : %12.0f           (%9.0f / sec      )\n", (double)stats->inspects, (double)stats->inspects / Time);
	printf("conflict literals : %12.0f           (%9.2f %% deleted  )\n", (double)stats->tot_literals, (double)(stats->max_literals - stats->tot_literals) * 100.0 / (double)stats->max_literals);
	printf("CPU time          : %12.2f sec\n", Time);
}

//solver* slv;
//static void SIGINT_handler(int signum) {
//    printf("\n"); printf("*** INTERRUPTED ***\n");
//    printStats(&slv->stats, cpuTime());
//    printf("\n"); printf("*** INTERRUPTED ***\n");
//    exit(0); }


//=================================================================================================

void getSuffix(char *suffix) {
	sprintf(suffix, "out");
	if (bigRect) {
		char suffix1[80];
		sprintf(suffix1, "bR_%s", suffix);
		sprintf(suffix, "%s", suffix1);
	}
	if (sameRect) {
		char suffix1[80];
		sprintf(suffix1, "sR_%s", suffix);
		sprintf(suffix, "%s", suffix1);
	}
	if (domainReduc) {
		char suffix1[80];
		sprintf(suffix1, "dmRd_%s", suffix);
		sprintf(suffix, "%s", suffix1);
	}
}

//=================================================================================================

char* getFileNameFromPath(char* filepath) {
	char str[80];
	strcpy(str, filepath);
	const char s[2] = "/";
	char *token, *temp="";

	/* get the first token */
	token = strtok(str, s);

	/* walk through other tokens */
	while (token != NULL) {
		temp = token;
		token = strtok(NULL, s);
	}
	
	temp = strtok(temp, ".");
	return temp;
}

//=================================================================================================

void recreate_solver_with_existing_Problem(solver *s){
	int verbosity_prev = s->verbosity;
	solver_delete(s);
	clear_SPP();
	s = solver_new();
	create_initial_vars();
	OPPtoCNF(s);
	SPPEXTtoCNF(s);
	s->verbosity = verbosity_prev;
}

//=================================================================================================
void writeOutputFile(char* infilepath, solver *s, lbool st, int ub_new, double no_of_clauses, double no_of_literals, clock_t clk_spend, int optimal, int optimal_within_range, int loop) {
	//print rectangles' position (FILE OUTPUT)
	char filename[20];
	strcpy(filename, getFileNameFromPath(infilepath));

	//create appropriate suffix
	char suffix[80];
	getSuffix(suffix);

	char outfilepath[200];
	sprintf(outfilepath, "../../IO/output_files/%s_%s.txt", filename, suffix);
	FILE* fp = fopen(outfilepath, "w");
	if (st == l_True)
	{
		fprintf(fp, "PROBLEM SIZE: clauses - %.0lf literals - %.0lf\n", no_of_clauses, no_of_literals);
		fprintf(fp, "TIME TO SOLVE: %.2lf sec\n", (float)(clk_spend/*clk_end - clk_start*/) / (float)(CLOCKS_PER_SEC));
		fprintf(fp, "HEIGHT: %d", ub_new);
		fprintf(fp, optimal == 1 ? " (optimal)\n" : "\n");
		for (int i = 1; i <= N; i++) {
			//find first truth assignment in horizontal direction
			for (int e = 0; e <= (W - wr[i]); e++) {
				if (s->model.ptr[x[i][e] - 1] == l_True) {
					fprintf(fp, "x%d = %d, ", i, e); break;
				}
			}
			//find first truth assignment in vertical direction
			for (int f = 0; f <= (ub_new - hr[i]); f++) {
				if (s->model.ptr[y[i][f] - 1] == l_True) {
					fprintf(fp, "y%d = %d\n", i, f);
					break;
				}
			}
		}
		if (output_for_table) {
			char outfilepath1[200];
			sprintf(outfilepath1, "../../IO/output_files/all_%s.txt", suffix);
			FILE* fp1 = fopen(outfilepath1, "a");
			fprintf(fp1, "%s %d %d %d %d %.0lf %.0lf %d %.2lf", filename, N, W, lb, ub, no_of_clauses, no_of_literals, ub_new, (float)(clk_spend/*clk_end - clk_start*/) / (float)(CLOCKS_PER_SEC));
			fprintf(fp1, optimal == 1 ? optimal_within_range == 1? " op within range %d\n" : " op %d\n": " %d\n", loop);
			fclose(fp1);
		}
	}
	else {
		fprintf(fp, "\n No solution found within the range in given time.");
	}
	fclose(fp);
}

//=================================================================================================

lbool solve_with_assumption(solver *s, int assumption, clock_t clk) {
	veci lits;
	veci_new(&lits);
	int literals[1];
	literals[0] = ph[assumption - lb];
	veci_resize(&lits, 0);
	veci_push(&lits, toLit(abs(literals[0]) - 1));
	lit* begin = veci_begin(&lits);
	int* end = begin + veci_size(&lits);
	lbool st = solver_solve(s, begin, end);
	printStats(&s->stats, clock() - clk);
	printf(st == l_True ? "SATISFIABLE\n" : st == l_TimeOut ? "TIMEOUT\n" : "UNSATISFIABLE\n");
	printf("\n");
	veci_delete(&lits);
	return st;
}

//=================================================================================================

void solve_2SPP_with_bisection_method(solver *s, char* infilepath, clock_t clk) {
	lbool st;
	//add H assumption
	double no_of_clauses, no_of_literals;
	//int literals[1];
	/*veci lits;
	veci_new(&lits);
	lit* begin;*/
	int loop = 0, optimal = 0, optimal_within_range = 0;
	int lb_new = lb, ub_new = ub, lb_updated = 0, ub_updated = 0;
	clock_t clk_start = clock();
	clock_t clk_best_value;
	while (loop < 1000) {
		int mid = lb_new + ((ub_new - lb_new) / 2);
		printf("mid = %d\n", mid);
		/*literals[0] = ph[mid - lb];
		veci_resize(&lits, 0);
		veci_push(&lits, toLit(abs(literals[0]) - 1));
		begin = veci_begin(&lits);
		int* end = begin + veci_size(&lits);
		st = solver_solve(s, begin, end);
		printStats(&s->stats, clock() - clk);
		printf(st == l_True ? "SATISFIABLE\n" : st == l_TimeOut ? "TIMEOUT\n" : "UNSATISFIABLE\n");
		printf("\n");*/
		st = solve_with_assumption(s, mid, clk);
		if (loop == 0) {
			no_of_clauses = (double)s->stats.clauses;
			no_of_literals = (double)s->stats.clauses_literals;
		}
		if (st == l_True) {
			ub_updated = 1;
			ub_new = mid;
			clk_best_value = clock();
		}
		else {
			lb_new = mid;
			lb_updated = 1;
		}
		printf("lb = %d ub = %d\n", lb_new, ub_new);
		loop++;
		if (st == l_TimeOut)
			break;
		if (((ub_new - lb_new) == 1 && lb_updated != 0) || ((ub_new - lb_new) == 0)) {
			optimal = 1;
			if (((ub_new - lb_new) == 0))
				optimal_within_range = 1;
			printf("last known solution is at ub = %d\n", ub_new);
			if (st != l_True) {
				/*literals[0] = ph[ub_new - lb];
				veci_resize(&lits, 0);
				veci_push(&lits, toLit(abs(literals[0]) - 1));
				begin = veci_begin(&lits);
				int* end = begin + veci_size(&lits);
				st = solver_solve(s, begin, end);
				printStats(&s->stats, clock() - clk);
				printf("\n");
				printf(st == l_True ? "SATISFIABLE\n" : st == l_TimeOut ? "TIMEOUT\n" : "UNSATISFIABLE\n");*/
				st = solve_with_assumption(s, ub_new, clk);
			}
			break;
		}
	}
	clock_t clk_end = clock();
	if (optimal == 0 && ub_updated)
	{
		clk_end = clk_best_value;
	}
	if (((ub_new - lb_new) > 1 || st == l_TimeOut)) {//loop breaks for maximum try or time_out
		if (st == l_TimeOut) {
			//recreate_solver_with_existing_Problem(s);
			int verbosity_prev = s->verbosity;
			solver_delete(s);
			clear_SPP();
			s = solver_new();
			create_initial_vars();
			OPPtoCNF(s);
			SPPEXTtoCNF(s);
			s->verbosity = verbosity_prev;
		}
		printf("loop breaks after maximum try with range = %d\n", ub_new - lb_new);
		printf("last known solution(if any) is at ub = %d\n", ub_new);
		/*literals[0] = ph[ub_new - lb];
		veci_resize(&lits, 0);
		veci_push(&lits, toLit(abs(literals[0]) - 1));
		begin = veci_begin(&lits);
		int* end = begin + veci_size(&lits);
		st = solver_solve(s, begin, end);
		printStats(&s->stats, clock() - clk);
		printf("\n");
		printf(st == l_True ? "SATISFIABLE\n" : st == l_TimeOut ? "TIMEOUT\n" : "UNSATISFIABLE\n");*/
		st = solve_with_assumption(s, ub_new, clk);
	}
	//veci_delete(&lits);
	//st, ub_new, no_of_clauses, no_of_literals, clk_end, clk_start, optimal
	writeOutputFile(infilepath, s, st, ub_new, no_of_clauses, no_of_literals, clk_end - clk_start, optimal, optimal_within_range, loop);
	solver_delete(s);
}

//=================================================================================================

int main(int argc, char** argv)
{
	solver* s = solver_new();
	lbool   st;
	FILE *  in;
	int     clk = clock();
	output_for_table = 1;
	sameRect = true;
	bigRect = true;
	/*if (argc != 2)
		fprintf(stderr, "ERROR! Not enough command line arguments.\n"),
		exit(1);

	in = fopen(argv[1], "rb");
	if (in == NULL)
		fprintf(stderr, "ERROR! Could not open file: %s\n", argc == 1 ? "<stdin>" : argv[1]),
		exit(1);
	st = parse_DIMACS(in, s);
	fclose(in);*/
	char infilepath[200];// = "../../IO/input_files/CGCUT02.txt";
	printf("Please enter the filepath: ");
	scanf("%s", &infilepath);
	printf("\n");
	in = fopen(infilepath, "r");
	if (createProblemFromfile(in) == false){

		fclose(in);
		printf("Trivial problem\nUNSATISFIABLE\n");
		exit(20);
	}
	fclose(in);
	//createProblem();
	printf("upper bound: %d ", ub);
	create_initial_vars();
	st = OPPtoCNF(s);
	st = SPPEXTtoCNF(s);
	//st = create_CNF(s);

	if (st == l_False) {
		solver_delete(s);
		printf("Trivial problem\nUNSATISFIABLE\n");
		exit(20);
	}

	s->verbosity = 1;
	//    slv = s;
	//    signal(SIGINT,SIGINT_handler);

	solve_2SPP_with_bisection_method(s, infilepath, clk);

	clear_SPP();
	clear_Problem();
	printf("\n\n->-> Please see output file ('inputfilename'_out.txt) in 'IO/outputfiles' folder \n->-> to find best height and each ractangles lower left coners position.\n\nPress any key to exit.....");
	getche();
	return 0;
}




