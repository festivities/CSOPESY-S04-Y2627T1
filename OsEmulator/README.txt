### Y2627T1 CSOPESY S04

### Group 6 Members:
- Ong, David Zachary
- Ho, Denise Liana
- Valle, Jose Edgardo
- Biacora, Luis Gabriel

Program is a console OS emulator.



### Entry file:
- main.cpp (contains main())



### How to run:
1. Open OsEmulator.sln (in this OsEmulator folder) in Visual Studio 2022
2. Set x64 Debug
3. Set the working directory to the `OsEmulator` folder so that `config.txt` can be found
4. Build the solution
5. Run without debugging or press F5



### How to use:
1. Run `initialize` first. It reads `config.txt` from the working directory (this OsEmulator folder when run from Visual Studio). Until it runs, only `initialize` and `exit` are recognized. If configuration validation fails, the program will fail to initialize. The required keys are:
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

   
Example configuration:

num-cpu 4
scheduler "rr"
quantum-cycles 5
batch-process-freq 1
min-ins 1000
max-ins 2000
delays-per-exec 0
tick-ms 10




### Main console commands
- `initialize`: load `config.txt` and start the scheduler.
- `exit`: terminate the emulator.
- `help`: display available commands.
- `clear`: clear the console.
- `scheduler-start`: continuously generate dummy processes.
- `scheduler-stop`: stop generating new dummy processes. Existing processes continue executing.
- `report-util`: write the CPU utilization and process report to `csopesy-log.txt`.



### Screen commands
- `screen -s <name>`: create a named process and enter its process screen.
- `screen -r <name>`: attach to an existing unfinished process.
- `screen -ls`: display CPU utilization, core usage, running processes, and finished processes. The current implementation may also show ready and sleeping processes as diagnostic sections.

Inside a process screen:

- `process-smi`: display the process name, ID, instruction progress, and PRINT logs. A completed process displays `Finished!`.
- `exit`: return to the main console.

If a process does not exist or has already finished, `screen -r <name>` displays "Process <name> not found."

Process instructions are generated internally and are not typed by the user. The supported instruction types are `PRINT`, `DECLARE`, `ADD`, `SUBTRACT`, `SLEEP`, and nested `FOR` loops.



### Optional marquee commands
The marquee feature was retained from a previous project:

- `start_marquee`: start the marquee animation.
- `stop_marquee`: stop the marquee animation.
- `set_text <text>`: change the marquee text.
- `set_speed <speed>`: set the marquee speed in milliseconds. The value must be a positive integer.