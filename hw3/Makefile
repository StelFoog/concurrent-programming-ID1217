# zip all files
zip:
	zip load.zip run.sh unisexBathroom.c

# Copy file to remote
rcopy:
	scp load.zip $(user)@student-shell.sys.kth.se:~/concurrent/hw3/load.zip

# Zip and copy file to remote
zrcp:
	zip load.zip run.sh unisexBathroom.c
	scp load.zip $(user)@student-shell.sys.kth.se:~/concurrent/hw3/load.zip

# Unzip file on remote
runzip:
	ssh $(user)@student-shell.sys.kth.se "unzip -o ~/concurrent/hw3/load.zip -d ~/concurrent/hw3"

# Open remote
ropen:
	ssh $(user)@student-shell.sys.kth.se
