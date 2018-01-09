# Server API Commands

| Command                                            | Request Syntax | Server Response  | Observation                                              |
|----------------------------------------------------|----------------|------------------|----------------------------------------------------------|
| Get current measured illuminance at desk (i).      | g l (i)        | l (i) (val)      | (val): float measured illuminance [lx]                   |
| Get current duty cycle at LED (i).                 | g d (i)        | d (i) (val)      | (val): float measured duty cycle  [%]                    |
| Get current occupancy state at desk (i).           | g o (i)        | o (i) (val)      | (val): bool  occupancy state [off/on]                    |
| Get current illuminance lower bound at desk (i).   | g L (i)        | L (i) (val)      | (val): float illuminance lower bound [lx]                |
| Get current reference at desk (i).                 | g r (i)        | r (i) (val)      | (val): float reference [lx]                              |
| Get instantaneous power consumption at desk (i).   | g p (i)        | p (i) (val)      | (val): float instantaneous power consumption [W]         |
| Get total instantaneous power consumption.         | g p T          | p T (val)        | (val): float total instantaneous power consumption [W]   |
| Get accumulated energy consumption at desk (i).    | g e (i)        | e (i) (val)      | (val): float accumulated energy consumption [J]          |
| Get total accumulated energy consumption.          | g e T          | e T (val)        | (val): float total accumulated energy consumption [J]    |
| Get accumulated comfort error at desk (i).         | g c (i)        | c (i) (val)      | (val): float accumulated comfort error [lx]              |
| Get total accumulated comfort error.               | g c T          | c T (val)        | (val): float total accumulated comfort error [lx]        |
| Get accumulated comfort variance at desk (i).      | g v (i)        | v (i) (val)      | (val): float accumulated comfort variance [lx/s^2]       |
| Get total accumulated comfort variance.            | g v T          | v T (val)        | (val): float total accumulated comfort variance [lx/s^2] |
| Set occupancy state at desk (i).                   | s (i) (val)    | ack              | (val): bool  occupancy state [off/on]                    |
| Restart system.                                    | r              | ack              | Resets the system.                                       |
| Get last minute buffer of var (x) at desk (i).     | b (x) (i)      | b (x) (i) (vals) | Values are returned in csv string                        |
| Start stream of var (x) at desk (i)                | c (x) (i)      | c (x) (i) (time) | Intiates data stream. x can be "l" or "d"                |
| Stop stream of var (x) at desk (i)                 | d (x) (i)      | d (x) (i) (time) | Interrupts data stream. x can be "l" or "d"              |
