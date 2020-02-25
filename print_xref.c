#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <regex.h>
#include <string.h>
#define MAX_MATCHES 10

void print_xref(int *arr, int xref_loc, int max) {

    printf("xref\n");
    printf("0 %d\n", max+1);
    for(int i = 0; i<=max; i++) {
        int gen = 0;
        char fn = 'n';
        if ( i == 0 || *(arr + 2*i) == 0) {
            fn = 'f';
        }
        if ( i == 0 ) {
            gen = 65535;
        }
        printf("%010d %05d %c \n", *(arr + 2*i + 1), gen, fn);
    }

    if ( xref_loc >= 0 ) {
        printf("\n");
        printf("trailer << /Size %d /Root 1 0 R >>\n", max+1);
        printf("startxref\n");
        printf("%d\n", xref_loc);
    }
    return;
}



int find_objs(char *str, int *objs, int loc, int *max) {
    regex_t regex;
    int reti;

    /* Compile regular expression */
    reti = regcomp(&regex, "([[:digit:]]+) [[:digit:]]+ obj", REG_EXTENDED);
    if (reti) {
        fprintf(stderr, "Could not compile regex\n");
        exit(1);
    }

    regmatch_t matches[MAX_MATCHES];
    char buff[10];  // upto 10 digit number.
    if (regexec(&regex, str, MAX_MATCHES, matches, 0) == 0) {
        int len = matches[1].rm_eo - matches[1].rm_so;
        memcpy(buff, str + matches[1].rm_so, len);
        buff[len] = '\0';

        int objnum = atoi(buff);
        if (objnum > *max) {
            *max = objnum;
        }

        *(objs + 2* objnum) = objnum;
        *(objs + 2* objnum + 1) = loc + matches[1].rm_so;
    
        return 0;
    }
    return -1;

}
int main(int argc, char * argv[])
{
	unsigned long off = 0;
    int num_objs = 1024;
    int max = -1;

    /*  We store object info as:
        obj[0][1] = [obj_id][location]
        with everything initialized to 0.
    */
    int *objs = (int*) calloc(num_objs * 2, sizeof(int));
    
    // some of the lines (caused by image streams)
    // can be exceptionally big.
    // 100k should be a good enough max-line size for most cases.
    int max_linesize = 1024 * 100;
    char *str = (char*) malloc(sizeof(char) * max_linesize);

	if (argc!=2)
	{
		fprintf(stderr, "Usage:\n\t%s filename.pdf\n\n", argv[0]);
		return(1);
	}

	FILE *f;
	f = fopen(argv[1], "r");
	if (f == NULL)
	{
		fprintf(stderr, "Cannot open file %s\n", argv[1]);
		return(2);
	}


    int xref_loc = -1;
	while (!feof(f))
	{
        fgets(str, max_linesize, f);
        if ( strncmp(str, "xref", 4) == 0) {
            xref_loc = off;
        }

        find_objs(str, objs, off, &max);
		off += strlen(str);
	}

    print_xref(objs, xref_loc, max);
	fclose(f);
	return(0);
}
