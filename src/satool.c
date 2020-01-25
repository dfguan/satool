/*
 * =====================================================================================
 *
 *       Filename:  satool.c
 *
 *    Description:  tool to manage a SAT file
 *
 *        Version:  1.0
 *        Created:  19/11/2019 17:12:25
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dengfeng Guan (D.G.), dfguan9@gmail.com
 *   Organization:  Harbin Institute of Technology
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <math.h>

#include "sdict.h"
#include "graph.h"
#include "m2da.h"


int sat2agp(char *sat_fn, char *out_fn)
{
	graph_t *g = load_sat(sat_fn);
	asm_t *as = &g->as.asms[g->as.casm];
	vertex_t *vt = g->vtx.vertices;
	path_t *pt = g->pt.paths;
	uint32_t n = as->n;
	uint32_t i;	
	FILE *fp = out_fn ? fopen(out_fn, "w") : stdout;
	for ( i = 0; i < n; ++i) {
		uint32_t m;
		uint32_t *p = parse_path(g, as->pn[i], &m);
		uint32_t j, len = 0, len_ctg, idx = 1;
		//push scaffold name to scfs 
		for ( j = 0; j < m; ++j ) { // pid, length,   
			len_ctg = vt[p[j]>>2].len;	
			fprintf(fp, "%s\t%u\t%u\t%u\tW\t%s\t%u\t%u\t%c\n",  pt[as->pn[i]>>1].name, len + 1, len + len_ctg, idx++,  vt[p[j]>>2].name, 1, len_ctg, p[j]&1 ? '+':'-');
			if (j == m - 1) 				
				len += len_ctg;
			else {
				fprintf(fp, "%s\t%u\t%u\t%u\tN\t%u\tgap\tyes\tna\n",  pt[as->pn[i]>>1].name, len + len_ctg + 1, len + len_ctg + GAP_SZ, idx++, GAP_SZ);
				len += len_ctg + GAP_SZ;
			} 
		}
		//reset scaffold length le rs l_snp_n, r_snp_n
		free(p);
	}
	
	graph_destroy(g);
	if (out_fn) fclose(fp);
	return 0;
}

int simpsat(char *sat_fn, char *out_fn)
{
	//simplify a sat file	
	graph_t *g = load_sat(sat_fn);
	simp_graph(g);
	dump_sat(g, out_fn);
	graph_destroy(g);
	return 0;
}

int sat2seq(char *seq_fn, char *sat_fn, char *out_fn, uint32_t min_l)
{
	graph_t *g = load_sat(sat_fn);
	if (seq_fn) read_seq(g, seq_fn);
	get_path(g, min_l, out_fn);
	graph_destroy(g);	
	return 0;
}
int cmp_vids(const void *p, const void *q)
{
	uint32_t a = *(uint32_t *)p;
	uint32_t b = *(uint32_t *)q;
	if (a == b) return 0;
	else if (a > b) return 1;
	else return -1;
}
int satpathbreak(char *brk_fn, char *sat_fn, char *out_fn)
{
	/*fprintf(stderr, "%s\n", sat_fn);	*/
	graph_t *g = load_sat(sat_fn);
	simp_graph(g);
	FILE *fp = brk_fn[0] == '-' ? stdin	: fopen(brk_fn, "r");
	char vname[1024];
	m2da_t(uint32_t) v;
	m2da_init(v);
	while (fscanf(fp, "%s\n", vname) != EOF) {
		uint32_t pid, vid;
		if (!loc_node(g, vname, &pid, &vid)) {
			m2da_push(uint32_t, v, pid, vid); 	
		} 
	}
	uint32_t i;
	for ( i = 0; i < v.n; ++i) qsort(v.a[i].a, v.a[i].n, sizeof(uint32_t), cmp_vids), break_path(g, i, v.a[i].a, v.a[i].n);
	m2da_destroy(v);
	dump_sat(g, out_fn);
	if (brk_fn[0] != '-') fclose(fp);
	graph_destroy(g);
	
	return 0;
}


int main_simp(int argc, char *argv[])
{
	int c;
	char *program;
	char *sat_fn = 0, *out_fn = 0;
   	(program = strrchr(argv[0], '/')) ? ++program : (program = argv[0]);
	--argc, ++argv;
	while (~(c=getopt(argc, argv, "o:h"))) {
		switch (c) {
			case 'o':
				out_fn  = optarg;
				break;
			default:
				if (c != 'h') fprintf(stderr, "[E::%s] undefined option %c\n", __func__, c);
help:	
				fprintf(stderr, "\nUsage: %s [options] <SAT>\n", program);
				fprintf(stderr, "Options:\n");
				fprintf(stderr, "         -o    STR      output file\n");
				fprintf(stderr, "         -h             help\n");
				return 1;	
		}		
	}
	if (optind + 1 > argc) {
		fprintf(stderr,"[E::%s] require a sat file!\n", __func__); goto help;
	}
	sat_fn = argv[optind];
	fprintf(stderr, "Program starts\n");	
	simpsat(sat_fn, out_fn);
	fprintf(stderr, "Program ends\n");	
	return 0;	
		
}

int main_break(int argc, char *argv[])
{
	int c;
	char *program;
	char *sat_fn = 0, *out_fn = 0;
   	(program = strrchr(argv[0], '/')) ? ++program : (program = argv[0]);
	--argc, ++argv;
	while (~(c=getopt(argc, argv, "o:h"))) {
		switch (c) {
			case 'o':
				out_fn  = optarg;
				break;
			default:
				if (c != 'h') fprintf(stderr, "[E::%s] undefined option %c\n", __func__, c);
help:	
				fprintf(stderr, "\nUsage: %s [options] <BREAK_FILE> <SAT>\n", program);
				fprintf(stderr, "Options:\n");
				fprintf(stderr, "         -o    STR      output file\n");
				fprintf(stderr, "         -h             help\n");
				return 1;	
		}		
	}
	if (optind + 2 > argc) {
		fprintf(stderr,"[E::%s] require a sat file and a break point file!\n", __func__); goto help;
	}
	char *brk_fn = argv[optind++];
	sat_fn = argv[optind];
	fprintf(stderr, "Program starts\n");	
	satpathbreak(brk_fn, sat_fn, out_fn);
	fprintf(stderr, "Program ends\n");	
	return 0;	
}

int main_agp(int argc, char *argv[])
{
	int c;
	char *program;
	char *sat_fn = 0, *out_fn = 0;
   	(program = strrchr(argv[0], '/')) ? ++program : (program = argv[0]);
	--argc, ++argv;
	while (~(c=getopt(argc, argv, "o:h"))) {
		switch (c) {
			case 'o':
				out_fn  = optarg;
				break;
			default:
				if (c != 'h') fprintf(stderr, "[E::%s] undefined option %c\n", __func__, c);
help:	
				fprintf(stderr, "\nUsage: %s [options] <SAT>\n", program);
				fprintf(stderr, "Options:\n");
				fprintf(stderr, "         -o    STR      output file\n");
				fprintf(stderr, "         -h             help\n");
				return 1;	
		}		
	}
	if (optind + 1 > argc) {
		fprintf(stderr,"[E::%s] require a sat file!\n", __func__); goto help;
	}
	sat_fn = argv[optind];
	fprintf(stderr, "Program starts\n");	
	sat2agp(sat_fn, out_fn);
	fprintf(stderr, "Program ends\n");	
	return 0;	
}

int main_gets(int argc, char *argv[])
{
	int c;
	char *program;
	char *sat_fn = 0, *out_fn = 0;
	uint32_t min_l = 0;
   	(program = strrchr(argv[0], '/')) ? ++program : (program = argv[0]);
	--argc, ++argv;
	while (~(c=getopt(argc, argv, "o:l:h"))) {
		switch (c) {
			case 'o':
				out_fn  = optarg;
				break;
			case 'l':
				min_l  = atoi(optarg);
				break;
			default:
				if (c != 'h') fprintf(stderr, "[E::%s] undefined option %c\n", __func__, c);
help:	
				fprintf(stderr, "\nUsage: %s [options] <FASTA> <SAT>\n", program);
				fprintf(stderr, "Options:\n");
				fprintf(stderr, "         -o    STR      output file\n");
				fprintf(stderr, "         -l    INT      minimum scaffold length [0]\n");
				fprintf(stderr, "         -h             help\n");
				return 1;	
		}		
	}
	if (optind + 2 > argc) {
		fprintf(stderr,"[E::%s] require a sat file and a sequence file!\n", __func__); goto help;
	}
	char *seq_fn = argv[optind];
	sat_fn = argv[optind++];
	fprintf(stderr, "Program starts\n");	
	sat2seq(seq_fn, sat_fn, out_fn, min_l);
	fprintf(stderr, "Program ends\n");	
	return 0;	

}

int main(int argc, char *argv[])
{
	if (argc < 2) {
help:
		fprintf(stderr, "\n  satool [-v] [-h] <command> [<args>]\n");
		fprintf(stderr, "  commands:\n");
		fprintf(stderr, "           agp        convert SAT format to AGP format\n");
		fprintf(stderr, "           sim        simplify SAT path composition to atomic sequences\n");
		fprintf(stderr, "           brk        break SAT paths\n");
		fprintf(stderr, "           get        get scaffolds from a scaffolding graph\n");
		return 1;
	} else {
		if (!strcmp(argv[1], "agp")) main_agp(argc , argv);
	   	else if (!strcmp(argv[1], "sim")) main_simp(argc , argv);	
		else if (!strcmp(argv[1], "brk")) main_break(argc, argv);
		else if (!strcmp(argv[1], "get")) main_gets(argc, argv);
	   	else if (!strcmp(argv[1], "-h")) goto help;	
	   	else if (!strcmp(argv[1], "-v")) fprintf(stderr, "version: 0.0.0\n");	
		else {
			fprintf(stderr, "  [E::%s] unrecognized command %s\n", __func__, argv[1]);
			goto help;	
		}	
	}
	return 0;
}



