#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <math.h>

using namespace std;

FILE *file_input, *file_output;

int main(int argc, char *argv[])
{
    if (argc!=6) {
    	cout << "****************************************************" << endl;
    	cout << "* The number of parameters is not correct!         *" << endl;
    	cout << "****************************************************" << endl;
    	cout << "* 'example' expects a call of the form:            *" << endl;
    	cout << "*                                                  *" << endl;
    	cout << "*    example.x dir input_f output_f numi numr      *" << endl;
    	cout << "*                                                  *" << endl;
    	cout << "* where                                            *" << endl;
    	cout << "*     - dir is the working directory               *" << endl;
    	cout << "*     - input_f is name of input file              *" << endl;
    	cout << "*     - output_f is name of output file            *" << endl;
    	cout << "*     - numi is an integer number                  *" << endl;
    	cout << "*     - numr is a real (double) number             *" << endl;
    	cout << "****************************************************" << endl;
    	return 1;
    }

    char name[80];
    strcpy(name,argv[1]);
    strncat(name,"/",50);
    strncat(name,argv[2],50);

    file_input=fopen(name,"r"); 
    // Here we open an input file that contains values of
    // parameters that we can change eventually, but we do
    // not want to compite the .c every time that we change them
                             
    int dummy_par1, retscan;
    retscan=fscanf(file_input,"%d",&dummy_par1);
    if (retscan==EOF) {cout << "The input file has bad/incorrect format" << endl; return 0;}
    double dummy_par2;
    retscan=fscanf(file_input,"%lf",&dummy_par2);
    if (retscan==EOF) {cout << "The input file has bad/incorrect format" << endl; return 0;}

    fclose(file_input);

    strcpy(name,argv[1]);
    strncat(name,"/",50);
    strncat(name,argv[3],50);

    int par_numi;
    double par_numr;
    sscanf(argv[4], "%d", &par_numi);
    sscanf(argv[5], "%lf", &par_numr);

    file_output=fopen(name,"w"); 
    
    fprintf(file_output,"Dummy parameters: %d %e\n",dummy_par1,dummy_par2);
    fprintf(file_output,"External parameters: %d %e\n",par_numi,par_numr);

    fclose(file_output);

    return 0;
}

