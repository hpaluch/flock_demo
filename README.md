# `flock_demo`

Here is an simple example of using `flock(2)` on 
QNX Neutrino RTOS 7/x86. It hopefully make you aware
that some filesystems (for example RAM disk filesystem) d
do not support `flock(2)` call - will return `Function not implemented`.


# Setup

You need to have installed `QNX SDP 7.0`.

You may choose any of these options:
* import this project into Momentics IDE 
* run `make` from CLI

To run `make` from CLI you need to:

* setup environment variables and PATH for build using command like:

  ```
  source ~/qnx700/qnxsdp-env.sh
  ```

* then you can just issue:
  ```
  make
  ```

# Running 

To run binary you can use any of:

* Run or Debug function from Momentics IDE
* upload and run binary on QNX Neutrino RTOS target, for example using commands
  like:

  ```
  # replace 192.168.56.102 wiht IP of your QNX Neutrino RTOS
  qnx_ip=192.168.56.102
  curl -T build/x86-debug/flock_demo  ftp://qnxuser:qnxuser@$qnx_ip:/tmp/
  # trick to chmod binary without chmod command
  curl  -Q "site chmod 0777 /tmp/flock_demo"  ftp://qnxuser:qnxuser@$qnx_ip
  ```

Now you can try on your QNX RTOS target:
```bash
/tmp/flock_demo /tmp/lock1

6324.186013936 Acquiring LOCK_EX on fd=3, fname='/tmp/lock1'
flock(fd=3, fname='/tmp/lock1', LOCK_EX): Function not implemented

```
Yes - it fails on `/tmp/` which is RAM disk filesystem (intentionally limited)

Need to try other FS.

# Building on Linux

You can use just this one-liner on Linux:
```bash
gcc -Wall -o /tmp/linux_flock_demo src/flock_demo.c 
```

The command should work on any Unix like fileystem on Linux:
```
/tmp/linux_flock_demo /tmp/lock1
6860.105908191 Acquiring LOCK_EX on fd=3, fname='/tmp/lock1'
6860.109659950 Acquired LOCK_EX on fd=3, fname='/tmp/lock1'
6860.110105333 Sleeping for 10 seconds...
6870.110866096 Releasing LOCK_EX on fd=3, fname='/tmp/lock1'
6870.110948286 Released LOCK_EX on fd=3, fname='/tmp/lock1'
```

While in sleep you can try run another instance of `/tmp/linux_flock_demo`
to see `LOCK_EX` (exclusive) in action.



--hp

