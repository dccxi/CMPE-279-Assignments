# Questions:
Students: Tong Wu, Lu Yu
## 1. What happens if the file being requested is not inside the chroot? Does it matter?

It doesn’t matter. Beacuse the file descriptor has already been passed to the child process through command line arguments. A file descriptor is a number that uniquely identifies an open file in a computer's operating system. It describes a data resource, and how that resource may be accessed. Therefore, the file being requested does not need to be inside the chroot.

## 2. Explain the mechanism you are using to pass file descriptors between parent and child processes.

1. using open() function to get the reference of the file descriptor of the file passed in the command line arguments.
1. using fileno() to convert the reference to an integer
1. pass the integer as a parameter when executing the child process
1. in the child process, read the file descriptor from argv
1. use fdopen() to open the file from the file descriptor reference

## 3. What happens if the file size of the disk file exceeds the size of the client’s hardcoded buffer? Does the client have sufficient checks to ensure no buffer overruns occur?

If the file size exceeds the size of the clients’ buffer, the client will receive the data in multiple batches. This guarantees that each batch is under buffer size and the client still receives the entire data from the server. However, bounds checking can always be added to completely ensure no buffer overruns occur for the client, but that will require additional code and processing time too. 
