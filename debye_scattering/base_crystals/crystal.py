import numpy as np 
import ctypes 
import os 

path  = "/home/lmcorrea/debye_sca_eq/src/base_crystal/crystal.so"  
if(os.path.isfile(path) == True):
    print(f"File in path {path} found")
    crystal = ctypes.cdll.LoadLibrary(path)
else:
    print(f"File in path {path} not found")





#count atoms
def count_atoms(input_dict):
    
    if (input_dict['shape'] == "sphere"):
        required = ['cell_volume', 'diameter', 'N_atoms_unit_cell']
        for value in required:
            if(value not in input_dict):
                    raise ValueError(f"Parameter not present! Sphere shape requires: {value}")
            n_atoms = int(input_dict['N_atoms_unit_cell'] * ((4*np.pi/3) * (input_dict['diameter']/2)**3) / input_dict['cell_volume']) 
    
    elif(input_dict['shape'] == "cube"):
         required = ['edge_length', 'cell_volume', 'N_atoms_unit_cell']                       
         for value in required:                                                  
             if(value not in input_dict):
                raise ValueError(f"Parameter not present! Cube shape requires: {value}")
         n_atoms =  int(input_dict['N_atoms_unit_cell'] * (input_dict['edge_length'])**3 / input_dict['cell_volume']) 
    
    else:
        raise ValueError(f"Select a valid shape: sphere, cube")
    return n_atoms

# cubic ########

def crystal_build(input_dict, n_atoms):
    coordinates = np.zeros((n_atoms, 3), dtype = np.float32)
    lattice_length = np.array((input_dict['length_a'], input_dict['length_b'], input_dict['length_c']), dtype = np.float32)
    ctypes_float_array_coordinates = np.ctypeslib.ndpointer(dtype = np.float32,
                                                shape = coordinates.shape,
                                                flags = 'CONTIGUOUS')
    ctypes_float_array_lattice = np.ctypeslib.ndpointer(dtype = np.float32,
                                                shape = lattice_length.shape,
                                                flags = 'CONTIGUOUS')
    
    crystal.cubic.argtypes = [ctypes_float_array_coordinates,
                              ctypes_float_array_lattice, 
                              ctypes.c_int, ctypes.c_char_p]
    if(input_dict['crystal'] == "cubic"):
        crystal.cubic(coordinates, lattice_length, n_atoms, input_dict["shape"].encode('utf-8'))
    #print(coordinates)
    return coordinates

# if(len(coordinates) == 0):
   #     print("Error in coordinates calculation")
   #     return 0
   # else:
   #     return coordinates

