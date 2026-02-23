const express = require('express');
const { spawn } = require('child_process');
const cors = require('cors');
const path = require('path');

const app = express();
app.use(cors());

// --- C++ EXE FILE KA PATH ---
// Dhyan de: Ye path wahi hona chahiye jahan aapka .exe banta hai.
// Screenshot ke hisaab se: BACKEND -> RoadLane -> build -> Debug -> RoadLaneDetection.exe
const exePath = path.join(__dirname, '..', 'BACKEND', 'RoadLane', 'build', 'Release', 'RoadLaneDetection.exe');

let cppProcess = null;

// Start Route
app.get('/start', (req, res) => {
    if (cppProcess) return res.send('Already Running');
    console.log("Starting C++ Engine from: " + exePath);
    
    try {
        cppProcess = spawn(exePath);
        
        // Agar process start ho gaya
        if (cppProcess.pid) {
            console.log("System Started! PID:", cppProcess.pid);
            res.send('Detection Started');
        } else {
            console.error("Failed to spawn process (Check path!)");
            res.status(500).send("Failed to spawn");
        }
        
        cppProcess.stdout.on('data', (data) => console.log(`C++ Output: ${data}`));
        cppProcess.stderr.on('data', (data) => console.error(`C++ Error: ${data}`));
        
        cppProcess.on('close', (code) => {
            console.log(`System Stopped (Code: ${code})`);
            cppProcess = null;
        });
    } catch (error) {
        console.error("Error:", error);
        res.status(500).send("Failed to start");
    }
});

// Stop Route
app.get('/stop', (req, res) => {
    if (cppProcess) {
        cppProcess.kill();
        cppProcess = null;
        console.log("Stopped Manually.");
        res.send('Detection Stopped');
    } else {
        res.send('Not Running');
    }
});

app.listen(5000, () => {
    console.log('Bridge Server Ready on Port 5000 🚀');
});