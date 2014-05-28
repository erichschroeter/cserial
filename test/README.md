In order to run the unit tests some environment variables need to be set
correctly. These environment variables are used in the unit test because there
wasn't a way for me to statically define them that would work across all
development environments.

You need two device files that the unit test will be able to communicate
through. I use [socat][0] to create these files. To install on Ubuntu:

    sudo apt-get install socat

Then to create the two psuedo tty device files:

    socat -d -d pts,raw,echo=0 pts,raw,echo=0

That command will print out the two files it created. In my case:

    2014/05/27 15:33:35 socat[6498] N PTY is /dev/pts/8
    2014/05/27 15:33:35 socat[6498] N PTY is /dev/pts/9
    2014/05/27 15:33:35 socat[6498] N starting data transfer loop with FDs [3,3] and [5,5]

With that information we need to set two environment variables for the unit test
to use.

    export CSERIAL_TX=/dev/pts/8
    export CSERIAL_RX=/dev/pts/9

Now, to run the unit tests simply execute:

    ./test/run_tests

[0]: http://www.dest-unreach.org/socat/
