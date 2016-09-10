#include <assert.h>
#include <stdio.h>

int main() {
	int number = -1;
  /*
	char string[101];
	int match_count = sscanf("hello-world\t100", "%100s\t%d\n", string, &number);
	assert(2 == match_count);
  */

  sscanf("01-", "%2d", &number);

  printf("%d\n", number);

}
