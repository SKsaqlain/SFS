

./sfs.out: main.o disk.o inode.o 
		gcc  -Wall main.o disk.o inode.o  `pkg-config fuse --cflags --libs` -o ./sfs


disk.o:	disk.c 
		gcc -c -Wall disk.c  `pkg-config fuse --cflags --libs`

inode.o: inode.c 
		gcc -c -Wall inode.c  `pkg-config fuse --cflags --libs`

main.o: main.c 
		gcc -c -Wall main.c `pkg-config fuse --cflags --libs` 
