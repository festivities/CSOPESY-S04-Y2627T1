# Y2627T1 CSOPESY S04 – Group 6: MO1 Process Scheduler and CLI

This branch contains **MO1 – Process Scheduler and CLI** by **CSOPESY – S04, Group 6**. It extends the console OS emulator from the earlier projects with a CPU scheduler (FCFS / Round Robin), a `screen`-style process multiplexer, generated process instructions and a CPU utilization report.

Earlier projects are listed under [Previous Projects](#previous-projects). Submitted versions are in our **[Releases/Tags](https://github.com/festivities/CSOPESY-S04-Y2627T1/releases)**.

## Group 6 Members
- Ong, David Zachary
- Ho, Denise Liana
- Valle, Jose Edgardo
- Biacora, Luis Gabriel


## MO1 – Process Scheduler and CLI
- Located in the `.\OsEmulator` folder. See `README.txt` under that folder for the full command list and details.  
- **Entry file:** `OsEmulator\main.cpp` (contains `main()`)  
- **Branch:** [mo1/dev/scheduler](https://github.com/festivities/CSOPESY-S04-Y2627T1/tree/mo1/dev/scheduler/OsEmulator)

### Source Files
| File | Purpose |
| :--- | :--- |
| `main.cpp` | Main menu input loop. Recognizes commands and calls the `OperatingSystem` class |
| `OperatingSystem.h/.cpp` | Command handlers: `initialize` (reads `config.txt`), process screens, `screen -ls`, `report-util` |
| `Scheduler.h/.cpp` | Tick-based CPU scheduler thread: ready queue, cores, sleeping and finished processes (FCFS / RR) |
| `Process.h/.cpp` | Process and its instructions (`PRINT`, `DECLARE`, `ADD`, `SUBTRACT`, `SLEEP`, `FOR`) |
| `Marquee.h/.cpp` | Marquee animation kept from MO3, plus the console mutex |
| `config.txt` | Scheduler and process settings read by `initialize` |

### Commands
- `initialize` – read `config.txt` and start the scheduler. Only `initialize` and `exit` work before this.
- `screen -s <name>` – create a process and open its screen (`process-smi` shows it, `exit` returns to the main menu).
- `screen -r <name>` – reopen the screen of a process that has not finished.
- `screen -ls` – show CPU utilization, cores used/available, running and finished processes.
- `scheduler-start` / `scheduler-stop` – start or stop generating dummy processes (`p01`, `p02`, ...).
- `report-util` – save the `screen -ls` report to `csopesy-log.txt`.
- `exit` – close the emulator.

### config.txt
Space-separated key and value per line. Example:

```
num-cpu 4
scheduler "rr"
quantum-cycles 5
batch-process-freq 1
min-ins 1000
max-ins 2000
delay-per-exec 0
tick-ms 10
```

| Key | Range | Description |
| :--- | :--- | :--- |
| `num-cpu` | 1 – 128 | Number of CPU cores |
| `scheduler` | `"fcfs"` or `"rr"` | Scheduling algorithm |
| `quantum-cycles` | 1 – 2^32 | Time slice in CPU ticks (RR only) |
| `batch-process-freq` | 1 – 2^32 | A new process is generated every N ticks during `scheduler-start` |
| `min-ins` / `max-ins` | 1 – 2^32 | Range of instructions per process |
| `delay-per-exec` (or `delays-per-exec`) | 0 – 2^32 | Busy-wait ticks between instructions |
| `tick-ms` (optional) | 1 – 1000 | Real time length of one CPU tick in milliseconds (default 10) |

Values outside a range are clamped. A missing key, a non-numeric value or an unknown scheduler makes `initialize` fail with an error message.


## How to Run
Developed and tested using **Visual Studio 2022** (Platform Toolset v143, ISO C++20 Standard `/std:c++20`).

1. Open `OsEmulator\OsEmulator.sln` in **Visual Studio 2022**.  
2. Ensure the configuration is set to **Debug | x64**.  
3. Build the project (`Build > Build Solution` or press `Ctrl+Shift+B`).  
4. Run the project (`Debug > Start Without Debugging` or press `Ctrl+F5`).  
5. Type `initialize`, then `scheduler-start` and `screen -ls`.

`config.txt` is read from the working directory, which is the `OsEmulator` folder when run from Visual Studio. `csopesy-log.txt` is written to the same folder.


## Previous Projects

### Week 2 Homework – OS Emulator CLI Setup
- Located in the `.\OsEmulator` folder.  
- **Release:** [Week 2 Submission](https://github.com/festivities/CSOPESY-S04-Y2627T1/releases/tag/submission%3Dweek2-os-emulator)

### MO3 – OS Emulator CLI (Marquee)
- Located in the `.\OsEmulator` folder.  
- **Release:** [Week 4 Submission](https://github.com/festivities/CSOPESY-S04-Y2627T1/releases/tag/submission%3Dweek4-mo3)

### MO4 – OS Emulator Desktop GUI (Dear ImGui)
- Located in the `.\DesktopUI` folder. See `README.txt` under that folder for details.  
- **Main Branch:** [DesktopUI](https://github.com/festivities/CSOPESY-S04-Y2627T1/tree/main/DesktopUI)  
- **Release:** [Week 5 Submission](https://github.com/festivities/CSOPESY-S04-Y2627T1/releases/tag/submission%3Dweek5-mo4)
