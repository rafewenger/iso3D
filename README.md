# iso3D
 3D isosurface generation code

Classes and routines for representing scalar grids,
reading/writing nrrd files into/from scalar grids.
Also includes some simple programs for processing scalar grids.

## Programs

iso3D_nrrd_info.cpp: Read an nrrd file and output information
about the scalar grid stored in the nrrd file.

iso3D_scalar.cpp: Read an nrrd file, apply some operation
to the scalar grid, and then write an nrrd file.

Directory Test contains programs to test the iso3D data structures
and routines.
