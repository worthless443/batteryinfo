#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>

const char cmd[] = "upower -i /org/freedesktop/UPower/devices/battery_BAT0";

int wildcard_match_key(char *_key, const char *match) {
	int match_len = strlen(match);
	int key_len = strlen(_key);
	if(_key[key_len - 1] != '*')	
		return 0 == strcmp(_key,match);
	char key[key_len];
	memcpy(key,_key, key_len - 1);
	key[key_len - 1] = '\0';
	key_len = strlen(key);
	char tmp[key_len];
	for(int i=0;i<match_len;i+=key_len) {
		if(i + key_len > match_len)
			return 0;
		memcpy(tmp,match + i, key_len);
		if(0 == strcmp(key,tmp))
			return 1;
	}
	return 0;
}

char *key2value(char **lines, int size,char *ikey) {
	for(int a=0;a<size - 7;++a) {
		int i;
		char *line = lines[a];
		// TODO use realloc() instead of a static size 
		char *key = malloc(sizeof(char) * (1 << 6));
		char *value = malloc(sizeof(char) * (1 << 6));
		for(i=0;line[i] != ':';++i)
			key[i] = line[i];
		if(wildcard_match_key(ikey,key)) {
			free(key);
			memcpy(value,line + i + 1, strlen(line) - i);
			return value;
		}
		free(key);
	}
	return NULL;
}

char *parse_by_key(char *key) {
	FILE *cf = popen(cmd, "r");
	char *buffer = malloc(sizeof(char));
	char *tmp = buffer;
	int size_buffer;
	int a = 0;
	for(int i=0;fread(buffer + i,1,1,cf);++i) {
		buffer = realloc(buffer, size_buffer=sizeof(char) *(i + 1) * 2);
	}
	char **lines = malloc(sizeof(char*));
 	for(int i=0,b=-1,nl=0;i<size_buffer/sizeof(char);++i)	{
		if(nl == 0) {
			lines[a] = malloc(sizeof(char) * 2);
		}
		if(buffer[i] == '\n') {
			lines = realloc(lines, sizeof(char*) * (a + 1) * 2);
			a++;
			b = -1;
			i+=1;	
			nl = 0;
		}
		else if(buffer[i] != ' ') {
			//printf("%c\n",buffer[i]);
			lines[a][++b]	= buffer[i];
			lines[a] = realloc(lines[a], sizeof(char) * (b + 1) * 2);
			nl++;
		}

	}
	free(buffer);
	return key2value(lines,a,key);
}

int main(int argc, char **argv) {
	char *key = NULL;
	char *res = NULL;
	if(argc < 2) 
		key = "percentage";
	else 
		key = argv[1];
	res = parse_by_key(key);
	printf("%s\n", res ? res : "key not found");
	
}
