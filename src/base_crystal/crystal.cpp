#define FMT_HEADER_ONLY
#include <fmt/format.h>
#include <iostream>
#include <string>
#include <algorithm>
float** allocate_coords(int N_atoms){
	float** coord = new float*[N_atoms]();
	for(int i = 0; i < N_atoms; i++){ 
		coord[i] = new float[3]();
	}
	return coord;
}
void cut_sphere(float output[][3], float input[][3], int N_output, int N_input){
	int n_count = 1;
	auto dis_square = new float [N_input];
	auto index  = new int [N_input];
	for(int i = 0; i < N_input; i++){
     		float x = input[i][0];
     		float y = input[i][1];
     		float z = input[i][2];
		dis_square[i] =  x*x + y*y + z*z;
		index[i] = i; 
	}
	std::sort(index, index + N_input, [&](int i, int j){return dis_square[i] < dis_square[j];});
	for(int i = 0; i < N_output; i++){
		output[i][0] = input[index[i]][0];
		output[i][1] = input[index[i]][1];
		output[i][2] = input[index[i]][2];
	}

}

extern "C"{

	void cubic(float atoms_coordinates[][3], float lattice_parameter[3], int N_atoms, const char* shape){
		
		int *base_vector; 
		base_vector  = new int[3]();
		std::string shape_string = fmt::format(shape);	
		//std::string error = fmt::format("Base vector ({}, {} ,{})", base_vector[0], base_vector[1], base_vector[2]);
		//std::cout << error <<std::endl;
		int n_count = 1;
		int index = 0;
		int shell = 1;
		int step = 1;
		//std::cout << shape_string << std::endl;
		if(shape_string == "cube"){
			while(n_count < N_atoms){
				for(int x = -shell;  x <= shell; x++){
					for(int y = -shell;  y <= shell; y++){
						if(abs(x) == shell || abs(y) == shell){
							//std::cout<< "Set step" << std::endl; 
							step = 1;
						}
						else{
							step = 2*shell; 
							//std::cout << "Set step : " << step << std::endl;
						}
						for(int z = -shell;   z <= shell; z += step){
							if(x != 0 || y != 0 || z != 0){
								atoms_coordinates[n_count][0] = lattice_parameter[0]*x;				
								atoms_coordinates[n_count][1] = lattice_parameter[1]*y;
								atoms_coordinates[n_count][2] = lattice_parameter[2]*z;
								//std::string error_coor = fmt::format("index : {}, coor ({}, {}, {})", n_count, x, y, z);
								//std::cout << error_coor << std::endl;
								n_count++;
								if(n_count == N_atoms){return;}
							}
						}
					}	
				}
			shell++;
			}
		}
		if(shape_string == "sphere"){
			int  N_atoms_cube = N_atoms*2;
			auto atoms_coordinate_cube = new float [N_atoms_cube][3];
			cubic(atoms_coordinate_cube, lattice_parameter, N_atoms_cube, "cube");
			cut_sphere(atoms_coordinates, atoms_coordinate_cube, N_atoms, N_atoms_cube);	
			
		}
	}
}
