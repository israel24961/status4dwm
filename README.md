# STATUS4DWM
Simple status for [dwm](https://dwm.suckless.org/)

## Build
A simple make would be enough
```
git clone https://github.com/israel24961/status4dwm
cd status4dwm
make
```

### Debug Mode
To see exactly when xsetroot updates occur, build with debug mode:
```
make debug
```
This will print timestamps and messages to stderr whenever the status bar updates, for example:
```
[2026-02-17 12:34:56] xsetroot update: 2.000000Secs:5|5.000000Secs:2|💿0.15,0.12,0.08|hi
[2026-02-17 12:34:58] xsetroot update: 2.000000Secs:6|5.000000Secs:2|💿0.15,0.12,0.08|hi
```

## Status Bar Updates

### When does it write to xsetroot?
The status bar is updated using `xsetroot -name` **every time any task's timer fires**. 

For example, with the default configuration:
- Task with 2 second period → updates every 2 seconds
- Task with 3 second period → updates every 3 seconds  
- Task with 5 second period → updates every 5 seconds

**Result**: The status bar updates frequently (whenever any timer fires), ensuring all information stays current.

### Update Mechanism
- Uses **libev** event loop with `ev_timer` watchers
- Each task has its own timer based on its `period_secs`
- When a timer fires: task executes → message updates → `xsetroot` is called
- All task messages are combined with `|` separators and displayed

**Note**: The application uses `xsetroot`, not `xprop`.

## How it works
Simple linked list of structs,with  period of execution, function to execute and message

```
                  ┌───────────┐
                  │ CPU usage │
                  └─────┬─────┘
       ┌───────────┬────┴─────┬───────────┐
    message    Frecuency   Function    Section
┌──────┴──────┐  ┌─┴──┐  ┌────┴─────┐  ┌──┴───┐
│ 2.0|1.0|3.0 │  │ 3s │  │ getCPU() │  │ Time │
└─────────────┘  └────┘  └──────────┘  └──┬───┘
                        ┌──────────┬──────┴────┬─────────────┐
                     message   Frecuency    Function      Section
                    ┌───┴───┐   ┌──┴──┐  ┌─────┴─────┐  ┌────┴────┐
                    │ 12:39 │   │ 60s │  │ getTime() │  │ XMR/USD │
                    └───────┘   └─────┘  └───────────┘  └────┬────┘
                                          ┌──────────┬───────┴─────┬─────────────┐
                                       message   Frecuency      Function      Section
                                       ┌──┴───┐   ┌──┴───┐  ┌──────┴───────┐  ┌──┴───┐
                                       │ 110$ │   │ 150s │  │ binanceAPI() │  │ NULL │
                                       └──────┘   └──────┘  └──────────────┘  └──────┘
```
## Extra:
xmr_query is implemented in the [libcurl](https://github.com/israel24961/status4dwm/tree/libcurl) branch of this same repository, using a [public api](https://binance-docs.github.io/apidocs/spot/en/#wallet-endpoints) to Binance spot(i think) market, the bid side.
