./codexion 2 800 150 150 150 2 10 fifo


USB = 2
number_of_coders                                                                Number of coders (and dongles) in the simulation
time_to_burnout                                                             Deadline (ms) since the last compile start before a coder burns out
time_to_compile                                                             Duration (ms) of the compiling phase
time_to_debug                                                               Duration (ms) of the debugging phase
time_to_refactor                                                                Duration (ms) of the refactoring phase
compiles_required                                                             Simulation stops once every coder has compiled at least this many times
dongle_cooldown                                                             Time (ms) a dongle stays unavailable after being released
scheduler                                                                   Arbitration policy for contested dongles: fifo or edf


1 -> USD 1
0 1 take a dongle 
0 1 take a dongle
0 1 compile

10 2 take a dongle
10 2 take a dongle
10 2 compile

150 1 debug
160 debug
310 refactor
300 1 refactor
300 1 take a dongle
300 1 take a dongle
...

burnout
