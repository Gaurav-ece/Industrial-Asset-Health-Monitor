import serial
import matplotlib.pyplot as plt

# --- CONFIGURATION ---
SERIAL_PORT = "COM5"   # Change this to your active ESP32 port
BAUD_RATE   = 921600   # Must match your ESP32 baud rate
N           = 1024     # Expected Time-Domain samples
F_POINTS    = 512      # Expected Frequency-Domain bins

time_axis = [i * 0.5 for i in range(N)]
frequency_axis = [i * (2000.0 / 1024.0) for i in range(F_POINTS)]

plt.ion() 
fig, (ax_time, ax_freq) = plt.subplots(2, 1, figsize=(10, 7))
fig.suptitle("Vibration Analyzer Dashboard", fontsize=14, fontweight='bold')

print(f"Opening port {SERIAL_PORT}...")

try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    
    while True:
        if ser.in_waiting > 0:
            raw_packet = ser.readline().decode('utf-8', errors='ignore').strip()
            
            domains = raw_packet.split("|")
            if len(domains) < 2:
                continue
                
            time_section = domains[0]
            freq_section = domains[1]
            
            time_axes = time_section.split(";")
            freq_axes = freq_section.split(";")
            
            if len(time_axes) >= 3 and len(freq_axes) >= 3:
                t_x = [float(val) for val in time_axes[0].split(",") if val]
                t_y = [float(val) for val in time_axes[1].split(",") if val]
                t_z = [float(val) for val in time_axes[2].split(",") if val]
                
                f_x = [float(val) for val in freq_axes[0].split(",") if val]
                f_y = [float(val) for val in freq_axes[1].split(",") if val]
                f_z = [float(val) for val in freq_axes[2].split(",") if val]
                
                if len(t_x) == N and len(f_x) == F_POINTS:
                    
                    ax_time.clear() 
                    ax_time.plot(time_axis, t_x, 'r-', label="X-Axis (Horizontal)")
                    ax_time.plot(time_axis, t_y, 'g-', label="Y-Axis (Vertical)")
                    ax_time.plot(time_axis, t_z, 'b-', label="Z-Axis (Axial)")
                    ax_time.set_title("Real-Time Waveform (Time Domain)")
                    ax_time.set_ylabel("Acceleration (m/s²)")
                    ax_time.set_ylim(-20, 20)
                    ax_time.grid(True, linestyle=':')
                    ax_time.legend(loc="upper right")
                    
                    ax_freq.clear() 
                    ax_freq.plot(frequency_axis, f_x, 'r-')
                    ax_freq.plot(frequency_axis, f_y, 'g-')
                    ax_freq.plot(frequency_axis, f_z, 'b-')
                    ax_freq.set_title("Frequency Spectrum (On-Chip FFT)")
                    ax_freq.set_xlabel("Frequency (Hz)")
                    ax_freq.set_ylabel("Magnitude")
                    ax_freq.set_xlim(0, 300)
                    ax_freq.set_ylim(0, 150)
                    ax_freq.grid(True, linestyle=':')
                                        
                    plt.tight_layout()
                    plt.pause(0.001)

except KeyboardInterrupt:
    print("\nClosing application.")
    ser.close()