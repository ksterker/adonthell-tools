#include <stdio.h>

int main(int argc, const char* argv[])
{
	int c, newline=1;
	while ((c=getchar())!=EOF) {
    	if (c=='"') {
			putchar('\\');
		}
		if (newline && c!=' ') {
			putchar('"');
			newline=0;
		}
		if (c=='\n') {
			printf("\"\n");
			newline=1;
		} else {
			if (c=='\\') putchar('\\');
			putchar(c);
		}
	}
    return 0;
}

