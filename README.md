plexio
======

Installation
------------

    make
    cp plexio /usr/local/bin/

Usage
-----

First argument is the UNIX socket to listen on. The next arguments are the
command to mask.

For instance, to launch a Minecraft server:

    plexio /tmp/minecraft java -jar minecraft_server.jar

Now, to access the stdin and out of the process, open a connection to the UNIX
socket specified. For instance, using netcat wrapped in readline:

    rlwrap nc -U /tmp/minecraft

(For accessing Minecraft servers that are wrapped in anyconsole, I've written
[lolmc](https://github.com/Mortal/lolmc) as a front-end to this UNIX socket for
Node.js scripts.)

This is approximately equivalent to

    stdbuf -oL java -jar minecraft_server.jar > minecraft.log 2>&1
    tail -fn 0 minecraft.log

except, of course, that you have access to the stdin of the process as well.
