#SocketWatcher

SocketWatcher is a JavaScript binding for `uv_poll` based on code by @TobyEalden for node_mdns. It's useful as a drop-in replacement for Node's old, private module `IOWatcher`.

More information on `uv_poll`:

- [libuv header (look for `uv_poll_t`)](https://github.com/joyent/libuv/blob/master/include/uv.h)
- [External I/O with polling (libuv docs)](http://nikhilm.github.io/uvbook/utilities.html#external-i-o-with-polling)

##Installation and use

```$ npm install socketwatcher```

```javascript
var SocketWatcher = require("SocketWatcher");
var watcher = new SocketWatcher();
watcher.callback = function() { [...] };
watcher.set(fd, read_flag, write_flag);
watcher.start();
// watcher.stop();
```

##Original work

The files contributed to node_mdns:

- [socket_watcher.hpp](https://github.com/agnat/node_mdns/blob/6dbd4619c2fe47a17cbc5d236a8e057eb08a1b41/src/socket_watcher.hpp)
- [socket_watcher.cpp](https://github.com/agnat/node_mdns/blob/6dbd4619c2fe47a17cbc5d236a8e057eb08a1b41/src/socket_watcher.cpp)
