@echo off
title RoadVision Launcher
echo ==========================================
echo      STARTING ROAD VISION SYSTEM v2.0
echo ==========================================
echo.

:: 1. Server Start Karna
echo >> Starting Backend Server...
start "Backend Server" cmd /k "cd server && node server.js"

:: 2. Thoda wait
timeout /t 3 /nobreak >nul

:: 3. Frontend Start Karna
echo >> Starting Frontend Interface...
start "Frontend App" cmd /k "cd frontend && npm start"

:: 4. Exit launcher
echo.
echo >> System Live! You can close this window.
timeout /t 5
exit