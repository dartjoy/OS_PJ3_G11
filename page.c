#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM 15

FILE *fp, *ft;

void op(char *in, int num) {
	char *addr;

	strtok(in, "x");
	addr = strtok(NULL, "x");

	int i = 0;
	unsigned int mask = 0, binary = (unsigned int)strtol(addr, NULL, 16);

	for(i = 0; i < num; ++i) mask = (mask << 1) + 1;

	fprintf(ft, "%-10u%-10u\n", binary >> num, binary & mask);

	return;
}

int main(int argc, char **argv) {
	char buf[NUM];
	int l;

	if((fp = fopen("test.txt", "r")) == NULL) {
		perror("Fail to open the file!\n");
		exit(1);
	}

	if((ft = fopen("group11_ans.txt", "w")) == NULL) {
		perror("Fail to create the file!\n");
		exit(1);
	}

	while(fgets(buf, NUM, fp) != NULL) {
		l = strlen(buf);
		buf[l - 1] = '\0';
		if(argc == 3) op(buf, atoi(argv[2]));
		else {
			perror("Argument count error!\n");
			exit(1);
		}
	}

	fclose(fp);
	fclose(ft);

	return 0;
}
