#include <stdio.h>
#include <string.h>
#include <regex.h>   
#include <stdlib.h>

//compile with gcc veeamid.c -o /bin/veeamid

enum mode {SELECT,COUNT,LAST,ALL};
#define MAXALLRETURN 1000

void printhelparg(char*argname,char*arghelp) { 
fprintf(stdout," -%-8s : %s\n",argname,arghelp);
}
void printhelp() {
	puts("Veeamid looks for UUID in output and print out latest match");
	puts("Helper tool for veeamconfig");
	puts("");
	puts("Flags:");
	printhelparg("l","explicit last match");
	printhelparg("n<num>","will print out <num> match. -n1 will print out first match, -n2 second etc.");
	printhelparg("c","count the amoutn of matches");
	printhelparg("a","prints out all matches");
	printhelparg("d","change the pattern to dates");
}

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
	char 		*datepattern = "([0-9]{4}[-][0-9]{2}[-][0-9]{2} [0-9]{2}:[0-9]{2})";
	//2016-12-14 14:41

	int status = 0;
	size_t nmatch = 2;
	regmatch_t pmatch[2];
	char *resultmatch = NULL;
	char *resultarg = NULL;


	char **allresults = malloc(MAXALLRETURN * sizeof(char*)) ;
	int stored = 0;
	int r;
	for (r=0;r < MAXALLRETURN;r++) {
		allresults[r] = NULL;
	}

	status = regcomp(&argreg,argpattern, REG_EXTENDED);
	if (status != 0) { fprintf(stdout,"Error compiling arg regex (should never happend)");return 1;};

	if (argc > 1) {
		int i = 0;
		for(i=1;i< argc;i++) {
			status = regexec(&argreg,argv[i], 0, NULL, 0);
			if (status > 0) {	
				fprintf(stdout,"Don't know arg %s\n",argv[i]);	
				printhelp();
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
					case 'a':
						wmode = ALL;
					break;
					case 'd':
						pattern = datepattern;
					break;
					case 'h':
						printhelp();return 0;
					break;
					default:
						fprintf(stdout,"Don't know %s\n",argv[i]);
						printhelp();
						return 1;
					break;
				}
				free(resultarg);
			}
		}
	}
   	regfree(&argreg);

        status = regcomp(&preg, pattern, REG_EXTENDED);
        if (status != 0) { fprintf(stdout,"Error compiling regex (should never happend)");return 1;}
		

	while( fgets( buf, sizeof buf, stdin ) != NULL && stored < MAXALLRETURN ) {
	 if (strlen(buf) > 0) {	
		//very small protection
		int maxloop = 100;

		char* substr = &buf[0];
		status = regexec(&preg,substr, nmatch, pmatch, 0);
		
		while( status == 0 && maxloop-- && stored < MAXALLRETURN) {
			counter++;
			if ((wmode == SELECT && counter == select) || wmode == LAST || wmode == ALL) {
				if(resultmatch != NULL && wmode != ALL) {
					free(resultmatch);
				}
				int lengthofstr = (pmatch[1].rm_eo - pmatch[1].rm_so);
				resultmatch = (char*) malloc ((lengthofstr+1)*sizeof(char));
				memcpy(resultmatch,&substr[pmatch[1].rm_so],lengthofstr);
				resultmatch[lengthofstr] = 0;

				if(wmode == ALL) {
					allresults[stored++] = resultmatch;
				}
			} 

			if((pmatch[1].rm_eo+1) < strlen(substr)) {
				substr = &substr[pmatch[1].rm_eo];
				status = regexec(&preg,substr,nmatch,pmatch,0);
			} else {
				status = 1;
			}
		}
	 }
	}
	if (wmode == COUNT) {
			fprintf(stdout,"%d\n",counter);
	} else if ((wmode == LAST  || wmode == SELECT ) && resultmatch != NULL) {
			fprintf(stdout,"%s\n",resultmatch);
	} else if ((wmode == ALL)) {
		//fprintf(stdout,"All results\n");
		for(r=0;r < MAXALLRETURN && allresults[r] != NULL;r++) {
			fprintf(stdout,"%s\n",allresults[r]);
			free(allresults[r]);
		}
	}
	//if we are running in all mode, we have already freed resultmatch
	if (wmode == ALL) {
		free(allresults);
	} else if (resultmatch != NULL) {
		free(resultmatch);
	}

	regfree(&preg);
	return 0;
}
