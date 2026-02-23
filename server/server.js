const express = require('express');
const { spawn } = require('child_process');
const cors = require('cors');
const path = require('path');

const app = express();
app.use(cors());


const exePath = path.join(__dirname, '..', 'BACKEND', 'RoadLane', 'build', 'Release', 'RoadLaneDetection.exe');

let cppProcess = null;


app.get('/start', (req, res) => {
    if (cppProcess) return res.send('Already Running');
    console.log("Starting C++ Engine from: " + exePath);
    
    try {
        cppProcess = spawn(exePath);
        

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