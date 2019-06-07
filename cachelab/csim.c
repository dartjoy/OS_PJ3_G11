#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
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
}cache_line;

unsigned s, b;
unsigned S, E, B;
bool v;
unsigned h, m, e;
char *file_name;
cache_line **cache;
long t;

void revise(int s_index, int e_index, unsigned long addr) {
	cache[s_index][e_index].valid = true;
	cache[s_index][e_index].start = (addr >> b) << b;
	cache[s_index][e_index].end = cache[s_index][e_index].start + (B - 1);
	cache[s_index][e_index].lru = t;
}

int find_s_index(unsigned long addr) {
	int idx = addr >> b;
	int mask = 0;

	for (int i = 0; i < s; ++i)
		mask = (mask << 1) + 1;

	if (idx >= S)
		idx &= mask;

	return idx;
}

int find_e_index(unsigned long addr, int size) {
	int s_index = find_s_index(addr);
	cache_line *tgt_set = cache[s_index];

	for (int i = 0; i < E; i++) {
		cache_line block = tgt_set[i];

		if (block.valid && addr >= block.start && addr <= block.end)
			return i;
	}

	return -1;
}

int find_unsed_e_index(unsigned long addr, int size) {
	int s_index = find_s_index(addr);
	cache_line *tgt_set = cache[s_index];

	for (int i = 0; i < E; i++)
		if (!tgt_set[i].valid)
			return i;

	return -1;
}

char *save_data(unsigned long addr, int size) {
	char *res;
	int s_index = find_s_index(addr);
	int e_index = find_e_index(addr, size);

	if (e_index == -1) {
		res = "miss";
		m += 1;
	}
	else {
		res = "hit";
		h += 1;

		revise(s_index, e_index, addr);
	}

	return res;
}

char *load_data(unsigned long addr, int size) {
	char *res;
	int s_index = find_s_index(addr);
	int e_index = find_e_index(addr, size);

	if (e_index != -1) {
		res = "hit";
		h += 1;
	}
	else {
		e_index = find_unsed_e_index(addr, size);
		if (e_index != -1) {
			res = "miss";
			m += 1;
		}
		else {
			cache_line *tgt_set = cache[s_index];
			e_index = 0;
			for (int i = 0; i < E; i++) {
				cache_line block = tgt_set[i];

				if (tgt_set[e_index].lru > block.lru)
					e_index = i;
			}
			res = "miss eviction";
			m += 1;
			e += 1;
		}
	}

	revise(s_index, e_index, addr);
	return res;
}

void cache_instr(char id, unsigned long addr, int size) {
	switch (id) {
		case 'S': {
			char *res = load_data(addr, size);
			if (v)
				printf("%c %lx,%d %s \n", id, addr, size, res);
			break;
		}
		case 'M': {
			char *load = load_data(addr, size), *save = save_data(addr, size);
			if (v)
				printf("%c %lx,%d %s %s \n", id, addr, size, load, save);
			break;
		}
		case 'L': {
			char *res = load_data(addr, size);
			if (v)
				printf("%c %lx,%d %s \n", id, addr, size, res);
			break;
		}
	}
	t++;
}

void usage() {
	char *usage =
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
	printf("%s\n", usage);
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

	cache = malloc(S * sizeof(cache_line *));
	for(int i = 0; i < S; ++i)
		cache[i] = malloc( E * sizeof(cache_line));

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