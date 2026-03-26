# ACC-TX-UDP

A lightweight UDP telemetry server for **Assetto Corsa Competizione**.

It reads real-time data from ACC's shared memory and broadcasts it over UDP to one or more connected clients.

---

## Requirements

- Windows (ACC shared memory is Windows-only)
- Visual Studio 2019 or later
- Assetto Corsa Competizione running on the same machine

---

## Build

Open `ACC-TX-UDP.sln` in Visual Studio and build in Release or Debug configuration. No external dependencies required.

---

## Configuration

On first launch, a `config.ini` file is automatically created in the same directory as the executable:

```ini
[network]
serverPort=9999

[telemetry]
updateHz=60
```

| Parameter    | Description                              | Default |
|--------------|------------------------------------------|---------|
| `serverPort` | UDP port used for registration and data  | 9999    |
| `updateHz`   | Telemetry broadcast frequency (1–120 Hz) | 60      |

---

## Client Registration Protocol

Clients communicate with the server over UDP on `serverPort`.

| Message | Direction       | Effect                                          |
|---------|-----------------|-------------------------------------------------|
| `START` | Client → Server | Registers the client, starts receiving telemetry |
| `STOP`  | Client → Server | Unregisters the client                          |

Once registered, the client receives a binary `Packet` struct at the configured frequency.

> **Manual registration:** pressing **Right Ctrl** in the server console allows manually entering a client IP address, useful if the client cannot send a `START` packet for any reason.

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

## Adding new sims

To add support for new racing sims, add NewSimFactory.hpp inside "Factory" folder in "include", it must implement "DataFactory" interface.
Create the concrete Factory in "src/Factory" folder, the concrete factory must include his own header, the specific reader header and specific model header.
EX: ACCFactory.cpp includes "ACCFactory.hpp", "ACCReader.hpp" and "ACCModel.hpp".
Then, in the "src/DataFactory.ccp" add the header of the factory and inside thegetFactory method add your own case.

To add the Reader and Model object the procces is similar, create the header for the object (it must implement IDataModel or DataReader interface),
then create the concrete class in the "src" folder, include the header of the object and implement all the methods.

***THE MODEL MUST IMPLEMENT THE UPDATEDATA METHOD***


## License

This project is released for personal and educational use.  
ACC shared memory structures are based on the official SDK provided by Kunos Simulazioni.
