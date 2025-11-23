# OpenFOAM blockMeshDict generator
blockMeshDict is a file used in the OpenFOAM open-source code to generate simple (but high quality) structured meshes.\
This code generates a blockMeshDict file partially filled with the necessary data, based on the blocks smoothly created by the user in the code. The missing parts can be easily filled thanks to the CAD-like visualization of the fluid domain with the numbered vertices.\
Outputs of the application:
* blockMeshDict file with already included: **list of vertices coordinates**, **list of hex blocks** (no vertices, mesh and grading included), **list of typical boundaries** (inlet, outlet, walls).
* **3D visualization** of the generated **fluid domain with numbered vertices** to simplify and speed up filling the missing sections.
