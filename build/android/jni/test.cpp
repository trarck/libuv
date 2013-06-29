#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <uv.h>

#define LOG(s,...)  do{}while(0);

uv_fs_t open_req;
uv_fs_t read_req;
uv_fs_t write_req;

uv_fs_t open_req1;

bool input_opened=false;

bool output_opened=false;

char buffer[4];
void on_read(uv_fs_t *req);

void on_write(uv_fs_t *req){
	printf("on_write\n");
	uv_fs_req_cleanup(req);
	if(req->result<0){
		fprintf(stderr,"Write error:%s\n",uv_strerror(uv_last_error(uv_default_loop())));
	}else{
		uv_fs_read(uv_default_loop(),&read_req,open_req.result,buffer,sizeof(buffer),-1,on_read);
	}
}

void on_read(uv_fs_t *req){
	LOG("on_read\n");
	uv_fs_req_cleanup(req);

	if(req->result <0){
		fprintf(stderr,"read error:%s\n",uv_strerror(uv_last_error(uv_default_loop())));
	}else if(req->result==0){
		LOG("on_read 2\n");
		uv_fs_t close_req;
		uv_fs_close(uv_default_loop(),&close_req,open_req.result,NULL);
	}else{
		LOG("on_read 3\n");
		uv_fs_write(uv_default_loop(),&write_req,open_req1.result,buffer,req->result,-1,on_write);
		//uv_fs_write(uv_default_loop(),&write_req,1,buffer,req->result,-1,on_write);
		//uv_fs_read(uv_default_loop(),&read_req,open_req.result,buffer,sizeof(buffer),-1,on_read);
	}
}


void on_open(uv_fs_t *req){
	LOG("on_open\n");
	if(req->result!=-1){
		if(output_opened==true){
			uv_fs_read(uv_default_loop(),&read_req,req->result,
				buffer,sizeof(buffer),-1,on_read);
		}
		input_opened=true;
	}else{
		fprintf(stderr,"error opening file:%d\n",req->errorno);
	}
	uv_fs_req_cleanup(req);

}

void on_open1(uv_fs_t *req){
	LOG("on_open1\n");
	if(req->result!=-1){
		LOG("open fd:%d",open_req.result);
		if(input_opened){
			uv_fs_read(uv_default_loop(),&read_req,open_req.result,
			buffer,sizeof(buffer),-1,on_read);
		}
		output_opened=true;
	}else{
		fprintf(stderr,"error opening file:%d,%s,%s\n",req->errorno,req->path,uv_strerror(uv_last_error(uv_default_loop())));
	}
	uv_fs_req_cleanup(req);

}

int main(int argc, char **argv) {
	const char* file="t.txt";
	const char* outFile="out.txt";

	printf("11\n");
	uv_fs_open(uv_default_loop(),&open_req,file,O_RDONLY,0,on_open);
	printf("22\n");
	uv_fs_open(uv_default_loop(),&open_req1,outFile,O_CREAT|O_WRONLY,438,on_open1);
	//printf("33\n");
	uv_run(uv_default_loop(),UV_RUN_DEFAULT);
	printf("44\n");
	getchar();
	return 0;
}