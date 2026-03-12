# ACC-TX-UDP

A lightweight UDP server that reads **Assetto Corsa Competizione** telemetry data from shared memory and broadcasts it in real time to connected clients over the network.

Useful for building external dashboards, sim racing overlays, or connecting to hardware devices (displays, button boxes, motion platforms) on a local network.

---

## How it works

ACC exposes real-time telemetry (physics, graphics, session data) via Windows shared memory. ACC-TX-UDP reads that data at ~60Hz and forwards it as UDP packets to any registered client.

```
┌─────────────────────┐        Shared Memory        ┌─────────────────────┐
│  Assetto Corsa      │ ──────────────────────────► │   ACC-TX-UDP        │
│  Competizione       │   acpmf_physics              │   (this server)     │
└─────────────────────┘   acpmf_graphics            └──────────┬──────────┘
                          acpmf_static                          │ UDP packets
                                                    ┌───────────┼───────────┐
                                                    ▼           ▼           ▼
                                               Client 1    Client 2    Client 3
```

---

## Requirements

- Windows 10/11
- Assetto Corsa Competizione (Steam)
- Visual Studio 2019 or later

---

## Build

1. Open `ACC-TX-UDP.sln` in Visual Studio
2. Select **Release** configuration
3. Build → Build Solution (`Ctrl+Shift+B`)

---

## Configuration

On first launch, a `config.ini` file is created automatically in the same directory as the executable:

```ini
[network]
serverPort=9999

[telemetry]
updateHz=60
```

| Parameter    | Description                              | Default |
|--------------|------------------------------------------|---------|
| `serverPort` | UDP port for both registration and data  | 9999    |
| `updateHz`   | Telemetry update frequency (1–120 Hz)    | 60      |

---

## Client registration

Clients register themselves by sending a UTF-8 string `START` to the server port. The server will then start streaming telemetry packets to that client.

```
Client  ──── "START" (UDP) ────►  Server:9999
Client  ◄─── Telemetry packets ── Server:9999
```

If a client cannot send a `START` packet (e.g. network restrictions), you can register it manually by pressing **Right Ctrl** in the server console and typing the client IP address.

---


## Architecture

| Component | Description |
|---|---|
| `SharedMemoryReaderThread` | Reads ACC shared memory at the configured Hz and updates the DataModel |
| `ClientHandler` | Listens for incoming `START` packets and registers new clients |
| `InputReaderThread` | Monitors keyboard for manual client registration (Right Ctrl) |
| `DataModel` | Thread-safe singleton holding the current telemetry snapshot and client list |
| `ThreadManager` | Manages thread lifecycle and clean shutdown |
| `ConfigManager` | Loads and provides access to `config.ini` settings |
| `ReadData` | Low-level shared memory access (open, read, close) |

---

## License

The shared memory structures in `SharedFileOut.h` are based on the official ACC SDK provided by Kunos Simulazioni.