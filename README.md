# 🛣️ Real-Time Advanced Lane Detection & ADAS

A high-performance, full-stack Advanced Driver Assistance System (ADAS). It bridges a computationally heavy C++ visual engine with a modern React.js dashboard, delivering real-time mathematical lane tracking and a Sci-Fi inspired HUD at a stable 28+ FPS.

## ✨ Key Features

* **🧠 Core Vision Engine:** Custom C++ pipeline using Grayscale, Gaussian Blur, Canny Edge Detection, and Hough Transforms.
* **🚨 Smart Danger Alerts:** Dynamic "Smart Carpet" algorithm that calculates steering deviation and recolors the road overlay (Green/Yellow/Red).
* **🎯 Sci-Fi Telemetry HUD:** Real-time transparent dashboard rendering targeting crosshairs, deviation percentages, and dynamic steering arrows.
* **🚁 Top-Down Radar:** "Bird's Eye" mini-map visualizing the car's position relative to lane curves.
* **⚡ Performance Optimized:** Downscales processing frames while upscaling UI, maintaining high FPS without dedicated GPU hardware.
* **🔗 Full-Stack Bridge:** Node.js backend seamlessly connects the compiled C++ executable to a React.js control panel.



<video src="R.mp4" controls="controls" width="100%"></video>

## ⚙️ Architecture
1. **Frontend (React.js):** Dashboard to initialize the system and view live stats.
2. **Backend (Node.js/Express):** Bridge server routing data and spawning the C++ process.
3. **Vision Engine (C++):** Executes mathematical lane calculations and renders the video output.

## 🚀 How to Run

1. Clone the repository.
2. Ensure Node.js, CMake, and OpenCV are installed.
3. Build the C++ executable in `Release` mode.
4. Run `StartProject.bat` to concurrently launch all microservices.
5. Click **START** on the React web dashboard!

---
*Developed by Preeti Verma*
