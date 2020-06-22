# LineBasedTime Server
A simple protocol & its reference server implementation used to fetch time information from the machine, where the server is running, by remote clients. The client implementation can be found [here](/ethernetlord/linebasedtime-client/).

The documentation of the LineBasedTime protocol can be found in [this repository](/ethernetlord/linebasedtime-protocol/).

---

## Setup
A complete setup of the server consists of 4 steps:
* **compiling**,
* **installing**,
* **cleanup** and optionally,
* **installation of a systemd service unit**.

### Compiling
Change your working directory to this repository's root and run this command:
```
make
```
The executable file ```linebasedtime_server``` will be created in your working directory. If you don't want to install it, it can be run directly from there.

### Installing
After compiling, run:
```
sudo make install
```
The executable will be available as ```lbtimed``` in the ```/usr/local/bin/``` directory.

### Cleanup
After compiling or installing, run:
```
make clean
```
This will remove the ```linebasedtime_server``` file from your working directory. You can perform this cleanup without installing the program. If you wished to install it in the future, you would need to compile the program again.

### Installation of a systemd service unit
If your Linux distribution uses systemd as its init system, you may also wish to install the default systemd service unit file. Perform any edits of the ```lbtimed.service``` file in the repository's root, if you wish so, and then run:
```
sudo make systemd-unit
```
It will be now possible to enable and start the service using ```sudo systemctl enable lbtimed``` and ```sudo systemctl start lbtimed``` commands.

The service will now automatically run at startup; if you want to run it only manually, do not enable it and just start it, when it's needed.

---

## Usage
The compiled and/or installed executable takes exactly 2 arguments:
1. IPv4/v6 address or hostname to listen on
2. TCP port number or service name to listen on


#### Example 1
If you installed the program and ```/usr/local/bin``` is in your ```PATH``` environment variable, you can run it as following:
```
lbtimed :: 5567
```
The server will now accept connections on the unspecified IPv6 address (--> remote connections from all network interfaces, including IPv4 connections using IPv4 mapped addresses) and port ```5567```.

The server's operation can be peacefully terminated using ```SIGINT```, ```SIGHUP``` or ```SIGTERM``` signal, which includes the keypress of ```Ctrl+C```.


#### Example 2
If you compile the program (and don't perform a cleanup), you can also run it directly from the working directory (the repository's root) without its installation:
```
./linebasedtime_server 0.0.0.0 5567
```
This time, the server will be listening on the unspecified IPv4 address (--> won't accept any IPv6 connections).

---

## Author
Made by **EthernetLord**.
* Website: https://ethernetlord.eu/
* GitHub: @ethernetlord (https://github.com/ethernetlord/)
* Twitter: @ethernetlordcz (https://twitter.com/ethernetlordcz)


## Licensing
This project is licensed under the **[MIT License](/ethernetlord/linebasedtime-server/blob/master/LICENSE)**.


## Contributing
If you want to remind me of any bugs or fix them right away, add some new functionality or just make something better, feel free to create a pull request or an issue.
