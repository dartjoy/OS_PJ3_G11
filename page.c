#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

int main(int argc, char **argv) {
	int opt;
	int n;

	while ((opt = getopt(argc, argv, "n:")) != -1) {
		switch (opt) {
			case 'n': {
				n = atoi(optarg);
				break;
			}
			default: {
				perror("Wrong Argument!\n");
				exit(1);
			}
		}
	}

	FILE *fp = fopen("test.txt", "r"), *ft = fopen("group11_ans.txt", "w");
	unsigned long addr;
	unsigned long mask = 0;

	for (int i = 0; i < n; ++i)
		mask = (mask << 1) + 1;

	while(fscanf(fp, "%lx", &addr) > 0)
		fprintf(ft, "%-10lu%-10lu\n", addr >> n, addr & mask);

	fclose(fp);
	fclose(ft);

	return 0;
}