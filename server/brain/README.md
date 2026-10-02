# server/brain

Tofu's voice and personality.

Pipeline: head mic audio → wake word → speech-to-text → Claude (with tools) → text-to-speech → head speaker.

Planned tools exposed to Claude:

| Tool | Effect |
|---|---|
| `set_face(emotion)` | Changes the expression on the head screen |
| `walk(direction, steps)` | Short manual moves |
| `go_to(room)` | Sends a Nav2 goal to a named location |
| `go_charge()` | Starts the docking behavior |
| `get_status()` | Battery, location, charging state |
