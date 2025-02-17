# EErratic
EErratic : C/C++ headers for avoiding Embedded Erratic test

> [!NOTE]
> This project has the ability to turn a completely doomed project into a slightly less doomed one... 🔥


## EErratic timer

In embedded systems, execution time and timing must be considered in the design, but the difficulty of creating and debugging such systems is high for teams with immature design technology and coordination with other devices.

I have therefore created a timed execution header that is independent of the system implementation.

This system defines an elapsed time for each step and provides a variable sleep time function in case the time is approaching or a timeout occurs.

In addition, because the execution time is defined first, it is easier to view the execution flow of the device on a time axis during the design phase.
