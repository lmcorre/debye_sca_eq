import sys
sys.path.insert(0, '/home/lmcorrea/debye_sca_eq/debye_scattering/base_crystals')
import crystal
import cif_read
import numpy as np
import matplotlib.pyplot as plt

build_crystal = True
test_diameter = False



cif_path = ("/home/lmcorrea/debye_sca_eq/debye_scattering/base_crystals/au_test.cif")

input_dict = {}
input_dict = {
        'crystal': "cubic",
        #'shape': "sphere",
        #'diameter': 4,  
        'base': [[0, 0, 0], [1/2, 1/2, 0], [1/2, 0, 1/2], [0, 1/2, 1/2]],
        'shape': "cube",
        'edge_length': 4,
}

cif_read.read_from_path(cif_path, input_dict)
print(input_dict)
if build_crystal == True:

    n_atoms = crystal.count_atoms(input_dict)
    print(f"Number of atoms = {n_atoms}")
    coordinates  = crystal.crystal_build(input_dict, n_atoms)

    unique_coordinates, unique_index = np.unique(coordinates, axis = 0, return_index=True)
    print(f"Coordinates output: {len(coordinates)}")
    print(f"Coordinates unique: {len(unique_coordinates)}")
    print(f"Maximum distance: {np.amax(np.sqrt(np.sum(coordinates**2, axis  =1)))}")

    if(len(coordinates) != len(unique_coordinates)):
        for i in range(len(unique_coordinates)):
            if(i not in unique_index):
                print(f"Repeated coordinate: {unique_coordinates[i]}")


    fig, axs = plt.subplots(3,1, figsize = (10,30))
    for i in range(len(coordinates)):
        axs[0].plot(coordinates[i, 0],coordinates[i,1], 'ro')
        axs[1].plot(coordinates[i, 1],coordinates[i,2], 'bo')
        axs[2].plot(coordinates[i, 2],coordinates[i,0], 'ko')
    plt.savefig("./fig_test.png")
    plt.close()



if test_diameter == True:

    for diameter in range(3, 9, 1):
        coor_test = []
        max_range = 2*int(diameter/input_dict['length_a'])
        for base_vector in np.array(input_dict['base']):
            for x in range(-max_range, max_range, 1):
                for y in range(-max_range, max_range, 1):
                    for z in range(-max_range, max_range, 1):
                        v = input_dict['length_a']*(np.array([x, y, z]) + base_vector)
                        if (np.dot(v,v) <= (diameter/2)**2):
                            coor_test.append(v)
        coor_unique  = np.unique(coor_test, axis = 0)
        N_true = coor_unique.shape[0]
        N_app = int(np.around(4 * ((4*np.pi/3) * (diameter/2)**3) / input_dict['cell_volume'], 0))
        plt.plot(diameter, N_true, 'ro')
        plt.plot(diameter, N_app , 'bo')
        print("xxxxxxxxxxxxxxxxxxxxxxxxxxx")
        print(f"Diameter {diameter} nm, Difference: {100*abs(N_true - N_app)/N_true}")
        print(f"True: {N_true}, Aprox {N_app}") 
    plt.savefig('test_diameter.png')
    plt.close()
