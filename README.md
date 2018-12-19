# SFS
simple file system created using FUSE library. <br/>
This project was carried out as a part of "Introduction to Operating Systems" course at PES UNIVERSITY 2018.

## File System
File system provides a means of storage and retrieval of data from and to the disk. It determines how the data is stored and performs several other functions like creating files, directories, updating the contents of a file or directories and various other options such as deleting the contents of file and directories.

## Fuse.
FUSE (File system in user space) is a software that lets nonprivileged users create their own file system without editing the kernel code. Thus FUSE allows users to create a file system on top of an actual file system. 



## Features
The following functions are implemented in the File System.
<ul type=1>
  <li>Create a directory</li>
  <li>Remove a directory </li>
  <li>Create a file using touch,gedit...etc</li>
  <li>Open and Close a file</li>
  <li>Read and Write to a file</li>
  <li>Remove a file</li>
  <li>Change the mode of a file</li>
  <li>Truncate and Append to a file</li>
  <li>Get attributes of a file or folders</li>
  <li>Functions to make the File System persistent</li>
  </ul>
  
## Using the File System
The file system was built on ubuntu 16.04 using FUSE version 30.<br/>
Clone this repository
```
$git clone https://github.com/SKsaqlain/SFS SFS
```
Cd into SFS to install Dependencies
```
$cd SFS
$chmod 777 fuse_install.sh
$./fuse_install.sh
```
Compile and Mount the File System
```
$chmod 777 start.sh
$./start.sh
```
 
 Run the test.sh script to create some files and folders for testing purpose
 ```
 $chmod 777 test.sh
 $./test.sh
 ```
 To run the commands with out the start.sh fileRun the below commands<br/>
 Create a Mount point
 ```
 $mkdir fs1
 ```
 Run the make file
 ```
 $make -f SFS.make
 ```
 Mount the File System in the Foreground mode.
 ```
 $./sfs -f fs1
 ```
 To create some file or directories cd to the mount-point.
 ```
 $cd fs1
 $mkdir testfolder
 ```
 To come out of the file system.
 ```
 cd ..
 ```
 To unmount the file system.
 ```
 $fusermount -u [MOUNT-POINT]
 ```
To unmount fs1.
 ```
 $fusermount -u fs1
 ```
 ## License
This project is licensed under the [MIT License](LICENSE.md)
