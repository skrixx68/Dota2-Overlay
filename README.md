# Dota2 Overlay
The purpose if this program is to notify the user if his current position is visible to the enemy team.
There are 2 programs that does the job individualy, One checks the memory for updated information and the other display the current information through overlay.

The overlay is designed simply using c# forms(yep no need for direct draw or stuff :P) to lessen CPU/GPU usage while still updating realtime.

Memory reading is also done safely to avoid simple detections by minimizing privilege and is done externally.

Usage :
Run Dota2Overlay.exe ingame
