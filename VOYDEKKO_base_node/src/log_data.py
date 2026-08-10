import serial
import time
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

packet_id = []
rssi = []
snr = []
temperature = []

packet_id_log_file = open("packet_id_log.txt", "w")
temp_log_file = open("temp_log.txt", "w")
rssi_log_file = open("rssi_log.txt", "w")
snr_log_file = open("snr_log.txt", "w")

s = serial.Serial("/dev/ttyUSB0", 115200)

max_samples = 1_000_000
reset = 0

fig, (ax1, ax2, ax3) = plt.subplots(3, 1)


print("Specify duration of value logging (in minutes): ")
duration = int(int(input()) * 60)

start = time.time()


def update(frame):
    global reset
    # Stop after duration
    if time.time() - start >= duration:

        # Save final graph snapshot
        fig.savefig("voydekko_final_graph.png", dpi=300)

        packet_id_log_file.close()
        temp_log_file.close()
        rssi_log_file.close()
        snr_log_file.close()

        s.close()

        plt.close(fig)
        return


    line = s.readline().decode("utf-8", errors="ignore").strip()

    if line:

        split_line = line.split(" ")

        try:

            if split_line[0] == "PKT_ID:":
                packet_id.append(int(split_line[1]))
                packet_id_log_file.write(split_line[1] + "\n")
                if(int(split_line[1]) == 999):
                    reset = 1

            elif split_line[0] == "TMP:":
                temperature.append(float(split_line[1]))
                temp_log_file.write(split_line[1] + "\n")

            elif split_line[0] == "RSSI:":
                rssi.append(float(split_line[1]))
                rssi_log_file.write(split_line[1] + "\n")

            elif split_line[0] == "SNR:":
                snr.append(float(split_line[1]))
                snr_log_file.write(split_line[1] + "\n")


            # Limit arrays
            if (reset == 1):
                for arr in [packet_id, temperature, rssi, snr]:
                    arr.clear()
            else:        
                for arr in [packet_id, temperature, rssi, snr]:
                    if len(arr) > max_samples:
                        arr.pop(0)

        except ValueError:
            print("Bad line:", line)


    # Only plot when data lengths match
    n = min(len(packet_id), len(temperature), len(rssi), len(snr))

    if n > 0:
        ax1.clear()
        ax2.clear()
        ax3.clear()

        ax1.plot(packet_id[:n], temperature[:n])
        ax1.set_ylabel("Temperature")

        ax2.plot(packet_id[:n], rssi[:n])
        ax2.set_ylabel("RSSI")

        ax3.plot(packet_id[:n], snr[:n])
        ax3.set_ylabel("SNR")
        ax3.set_xlabel("Packet ID")


ani = FuncAnimation(
    fig,
    update,
    interval=50
)

plt.tight_layout()
plt.show()