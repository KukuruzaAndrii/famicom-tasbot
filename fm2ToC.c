#include <stdio.h>
#include <stdlib.h>

int main() {
	FILE *fm2 = fopen("./assets/lordtom-smb3-totalcontrol2.fm2", "r");

	// checking if the file is opened successfully
	if (fm2 == NULL) {
		printf("fm2 file is not opened");
		exit(0);
	}

	FILE *cfile = fopen("tas.inc", "w");
	if (cfile == NULL) {
		printf("c file is not created");
		exit(0);
	}
	fprintf(cfile, "unsigned char tas_data[] = {\n");

	char line[256];
	while (fgets(line, sizeof(line), fm2)) {
		if (line[0] != '|') {
			continue;
		}

		unsigned byte1p = 0;
		for (unsigned i = 0; i < 8; i++) {
			if (line[i + 3] != '.') {
				byte1p |= 1 << (7 - i);
			}
		}

		unsigned byte2p = 0;
		for (unsigned i = 0; i < 8; i++) {
			if (line[i + 3 + 8 + 1] != '.') {
				byte2p |= 1 << (7 - i);
			}
		}

		// printf("%s", line);
		fprintf(cfile, "0x%02X,0x%02X,\n", byte1p, byte2p);
	}

	fprintf(cfile, "}; \n");
	fclose(cfile);
	printf("Done\n");
	return 0;
}
