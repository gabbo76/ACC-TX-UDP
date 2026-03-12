# ACC-TX-UDP

A lightweight UDP server that reads telemetry data from **Assetto Corsa Competizione** via shared memory and broadcasts it to connected clients over the network.

## How it works

ACC exposes real-time telemetry (physics, graphics, session info) through Windows shared memory. This server reads that data at a configurable frequency and forwards it as UDP packets to all registered clients.

## Requirements

- Windows 10/11
- Assetto Corsa Competizione running on the same machine
- Visual Studio 2019 or later (with the C++ Desktop Development workload)

## Building

1. Clone the repository
2. Open `ACC-TX-UDP.sln` in Visual Studio
3. Select the desired configuration (`Debug` or `Release`)
4. Build with **Ctrl+Shift+B**

## Configuration

On first launch, the server automatically creates a `config.ini` file in the same directory as the executable:

```ini
[network]
serverPort=9999

[telemetry]
updateHz=60
```

| Parameter    | Description                              | Default |
|--------------|------------------------------------------|---------|
| `serverPort` | UDP port used to send and receive data   | `9999`  |
| `updateHz`   | Telemetry broadcast frequency (1–120 Hz) | `60`    |

## Client registration protocol

Communication is based on plain UDP. To receive telemetry, a client must first register with the server.

**Register:**
Send the string `START` as a UDP packet to the server on `serverPort`. The server will add the client's IP and port to the active list and start sending telemetry packets.

**Unregister:**
Send the string `STOP` to remove the client from the list.

**Manual registration:**
If a client cannot send `START` for any reason, press **Right Ctrl** while the server is running and type the client's IP address manually.
