import numpy as np
import pyvista as pv

data = np.loadtxt("outputCoords.csv", delimiter=",")

x = data[:,0]
y = data[:,1]
z = data[:,2]

# Create a PyVista Plotter
plotter = pv.Plotter()
plotter.add_axes()
plotter.show_grid()

# Plot vertices
points = np.column_stack((x, y, z))
point_cloud = pv.PolyData(points)
plotter.add_points(point_cloud, color="red", point_size=15)

# Add the numbering labels
for i in range(len(points)):
    plotter.add_point_labels(points[i:i+1], [str(i)], point_size=2, text_color="red")

# draw edges
for i in range(len(points)):
    xi, yi, zi = points[i]

    for j in range(len(points)):
        if i == j:
            continue

        xj, yj, zj = points[j]
        
        if yj == yi and zj == zi and xj != xi:
            line = pv.Line(points[i], points[j])
            plotter.add_mesh(line, color="black", line_width=5)
        
        if xj == xi and zj == zi and yj != yi:
            line = pv.Line(points[i], points[j])
            plotter.add_mesh(line, color="black", line_width=5)

        if xj == xi and yj == yi and zj != zi:
            line = pv.Line(points[i], points[j])
            plotter.add_mesh(line, color="black", line_width=5)

# Show cad-like window
plotter.show(title="Domain (edges may include fictitious alignments)")