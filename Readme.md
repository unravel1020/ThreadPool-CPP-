# Readme
## Tips the project don't conclude the directory pool and ThreadPool.h. The ThreadPool.cpp is the whole project.
## introduction

This is a ThreadPool made with C++ by using visual studio and it should be connected to a linux system to compile it.

## How to connect to a linux system with visual studio

I will show you how to deal it by using wsl2(Ubuntu22.0.4LTS)

### First  download ssh both in windows and ubuntu

Above all, you should install ssh both in windows and ubuntu.

For windows, open the powershell by using admin then use:

```powershell
Get-WindowsCapability -Online | ? Name -like 'OpenSSH*'
Add-WindowsCapability -Online -Name OpenSSH.Client~~~~0.0.1.0
Add-WindowsCapability -Online -Name OpenSSH.Server~~~~0.0.1.0

```

then set up the ssh

```powershell
Start-Service sshd
# OPTIONAL but recommended:
Set-Service -Name sshd -StartupType 'Automatic'
# Confirm the Firewall rule is configured. It should be created automatically by setup. 
Get-NetFirewallRule -Name *ssh*
# There should be a firewall rule named "OpenSSH-Server-In-TCP", which should be enabled
# If the firewall does not exist, create one
New-NetFirewallRule -Name sshd -DisplayName 'OpenSSH Server (sshd)' -Enabled True -Direction Inbound -Protocol TCP -Action Allow -LocalPort 22

```

For ubuntu, use:

```shell
sudo apt-get install openssh-client
$ sudo apt-get install openssh-server

```

then start ssh service:

```shell
sudo /etc/init.d/ssh start
```

### Second set up visual studio

- open the tools then choose the options
- choose the connection manager
- add a new connection
- the first one is the ip of your ubuntu, the second one port is 22 as default for ssh the third one is user name for yourself to login your ubuntu. And you can choose your password of your username to verify or use key to verify.

### Third, set up a new project for linux by visual studio

choose a console application compiling by Linux

