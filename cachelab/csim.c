#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <math.h>
#include <time.h>

#define bool char
#define true 1
#define false 0

typedef struct {
	bool valid;
	long start;
	long end;
	long lru;
} cache_blk;

unsigned s, b;
unsigned S, E, B;
bool v;
unsigned h, m, e;
char *file_name;
cache_blk **cache;
long t;

void revise(int s_idx, int e_idx, unsigned long addr) {
	cache[s_idx][e_idx].valid = true;
	cache[s_idx][e_idx].start = (addr >> b) << b;
	cache[s_idx][e_idx].end = cache[s_idx][e_idx].start + B - 1;
	cache[s_idx][e_idx].lru = t;
}

int get_s_idx(unsigned long addr) {
	int idx = addr >> b;

	if (idx >= S) {
		int mask = 0;

		for (int i = 0; i < s; ++i)
			mask = (mask << 1) + 1;

		idx &= mask;
	}

	return idx;
}

int get_e_idx(unsigned long addr, int size) {
	int s_idx = get_s_idx(addr);
	cache_blk *tgt_set = cache[s_idx];

	for (int i = 0; i < E; ++i) {
		cache_blk tmp = tgt_set[i];

		if (tmp.valid && addr >= tmp.start && addr <= tmp.end)
			return i;
	}

	return -1;
}

int find_unsed_e_idx(unsigned long addr, int size) {
	int s_idx = get_s_idx(addr);
	cache_blk *tgt_set = cache[s_idx];

	for (int i = 0; i < E; ++i)
		if (!tgt_set[i].valid)
			return i;

	return -1;
}

char *save(unsigned long addr, int size) {
	char *res;
	int s_idx = get_s_idx(addr);
	int e_idx = get_e_idx(addr, size);

	if (e_idx == -1) {
		res = "miss";
		m += 1;
	}
	else {
		res = "hit";
		h += 1;

		revise(s_idx, e_idx, addr);
	}

	return res;
}

char *load(unsigned long addr, int size) {
	char *res;
	int s_idx = get_s_idx(addr);
	int e_idx = get_e_idx(addr, size);

	if (e_idx != -1) {
		res = "hit";
		h += 1;
	}
	else {
		e_idx = find_unsed_e_idx(addr, size);
		if (e_idx != -1) {
			res = "miss";
			m += 1;
		}
		else {
			cache_blk *tgt_set = cache[s_idx];
			e_idx = 0;
			for (int i = 0; i < E; i++) {
				cache_blk tmp = tgt_set[i];

				if (tgt_set[e_idx].lru > tmp.lru)
					e_idx = i;
			}
			res = "miss eviction";
			m += 1;
			e += 1;
		}
	}

	revise(s_idx, e_idx, addr);
	return res;
}

void cache_instr(char id, unsigned long addr, int size) {
	switch (id) {
		case 'S': {
			char *res = load(addr, size);
			if (v)
				printf("%c %lx,%d %s \n", id, addr, size, res);
			break;
		}
		case 'M': {
			char *l = load(addr, size), *s = save(addr, size);
			if (v)
				printf("%c %lx,%d %s %s \n", id, addr, size, l, s);
			break;
		}
		case 'L': {
			char *res = load(addr, size);
			if (v)
				printf("%c %lx,%d %s \n", id, addr, size, res);
			break;
		}
	}
	t++;
}

void usage() {
	char *help =
		"Usage: ./csim [-hv] -s <num> -E <num> -b <num> -t <file>\n\
		Options:\n\
		-h         Print this help message.\n\
		-v         Optional verbose flag.\n\
		-s <num>   Number of set index bits.\n\
		-E <num>   Number of lines per set.\n\
		-b <num>   Number of block offset bits.\n\
		-t <file>  Trace file.\n\
		\n\
		Examples:\n\
		linux>  ./csim -s 4 -E 1 -b 4 -t traces/yi.trace \n\
		linux>  ./csim -v -s 8 -E 2 -b 4 -t traces/yi.trace";
	printf("%s\n", help);
}

int main(int argc, char **argv) {
	int opt;

	v = false;
	h = 0;
	m = 0;
	e = 0;
	t = 0;

	while((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
		switch(opt) {
			case 'h': {
				usage();
				break;
			}
			case 'v': {
				v = true;
				break;
			}
			case 's': {
				s = atoi(optarg);
				break;
			}
			case 'E': {
				E = atoi(optarg);
				break;
			}
			case 'b': {
				b = atoi(optarg);
				break;
			}
			case 't': {
				file_name = optarg;
				break;
			}
			default: {
				printf("Wrong Argument!\n");
				break;
			}
		}
	}

	S = pow(2.0, (float)s);
	B = pow(2.0, (float)b);

	cache = malloc(S * sizeof(cache_blk *));
	for(int i = 0; i < S; ++i)
		cache[i] = malloc( E * sizeof(cache_blk));

	for(int i = 0; i < S ; ++i)
		for(int j = 0; j < E; ++j)
			cache[i][j].valid = false;

	FILE *fp = fopen(file_name, "r");
	char id;
	unsigned long addr;
	int size;

	while(fscanf(fp, " %c %lx,%d", &id, &addr, &size) > 0)
		cache_instr(id, addr, size);

	for(int i = 0; i < S; ++i)
		free(cache[i]);

	fclose(fp);
	printSummary(h, m, e);

	return 0;
}