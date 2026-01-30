#include "common.h"
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
static void usage(const char *a){fprintf(stderr,"Usage: %s <pid>\n",a); exit(1);}
static int isnum(const char*s){for(;*s;s++) if(!isdigit(*s)) return 0; return 1;}



int main(int c,char**v){

	if(c!=2 || !isnum(v[1])) {//non numeric pid
		usage(v[0]);	
	}


	char source[256];
	snprintf(source, sizeof(source), "/proc/%s/stat", v[1]);

 
	FILE *fp = fopen(source, "r");
	if(!fp){
		perror("fopen");
		return 1;
	}

	int pid;
	char comm[256];
	char state;
	int parent_pid;
	unsigned long utime;
	unsigned long stime;	


        fscanf(fp, "%d (%255[^)]) %c %d", &pid, comm, &state, &parent_pid);
        
        unsigned long tmp;

        for(int i = 0; i < 9; i++){
                fscanf(fp, "%lu", &tmp);
        }

     
        fscanf(fp, "%lu %lu", &utime, &stime);
	fclose(fp); 

	
	//state/parent pid
	printf("State:%c\n", state);
        printf("PPID:%d\n", parent_pid);


	char command_line[256];
        snprintf(command_line, sizeof(command_line), "/proc/%s/cmdline", v[1]);

	FILE *command_file = fopen(command_line, "r");
	if(!command_file){
		perror("fopen command_line");
		return 1;
	}


	//cmd line

	char cmd_line[1024];
	size_t n =fread(cmd_line, 1, sizeof(cmd_line) -1, command_file);
	fclose(command_file);


	if(n==0){
		printf("Command error\n");
	}else{

		printf("Cmd:%s\n", cmd_line);
	}


	//cpu time
	long total_ticks = (utime + stime );
	long ticks = sysconf(_SC_CLK_TCK);
    	double seconds = (total_ticks) / (double)ticks;
    	printf("CPU:%ld %.3f\n", total_ticks, seconds);


	//memory vmrss 
	char status_file[256];
	snprintf(status_file, sizeof(status_file), "/proc/%s/status", v[1]);

	FILE *status_fp = fopen(status_file, "r");

	char status_line[256];
	unsigned long vmrss = 0;

	while(fgets(status_line, sizeof(status_line), status_fp)){
    		if(sscanf(status_line, "VmRSS: %lu kB", &vmrss) == 1){
        		break; 
    			}
	}
	fclose(status_fp);

	printf("VmRSS:%lu\n", vmrss);
	
return 0;
}


