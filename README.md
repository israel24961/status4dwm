# STATUS4DWM
Simple status for [dwm](https://dwm.suckless.org/)

## Build
A simple make would be enough
```
git clone https://github.com/israel24961/status4dwm
cd status4dwm
make
```
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
