#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM 15

void op(char *in, int num) {
	char *addr;

	int mask = 15; //0xf

	strtok(in, "x");
	addr = strtok(NULL, "x");
//	printf("%s\n", addr);

	size_t len = strlen(addr);
//	printf("%lu\n", len);
	char *b = malloc((len - 1) * 8 + 1);
	char *p, *s = b;
/*
	b[0] = '\0';
	for(size_t i = 0; i < len - 1; ++i) {
		char c = addr[i];
		for(int j = 7; j >= 0; --j) {
			if(!(c & 1 << j)) strcat(b, "0");
			else strcat(b, "1");
		}
	}*/

	for(p = addr; *p != '\0'; ++p)
		for(int i = 7; i >= 0; --i, b++)
			*b = (*p & (1 << i)) ? '1' : '0';

	*b = '\0';
	b = s;

	printf("%s\n", b);

	return;
}

int main(int argc, char **argv) {
	FILE *fp;
	char buf[NUM];
	int l;

	if((fp = fopen("test.txt", "r")) == NULL) {
		perror("Fail to open the file!\n");
		exit(1);
	}

	while(fgets(buf, NUM, fp) != NULL) {
		l = strlen(buf);
		buf[l] = '\0';
		if(argc == 3) op(buf, atoi(argv[2]));
		else {
			perror("Argument count error!\n");
			exit(1);
		}
	}

	return 0;
}
