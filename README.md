# ipmi-gather
Securely collect IPMI status information from remote machines

## Purpose

This project facilitates collection of hardware status and potentially
other data from remote computers that may not accept incoming connections,
due to being behind a NAT with no port forwarding, having restrictive
firewall rules, etc.

```
Incoming IP : 192.168.0.81
uid : 2002
gid : 2002
Local hostname : proliant1.acadix.biz
User name : admin
Group name : admin
System Power         : on
Power Overload       : false
Power Interlock      : inactive
Main Power Fault     : false
Power Control Fault  : false
Power Restore Policy : previous
Last Power Event     : 
Chassis Intrusion    : inactive
Front-Panel Lockout  : inactive
Drive Fault          : false
Cooling/Fan Fault    : false
Front Panel Control  : none
```

The remote machines need only be able to establish an outgoing connection
to a collection server and have a shared munge key installed.

This is useful, for example, in an LPJS cluster/grid, where some of the
compute nodes are behind a NAT and cannot be reached.  Monitoring
hardware health with IPMI tool can help head off problems with failed
nodes before they affect running jobs.

Since the remote nodes push the data back to the collector, the system
is immune to, and can detect changes in public IP addresses that would
thwart any system designed to pull data from the nodes.  It is also
immune to, and can detect changes to local IP addresses behind at NAT,
which may break port-forwarding.  The data pushed to the collector
contain the latest IP information, which can be used to quickly
correct any network configuration issues.

## Notes

This is currently a proof-of-concept project.

The project name will likely change as the code is generalized to
support any type of data.

The ipmi-send command will then become an example program using the
more generalized API.
