### Y2627T1 CSOPESY S04

### Group 6 Members:
- Ong, David Zachary
- Ho, Denise Liana
- Valle, Jose Edgardo
- Biacora, Luis Gabriel

### Entry file:
- main.cpp (contains main())


### How to run:
1. Open OsEmulator.sln (in this OsEmulator folder) in Visual Studio 2022
2. Set x64 Debug and press Run/Debug (F5).

### How to use:
1. Run `initialize` first. It reads `config.txt` from the working directory (this OsEmulator folder when run from Visual Studio). Until it runs, only `initialize` and `exit` are recognized. The keys are:
   - `num-cpu` (1-128): number of CPU cores
   - `scheduler`: `"fcfs"` or `"rr"`
   - `quantum-cycles`: time slice per core for `rr`
   - `batch-process-freq`: CPU ticks between generated processes
   - `min-ins` / `max-ins`: range of lines per generated process
   - `delay-per-exec` (or `delays-per-exec`): busy-wait ticks between instructions
   - `tick-ms` (1-1000, optional, default 10): real time length of one CPU tick in milliseconds
2. Commands:
   - `initialize`, `exit`, `clear`, `help`
   - `screen -s <name>`: create a process and open its screen (`process-smi` shows it, `exit` goes back to the main menu)
   - `screen -r <name>`: open the screen of a process that is still running
   - `screen -ls`: list running and finished processes
   - `scheduler-start` / `scheduler-stop`: start or stop generating dummy processes (p01, p02, ...)
   - `report-util`: save the CPU utilization report to `csopesy-log.txt` in the working directory
   - `start_marquee`, `stop_marquee`, `set_text <text>`, `set_speed <speed>`

Program is a console OS emulator.
