# Stable identifiers
It's not always easy to name functions any better than the default `func_02001234`, but keeping the default names is less than ideal when decompiling a library from many games at once. If we know that `func_02001234` in game A is the same as `func_02005678` in game B, then they should be named the same. However, having the function's address in the name would only work in one game, not multiple.

To solve this, we use a stable identifier system to act as a placeholder name until the function can be documented. Functions are categorised by module, then identified by a unique four-digit ID. The function identifiers take the form `{module}_func_{id}`, for example `OS_func_0175`. Likewise, data identifiers look like `{module}_data_{id}`.

The next available ID increases whenever a new ID is used, so refer to the table below for the next available identifier in each SDK module. Please update the table when you use a new identifier.

| Module | Next function    | Next data        |
| ------ | ---------------- | ---------------- |
| `CARD` | `CARD_func_0093` | `CARD_data_0001` |
| `FS`   |   `FS_func_0102` |   `FS_data_0006` |
| `FX`   |   `FX_func_0003` |   `FX_data_0001` |
| `G2`   |   `G2_func_0007` |   `G2_data_0001` |
| `G3`   |   `G3_func_0001` |   `G3_data_0001` |
| `GX`   |   `GX_func_0012` |   `GX_data_0001` |
| `MB`   |   `MB_func_0089` |   `MB_data_0001` |
| `MI`   |   `MI_func_0021` |   `MI_data_0001` |
| `OS`   |   `OS_func_0179` |   `OS_data_0001` |
| `PM`   |   `PM_func_0051` |   `PM_data_0001` |
| `RTC`  |  `RTC_func_0014` |  `RTC_data_0001` |
| `SND`  |  `SND_func_0056` |  `SND_data_0001` |
| `TP`   |   `TP_func_0008` |   `TP_data_0001` |
| `WM`   |   `WM_func_0040` |   `WM_data_0001` |
