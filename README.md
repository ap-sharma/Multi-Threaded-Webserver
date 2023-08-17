# Multi-Threaded-Webserver
HTTP Web server

Compile Instructions:-</br>
Use -l pthread flag to compile simple_server.cpp file</br>

Capabilities of server:-</br>
◦ Can handle HTTP requests using Socket APIs</br>
◦ Implemented master-worker thread pool architecture to handle multiple concurrent clients using pthread APIs</br>
◦ Achieved synchronization between worker threads using condition variable and lock from pthread API</br>
