#include <stdio.h>
#include <string.h>
#include <regex.h>   
#include <stdlib.h>

//compile with gcc veeamid.c -o /bin/veeamid

enum mode {SELECT, COUNT,LAST};

int main(int argc, char *argv[]) {
	char buf[BUFSIZ];
	int select = 1;
	int counter = 0;
	enum mode wmode = LAST;
	
	regex_t 	argreg;
	char 		*argpattern = "[-]([a-zA-Z]+)([a-zA-Z0-9]*)";

	
	regex_t    	preg;
	//char       	*pattern = "[{]([a-z0-9-]+)[}]";
	//based on https://en.wikipedia.org/wiki/Universally_unique_identifier
	char       	*pattern = "([a-z0-9-]{8}-[a-z0-9-]{4}-[a-z0-9-]{4}-[a-z0-9-]{4}-[a-z0-9-]{12})";

	int status = 0;
	size_t nmatch = 2;
	regmatch_t pmatch[2];
	char *resultmatch = NULL;
	char *resultarg = NULL;
	
	status = regcomp(&preg, pattern, REG_EXTENDED);
	if (status != 0) { fprintf(stdout,"Error compiling regex (should never happend)");return 1;}

	status = regcomp(&argreg,argpattern, REG_EXTENDED);
	if (status != 0) { fprintf(stdout,"Error compiling arg regex (should never happend)");return 1;};

	if (argc > 1) {
		int i = 0;
		for(i=1;i< argc;i++) {
			status = regexec(&argreg,argv[i], 0, NULL, 0);
			if (status > 0) {	
				fprintf(stdout,"Don't know arg %s\n",argv[i]);
				return 1;
			} else {
				char switchvar = argv[i][1];
				int len = strlen(argv[i]);
				resultarg = (char*) malloc (len);
				
				memcpy(resultarg,&argv[i][2],len-2);
				//fprintf(stdout,"%c : %s",switchvar,resultarg);
				switch (switchvar) {
					case 'n':
						if(strlen(resultarg) == 0) { select = 1; } 
						select = atoi(resultarg);
						if (select < 1) { select = 1; }
						wmode = SELECT;
					break;
					case 'c':
						wmode = COUNT;
					break;
					case 'l':
						wmode = LAST;
					break;
					default:
						fprintf(stdout,"Don't know %s\n",argv[i]);
						return 1;
					break;
				}
				free(resultarg);
			}
		}
	}
   	regfree(&argreg);
	//thanks ibm http://www.ibm.com/support/knowledgecenter/ssw_ibm_i_72/rtref/regexec.htm
		
	while( fgets( buf, sizeof buf, stdin ) != NULL ) {	
		status = regexec(&preg, buf, nmatch, pmatch, 0);
		if (status == 0) {
			counter++;
			if ((wmode == SELECT && counter == select) || wmode == LAST) {
				if(resultmatch != NULL) {
					free(resultmatch);
				}
				resultmatch = (char*) malloc (pmatch[1].rm_eo - pmatch[1].rm_so+1);
				memcpy(resultmatch,&buf[pmatch[1].rm_so],pmatch[1].rm_eo - pmatch[1].rm_so);
			}
		}
	}
	if (wmode == COUNT) {
			fprintf(stdout,"%d\n",counter);
	} else if ((wmode == LAST  || wmode == SELECT ) && resultmatch != NULL) {
			fprintf(stdout,"%s\n",resultmatch);
	}
	
	if(resultmatch != NULL) {
		free(resultmatch);
	}

	regfree(&preg);
	return 0;
}
