How to build:

1. Run cmake --build .
2. Run sudo ./kwserver

Some Humble Brag
```
aykay@Aykay-Lappy:/mnt/c/Users/Khaliq$ ab -n 100000 -c 1000 localhost:8080/
This is ApacheBench, Version 2.3 <$Revision: 1843412 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking localhost (be patient)
Completed 10000 requests
Completed 20000 requests
Completed 30000 requests
Completed 40000 requests
Completed 50000 requests
Completed 60000 requests
Completed 70000 requests
Completed 80000 requests
Completed 90000 requests
Completed 100000 requests
Finished 100000 requests


Server Software:
Server Hostname:        localhost
Server Port:            8080

Document Path:          /
Document Length:        101 bytes

Concurrency Level:      1000
Time taken for tests:   23.941 seconds
Complete requests:      100000
Failed requests:        0
Total transferred:      16200000 bytes
HTML transferred:       10100000 bytes
Requests per second:    4176.91 [#/sec] (mean)
Time per request:       239.411 [ms] (mean)
Time per request:       0.239 [ms] (mean, across all concurrent requests)
Transfer rate:          660.80 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0    0   2.5      0      41
Processing:     7  238  15.2    238     274
Waiting:        2  238  15.2    238     274
Total:         43  238  13.0    238     274

Percentage of the requests served within a certain time (ms)
  50%    238
  66%    241
  75%    243
  80%    245
  90%    248
  95%    251
  98%    256
  99%    259
 100%    274 (longest request)
 ```

 