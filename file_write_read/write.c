#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct person
{
	char name[20];
	int age;
};

static int write_to_file(void)
{
	FILE *fp = NULL;
	struct  person kim ={
		.name = "kim",
		.age = 20
	};
	struct  person lee ={
		.name = "lee",
		.age = 20
	};

	if (!(fp = fopen("person", "w"))){
		return -1;
	}

	if(fwrite(&kim, sizeof(struct person), 1, fp) != 1){
		goto err;
	}
	if(fwrite(&lee, sizeof(struct person), 1, fp) != 1){
		goto err;
	}
	fclose(fp);

	return 0;

err:
	if(fp){
		fclose(fp);
	}
}

static int read_from_file(void)
{
	FILE *fp = NULL;

	struct person persons[2];


	if (!(fp = fopen("person", "r"))){
		return -1;
	}


	if(fread(persons,sizeof(struct person), 2, fp) != 2){
		goto err;
	}
	fclose(fp);

	for(int i = 0 ; i < 2; ++i){
		printf("name : %s\nage: %d\n",
				persons[i].name,
				persons[i].age);
	}
	return 0;
	err:
	if(fp){
		fclose(fp);
	}
}

int main(int argc,char **argv)
{
	if(write_to_file()){
		printf("write fail\n");
		return -1;
	}

	if(read_from_file()){
		printf("read fail\n");
		return -1;
	}

	return 0;
}