# Industrial Asset Health Monitor

This repository contains an embedded digital signal processing (DSP) pipeline that captures real-time 3-axis acceleration data, computes the Fast Fourier Transform (FFT) on-chip, and streams both the raw time-domain waveforms and calculated frequency spectra over serial to a desktop Python visualizer.

## Technical Specifications

* **Sampling Frequency ($F_s$):** $2000\text{ Hz}$
* **Sampling Interval ($\Delta t$):** $500\,\mu\text{s}$ ($1 / 2000\text{ Hz}$)
* **FFT Block Length ($N$):** 1024 samples
* **Total Time Capture Window:** $0.512\text{ seconds}$ per frame block
* **Frequency Bin Resolution:** $\frac{2000\text{ Hz}}{1024\text{ samples}} = 1.953\text{ Hz}$ per bin
* **Nyquist Frequency Limit:** $1000\text{ Hz}$ ($F_s / 2$)
* **Serial Transmission Bandwidth:** $921,600\text{ bps}$
* **Default Plot Visualization Window:** Time-Domain: $0 - 512\text{ ms}$ | Frequency-Domain: $0 - 100\text{ Hz}$

## System Architecture

The project is structured around a localized single-core execution loop on an ESP32 microcontroller paired with an MPU6050 accelerometer over $I^2C$.

### 1. Data Collection
The system operates at a deterministic sampling rate of $2000\text{ Hz}$ using microsecond timing gating (`micros()`). A total of 1024 discrete data points are collected per block over a window of $0.512$ seconds. During this capture loop, running mathematical sums are calculated on the fly to establish the DC offset.

### 2. Signal Processing
To protect against spectral leakage and eliminate the baseline gravity component before running the FFT calculation, the system executes two embedded pre-processing routines:
* **DC Bias Removal:** The calculated mean of the active sample window is subtracted from each point to force the signal baseline to $0.0\,\text{m/s}^2$, preventing a dominant spike at $0\text{ Hz}$.
* **Windowing:** A mathematical Hamming Window function is applied to smooth the boundaries of the finite data block to zero.

The `arduinoFFT` library converts the modified time-domain array into complex coordinate vectors via the Fast Fourier Transform algorithm, outputting 512 independent magnitude frequency bins spanning $0 - 1000\text{ Hz}$.

### 3. Data Transmission Protocol
Data is packed into a structured ASCII string format and transmitted down a high-bandwidth UART hardware connection at $921,600\text{ Baud}$ to maintain real-time throughput.

**Packet Structure:**
`Time_X;Time_Y;Time_Z | FFT_X;FFT_Y;FFT_Z \n`

### 4. Desktop Visualization
The Python script acts as a rendering terminal. It reads the raw incoming serial packet, tokenizes the string by delimiters (`|` and `;`), converts the raw metrics to floating-point lists, and updates a dual-plot canvas window showing synchronized time-domain and frequency-domain charts simultaneously. The horizontal axis of the frequency spectrum plot is restricted to $0 - 100\text{ Hz}$ to focus pixels on low-frequency mechanical failure signatures (e.g., unbalance, misalignment).

---
## Repository Structure

```text
├── src/
│   └── main.cpp                    # C++ embedded source code
├── plotter/
│   └── vibration_plotter.py        # Python parsing and plotting script
├── platformio.ini                  # PlatformIO project configuration file
└── README.md                       # Documentation
