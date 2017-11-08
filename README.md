
Dovecot checkpassword implementaion

For the protocol, see [https://wiki2.dovecot.org/AuthDatabase/CheckPassword]

Expects two inputs on fd 3, a username and password.

Connects to a socket on localhost and echos the inputs, plus an IP address
we find in the environment.

Expect back (on the socket) either "fail\n" or "success\n".

On fail, exit with code 1.

On success, read "KEY=VALUE\n" lines from the socket, write them
into the environment, and then run the file found on our command line.

Any sign of trouble, exit with code 111.

