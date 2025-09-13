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
void cut_sphere(float output[][3], float input[][3], int N_output, int N_input, float lattice_parameter[3]){
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
		output[i][0] = input[index[i]][0]*lattice_parameter[0];
		output[i][1] = input[index[i]][1]*lattice_parameter[1];
		output[i][2] = input[index[i]][2]*lattice_parameter[2];
	}

}



void cut_cube(float output[][3], float input[][3], int N_output, int N_input, float lattice_parameter[3]){
	
	int n_count = 0;
	int shell = 0; 		
     	auto index  = new int [N_input];
	for(int i = 0; i < N_input; i++){
		index[i] = i;
	}
	while(n_count < N_output){
		for(int i = n_count; i < N_input; i++){
			float x = input[index[i]][0];
			float y = input[index[i]][1];
			float z = input[index[i]][2];
			if(x <= shell || y <= shell || z <= shell){
				output[n_count][0] = x*lattice_parameter[0];
				output[n_count][1] = y*lattice_parameter[1];
				output[n_count][2] = z*lattice_parameter[2];
				index[i] = index[n_count];
				n_count++;
			}
		}
	}
}

extern "C"{

	void cubic(float atoms_coordinates[][3], float lattice_parameter[3], int N_atoms, const char* shape){
		
		int *base_vector; 
		base_vector  = new int[3]();
		std::string shape_string = fmt::format(shape);	
		//std::string error = fmt::format("Base vector ({}, {} ,{})", base_vector[0], base_vector[1], base_vector[2]);
		//std::cout << error <<std::endl;
		int n_edge = pow(N_atoms, 1.0/3.0) + 3;
		int N_atoms_cube = n_edge*n_edge*n_edge;
		auto atoms_coordinate_cube = new float [N_atoms_cube][3];
		int n_count = 1;
		std::cout << "n_edge: " << n_edge << std::endl;
		for(int x = -n_edge/2; x <= n_edge/2; x++){
			for(int y = -n_edge/2; y <= n_edge/2; y++){
				for(int z = -n_edge/2; z <= n_edge/2; z++){	
					if(x != 0 || y != 0 || z != 0){
						atoms_coordinate_cube[n_count][0] = x;				
						atoms_coordinate_cube[n_count][1] = y;
						atoms_coordinate_cube[n_count][2] = z;
						n_count++;
						//std::cout << "n_count: " << n_count << ",  N_atoms: "<< N_atoms_cube << std::endl;
					}
				}
			}
		}
			std::cout << "Finished creating the big cube!" << std::endl;

		if(shape_string == "cube"){cut_cube(atoms_coordinates, atoms_coordinate_cube, N_atoms, N_atoms_cube, lattice_parameter);}
		if(shape_string == "sphere"){cut_sphere(atoms_coordinates, atoms_coordinate_cube, N_atoms, N_atoms_cube, lattice_parameter);}
	}
}
