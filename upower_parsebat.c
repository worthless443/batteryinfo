#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>

static const char cmd[] = "upower -i /org/freedesktop/UPower/devices/battery_BAT0";

static int wildcard_match_key(char *_key, 
		char *value, int size) {

	value[size] = '\0';
	int match_len = strlen(value);
	int key_len = strlen(_key);
	if(_key[key_len - 1] != '*')	
		return 0 == strcmp(_key,value);
	char key[key_len];
	memcpy(key,_key, key_len - 1);
	key[key_len - 1] = '\0';
	key_len = strlen(key);
	for(int i=0;i<match_len;i+=key_len) {
	
		char tmp[key_len*2];
		if(i + key_len > match_len)
			return 0;
		memcpy(tmp,value + i, key_len);
		tmp[key_len] = '\0';
		if(0 == strcmp(key,tmp))
			return 1;
	}
	return 0;
}


char *key2value(char **lines, int size,char *ikey, char **keyout) {
	char *key = malloc(sizeof(char));
	char *value = malloc(sizeof(char));
	int i;
	for(int a=0,j=-1;a<size - 7;++a) {
		char *line = lines[a];
		// TODO use realloc() instead of a static size 
		for(i=0;line[i] != ':';++i) {
			key[i] = line[i];
            key = realloc(key, sizeof(char)*(i + 1));
			if(i > 20) break;
		}
		//keyout[j][0] = *(char*)key;
		memcpy(keyout[++j], key, i);
		if(wildcard_match_key(ikey,key,i)) {
            value = realloc(value,(strlen(line) - 1)*sizeof(char));
			memcpy(value,line + i + 1, strlen(line) - i - 1);
            memset(value + strlen(line) - i - 1, '\0', 4);
			free(key);
			for(int i=0;i<size;++i)
				free(lines[i]);
			free(lines);
			return value;
		}
        memset(key, '\0', i + 1);
	}
	free(key);
	free(value);
	for(int i=0;i<size;++i)
		free(lines[i]);
	return NULL;
}

char *parse_by_key(char *key, char **keyout) {
	keyout[0][0] =  1;
	FILE *f = popen(cmd, "r");
	char *buffer = malloc(sizeof(char)*10000);
	int size_buffer;
	int a = 0;
	int track_b = 0;
	for(int i=0;fread(buffer + i,1,1,f);++i) {
		buffer = realloc(buffer, size_buffer=sizeof(char) *(i + 1) * 2);
	}
	pclose(f);
	//printf("%s\n", buffer);
	char **lines = malloc(sizeof(char*) * (1 << 6));
 	for(int i=0,b=-1,nl=0;i<size_buffer/sizeof(char);++i)	{
		if(nl == 0) {
			lines[a] = malloc(sizeof(char) * 2);
			memset(lines[a],'\0', (b + 1) * 10);
		}
		if(buffer[i] == '\n') {
			lines[a][b + 1] = '\0';
			a++;
			b = -1;
			i+=1;	
			nl = 0;
			if(track_b == 15) {
				break;
			}
			track_b = 0;
		}
		else if(buffer[i] != ' ') {
			//printf("%c\n",buffer[i]);
			lines[a][++b]	= buffer[i];
			lines[a] = realloc(lines[a], sizeof(char) * (b + 1) * 10);
			nl++;
		}

		if(a > 60) {
			exit(1);
		}
		track_b++;
	}

	free(buffer);
	return key2value(lines,a,key,keyout);
}

#ifdef __MAIN__
int main(int argc, char **argv) {
	int pk = 0;
	char *key = "find";
	char *res = NULL;
	int kval = -1;
	
	if(argc < 2)
		return 1;

	for(int i=1;i<argc;++i) {
		if(*(argv[i] + 1) == 'l') {
			if(strlen(argv[i]) > 2)
				kval = atoi(argv[i] + 2);
			pk = 1;
		}
		if(*argv[i] != '-')
			key = argv[i];
	}


	char **keyout = malloc(sizeof(char*) * 100);
	for(int i=0;i<100;++i) {
		keyout[i] = (char*)malloc(sizeof(char) * 100);
		//memset(keyout, '\0',100);
	}
	res = parse_by_key(key, keyout);
	if(pk) {
		for(int i=0;i<kval;++i)
			printf("%s\n",keyout[i]);
	}
	else
		printf("%s\n", res ? res : "key not found");

	for(int i=0;i<100;++i)
		free(keyout[i]);
	if(res) {
	free(res);
	return 0;
}
return 1;
}
#endif
