nt Format()
{
	struct pwd passwd [BLOCKSIZ/PWDSIZ];

	int i;
	
	/*	creat the file system file */
	fd = fopen (fsystemname, "wb");/*读写创建一个二进制文件*/
	
	if(fd==NULL)
	{
		printf("硬盘模拟文件创建失败!\n");
		return 0;
	}
	//超级块
	filsys.s_inodes_count=DINODENUM ;      /* inodes 计数 */
	filsys.s_blocks_count=DATABLKNUM;      /* blocks 计数 */
	filsys. s_r_blocks_count=0;    /* 保留的 blocks 计数 */
	filsys. s_free_blocks_count=DATABLKNUM-5; /* 空闲的 blocks 计数 */
	filsys.s_free_blocks_group[0]=50-5;//第一个block group 已经被用了5个
	for(i=1;i<GROUPNUM-1;i++)
	{
		filsys.s_free_blocks_group[i]=50; //后面的group 全部空闲
	//	printf("block group %d 空闲个数为%d\n",i,filsys.s_free_blocks_group[i]);
	}
	filsys.s_free_blocks_group[GROUPNUM-1]=12;//最后一个block组 只有12个block
	filsys.s_free_inodes_count=DINODENUM-5; /* 空闲的 inodes 计数 */
	filsys.s_first_data_block=DATASTARTNO;  /* 第一个数据 block 也就是*/
	filsys.s_log_block_size=BLOCKSIZ;    /* block 的大小 */
	filsys.s_blocks_per_group=BLKGRUPNUM;  /* 每 block group 的 block 数量 */
	filsys.s_inodes_per_group=0;  //每 block group 的 inode 数量  暂未使用
	
	fseek(fd, BLOCKSIZ, SEEK_SET);
	fwrite (&filsys,BLOCKSIZ, 1,fd);
	
	//初始化dinode位图 block位图
	di_bitmap[0]=1;
	di_bitmap[1]=1;
	di_bitmap[2]=1;//前三个inode 分别被 root etc 用户passwd文件占用
	di_bitmap[3]=1;
	di_bitmap[4]=1;

	bk_bitmap[0]=1;
	bk_bitmap[1]=1;
	bk_bitmap[2]=1;//前三个inode 分别被 root etc 用户passwd文件占用
	bk_bitmap[3]=1;
	bk_bitmap[4]=1;

	for(i=5;i<DINODENUM;i++)
	{
		di_bitmap[i]=0;
		bk_bitmap[i]=0;
	}
	for(;i<DATABLKNUM;i++)
	{
		bk_bitmap[i]=0;
	}
	fseek(fd, BLOCKSIZ*2, SEEK_SET);
	fwrite (di_bitmap,BLOCKSIZ, 1,fd);
	fseek(fd, BLOCKSIZ*3, SEEK_SET);
	fwrite (bk_bitmap,BLOCKSIZ, 1,fd);


	//初始化主目录
	struct 	inode *ininode;
	ininode=(struct inode *)malloc(sizeof (struct inode));
	if(!ininode)
	{
		printf("ininode 内存分配失败!");
		return 0;
	}
	//	strcpy(dinodef->di_name,"/");
	ininode->di_ino=0;//i节点标志
	ininode->di_number=3;//关联3个文件夹
	ininode->di_mode=DIMODE_DIR|DIMODE_SYSTEM;//0为目录
	ininode->di_uid=1;//用户id 第一个用户
	ininode->di_gid=1;//组id 管理员组
	ininode->di_size=0;//为目录
	ininode->di_ctime=0;   /* Creation time */
	ininode->di_mtime=0;   /* Modification time */
	ininode->di_block[0]=0;//所占物理块号 后3块分别是 一级指针，二级指针，3级指针

	fseek(fd,DINODESTART, SEEK_SET);
	fwrite(ininode,sizeof(struct inode), 1, fd);

	
	strcpy(dir_buf[0].d_name, ".");
	dir_buf[0].d_ino= 0;//当前目录的dinode号
	strcpy(dir_buf[1].d_name,"..");
	dir_buf[1].d_ino= 0;//主目录的上级目录还是自己
	strcpy(dir_buf[2].d_name, "etc");
	dir_buf[2].d_ino = 1;//etc目录
	
	fseek(fd, DATASTART, SEEK_SET);
	fwrite(dir_buf, BLOCKSIZ, 1, fd);

	//etc目录
	ininode->di_ino=1;//i节点标志
	ininode->di_number=5;//
	ininode->di_gid=1;//组id
	ininode->di_block[0]=1;//所占物理块号 后3块分别是 一级指针，二级指针，3级指针
	fseek(fd, DINODESTART+BLOCKSIZ, SEEK_SET);
	fwrite(ininode,sizeof(struct inode), 1, fd);
	
	strcpy (dir_buf[0].d_name, ".");
	dir_buf[0].d_ino = 1;
	strcpy(dir_buf[1].d_name, "..");
	dir_buf[1].d_ino = 0;
 	strcpy(dir_buf[2].d_name, "passwd");
 	dir_buf[2].d_ino = 2;
	strcpy(dir_buf[3].d_name, "admin");
 	dir_buf[3].d_ino = 3;
	strcpy(dir_buf[4].d_name, "xiao");
 	dir_buf[4].d_ino = 4;

	fseek(fd, DATASTART+BLOCKSIZ, SEEK_SET);
	fwrite (dir_buf, BLOCKSIZ,1,fd);
	
	// admin 目录
	ininode->di_ino=3;//i节点标志
	ininode->di_number=2;//
		ininode->di_gid=0;//组id
	ininode->di_block[0]=3;//所占物理块号 后3块分别是 一级指针，二级指针，3级指针
	fseek(fd, DINODESTART+BLOCKSIZ*3, SEEK_SET);
	fwrite(ininode,sizeof(struct inode), 1, fd);
	
	strcpy (dir_buf[0].d_name, ".");
	dir_buf[0].d_ino = 3;
	strcpy(dir_buf[1].d_name, "..");
	dir_buf[1].d_ino = 1;

	
	fseek(fd, DATASTART+BLOCKSIZ*3, SEEK_SET);
	fwrite (dir_buf, BLOCKSIZ,1,fd);
	
	// xiao 目录
	ininode->di_ino=4;//i节点标志
	ininode->di_number=2;//
	ininode->di_uid=2;//用户id 
	ininode->di_gid=1;//组id
	ininode->di_block[0]=4;//所占物理块号 后3块分别是 一级指针，二级指针，3级指针
	fseek(fd, DINODESTART+BLOCKSIZ*4, SEEK_SET);
	fwrite(ininode,sizeof(struct inode), 1, fd);
	
	strcpy (dir_buf[0].d_name, ".");
	dir_buf[0].d_ino =4;
	strcpy(dir_buf[1].d_name, "..");
	dir_buf[1].d_ino = 1;

	fseek(fd, DATASTART+BLOCKSIZ*4, SEEK_SET);
	fwrite (dir_buf, BLOCKSIZ,1,fd);


	//用户passwd文件
	passwd[0].p_uid= 1; 
	passwd[0].p_gid = GRUP_0; //管理员
	strcpy(passwd[0].username, "admin");
	strcpy(passwd[0].password, "admin");
	
	passwd[1].p_uid= 2;
	passwd[1].p_gid = GRUP_1;
	strcpy(passwd[1].username, "xiao");
	strcpy(passwd[1].password, "xiao");


	for (i=2; i<PWDNUM; i++)
	{
		passwd[i].p_uid = 0;
		passwd[i].p_gid = GRUP_4;
		strcpy(passwd[i].username, "no this user");
		strcpy(passwd[i].password,"");
	}
	fseek(fd,DATASTART+BLOCKSIZ*2, SEEK_SET);
	fwrite(passwd,BLOCKSIZ,1,fd);
	
	ininode->di_ino=2;//i节点标志
	ininode->di_number=2;//
	ininode->di_mode=DIMODE_PASSWD|DIMODE_SYSTEM;//
	ininode->di_uid=1;//用户id 第一个用户
	ininode->di_gid=1;//组id 管理员组
	ininode->di_size=BLOCKSIZ;//大小
	ininode->di_ctime=0;   /* Creation time */
	ininode->di_mtime=0;   /* Modification time */
	ininode->di_block[0]=2;//所占物理块号 后3块分别是 一级指针，二级指针，3级指针
	fseek(fd, DINODESTART+BLOCKSIZ*2, SEEK_SET);
	fwrite(ininode,sizeof(struct inode), 1, fd);
	
	fclose(fd);
	free(ininode);
	return 1;
}

