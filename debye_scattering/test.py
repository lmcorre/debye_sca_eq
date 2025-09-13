import sys
sys.path.insert(0, '/home/lmcorrea/debye_sca_eq/debye_scattering/base_crystals')
import crystal
import cif_read
import numpy as np
import matplotlib.pyplot as plt

cif_path = ("/home/lmcorrea/debye_sca_eq/debye_scattering/base_crystals/au_test.cif")

input_dict = {}
input_dict = {
        'crystal': "cubic",

        'shape': "sphere",
        'diameter': 4,  

        #'shape': "cube",
        #'edge_length': 4,
}

cif_read.read_from_path(cif_path, input_dict)
print(input_dict)

n_atoms = crystal.count_atoms(input_dict)
print(f"Number of atoms = {n_atoms}")
coordinates  = crystal.crystal_build(input_dict, n_atoms)
print(f"Coordinates output: {len(coordinates)}")
print(f"Coordinates unique: {len(np.unique(coordinates, axis = 0))}")

n = np.zeros(len(coordinates))
for i, r1 in enumerate(coordinates):
    for r2 in coordinates:
        if(r1[0] == r2[0] and r1[1] == r2[1] and r1[2]== r2[2]):
            n[i] += 1

np.savetxt("test_n.txt", n)

fig, axs = plt.subplots(3,1, figsize = (10,30))
for i in range(len(coordinates)):
    axs[0].plot(coordinates[i, 0],coordinates[i,1], 'ro')
    axs[1].plot(coordinates[i, 1],coordinates[i,2], 'bo')
    axs[2].plot(coordinates[i, 2],coordinates[i,0], 'ko')
plt.savefig("./fig_test.png")
plt.close()
