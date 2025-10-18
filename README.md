# ipmi-gather
Securely collect IPMI status information from remote machines

## Purpose

This project facilitates collection of hardware status and potentially
other data from remote computers that may not accept incoming connections,
due to being behind a NAT with no port forwarding, having restrictive
firewall rules, etc.

The remote machines need only be able to establish an outgoing connection
to a collection server and have a shared munge key installed.

This is useful, for example, in an LPJS cluster/grid, where some of the
compute nodes are behind a NAT and cannot be reached.  Monitoring
hardware health with IPMI tool can help head off problems with failed
nodes before they affect running jobs.

## Notes

This is currently a proof-of-concept project.

The project name will likely change as the code is generalized to
support any type of data.

The ipmi-send command will then become an example program using the
more generalized API.
