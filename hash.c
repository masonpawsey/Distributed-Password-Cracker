// Compile with: gcc -o hash md5.c
// Run with: ./hash <string to hash>
// NOTE: currently only works on strings WITHOUT spaces

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(__APPLE__)
#  define COMMON_DIGEST_FOR_OPENSSL
#  include <CommonCrypto/CommonDigest.h>
#  define SHA1 CC_SHA1
#else
#  include <openssl/md5.h>
#endif

// This function takes "string", it's length
// and then returns the md5 hash
// Credit to: Todd https://stackoverflow.com/a/8389763

char *str2md5(const char *str, int length) {
	int n;
	MD5_CTX c;
	unsigned char digest[16];
	char *out = (char*)malloc(33);

	MD5_Init(&c);

	while (length > 0) {
		if (length > 512) {
			MD5_Update(&c, str, 512);
		} else {
			MD5_Update(&c, str, length);
		}
		length -= 512;
		str += 512;
	}

	MD5_Final(digest, &c);

	for (n = 0; n < 16; ++n) {
		snprintf(&(out[n*2]), 16*2, "%02x", (unsigned int)digest[n]);
	}

	return out;
}

int main(int argc, char **argv) {
	// We must call the program with a string to hash i.e. `./hash example`
	if(argc != 2) {
		printf("Please supply a string to hash\n");
		return 1;
	}
	// Call the hashing function on the string we want to hash
	char *output = str2md5(argv[1], strlen(argv[1]));
	// Print the result
	printf("%s\n", output);
	free(output);
	return 0;
}