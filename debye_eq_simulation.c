#define _CRT_SECURE_NO_WARNINGS


#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>


/* Function that calculate the distance between to points*/
double dis_2points(double* x1,double* x2)
{
    double dif; /* computes squares diffences*/
    
    dif = sqrt(pow(x1[0]-x2[0],2) +  pow(x1[1]-x2[1],2) + pow(x1[2]-x2[2],2));
    
    return dif;
}

/*function that counts the number of lines in a file*/

int line_count(FILE* data)
{   
    int n = 0;
    char line[1000*1000];
    
    while((fgets(line, 1000*1000, data)!= NULL))
    {
        if(line != NULL)
        {  
            n = n + 1;    
        }
    }

    if(n >= 1000*1000){return 1;}
    else {return n;}
}

/*function that constructs a matrix from a file*/
void matrix_build(FILE* data, int lin, int coll, double *matrix)
{

    int i,j;
    char line[1000*1000];
    char *number, *virg;

    j = 0;
    i = 0;
    while((fgets(line, 1000*1000, data)!= NULL))
    {
       
        if(line != NULL)
        { 
            j = 0;
            number = strtok(line,",");   
            
            while(number != NULL)
            {
        
                matrix[i*coll + j] = strtod(number,&virg);
                number = strtok(NULL,",");
                j = j + 1;
            }
        
            i = i + 1;   
        }
    }
    
}

/*define a sinc function*/

double sinc (double x){

    double y;

    if (x==0){y = 1.0;}
    else{
        y = sin(x)/x;
    }
    return y;
}
int main()
{

    clock_t toc = clock();

    int n;
    int i, j;
    FILE* coor = fopen("FCC_coor.txt", "r");
    
    /*find number of lines*/

    n = line_count(coor);
    printf("Number of coordinates = %d\n", n);
    fclose(coor);

	 /*  builds the coordinate array*/
    double* R = malloc(sizeof(double) * (n * 3));
    FILE* coor2 = fopen("FCC_coor.txt", "r");

    matrix_build(coor2, n, 3, R);
    fclose(coor2);

    /*Visualizes coordinate matrix*/
    
    /*
    for (i=0; i < 10; i++){
        printf("%d: [",i);
        for (j=0;j<3;j++){
            printf("%f,",R[i*3 + j]);
        }
        printf("]\n");
    }
	*/
	/* build the matrix of atomic scattering factors*/

	int m;
	FILE *atom_Au = fopen("atom_sca_fac_Au.txt","r");

	m = line_count(atom_Au);
	/* builds the matrix of the atomic scattering, fist Q, second f*/

	fclose(atom_Au);

	double* f_Au = (double*)malloc((m*2)*sizeof(double*)); 
	FILE *atom_Au_2 = fopen("atom_sca_fac_Au.txt","r");

	matrix_build(atom_Au_2, m, 2,f_Au);

	fclose(atom_Au_2);

   
	/*Visualizes atomic scattering matrix*/

	/*
	for (i=0; i < 10; i++){
		printf("%d: [",i);
		for (j=0;j<2;j++){
			printf("%f,",f_Au[i*2 + j]);
		}
		printf("]\n");
	}
	*/

	/* makes the sum to form the diffraction pattern*/

	//
	double Qmax ;
	double step;
	double r0[3],r1[3],r2[3];
	double diameter;
	double dis,dis0;
	int mult;
	int N;
	int ind[n];
	int time;
	



	Qmax= f_Au[2*(m-1)];
	step = Qmax/m;
	printf("Qmax = %f nm-1, points = %d, step = %f\n", Qmax*10, m, step*10 );

	r0[0] = 0.0;
	r0[1] = 0.0;
	r0[2] = 0.0;

	diameter =70.0;
	N=0;
	j=0;
	for (i=0;i < n; i++){
		r1[0] = R[i*3 + 0];
		r1[1] = R[i*3 + 1];
		r1[2] = R[i*3 + 2];
		dis0 = dis_2points(r0,r1);
		dis = sqrt(pow(r1[0],2) + pow(r1[1],2)+pow(r1[2],2));
		if(dis0 <= diameter/2.0){
			/*printf("V =[%f,%f,%f] ; dis0 = %f; dis =%f\n",r1[0],r1[1],r1[2],dis0,dis);*/
			N++;
			ind[j] = i;
			j = j + 1;
			}
	}

	printf("Diameter = %f nm, Atoms = %d\n",diameter/10.0,N);
	time = 0;

	int N1;

	N1 = 1*N;
	double* I1 = (double*)malloc(m*sizeof(double*));
	
	double* dis1 = (double*)malloc((N1*N1)*sizeof(double*));

	int* mult1 = (int*)malloc((N1*N1)*sizeof(int*));


	/*clock_t cycle1_begin = clock();*/
	omp_set_num_threads(8);

		#pragma omp parallel for shared(dis1,mult1) private(r1,r2,dis)
		for (int i1=0; i1<N1;i1++){
			//clock_t ini = clock();
			r1[0] = R[ind[i1]*3 + 0];
			r1[1] = R[ind[i1]*3 + 1];
			r1[2] = R[ind[i1]*3 + 2];
			for (int j1 = i1; j1<N1; j1++){
				r2[0] = R[ind[j1]*3 + 0];
				r2[1] = R[ind[j1]*3 + 1];
				r2[2] = R[ind[j1]*3 + 2];
				dis = dis_2points(r1,r2);
				//printf("%d ,%d,    %f, \n",i1,j1,dis);
				dis1[i1*N1 + j1] = dis; 	
				if(dis==0){mult1[i1*N1 + j1] = 1;} else{mult1[i1*N1 + j1] = 2;}

			}
		}
		printf("OK 1\n");
		#pragma omp parallel for private(dis,mult) reduction(+:I1[:m]) collapse(1)
		for (int i1 = 0; i1 < N1*N1; i1++){
			for (int k1=0;k1<m;k1++){
				dis = dis1[i1];
				mult = mult1[i1]; 
				I1[k1] = I1[k1] + mult*f_Au[k1*2 + 1]*f_Au[k1*2 + 1]*sinc(dis*f_Au[k1*2 + 0]);
			}
			
		}
			/*clock_t cycle1_end = clock();
			printf("First loop time elapse = %lds\n",(cycle1_end - cycle1_begin)/CLOCKS_PER_SEC);*/

			/*clock_t cycle2_begin = clock();*/
			
		
	/*clock_t cycle2_end = clock();
	printf("Second loop time elapse = %lds\n",(cycle2_end - cycle2_begin)/CLOCKS_PER_SEC);*/


	/*saves the diffracted simulations*/
	char name[100];
	sprintf(name,"DP_Au_fcc_%1.fA_mult_v2.txt",diameter);
	puts(name);
	FILE *DP = fopen(name,"w+");
	for (i = 0; i<m; i++){
	//for (i=0; i<10; i++){
			fprintf(DP,"%f,%f\n",f_Au[i*2 + 0],(I1[i])/N);
			//printf("(%d,%f)\n",i,(I1[i])/N);
		}

	/*test the sinc function*/
	/*
	double* x  = (double*)malloc(1001*sizeof(double*));
	double* y = (double*)malloc(1001*sizeof(double*));
	double mvalue;

	mvalue = 200.0;
	for (i=0; i<1001; i++){
		x[i] = (mvalue*i/500.0)-mvalue;
	}
	for (i=0; i<1001; i++){
		y[i] = sinc(x[i]);
	}

	FILE *test_sinc = fopen("C:\\Users\\lmcor\\Desktop\\programacao\\c\\sinc.txt","w+");
	for (i=0; i<1001; i++){
			fprintf(test_sinc,"%f,%f\n",x[i],y[i]);
		}
	*/

	clock_t tic = clock();

	printf("Time elapse =%ld s",(tic-toc)/CLOCKS_PER_SEC);

	}


