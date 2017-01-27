Smart Modem Manager (SMM)

Smart Modem Manager is a component running on embedded Linux to proxify the access to a modem from a host device. It enables intercepting commands on the control channel before sending them to the modem or to handle specific modem responses to perform custom operations on the embedded Linux and reply custom responses to the host device. The routing table of the embedded Linux can also be controlled through the Smart Modem Manager to allow fine tuning of the routing of IP packets.

Alternatively, SMM can be extended to handle request / response based protocols other than AT command.


The main TODOs for SMM are:
- Use netlink sockets to enable IP forwarding properly without needing a dirty system() command.
- Use netlink socket to set IP routes
- Implement a proper AT command parser from the recommandation given at https://www.itu.int/rec/T-REC-V.250/en

