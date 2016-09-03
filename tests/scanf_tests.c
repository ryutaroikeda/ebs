#include <assert.h>
#include <stdio.h>

int main() {
	char string[101];
	int number = 0;
	int match_count = sscanf("hello-world\t100", "%100s\t%d\n", string, &number);
	assert(2 == match_count);
}
