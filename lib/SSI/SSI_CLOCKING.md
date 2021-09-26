# SSI CLOCKING

This page presents the equations, adjustable variables, and potential values of the SSI bit rate.

---
## Equation

There are three adjustable variables in regards to SSI clocking.

- SysClk: This is the main execution clock of the TM4C. This is typically 80 MHZ.
- CPSDVSR: This is this clock prescale, which is a division factor to derive the clock rate. As it increases, the bit rate decreases. It has values 2 - 254, and are only even.
- SCR: This is the serial clock rate. As it increases, the bit rate decreases. It has values from 0 - 255.

```c
Clock Rate = SysClk / (CPSDVSR * (1 + SCR))
```

---
## Example bit rates

| SysClk | CPSDVSR | SCR |     Output |
|--------|---------|-----|------------|
| 80 MHZ |       2 |   0 |     40 MHZ |
| 80 MHZ |       2 |   1 |     20 MHZ |
| 80 MHZ |       2 |   2 |  13.33 MHZ |
| 80 MHZ |       2 | ... |        ... |
| 80 MHZ |       2 | 255 | 156.25 KHZ |
| 80 MHZ |       4 |   0 |     20 MHZ |
| 80 MHZ |       4 |   1 |     10 MHZ |
| 80 MHZ |       4 |   2 |   6.67 MHZ |
| 80 MHZ |       4 | ... |        ... |
| 80 MHZ |       4 | 255 | 78.125 KHZ |
| 80 MHZ |       6 |   0 |  13.33 MHZ |
| 80 MHZ |       8 |   0 |     10 MHZ |
| 80 MHZ |     ... | ... |        ... |
| 80 MHZ |     254 |   0 | 314.96 KHZ |
| 80 MHZ |     254 |   1 | 157.48 KHZ |
| 80 MHZ |     254 |   2 | 104.99 KHZ |
| 80 MHZ |     254 | ... |        ... |
| 80 MHZ |     254 | 255 | 1.2303 KHZ |

---
## Implementation

SSI API grants the user free reign to select any possible CPSDVSR and SCR, although a couple are provided (see SSI.h). The user should consult the datasheets of the SSI secondary devices to determine the appropriate SSI clock speed.