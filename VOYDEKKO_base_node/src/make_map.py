import numpy as np
from scipy.interpolate import griddata
import matplotlib.pyplot as plt

# Measured coordinates
x = np.array([0, 190, 420, 680, 350, 280, 120, 280, 230])
y = np.array([300, 255, 200, 50, 180, 40, 50, 120, 180])

# Measured RSSI
rssi = np.array([-55, -85, -72, -95, -75, -85, -98, -110, -100])
rssi_min = -120
rssi_max = -30
snr = np.array([11, 10, 8.8, 7.5, 8.2, 6.8, 6.5, 5, 8])
snr_min = -5
snr_max = 12


# Create regular grid
grid_x, grid_y = np.meshgrid(
    np.linspace(0, 700, 700),
    np.linspace(0, 300, 300)
)

# Interpolate
grid = griddata(
    (x, y),
    snr,
    (grid_x, grid_y),
    method="cubic"
)

plt.imshow(
    grid,
    extent=(0, 700, 0, 300),
    origin="lower",
    aspect="auto",
    vmin=snr_min,
    vmax=snr_max
)

plt.scatter(x, y, color="black")
plt.colorbar(label="RSSI (dBm)")
plt.xlabel("Distance X (m)")
plt.ylabel("Distance Y (m)")
plt.show()