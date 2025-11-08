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
	float eps = 10e-6;
	std::cout << "N_input: " << N_input << std::endl;
	for(int i = 0; i < N_input; i++){
		//std::cout << "i: " << i << std::endl;
     		float x = lattice_parameter[0]*input[i][0];
     		float y = lattice_parameter[1]*input[i][1];
     		float z = lattice_parameter[2]*input[i][2];
		dis_square[i] =  x*x + y*y + z*z;
		index[i] = i; 
	}
	std::cout << "The distance have been calculated!" << std::endl;
	std::sort(index, index + N_input, [&](int i, int j){return dis_square[i] < dis_square[j];});
	int k = 1;
	int i = 1;

	std::cout << "The distance have been sorted!" << std::endl;
	while(k < N_output && i < N_input){
		if(k >= N_output){std::cout << "Problems during sphere cutting" << std::endl;}
		float x = input[index[i]][0]*lattice_parameter[0];
		float y = input[index[i]][1]*lattice_parameter[1];
		float z = input[index[i]][2]*lattice_parameter[2];
		//std::cout << "k: " << k << ", i" << i << std::endl;
		if(fabs(x - output[k - 1][0]) < eps && fabs(y - output[k - 1][1]) < eps && fabs(z - output[k - 1][2]) < eps){ i++;} 
		else{output[k][0] = x; output[k][1] = y; output[k][2] = z; k++;	i++;}
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
		for(int i = 0; i < N_input; i++){
			float x = input[index[i]][0];
			float y = input[index[i]][1];
			float z = input[index[i]][2];
				if(abs(x) <= shell || abs(y) <= shell || abs(z) <= shell){

				output[n_count][0] = x*lattice_parameter[0];
				output[n_count][1] = y*lattice_parameter[1];
				output[n_count][2] = z*lattice_parameter[2];
				index[i] = index[n_count];
				n_count++;
			}
		}
		shell++;
	}
}

extern "C"{

	void cubic(float atoms_coordinates[][3], float lattice_parameter[3], int N_atoms, const char* shape){
		
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
					if(x != 0 && y != 0 && z != 0){
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

extern "C"{
	void crystal_coordinates(float atoms_coordinates[][3], float base_vectors[][3], float lattice_parameter[3], int N_base, int N_atoms, const char* shape){
		
		std::string shape_string = fmt::format(shape);	
		//std::string error = fmt::format("Base vector ({}, {} ,{})", base_vector[0], base_vector[1], base_vector[2]);
		//std::cout << error <<std::endl;
		int n_edge = (pow(N_atoms, 1.0/3.0) + 5);
		int N_atoms_cube = N_base*n_edge*n_edge*n_edge;
		auto atoms_coordinate_cube = new float [N_atoms_cube][3];
		int n_count = 1;
		std::cout << "cube has " << N_atoms_cube << " atoms" <<std::endl;  
	
		std::cout << "n_edge: " << n_edge << std::endl;
		for(int x = -n_edge/2; x <= n_edge/2; x++){
			for(int y = -n_edge/2; y <= n_edge/2; y++){
				for(int z = -n_edge/2; z <= n_edge/2; z++){	
					if(x != 0 || y != 0 || z != 0){
						for(int i = 0; i < N_base; i++){
							atoms_coordinate_cube[n_count][0] = x + base_vectors[i][0];		
							atoms_coordinate_cube[n_count][1] = y + base_vectors[i][1];
							atoms_coordinate_cube[n_count][2] = z + base_vectors[i][2];
							n_count++;
							if(n_count == N_atoms_cube){goto cut_volume;};
						//std::cout << "n_count: " << n_count << ",  N_atoms: "<< N_atoms_cube << std::endl;
						}
					}
				}
			}
		}
		cut_volume:
			std::cout << "Finished creating the big cube in crystal_coordinates!" << std::endl;
			if(shape_string == "cube"){cut_cube(atoms_coordinates, atoms_coordinate_cube, N_atoms, N_atoms_cube, lattice_parameter);}
			if(shape_string == "sphere"){cut_sphere(atoms_coordinates, atoms_coordinate_cube, N_atoms, N_atoms_cube, lattice_parameter);}
	}
}
	



