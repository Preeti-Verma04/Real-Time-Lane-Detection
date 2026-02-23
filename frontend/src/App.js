import React, { useState, useEffect, useRef } from 'react';
import './App.css';
import background from './images.jpeg';
import carImage from './car2-removebg-preview.png';

function App() {
  const [isRunning, setIsRunning] = useState(false);
  const [logs, setLogs] = useState(["System Ready...", "Waiting for user input..."]);
  const logsEndRef = useRef(null);

  const scrollToBottom = () => {
    logsEndRef.current?.scrollIntoView({ behavior: "smooth" });
  };

  useEffect(() => {
    scrollToBottom();
  }, [logs]);

  useEffect(() => {
    let interval;
    if (isRunning) {
      interval = setInterval(() => {
        const messages = [
          "Processing Frame: ID #4021...",
          "Lane Curvature: 0.04 rad - Stable",
          "Vehicle Speed: 45 km/h",
          "Object Detection: Clear",
          "ROI Updated: [200, 400, 600, 800]",
          "Memory Usage: 34MB... OK"
        ];
        const randomMsg = messages[Math.floor(Math.random() * messages.length)];
        const time = new Date().toLocaleTimeString();
        setLogs(prev => [...prev.slice(-5), `[${time}] ${randomMsg}`]);
      }, 1500);
    }
    return () => clearInterval(interval);
  }, [isRunning]);

  // --- YE NAYA CODE HAI ---

  const handleStart = async () => {
    // 1. Pehle user ko dikhao ki process shuru ho rahi hai
    setLogs(prev => [...prev, ">> SYSTEM INITIALIZING...", ">> CONNECTING TO ENGINE..."]);

    try {
      // 2. Server ko call karo (Ye line asli magic karegi)
      const response = await fetch('http://localhost:5000/start');
      
      // 3. Server ka jawab suno
      const message = await response.text(); 
      console.log("Server Response:", message);

      // 4. Agar sab sahi hai, to UI update karo
      if (response.ok) {
        setIsRunning(true);
        setLogs(prev => [...prev, `>> SERVER: ${message}`, ">> SUCCESS: SYSTEM ONLINE"]);
      } else {
        setLogs(prev => [...prev, ">> ERROR: SERVER REFUSED TO START"]);
      }

    } catch (error) {
      // Agar Server band hai ya koi error hai
      console.error("Connection Failed:", error);
      setLogs(prev => [...prev, ">> CRITICAL ERROR: CANNOT CONNECT TO SERVER", ">> CHECK IF BACKEND IS RUNNING"]);
      alert("Server connect nahi ho raha! Kya aapne 'node server.js' chalaya hai?");
    }
  };

  const handleStop = async () => {
    try {
      // Stop signal bhejo
      await fetch('http://localhost:5000/stop');
      
      setIsRunning(false);
      setLogs(prev => [...prev, ">> STOP SIGNAL SENT...", ">> SYSTEM HALTED."]);
    } catch (error) {
      console.error("Stop Failed:", error);
      setLogs(prev => [...prev, ">> ERROR: FAILED TO STOP BACKEND"]);
    }
  };

  // -------------------------

  return (
    <div className="app-container" style={{ backgroundImage: `url(${background})` }}>
      <div className="overlay">
        
        {/* Header */}
        <div className="header-bar">
           <div className="brand">ROADVISION <span className="version">v2.0</span></div>
           <div className="status-light">
              <span className={`indicator ${isRunning ? 'blink-green' : 'red'}`}></span>
              {isRunning ? "SYSTEM ACTIVE" : "STANDBY"}
           </div>
        </div>

        {/* Main Interface */}
        <div className="main-interface">
          
          {/* Left: Start Button (Spinning Ring) */}
          <div className="control-group">
            <div className={`hud-ring start-ring ${isRunning ? 'spinning' : ''}`}></div>
            <button className="btn start-btn" onClick={handleStart} disabled={isRunning}>
              START
            </button>
          </div>

         

          {/* Center: Car Icon & Title */}
          <div className="center-display">

            {/* NEW: Realistic Digital Car Image */}
            <div className="car-icon-container">
                {/* Ye link ek transparent futuristic car ka hai */}
                <img
                  src={carImage}
                  alt="Future Car"
                  className="digital-car-img"
                />
            </div>

            {/* Car ke just neeche ye add karo */}
<div className="hud-metrics">
    <div className="metric-box">
        <span className="metric-label">SPEED</span>
        <span className="metric-value">0<span className="unit">km/h</span></span>
    </div>
    <div className="metric-box">
        <span className="metric-label">STEERING</span>
        <span className="metric-value">0°<span className="unit">deg</span></span>
    </div>
</div>

            {/* UPDATED TITLE: Pura naam likha hai */}
            <h1 className="cyber-title">ROAD LANE<br/>DETECTION</h1>
            <h3 className="sub-title">AI-Powered Driving Assistant</h3>

            {/* Video Feed Box */}
            {isRunning && (
              <div className="video-frame">
                <div className="scan-line"></div>
                <p>LIVE FEED ACTIVE</p>
              </div>
            )}
          </div>

{/* -------------------------------------------------- */}
            
            

          {/* Right: Stop Button (Pulsing Ring) */}
          <div className="control-group">
            {/* Stop Ring: Hamesha dikhega, lekin stop dabane par gayab ho jayega ya alag effect dega */}
            <div className={`hud-ring stop-ring ${!isRunning ? 'pulsing' : ''}`}></div>
            <button className="btn stop-btn" onClick={handleStop} disabled={!isRunning}>
              STOP
            </button>
          </div>

        </div>

        {/* Footer Logs */}
        <div className="terminal-box">
          <div className="terminal-header">{/* SYSTEM LOGS */}</div>
          <div className="logs-content">
            {logs.map((log, index) => (
              <div key={index} className="log-line">{log}</div>
            ))}
            <div ref={logsEndRef} />
          </div>
        </div>

      </div>
    </div>
  );
}

export default App;